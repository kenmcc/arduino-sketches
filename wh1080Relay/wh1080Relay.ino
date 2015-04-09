/*
this code uses an arduino nano to listen to transmissions from the WH1080 field unit
*/

/*
PINS:
RF433 mod: Data to Pin 4 on arduino
RFM12mod: 
  13 SCK 
  12 SDO
  11 SDI
  10 SEL
   2  IRQ
*/

#include <JeeLib.h>
#include <Wire.h>

#define PIN_433 4  // AIO1 = 433 MHz receiver
#define myNodeID 2        // RF12 node ID in the range 1-30
#define network 99       // RF12 Network group

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
int __debug = 0; // set to enable debug mode
int __bmp_fitted = 0;  // set to enable BMP085

// State to track pulse durations measured in the interrupt code
//########################################################################################################################
//Data Structure to be sent
//########################################################################################################################

#define myNodeID 9       // RF12 node ID in the range 1-30
#define network 99       // RF12 Network group
#define freq RF12_433MHZ  // Frequency of RFM12B module


 typedef struct {
  	  long int tempX10;	// Temperature reading
  	  int supplyV;	// Supply voltage
          int humidity;
          int windAvg;
          int windGust;
          int windDir;
          int rain;
 } Payload;

Payload tinytx={0,1,0,0,0,0,0}
;
 
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
static void rfwrite(){
     rf12_sleep(-1);              // Wake up RF module
  
     while (!rf12_canSend())
     rf12_recvDone();
     rf12_sendStart(0, &tinytx, sizeof tinytx); 
     rf12_sendWait(2);           // Wait for RF to finish sending while in standby mode
     rf12_sleep(0);              // Put RF module to sleep
     return;
 }


void setup()
  {
    Serial.begin(57600);
    Serial.println();
    Serial.println("WH1080 Serial node with BMP085");
  
    pinMode(PIN_433, INPUT);
    digitalWrite(PIN_433, 1);   // pull-up

   rf12_initialize(myNodeID,freq,network); // Initialize RFM12 with settings defined above 
   rf12_control(0xC623);
   rf12_sleep(0);                          // Put the RFM12 to sleep

    if (__debug) {Serial.println("Debug mode");}
    // interrupt on pin change
    bitSet(PCMSK2, PIN_433);
    bitSet(PCICR, PCIE2);
    // enable interrupts
    sei();
    //Serial.println("Interrupts set up");
    
  }

void loop()
  {
    byte i;
    unsigned long now;
    //byte *packet;
  
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
                    Serial.println("WH1080:IDID:TT.T:H_:A:G:D:R:S");
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
                   
                    
                    Serial.print(spacing, DEC);
                    Serial.println(":END");
                    //Serial.println("------------------------------");
                    Serial.println();
            
                  
                    delay(1000);
                    Serial.println("Sent on RF PACK");
                    delay(1000);
                    rfwrite();
                    
                    
                    
                    
                   
                  }
              }
            state = 0;//reset ready for next stream
          }
      }
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
    byte data = packet[3];
    tinytx.humidity = (int)data;
    return data;
  }

byte get_avg_wind()
  {
    byte data = packet[4];
    tinytx.windAvg = (int)data;
    return data;
  }

byte get_gust_wind()
  {
    byte data = packet[5];
    tinytx.windGust = (int)data;
    return data;
  }

byte get_wind_direction()
  {
    int direction = packet[8] & 0x0f;
    tinytx.windDir = direction;
    return direction;
  }

byte get_rain()
  {
    byte data = packet[7];
    tinytx.rain = (int)data;
    return data;
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
    tinytx.tempX10 = (long int)temperature*10;
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





