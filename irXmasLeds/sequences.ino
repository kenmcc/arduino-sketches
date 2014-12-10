
void fillup()
{
  int lastpos = strip.numPixels() -1;
  int x, currentpos = 0;
  stripSet(0,0);
  for(x = 0; x < strip.numPixels(); x++)
  {
    for (int y = 0; y < lastpos; y++)
    {if (IRPT) return;
      strip.setPixelColor(y-1, 0,0,0);
      strip.setPixelColor(y, 30,30,0);
      strip.show();
//      delay(1);
    }
    lastpos -= 1;
  }
}
int shift = 0;
void alternate()
{
  stripSet(0,0);
  for(int x = 0; x < strip.numPixels(); x++)
  {
    if (IRPT) return;
    if (x+shift%2 == 0)
    {
      strip.setPixelColor(x, 100,0,0);
    }
    else
    {
      strip.setPixelColor(x, 0,100,0);
    }
      strip.show();
    
  }
  shift= shift+1;
  shift = shift % 2;
}


void fill()
{
  int pos = 0;
//  stripSet(0,0);
  for (pos = 0; pos < strip.numPixels(); pos++)
    { if (IRPT) return;
      strip.setPixelColor(pos, 30,30,80);
      strip.show();
      delay(10);
    }
    for (pos = 0; pos < strip.numPixels(); pos++)
    { if (IRPT) return;
      strip.setPixelColor(pos, 80,30,30);
      strip.show();
      delay(10);
    }
    for (pos = 0; pos < strip.numPixels(); pos++)
    { if (IRPT) return;
      strip.setPixelColor(pos, 30, 80,30);
      strip.show();
      delay(10);
    }
}

void topToBottom(){
  int start,finish,pos;
  start = 0; 
  finish = strip.numPixels();
  for (pos = 0; pos < strip.numPixels(); pos++)
  { if (IRPT) return;
    stripSet(0,0);
    strip.setPixelColor(pos, 255,0,0);
    strip.setPixelColor(pos+1, 0,255,0);
    strip.setPixelColor(pos+2, 0,0,255);
    
    
    strip.setPixelColor(finish-pos, 0,255,0);
    strip.setPixelColor(finish-pos-1, 255,0,0);
    strip.setPixelColor(finish-pos-2, 0,0,255);

    strip.show();
    delay(10);
  }
}

void rainbow()
{
  rainbow(10);
}

void rainbow(uint8_t wait) {
  int i, j;
   
  for (j=0; j < 256; j++) {     // 3 cycles of all 256 colors in the wheel
    for (i=0; i < strip.numPixels(); i++) {
      if (IRPT) return;
      strip.setPixelColor(i, Wheel( (i + j) % 256));
    }  
    strip.show();   // write all the pixels out
    delay(wait);
  }
}

int colorSweep = 0;
void chase(){
  int i,previous ;
  for (i = 0; i < strip.numPixels(); i++)
  {if (IRPT) return;
    previous = i -1;
    if (previous < 0)
    {
      previous = strip.numPixels()-1;
    }
    strip.setPixelColor(previous,Color(0,0,0));
    strip.setPixelColor(i,Wheel(colorSweep%255));
    strip.show();
    delay(1);
  }
  colorSweep += 10;

}

void slow(){
  stripSet(0,0);
  for (int i = 0; i < strip.numPixels(); i++)
  {
    stripSet(0,0);
    strip.setPixelColor(i, Color(255,0,0));
    delay(1000);
  }

}
  

// Slightly different, this one makes the rainbow wheel equally distributed 
// along the chain
void rainbowCycle(){
  rainbowCycle(20);
}
void rainbowCycle(uint8_t wait) {
  stripSet(0,0);

  int i, j;
  for (j=0; j < 256 * 1; j++) { // 5 cycles of all 25 colors in the wheel
    for (i=0; i < strip.numPixels(); i++) {
    // tricky math! we use each pixel as a fraction of the full 96-color wheel
    // (thats the i / strip.numPixels() part)
    // Then add in j which makes the colors go around per pixel
    // the % 96 is to make the wheel cycle around
       strip.setPixelColor(i, Wheel( ((i * 256 / strip.numPixels()) + j) % 256) );
    }
    strip.show(); // write all the pixels out
    delay(wait);
  }
}
// fill the dots one after the other with said color
// good for testing purposes
void colorWipe(uint32_t c, uint8_t wait) {
  int i;
  
  for (i=0; i < strip.numPixels(); i++) {
    if (IRPT) return;
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
}

void theaterChase()
{
  theaterChase(Color(0,255,0),100);
}
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        if (IRPT) return;
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();
      delay(wait);
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow()
{
  theaterChaseRainbow(100);
}

void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      
        for (int i=0; i < strip.numPixels(); i=i+3) {
          if (IRPT) return;
          strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
        }
        strip.show();
        delay(wait);
        for (int i=0; i < strip.numPixels(); i=i+3) {
          if (IRPT) return;
          strip.setPixelColor(i+q, 0);        //turn every third pixel off
        }
    }
  }
}
// twinkle random number of pixels
void twinkleRand()
{
  twinkleRand(random(strip.numPixels()),Color(random(255),random(255),random(255)),Color(0, 0, 0),100);
}
void twinkleRand(int num,uint32_t c,uint32_t bg,int wait) {
	// set background
	 stripSet(bg,0);
	 // for each num
	 for (int i=0; i<num; i++) {
  if (IRPT) return;
	   strip.setPixelColor(random(strip.numPixels()),c);
	 }
	strip.show();
	delay(wait);
}

// twinkle random number of pixels
void twinkleRandRed()
{
  twinkleRandColor(random(strip.numPixels()),Color(255,0,0),Color(0, 0, 0),100);
}
void twinkleRandGreen()
{
  twinkleRandColor(random(strip.numPixels()),Color(0,255,0),Color(0, 0, 0),100);
}
void twinkleRandBlue()
{
  twinkleRandColor(random(strip.numPixels()),Color(0,0,255),Color(0, 0, 0),100);
}
void twinkleRandMagenta()
{
  twinkleRandColor(random(strip.numPixels()),Color(255,255,0),Color(0, 0, 0),100);
}
void twinkleRandYellow()
{
  twinkleRandColor(random(strip.numPixels()),Color(255,0,255),Color(0, 0, 0),100);
}
void twinkleRandColor(int maxnum,uint32_t c,uint32_t bg,int wait) {
	// set background
	 stripSet(bg,0);
	 // for each num
         int num = random(maxnum) + 1;
	 for (int i=0; i<num; i++) {
           if (IRPT)return;
           uint32_t col = c;
	   strip.setPixelColor(random(strip.numPixels()),col);
	 }
	strip.show();
	delay(wait);
}

void simpleWaveRed()
{
  simpleWave(0.1,5,10,1,0,0);
}
void simpleWaveGreen()
{
  simpleWave(0.1,5,10,0,1,0);
}
void simpleWaveBlue()
{
  simpleWave(0.1,5,10,0,0,1);
}
void simpleWaveYellow()
{
  simpleWave(0.1,5,10,1,1,0);
}
void simpleWaveMagenta()
{
  simpleWave(0.1,5,10,1,0,1);
}
void simpleWaveCyan()
{
  simpleWave(0.1,5,10,0,1,1);
}
void simpleWave(float rate,int cycles, int wait,int r, int g, int b) {
   float pos=0.0;
  // cycle through x times
  for(int x=0; x<(strip.numPixels()*cycles); x++)
  {
      pos = pos+rate;
      for(int i=0; i<strip.numPixels(); i++) {
           if (IRPT)return;
        // sine wave, 3 offset waves make a rainbow!
        float level = (sin(i+pos) * 127) + 128;
        // set color level 
        strip.setPixelColor(i,(int)level*r, (int)level*g, (int)level*b);
      }
         strip.show();
         delay(wait);
  }
}  


/**************************************************************/
/* Helper functions */

void stripSet(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
  }
  // move the show outside the loop
  strip.show();
  delay(wait);
}

// Create a 24 bit color value from R,G,B
uint32_t Color(byte r, byte g, byte b)
{
  uint32_t c;
  c = r;
  c <<= 8;
  c |= g;
  c <<= 8;
  c |= b;
  return c;
}

//Input a value 0 to 255 to get a color value.
//The colours are a transition r - g -b - back to r
uint32_t Wheel(byte WheelPos)
{
  if (WheelPos < 85) {
   return Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if (WheelPos < 170) {
   WheelPos -= 85;
   return Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170; 
   return Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}
