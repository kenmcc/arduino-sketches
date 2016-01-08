#include <OneWire.h>
#include <DallasTemperature.h>
#include "DHT.h"
// include the library code:
#include <LiquidCrystal.h>



// Data wire is plugged into port 2 on the Arduino
#define DHT11PIN      14   // A0
#define DS18B20PIN 15   // A1. This is then tied high to +ve with a 4.7k Resistor
#define RELAYPIN      16   // A2
 
// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(DS18B20PIN);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

// arrays to hold device address
DeviceAddress insideThermometer;


// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(2,4, 9,10,11,12);

DHT dht;

byte Up[8] = {
  0b00100,
  0b01110,
  0b10101,
  0b00100,
  0b00100,
  0b00100,
  0b00100,
  0b01010
};

byte Down[8] = {
  0b01010,
  0b00100,
  0b00100,
  0b00100,
  0b00100,
  0b10101,
  0b01110,
  0b00100,
};
byte NOChange[8] = {
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000
};

byte relayOff[8] = {
  0b11111,
  0b10001,
  0b10001,
  0b10001,
  0b10001,
  0b10001,
  0b10001,
  0b11111
};

byte relayOn[8] = {
  0b11111,
  0b10001,
  0b10101,
  0b10101,
  0b10101,
  0b10101,
  0b10001,
  0b11111
};


void setup(void)
{
  // start serial port
  Serial.begin(9600);
  Serial.println("Dallas Temperature IC Control Library Demo");

  // locate devices on the bus
  Serial.print("Locating devices...");
  sensors.begin();
  Serial.print("Found ");
  Serial.print(sensors.getDeviceCount(), DEC);
  Serial.println(" devices.");

  if (!sensors.getAddress(insideThermometer, 0)) Serial.println("Unable to find address for Device 0"); 
  
  // show the addresses we found on the bus
  Serial.print("Device 0 Address: ");
  printAddress(insideThermometer);
  Serial.println();

  // set the resolution to 9 bit (Each Dallas/Maxim device is capable of several different resolutions)
  sensors.setResolution(insideThermometer, 9);
 
  Serial.print("Device 0 Resolution: ");
  Serial.print(sensors.getResolution(insideThermometer), DEC); 
  Serial.println();
  
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.setCursor(0, 0);
  lcd.print("Now:");
  
  lcd.createChar(0, NOChange);
  lcd.createChar(1, Up);
  lcd.createChar(2, Down);
  
  lcd.createChar(3, relayOn);
  lcd.createChar(4, relayOff);
  dht.setup(DHT11PIN); // data pin A0
  
  pinMode(RELAYPIN, OUTPUT);
  digitalWrite(RELAYPIN, LOW);
  
}

float maxT=-100.0;
float minT = 100.0;
float lastT = 0;
// function to print the temperature for a device
float getTemperature(DeviceAddress deviceAddress)
{
  float tempC = sensors.getTempC(deviceAddress);
  if (tempC < -100.0)return lastT;
  if (tempC > 100.0) return lastT;
  return tempC;
  
}

static int loopmode = 0;
void loop(void)
{ 
  // call sensors.requestTemperatures() to issue a global temperature 
  // request to all devices on the bus
  sensors.requestTemperatures(); // Send the command to get temperatures
  
  // It responds almost immediately. Let's print out the data
  int arrowSymbol = 0;
  int relaySymbol = 4;
  boolean relayOn = false;
  float thisT = getTemperature(insideThermometer); // Use a simple function to print out the data
  
  delay(dht.getMinimumSamplingPeriod());

  float garageTmp = dht.getTemperature();
  float humidity = dht.getHumidity();
  
  if (thisT > lastT) arrowSymbol = 1; // rising temp
  else if (thisT < lastT) arrowSymbol = 2; // falling temp
  lastT = thisT;
  
  if (thisT >= -6.0)  // dunno if -5 is right, but it's a start 
  {
    relayOn = true;
    relaySymbol = 3;
    digitalWrite(RELAYPIN, HIGH);
  }
  else if(thisT < -8.0)
  {
    relaySymbol = 4;
    digitalWrite(RELAYPIN, LOW);
  }
  
  if (lastT > maxT) maxT = lastT;
  if (lastT < minT) minT = lastT;
  
  Serial.print("Temp C: ");
  Serial.print(lastT);
  Serial.print("  Max C: ");
  Serial.print(maxT);
  Serial.print("  Min C: ");
  Serial.println(minT);
  
  lcd.setCursor(5, 0);
  lcd.print(String(lastT));
  lcd.setCursor(15,0);
  lcd.write(arrowSymbol);
  
  lcd.setCursor(0,1);
  if (loopmode == 0)
  {
      lcd.print("Min:");
      lcd.setCursor(5, 1);
      lcd.print(String(minT));
      loopmode = 1;
  }
  else if (loopmode== 1)
  {
      lcd.print("Max:");
      lcd.setCursor(5, 1);
      lcd.print(String(maxT));
      loopmode = 2;
  }
  else if(loopmode == 2)
  {
    lcd.print("Amb:");
      lcd.setCursor(5, 1);
      lcd.print(String(garageTmp));
      loopmode = 3;
  }
  else if(loopmode == 3)
  {
    lcd.print("Hum:");
      lcd.setCursor(5, 1);
      lcd.print(String(humidity));
      loopmode = 0;
  }

  /* and show the relay symbol */
    lcd.setCursor(15,1);
    lcd.write(relaySymbol);
  
  delay(2000);
}

// function to print a device address
void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}
