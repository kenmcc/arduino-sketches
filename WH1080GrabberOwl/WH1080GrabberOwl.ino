/*
** WH1080 decoder for OOK devices
** Compiles with Arduino 1.0.1  
** M Baldwin August 2013
**
** Resources used
** http://www.susa.net/wordpress/2012/08/raspberry-pi-reading-wh1081-weather-sensors-using-an-rfm01-and-rfm12b/
** http://www.sevenwatt.com/main/wh1080-protocol-v2-fsk/
** http://lucsmall.com/2012/04/27/weather-station-hacking-part-1/
** http://www.homeautomationhub.com/content/wh1080-weather-station-integration
** https://github.com/adafruit/Adafruit-BMP085-Library
** Future updates
** Add wind chill - unless I add it to LUA script
** Tw = 13.127 + 0.6215*T - 13.947*V*0.16 + 0.486*T*V*0.16
** where
** Tw = wind chill in Â°C
** v = wind velocity in m/s
** T = temperature in Â°C
** Reduce packet sent via serial for inclusion in HAH - Done
** Add this code to existing CC decoder combining OOK receiver and RFM12b @ 433MHz
*/

// Connect VCC of the BMP085 sensor to 3.3V (NOT 5.0V!)
// Connect GND to Ground
// Connect SCL to i2c clock - on '168/'328 Arduino Uno/Duemilanove/etc thats Analog 5
// Connect SDA to i2c data - on '168/'328 Arduino Uno/Duemilanove/etc thats Analog 4
// EOC is not used, it signifies an end of conversion
// XCLR is a reset pin, also not used here

//#include <JeeLib.h>
#include <Wire.h>
//#include <Adafruit_BMP085.h>

#define PIN_433 2  // AIO1 = 433 MHz receiver

volatile int pulse_433;
volatile word pulse_length;
volatile unsigned long old = 0, packet_count = 0; 
volatile unsigned long spacing, average_interval;
word last_433; // never accessed outside ISR's
volatile word pulse_count = 0; //
byte packet[10];
byte state = 0;
bool packet_acquired = false;
char *direction_name[] = {"N", "NNE", "NE", "ENE", "E", "ESE", "SE", "SSE", "S", "SSW", "SW", "WSW", "W", "WNW", "NW", "NNW"};
int __debug = 1; // set to enable debug mode
int __bmp_fitted = 0;  // set to enable BMP085
//Adafruit_BMP085 bmp;

class DecodeOOK {
protected:
    byte total_bits, bits, flip, state, pos, data[25];

    virtual char decode (word width) =0;
    
public:

    enum { UNKNOWN, T0, T1, T2, T3, OK, DONE };

    DecodeOOK () { resetDecoder(); }

    bool nextPulse (word width) {
        if (state != DONE)
        
            switch (decode(width)) {
                case -1: resetDecoder(); break;
                case 1:  done(); break;
            }
        return isDone();
    }
    
    bool isDone () const { return state == DONE; }

    const byte* getData (byte& count) const {
        count = pos;
        return data; 
    }
    
    void resetDecoder () {
        total_bits = bits = pos = flip = 0;
        state = UNKNOWN;
    }
    
    // add one bit to the packet data buffer
    
    virtual void gotBit (char value) {
        total_bits++;
        byte *ptr = data + pos;
        *ptr = (*ptr >> 1) | (value << 7);

        if (++bits >= 8) {
            bits = 0;
            if (++pos >= sizeof data) {
                resetDecoder();
                return;
            }
        }
        state = OK;
    }
    
    // store a bit using Manchester encoding
    void manchester (char value) {
        flip ^= value; // manchester code, long pulse flips the bit
        gotBit(flip);
    }
    
    // move bits to the front so that all the bits are aligned to the end
    void alignTail (byte max =0) {
        // align bits
        if (bits != 0) {
            data[pos] >>= 8 - bits;
            for (byte i = 0; i < pos; ++i)
                data[i] = (data[i] >> bits) | (data[i+1] << (8 - bits));
            bits = 0;
        }
        // optionally shift bytes down if there are too many of 'em
        if (max > 0 && pos > max) {
            byte n = pos - max;
            pos = max;
            for (byte i = 0; i < pos; ++i)
                data[i] = data[i+n];
        }
    }
    
    void reverseBits () {
        for (byte i = 0; i < pos; ++i) {
            byte b = data[i];
            for (byte j = 0; j < 8; ++j) {
                data[i] = (data[i] << 1) | (b & 1);
                b >>= 1;
            }
        }
    }
    
    void reverseNibbles () {
        for (byte i = 0; i < pos; ++i)
            data[i] = (data[i] << 4) | (data[i] >> 4);
    }
    
    void done () {
        while (bits)
            gotBit(0); // padding
        state = DONE;
    }
};

class OregonDecoderV3 : public DecodeOOK {
public:
    OregonDecoderV3() {}
    
    // add one bit to the packet data buffer
    virtual void gotBit (char value) {
        data[pos] = (data[pos] >> 1) | (value ? 0x80 : 00);
        total_bits++;
        pos = total_bits >> 3;
        if (pos >= sizeof data) {
            resetDecoder();
            return;
        }
        state = OK;
    }
    
    virtual char decode (word width) {
        if (200 <= width && width < 1200) {
            byte w = width >= 700;
            switch (state) {
                case UNKNOWN:
                    if (w == 0)
                        ++flip;
                    else if (32 <= flip) {
                        flip = 1;
                        manchester(1);
                    } else
                        return -1;
                    break;
                case OK:
                    if (w == 0)
                        state = T0;
                    else
                        manchester(1);
                    break;
                case T0:
                    if (w == 0)
                        manchester(0);
                    else
                        return -1;
                    break;
            }
        } else {
            return -1;
        }
        return  total_bits == 80 ? 1: 0;
    }
};

OregonDecoderV3 orscV3;

volatile word pulse;

void rupt (void) {

    static word last;
    // determine the pulse length in microseconds, for either polarity
    pulse = micros() - last;
    last += pulse;
}

// State to track pulse durations measured in the interrupt code
ISR(PCINT2_vect)
  {
    pulse_433 = 0;
    if (digitalRead(PIN_433) == LOW) {
      word ticker = micros();
      pulse_433 = ticker - last_433;
      last_433 = ticker;
      //++ pulse_count;
     }
    if (pulse_433 > 3000) {// reset pulse stream and counter
      state = 0;
      pulse_433 = 0;
      
    }
  }
void reportSerial (const char* s, class DecodeOOK& decoder) {
    byte pos;
    unsigned int watts;
    const byte* data = decoder.getData(pos);
    Serial.print(s);
    Serial.print(' ');
    for (byte i = 0; i < pos; ++i) {
        Serial.print(data[i] >> 4, HEX);
        Serial.print(data[i] & 0x0F, HEX);
    }
    
    if (pos > 5) {
      Serial.print('\t');
      watts = ((data[4] * 256) + (data[3] & 0xF0)) * 1.006196884; 
      Serial.print(watts);
    }
    // Serial.print(' ');
    // Serial.print(millis() / 1000);
    
    Serial.println();
    
    decoder.resetDecoder();
}


void setup()
  {
    Serial.begin(57600);
    Serial.println();
    Serial.println("WH1080 Serial node with BMP085");
  
     pinMode(2, INPUT);  // use
  attachInterrupt (0, rupt, CHANGE);
  
    pinMode(PIN_433, INPUT);
    digitalWrite(PIN_433, 1);   // pull-up
    //Serial.println("Just before rf12_init_OOK()");
    rf12_init_OOK();//set up RF12
    Serial.println("RFM12B set up for 433MHz OOK");
    if (__debug) {Serial.println("Debug mode");}
    // interrupt on pin change
    bitSet(PCMSK2, PIN_433);
    bitSet(PCICR, PCIE2);
    /*if(__bmp_fitted){
      if (!bmp.begin()) {
          Serial.println("Could not find a valid BMP085 sensor, check wiring!");
          __bmp_fitted = 0;
          Serial.print("BMP085 sensor status ");
          Serial.println(__bmp_fitted);
        }
      else
        {
          Serial.println("BMP085 sensor set up");
        }
      }
    else
      {
        Serial.println("BMP085 sensor disabled");
      }
    */
    // enable interrupts
    sei();
    //Serial.println("Interrupts set up");
    
  }

void loop()
  {
    byte i;
    unsigned long now;
    //byte *packet;
    noInterrupts();
    word p = pulse;
    
    pulse = 0;
    interrupts();

    
    if (p != 0) {
        if (orscV3.nextPulse(p))
            reportSerial("OSV3", orscV3);        
     }
    if (pulse_433)
      {
        cli();//disable interrupts and store pulse width
        word pulse = pulse_433;
        pulse_433 = 0;
        sei();//re-enable interrupts
        //if (__debug) {Serial.println(pulse);}
        //Serial.println(pulse); //remove comment to see raw timings
        extractData(pulse);//extract data from pulse stream
      
        if (state == 3)//pulse stream complete
          {
            state = 4;
            now = millis();
            spacing = ((now - old)/1000);
            old = now;
           /* 
            if (spacing < 500)
              {
                Serial.println();
                Serial.println("Too Soon!!!");
              }
            */
            if (spacing >= 10) // was 500
              { 
                packet_count ++;
                average_interval = now / packet_count;  
              
                if (__debug)
                  {
                    /*if (__bmp_fitted == 1){
                      Serial.print("Internal Temperature = ");
                      Serial.print(bmp.readTemperature());
                      Serial.println(" *C");
    
                      Serial.print("Air Pressure = ");
                      Serial.print(bmp.readPressure());
                      Serial.println(" Pa");
    
                      // Calculate altitude assuming 'standard' barometric
                      // pressure of 1013.25 millibar = 101325 Pascal
                      Serial.print("Calculated Altitude = ");
                      Serial.print(bmp.readAltitude());
                      Serial.println(" meters");
                    }*/
                    Serial.print("Packet count: ");
                    Serial.println(packet_count, DEC);
                    Serial.print("Spacing: ");
                    Serial.println(spacing, DEC);
                    Serial.print("Average spacing: ");
                    Serial.println(average_interval, DEC);
     
                    Serial.println("Packet Datastream: ");
                    Serial.println("ab cd ef gh ij kl mn op qr st");
                    for(i=0;i<=9;i++)
                      {
                        if ((int) packet[i]<16) Serial.print('0');
                        Serial.print(packet[i], HEX);
                        Serial.print(" ");
                      }
              
                    Serial.print("crc: ");
                    Serial.print(calculate_crc(), HEX);
                    Serial.println((valid_crc() ? " GOOD" : " BAD"));
  
                    Serial.print("Sensor ID: 0x");
                    Serial.println(get_sensor_id(), HEX);

                    Serial.print("Humidity: ");
                    Serial.print(get_humidity(), DEC);
                    Serial.println("%");
      
                    Serial.print("Average wind speed: ");
                    Serial.print(get_avg_wind(), DEC);
                    Serial.println("m/s");
      
                    Serial.print("Wind gust speed: ");
                    Serial.print(get_gust_wind(), DEC);
                    Serial.println("m/s");
      
                    Serial.print("Wind direction: ");
                    //get_wind_direction();
                    Serial.println(direction_name[get_wind_direction()]);
            
                    Serial.print("Rainfall: ");
                    Serial.print(get_rain(), DEC);
                    Serial.println("mm");
      
                    Serial.print("Temperature: ");
                    Serial.println(get_temperature_formatted());
                    //Serial.println("--------------");
                  }
            
                if(valid_crc())
                  {//unformatted string for LUA script decoding
                    //Serial.println("------------------------------");
                    Serial.print("WH1080:");
                    Serial.print(get_sensor_id());
                    Serial.print(":");
                    Serial.print(get_temperature_formatted());
                    Serial.print(":");
                    Serial.print(get_humidity(), DEC);
                    Serial.print(":");
                    Serial.print(get_avg_wind(), DEC);
                    Serial.print(":");
                    Serial.print(get_gust_wind(), DEC);
                    Serial.print(":");
                    Serial.print(direction_name[get_wind_direction()]);
                    //Serial.print(get_wind_direction(), DEC);//was HEX
                    Serial.print(":");
                    Serial.print(get_rain(), DEC);
                    Serial.print(":");
                    /*if(__bmp_fitted == 1){
                      Serial.print(bmp.readPressure());
                      Serial.print(":");
                      Serial.print(bmp.readTemperature());
                      Serial.print(":");
                    }*/
                    Serial.print(spacing, DEC);
                    Serial.println(":END");
                    //Serial.println("------------------------------");
                    Serial.println();
                  }
              }
            state = 0;//reset ready for next stream
          }
      }
  }

static void rf12_init_OOK() 
{
 /*   rf12_initialize(0, RF12_433MHZ);
    rf12_control(0x8017); // 8027    433 Mhz;disabel tx register; disable RX
    rf12_control(0x82c0); // 82C0    enable receiver; enable basebandblock 
    rf12_control(0xA620); // A68A    433.2500 MHz
    rf12_control(0xc691); // C691    c691 datarate 2395 kbps 0xc647 = 4.8kbps 
    rf12_control(0x9489); // 9489    VDI; FAST;200khz;GAIn -6db; DRSSI 97dbm 
    rf12_control(0xC220); // C220    datafiltercommand; ** not documented cmd 
    rf12_control(0xCA00); // CA00    FiFo and resetmode cmd; FIFO fill disabeld
    rf12_control(0xC473); // C473    AFC run only once; enable AFC; enable
                          //         frequency offset register; +3 -4
    rf12_control(0xCC67); // CC67    pll settings command
    rf12_control(0xB800); // TX register write command not used
    rf12_control(0xC800); // disable low dutycycle 
    rf12_control(0xC040); // 1.66MHz,2.2V not used see 82c0  
   */ return;
}

static void extractData(word interval)
  {
    /*  state 0 == reset state 
        state 1 == looking for preamble
        state 2 == got preamble, load rest of packet
        state 3 == packet complete
    */
    bool sample;
    byte preamble;
    byte preamble_check = B00011110;//check if we see the end of the preamble
    byte preamble_mask  = B00011111;//mask for preamble test (can be reduced if needed)
    static byte packet_no, bit_no, preamble_byte;
    if (interval >= 1350 && interval <= 1650)// 1 is indicated by 1500uS pulse
      {
        sample = 1;
      }
    else if (interval >= 2350  && interval <= 2750)// 0 is indicated by a 2500uS pulse
      {
        sample = 0;    
      }
    else
      {
        //Serial.println("Out of range");
        state = 0;//if interval not in range reset and start looking for preamble again
        return;
      } 
  
    if(state == 0)// reset
      {
        // reset stuff
        preamble_byte = 0; 
        packet[0] = packet[1] = packet[2] = packet[3] = packet[4] = packet[5] = packet[6] = packet[7] = packet[8] = packet[9] = 0;
        //Serial.println("Looking for preamble");
        state = 1;
      } 
  
      
    if (state == 1)// acquire preamble
      {
        preamble_byte <<= 1;// shift preamble byte left
        preamble_byte |= sample;//OR with new bit
        preamble = preamble_byte & preamble_mask;
        if (preamble == preamble_check)// we've acquired the preamble
          {
            packet_no = 0;
            bit_no = 2;// start at 2 because we know the first two bits are 10
            packet[0] = packet[1] = packet[2] = packet[3] = packet[4] = packet[5] = packet[6] = packet[7] = packet[8] = packet[9] = 0;
            packet[0] <<= 1;
            packet[0] |= 1;// first byte after preamble contains 10 so add 1
            packet[0] <<= 1;
            packet[0] |= 0;// first byte after preamble contains 10 so add 0 
            //Serial.println("Got the preamble");
            state = 2;
          }
        return; //not got the preamble yet so return
      }
  
    if (state == 2)// acquire packet
      {
        packet[packet_no] <<= 1;
        packet[packet_no] |= sample; //add bit to packet stream
      
        bit_no ++;
        if(bit_no > 7)
          {
            bit_no = 0;
            packet_no ++;
          }
    
        if (packet_no > 9)// got full packet stream
          {
            state = 3;
          }
      }
  }

byte calculate_crc()
  {
    return _crc8(packet, 9);
  }

bool valid_crc()
  {
    return (calculate_crc() == packet[9]);
  }

int get_sensor_id()
  {
    return (packet[0] << 4) + (packet[1] >> 4);
  }

byte get_humidity()
  {
    return packet[3];
  }

byte get_avg_wind()
  {
    return packet[4];
  }

byte get_gust_wind()
  {
    return packet[5];
  }

byte get_wind_direction()
  {
    int direction = packet[8] & 0x0f;
    return direction;
  }

byte get_rain()
  {
    return packet[7];
  }


/* Temperature in deci-degrees. e.g. 251 = 25.1 */
int get_temperature()
  {
    int temperature;
    temperature = ((packet[1] & B00000111) << 8) + packet[2];
    // make negative
    if (packet[1] & B00001000)
      {
        temperature = -temperature;
      }
    temperature -= 400;
    return temperature;
  }


String get_temperature_formatted()
  {
    int temperature;
    byte whole, partial;
    String s;
   
    temperature = ((packet[1] & B00000111) << 8) + packet[2];
    temperature -= 400;//temp offset by 400 points
    whole = temperature / 10;
    partial = temperature - (whole*10);
    s = String();
    if (packet[1] & B00001000)
      {
        s += String('-');
      }
    s += String(whole, DEC);
    s += '.';
    s += String(partial, DEC);
    return s;
  }


uint8_t _crc8( uint8_t *addr, uint8_t len)//nice crc snippet from OneWire library
  {
    uint8_t crc = 0;

    // Indicated changes are from reference CRC-8 function in OneWire library
    while (len--)
      {
        uint8_t inbyte = *addr++;
        for (uint8_t i = 8; i; i--)
          {
            uint8_t mix = (crc ^ inbyte) & 0x80; // changed from & 0x01
            crc <<= 1; // changed from right shift
            if (mix) crc ^= 0x31;// changed from 0x8C;
            inbyte <<= 1; // changed from right shift
          }
      }
    return crc;
  }










