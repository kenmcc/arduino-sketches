#include <Adafruit_NeoPixel.h>



#define LEFT 6
#define RIGHT 7


#define PIN            4

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS     8

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);


void setup() {
  // put your setup code here, to run once:

  pinMode(LEFT,INPUT_PULLUP);
  pinMode(RIGHT,INPUT_PULLUP);

 // digitalWrite(LEFT, 0);
 // digitalWrite(RIGHT, 0);
  
  //Serial.begin(57600);
pixels.begin(); // This initializes the NeoPixel library.
}

int mode = 2;

int blinkState = 0;
void doMode()
{
  int lefts[][8] = {{0,0,0,0,0,0,0,0},{1,0,1,0,0,0,0,0}};
  int rights[][8] = {{0,0,0,0,0,0,0,0},{0,0,0,0,1,0,1,0}};
  switch(mode)
  {
    case -1:
       if (blinkState == 0)
        {
          //Serial.println("left");
          blinkState = 1;
        }
        else
        {
          //Serial.println("LEFT");
           blinkState = 0;
        }
        for(int i=0;i<NUMPIXELS;i++){

    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    if (lefts[blinkState][i] == 1)
      pixels.setPixelColor(i, pixels.Color(250,150,0)); // Moderately bright green color.
    else
      pixels.setPixelColor(i, pixels.Color(0,0,0)); // Moderately bright green color.
    }

    
    
        break;
        
        case 1:
       if (blinkState == 0)
        {
         // Serial.println("right");
                    blinkState = 1;
        }
        else
        {
          //Serial.println("RIGHT");
                    blinkState = 0;
        }
        for(int i=0;i<NUMPIXELS;i++){
          
        if (rights[blinkState][i] == 1)
          pixels.setPixelColor(i, pixels.Color(250,150,0)); // Moderately bright green color.
        else
           pixels.setPixelColor(i, pixels.Color(0,0,0)); // Moderately bright green color.
        }
        break;
      default:
         blinkState = 0;  
         for(int i=0;i<NUMPIXELS;i++){
          
          pixels.setPixelColor(i, pixels.Color(0,0,0)); // Moderately bright green color.
        }
  }
  pixels.show(); // This sends the updated pixel color to the hardware.

}

void loop() {
  // put your main code here, to run repeatedly:
  int left = digitalRead(LEFT);
  int right = digitalRead(RIGHT);
  
 if (left == LOW)
 {
   if (mode != -1)
   {
     mode = -1;
   }
 }
else if (right == LOW)
 {
   if (mode != 1)
   {
     mode = 1;
   }
 }
 
 else
 {
   if (mode != 0)
   {
     mode = 0;
//     Serial.println("OFF");
   }
 }
doMode();
delay(500);


}
