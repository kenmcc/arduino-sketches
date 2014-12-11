/* Raw IR commander
 
 This sketch/program uses the Arduno and a PNA4602 to 
 decode IR received.  It then attempts to match it to a previously
 recorded IR signal
 
 Code is public domain, check out www.ladyada.net and adafruit.com
 for more tutorials! 
 */
#include <PinChangeInt.h>
#include <PinChangeIntConfig.h>
#include "Adafruit_WS2801.h"
#include "SPI.h" // Comment out this line if using Trinket or Gemma
// We need to use the 'raw' pin reading methods
// because timing is very important here and the digitalRead()
// procedure is slower!
//uint8_t IRpin = 2;
// Digital pin #2 is the same as Pin D2 see
// http://arduino.cc/en/Hacking/PinMapping168 for the 'raw' pin mapping
#define IRpin_PIN      PIND
#define IRpin          4

uint8_t dataPin  = 2;    // Yellow wire on Adafruit Pixels
uint8_t clockPin = 3;    // Green wire on Adafruit Pixels

// the maximum pulse we'll listen for - 65 milliseconds is a long time
#define MAXPULSE 65000
#define NUMPULSES 50

// what our timing resolution should be, larger is better
// as its more 'precise' - but too large and you wont get
// accurate timing
#define RESOLUTION 20 

// What percent we will allow in variation to match the same code
#define FUZZINESS 20

// we will store up to 100 pulse pairs (this is -a lot-)
uint16_t pulses[NUMPULSES][2];  // pair is high and low pulse 
uint8_t currentpulse = 0; // index for pulses we're storing

typedef void (*voidFuncPtr)(void);

Adafruit_WS2801 strip = Adafruit_WS2801(50, dataPin, clockPin);

#include "ircodes.h"

long funcNum;
voidFuncPtr functions[] = {/*fillup, fill, chase, theaterChase, theaterChaseRainbow, */
                          rainbowCycle,
                          //slow,
                          simpleWaveRed,
                          simpleWaveGreen,
                          simpleWaveBlue,
                          simpleWaveYellow,
                          simpleWaveMagenta,
                          twinkleRand,
                          twinkleRandRed,
                          twinkleRandGreen,
                          twinkleRandBlue,
                          twinkleRandYellow,
                          twinkleRandMagenta,
                          theaterChaseRainbow,
                        };

void setup(void) {
  Serial.begin(9600);
  Serial.println("Ready to decode IR!");
  
  pinMode(4, INPUT);     //set the pin to input
  digitalWrite(4, LOW); //use the internal pullup resistor
  PCintPort::attachInterrupt(4, burpcount,RISING); // attach a PinChange Interrupt to our pin on the rising edge

  pinMode(13, OUTPUT);  
  
  strip.begin();
  // Update LED contents, to start they are all 'off'
  strip.show();
  randomSeed(analogRead(4));
  funcNum = random(sizeof(functions)/sizeof(functions[0]));

}
int numberpulses = 0;
boolean IRPT = false;
void burpcount()
{
  //Serial.println("burp");
  numberpulses = listenForIR();
  IRPT = true;
}

voidFuncPtr runningFunc = functions[funcNum];
boolean running = true;
void loop()
{
  
  if (numberpulses > 0)
  {
    stripSet(0,0);
    if (IRcompare(numberpulses, STANDBY,sizeof(STANDBY)/4)) {
      if (running )
      {
        Serial.println("OFF");
        running = false;
        stripSet(0,0);
      }
      else
      {
        Serial.println("ON");
        running = true;
      }
      
    }
    
    else if(IRcompare(numberpulses, HOME,sizeof(HOME)/4)) {
      runningFunc = simpleWaveRed;
    }
    else if(IRcompare(numberpulses, STORAGE,sizeof(STORAGE)/4)) {
      runningFunc = twinkleRandRed;
    }
    else if(IRcompare(numberpulses, MUTE,sizeof(MUTE)/4)) {
      runningFunc = twinkleRand;
    }
    else if(IRcompare(numberpulses, PLAY,sizeof(PLAY)/4)) {
      runningFunc = theaterChaseRainbow;
    }
    else if(IRcompare(numberpulses, REWIND,sizeof(REWIND)/4)) {
      //runningFunc = theaterChase;
      funcNum -=1;
      if (funcNum < 0)
        funcNum = sizeof(functions)/sizeof(functions[0]) -1;
      runningFunc = functions[funcNum];
    }
    else if(IRcompare(numberpulses, FORWARD,sizeof(FORWARD)/4)) {
      //runningFunc = theaterChase;
      funcNum += 1;
      if (funcNum >= sizeof(functions)/sizeof(functions[0]))
        funcNum = 0;
      runningFunc = functions[funcNum];
    }

    else{
      Serial.println("int IRsignal[] = {");
      Serial.println("// ON, OFF (in 10's of microseconds)");
      for (uint8_t i = 0; i < numberpulses-1; i++) {
        Serial.print("\t"); // tab
        Serial.print(pulses[i][1] * RESOLUTION / 10, DEC);
        Serial.print(", ");
        Serial.print(pulses[i+1][0] * RESOLUTION / 10, DEC);
        Serial.println(",");
      }
      Serial.print("\t"); // tab
      Serial.print(pulses[currentpulse-1][1] * RESOLUTION / 10, DEC);
      Serial.print(", 0};");
    }
    numberpulses = 0;
    IRPT = false;
  }
  if(running)
  {
    runningFunc();
  }
  delay(1);
}



//KGO: added size of compare sample. Only compare the minimum of the two
boolean IRcompare(int numpulses, int Signal[], int refsize) {
  int count = min(numpulses,refsize);
  for (int i=0; i< count-1; i++) {
    int oncode = pulses[i][1] * RESOLUTION / 10;
    int offcode = pulses[i+1][0] * RESOLUTION / 10;
    
#ifdef DEBUG    
    Serial.print(oncode); // the ON signal we heard
    Serial.print(" - ");
    Serial.print(Signal[i*2 + 0]); // the ON signal we want 
#endif   
    
    // check to make sure the error is less than FUZZINESS percent
    if ( abs(oncode - Signal[i*2 + 0]) <= (Signal[i*2 + 0] * FUZZINESS / 100)) {
#ifdef DEBUG
      Serial.print(" (ok)");
#endif
    } else {
#ifdef DEBUG
      Serial.print(" (x)");
#endif
      // we didn't match perfectly, return a false match
      return false;
    }
    
    
#ifdef DEBUG
    Serial.print("  \t"); // tab
    Serial.print(offcode); // the OFF signal we heard
    Serial.print(" - ");
    Serial.print(Signal[i*2 + 1]); // the OFF signal we want 
#endif    
    
    if ( abs(offcode - Signal[i*2 + 1]) <= (Signal[i*2 + 1] * FUZZINESS / 100)) {
#ifdef DEBUG
      Serial.print(" (ok)");
#endif
    } else {
#ifdef DEBUG
      Serial.print(" (x)");
#endif
      // we didn't match perfectly, return a false match
      return false;
    }
    
#ifdef DEBUG
    Serial.println();
#endif
  }
  // Everything matched!
  return true;
}

int listenForIR(void) {
  currentpulse = 0;
  
  while (1) {
    uint16_t highpulse, lowpulse;  // temporary storage timing
    highpulse = lowpulse = 0; // start out with no pulse length
  
    while (IRpin_PIN & (1 << IRpin)) {
       // pin is still HIGH

       // count off another few microseconds
       highpulse++;
       delayMicroseconds(RESOLUTION);

       // If the pulse is too long, we 'timed out' - either nothing
       // was received or the code is finished, so print what
       // we've grabbed so far, and then reset
       
       // KGO: Added check for end of receive buffer
       if (((highpulse >= MAXPULSE) && (currentpulse != 0))|| currentpulse == NUMPULSES) {
         return currentpulse;
       }
    }
    // we didn't time out so lets stash the reading
    pulses[currentpulse][0] = highpulse;
  
    // same as above
    while (! (IRpin_PIN & _BV(IRpin))) {
       // pin is still LOW
       lowpulse++;
       delayMicroseconds(RESOLUTION);
        // KGO: Added check for end of receive buffer
        if (((lowpulse >= MAXPULSE)  && (currentpulse != 0))|| currentpulse == NUMPULSES) {
         return currentpulse;
       }
    }
    pulses[currentpulse][1] = lowpulse;

    // we read one high-low pulse successfully, continue!
    currentpulse++;
  }
}
void printpulses(void) {
  Serial.println("\n\r\n\rReceived: \n\rOFF \tON");
  for (uint8_t i = 0; i < currentpulse; i++) {
    Serial.print(pulses[i][0] * RESOLUTION, DEC);
    Serial.print(" usec, ");
    Serial.print(pulses[i][1] * RESOLUTION, DEC);
    Serial.println(" usec");
  }
  
  // print it in a 'array' format
  Serial.println("int IRsignal[] = {");
  Serial.println("// ON, OFF (in 10's of microseconds)");
  for (uint8_t i = 0; i < currentpulse-1; i++) {
    Serial.print("\t"); // tab
    Serial.print(pulses[i][1] * RESOLUTION / 10, DEC);
    Serial.print(", ");
    Serial.print(pulses[i+1][0] * RESOLUTION / 10, DEC);
    Serial.println(",");
  }
  Serial.print("\t"); // tab
  Serial.print(pulses[currentpulse-1][1] * RESOLUTION / 10, DEC);
  Serial.print(", 0};");
}


