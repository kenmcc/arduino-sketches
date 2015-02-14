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

#include <JeeLib.h>
//#include <Wire.h>
//#include <Adafruit_BMP085.h>

#if defined(__AVR_ATtiny84__)
#define PIN_433 10
#else
#define PIN_433 2  // AIO1 = 433 MHz receiver
#endif
volatile int pulse_433 = 0;
volatile word pulse_length = 0;
volatile unsigned long old = 0, packet_count = 0; 
volatile unsigned long spacing, average_interval;
word last_433 = 0; // never accessed outside ISR's
volatile word pulse_count = 0; //
byte packet[10];
byte state = 0;
bool packet_acquired = false;
char *direction_name[] = {"N", "NNE", "NE", "ENE", "E", "ESE", "SE", "SSE", "S", "SSW", "SW", "WSW", "W", "WNW", "NW", "NNW"};
int __debug = 1; // set to enable debug mode

 typedef struct {
  	  int temp;	// Temperature reading
  	  int batt;
          int humidity;	// Supply voltage
          int wind_avg;
          int wind_gust;
          int wind_dir;
          int rain;
          
 } Payload;

 Payload tinytx;
 
 
 static void rfwrite(){
  #ifdef USE_ACK
   for (byte i = 0; i <= RETRY_LIMIT; ++i) {  // tx and wait for ack up to RETRY_LIMIT times
     rf12_sleep(-1);              // Wake up RF module
      while (!rf12_canSend())
      rf12_recvDone();
      rf12_sendStart(RF12_HDR_ACK, &tinytx, sizeof tinytx); 
      rf12_sendWait(2);           // Wait for RF to finish sending while in standby mode
      byte acked = waitForAck();  // Wait for ACK
      rf12_sleep(0);              // Put RF module to sleep
      if (acked) { return; }      // Return if ACK received
  
   Sleepy::loseSomeTime(RETRY_PERIOD * 1000);     // If no ack received wait and try again
   }
  #else
     rf12_sleep(-1);              // Wake up RF module
     while (!rf12_canSend())
     rf12_recvDone();
     Serial.write("sendStart\n");
     rf12_sendStart(0, &tinytx, sizeof tinytx); 
     rf12_sendWait(2);           // Wait for RF to finish sending while in standby mode
     rf12_sleep(0);              // Put RF module to sleep
     return;
  #endif
 }

 long readVcc() {
   bitClear(PRR, PRADC); ADCSRA |= bit(ADEN); // Enable the ADC
   long result;
   // Read 1.1V reference against Vcc
   #if defined(__AVR_ATtiny84__) 
    ADMUX = _BV(MUX5) | _BV(MUX0); // For ATtiny84
   #else
    ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);  // For ATmega328
   #endif 
   delay(2); // Wait for Vref to settle
   ADCSRA |= _BV(ADSC); // Convert
   while (bit_is_set(ADCSRA,ADSC));
   result = ADCL;
   result |= ADCH<<8;
   result = 1126400L / result; // Back-calculate Vcc in mV
   ADCSRA &= ~ bit(ADEN); bitSet(PRR, PRADC); // Disable the ADC to save power
   return result;
} 

#if 1
// State to track pulse durations measured in the interrupt code
ISR(PCINT0_vect)
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
#endif

#define myNodeID 8        // RF12 node ID in the range 1-30
#define network 99       // RF12 Network group
#define freq RF12_433MHZ  // Frequency of RFM12B module

void setup()
  {
    Serial.begin(115200);
    Serial.println("WH1080");
  
    pinMode(PIN_433, INPUT);
    digitalWrite(PIN_433, 1);   // pull-up
    
  rf12_initialize(myNodeID,freq,network); // Initialize RFM12 with settings defined above 
  rf12_control(0xC623);
  rf12_sleep(0);                          // Put the RFM12 to sleep

//    Serial.println("Just before rf12_init_OOK()");
    //rf12_init_OOK();//set up RF12
//    Serial.println("RFM12B set up for 433MHz OOK");
    if (__debug) {Serial.println("Debug mode");}
    // interrupt on pin change
cli();
#if defined(__AVR_ATtiny84__)

  PCMSK0 = 0b00000001;     // PCINT1 and PCINT2  (pin 12 and 11)
  GIFR   = 0b00000000;     // clear any outstanding interrupts
  GIMSK  = 0b00010000;     // enable pin change interrupts PCIE0=1
  pinMode(8, OUTPUT);
  digitalWrite(8, 1);

#else
    bitSet(PCMSK2, PIN_433);
    bitSet(PCICR, PCIE2);
#endif    
    sei();
    Serial.println("Interrupts set up");
  }

void loop()
  {
    byte i;
    unsigned long now;
  
    if (pulse_433)
      {
        cli();//disable interrupts and store pulse width
        word pulse = pulse_433;
        pulse_433 = 0;
        sei();//re-enable interrupts
        extractData(pulse);//extract data from pulse stream
#if 1
      
        if (state == 3)//pulse stream complete
          {
            state = 4;
            now = millis();
            spacing = ((now - old)/1000);
            old = now;
            if (spacing >= 10) // was 500
              { 
                packet_count ++;
                average_interval = now / packet_count;  

                    Serial.println("Packet Datastream: ");
                    Serial.println("ab cd ef gh ij kl mn op qr st");
                    for(i=0;i<=9;i++)
                      {
                        if ((int) packet[i]<16) Serial.print('0');
                        Serial.print(packet[i], HEX);
                        Serial.print(" ");
                      }
                      Serial.println("");
  
                if(valid_crc())
                  {//unformatted string for LUA script decoding
                    Serial.println("ok");
//                    Serial.print(get_temperature_formatted());
//                    Serial.print(":");
//                    Serial.print(get_humidity(), DEC);
//                    Serial.print(":");
//                    Serial.print(get_avg_wind(), DEC);
//                    Serial.print(":");
//                    Serial.print(get_gust_wind(), DEC);
//                    Serial.print(":");
//                    Serial.print(direction_name[get_wind_direction()]);
//                    Serial.print(":");
//                    Serial.print(get_rain(), DEC);
//                    Serial.println();
                  }
              }
            state = 0;//reset ready for next stream
          }
      #endif
      }
  }

static void rf12_init_OOK() 
{
    rf12_initialize(0, RF12_433MHZ);
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
    return;
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










