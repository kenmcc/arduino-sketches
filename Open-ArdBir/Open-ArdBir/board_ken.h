// sensor and lcd
OneWire ds(7);
                               //    RS, E, 4,5,6,7  
LiquidCrystal lcd(12,10, 5,4,3,2);

// push buttons
const char Button_dn    = A3;
const char Button_up    = A2;
const char Button_enter = A1;
const char Button_start = A0;

// outputs
const byte Pump = 6;
const byte Buzz = 8;
const byte Heat = 9;

