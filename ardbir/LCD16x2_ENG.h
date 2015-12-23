#include <avr/pgmspace.h>

#define SETBOIL "Set Boil   "
#define PumpStopAt "Pump Stop at "
#define PUMP "Pump"
#define CONSTANTk "Constant k"


//char *PIDName[]   ={"Use ", "Constant kP", "Constant kI", "Constant kD", "SampleTime ", "WindowSet  ", "Use in Boil", "Calibration", "Hysteresi  "};
//char *stageName[] ={"Mash In   ", "Phytase   ", "Glucanase ", "Protease  ", "\xE2""Amylase  ", "\xE0""Amylase1 ", "\xE0""Amylase2 ", "Mash Out  ", "Boil      ", "Cooling   ", "Whirlpool "};
//char *unitName[]  ={"Set Degrees  ", "Sensor   ", "Set Boil   ", "Set Boil   ", "Pump Cycle   ", "Time PumpRest ", "Pmp PreMash",  "Pmp on Mash", "Pmp MashOut", "Pump on Boil ",  "Pump Stop at " ,  "Pump Stop at " , "PID Pipe ", "Skip Add      ", "Skip Remove ", "Skip Iodine ", "Iodine  ", "Whirlpool"};


const PROGMEM char Mash_In___s[]          = "Mash In   ";
const PROGMEM char Phytase___s[]          = "Phytase   ";
const PROGMEM char Glucanase_s[]          = "Glucanase ";
const PROGMEM char Protease__s[]          = "Protease  ";
const PROGMEM char Amylase__s[]           = "\xE2""Amylase  ";
const PROGMEM char Amylase1_s[]           = "\xE0""Amylase1 ";
const PROGMEM char Amylase2_s[]           = "\xE0""Amylase2 ";
const PROGMEM char Mash_Out__s[]          = "Mash Out  ";
const PROGMEM char Boil______s[]          = "Boil      ";
const PROGMEM char Cooling___s[]          = "Cooling   ";
const PROGMEM char Whirlpool_s[]          = "Whirlpool ";

const char Set_Degrees_s[] PROGMEM = "Set Degrees  ";
const char Sensor_s[] PROGMEM = "Sensor   ";
const char Set_Boil_s[] PROGMEM = "Set Boil   ";
const char Pump_Cycle_s[] PROGMEM = "Pump Cycle   ";
const char Time_PumpRest_s[] PROGMEM = "Time PumpRest ";
const char Pmp_PreMash_s[] PROGMEM = "Pmp PreMash";
const char Pmp_on_Mash_s[] PROGMEM = "Pmp on Mash";
const char Pmp_MashOut_s[] PROGMEM = "Pmp MashOut";
const char Pump_on_Boil_s[] PROGMEM = "Pump on Boil ";
const char Pump_Stop_at_s[] PROGMEM = "Pump Stop at ";
const char PID_Pipe_s[] PROGMEM = "PID Pipe ";
const char Skip_Add_s[] PROGMEM = "Skip Add      ";
const char Skip_Remove_s[] PROGMEM = "Skip Remove ";
const char Skip_Iodine_s[] PROGMEM = "Skip Iodine ";
const char Iodine_s[] PROGMEM = "Iodine  ";

const char Use_s[] PROGMEM = "Use ";
const char Constant_kP_s[] PROGMEM = "Constant kP";
const char Constant_kI_s[] PROGMEM = "Constant kI";
const char Constant_kD_s[] PROGMEM = "Constant kD";
const char SampleTime_s[] PROGMEM = "SampleTime ";
const char WindowSet_s[] PROGMEM = "WindowSet  ";
const char Use_in_Boil_s[] PROGMEM = "Use in Boil";
const char Calibration_s[] PROGMEM = "Calibration";
const char Hysteresi_s[] PROGMEM = "Hysteresi  ";


const char* const PIDName[] PROGMEM = {Use_s,Constant_kP_s,Constant_kI_s,Constant_kD_s,SampleTime_s,WindowSet_s,Use_in_Boil_s,Calibration_s,Hysteresi_s};

const char* const stageName[] PROGMEM = {Mash_In___s,Phytase___s,Glucanase_s,Protease__s,Amylase__s,Amylase1_s,Amylase2_s,Mash_Out__s,Boil______s,Cooling___s,Whirlpool_s};
const char* const unitName[] PROGMEM = {Set_Degrees_s,Sensor_s,Set_Boil_s,Set_Boil_s,Pump_Cycle_s,Time_PumpRest_s,Pmp_PreMash_s,Pmp_on_Mash_s,Pmp_MashOut_s,Pump_on_Boil_s,Pump_Stop_at_s,Pump_Stop_at_s,PID_Pipe_s,Skip_Add_s,Skip_Remove_s,Skip_Iodine_s,Iodine_s,Whirlpool_s};

byte HeatONOFF[8]    = {B00000, B01010, B01010, B01110, B01110, B01010, B01010, B00000};  // [5] HEAT symbol
byte RevHeatONOFF[8] = {B11111, B10101, B10101, B10001, B10001, B10101, B10101, B11111};  // [6] reverse HEAT symbol
byte Language[8]     = {B11111, B00010, B01000, B11111, B00000, B10001, B10101, B11111};  // [7] EN symbol

char buffer[30];  

void printMem(const char* const* array, int entry)
{
  strcpy_P(buffer, (char*)pgm_read_word(&(array[entry]))); // Necessary casts and dereferencing, just copy.
    
  lcd.print(buffer);
}

void showString (PGM_P s) {
        char c;
        while ((c = pgm_read_byte(s++)) != 0)
            lcd.print(c);
    }

void LCDClear(byte Riga){
  lcd.setCursor(0,Riga);
  LCDSpace(20);
}

void PrintTemp(float Temp){
  if (Temp<10.0)LCDSpace(1);
  if (Temp>=100.0)lcd.print(Temp,1);
  else lcd.print(Temp);
  lcd.write((byte)0);
}

void Clear_2_3(){
  LCDClear(1);
}

void Version(byte locX, byte locY){
  lcd.setCursor(locX, locY);
  LCDSpace(1);
   showString(PSTR("2.8.3 "));
  //lcd.write(7);
}

void Intestazione(){  
  lcd.setCursor(0,0);
  //lcd.print(F("Open ArdBir 2014"));
  showString(PSTR("Open ArdBir 2014"));
}

void LCD_OkEsci(){
  lcd.setCursor(8,1);
  //lcd.print(F("Ok Quit"));
  showString(PSTR("OK Quit"));
}

void LCD_Procedo(){
  lcd.setCursor(1,1);
  //lcd.print(F("Sure?-> Yes No "));
  showString(PSTR("Sure?-> Yes No "));
}

/*
void LCD_Conferma(){
  lcd.setCursor(1,1);
  lcd.print(F("Sure?-> Yes -- "));
}
*/

void LCD_Default(float Temp){
  Intestazione();
  lcd.setCursor(1,1);
  PrintTemp(Temp);
  Version(7,1);
}

void Menu_1(){
  lcd.clear();  
  lcd.setCursor(0,0);
  //lcd.print(F("  MANUAL  MODE  "));
  showString(PSTR("  MANUAL  MODE  "));
  delay(750);
}    
void Manuale(float Set, float Temp, float TempBoil){    
  lcd.setCursor(0,0);
  //lcd.print(F("MANUAL    "));
  showString(PSTR("MANUAL    "));
  
  if (Set>=TempBoil && Temp>=Set){ 
    PrintTemp(Temp);
  }else{
    if (Set>=100)lcd.print(Set,1);
    else lcd.print(Set);
    lcd.write(2);
    
    lcd.setCursor(1,1);
    PrintTemp(Temp);
  }
}    


void Menu_2(){
  lcd.clear(); 
  lcd.setCursor(0,0);
  //lcd.print(F(" AUTOMATIC MODE "));
  showString(PSTR(" AUTOMATIC MODE "));
  
}
void AddMalt(){
  lcd.setCursor(0,0);
  //lcd.print(F("   Add   Malt   "));
  showString(PSTR("   Add   Malt   "));
  
  LCD_OkEsci();
}

void Stage(byte Stage, float Set, float Temp){
  lcd.setCursor(0,0);
   
  if (Stage < 11)
  {
      printMem(stageName, Stage); // Necessary casts and dereferencing, just copy.
  }
  else
  {  
      printMem(stageName, 10); // Necessary casts and dereferencing, just copy.
  }
  
  lcd.setCursor(10,0);
  if (Set>=100)lcd.print(Set,1);
  else lcd.print(Set);
  lcd.write(2);
  
  lcd.setCursor(1,1);
  PrintTemp(Temp);
}

void SaltoStep(){
  lcd.setCursor(0,0);
   showString(PSTR("Go to Next Step?"));
  lcd.setCursor(8,1);
   showString(PSTR("  Yes No "));
}

void RemoveMalt(){
  lcd.setCursor(0,0);
   showString(PSTR("  Remove  Malt  "));
  LCD_OkEsci();
}

void Temp_Wait(float Temp){
  lcd.setCursor(1,1);
  PrintTemp(Temp);
}

void Boil(float Heat, float Temp, byte Tipo){
  if (Tipo==1){
    lcd.setCursor(0,0);
     showString(PSTR("Boil      "));
  }
  
  lcd.setCursor(1,1);
 showString(PSTR("H="));       //Display output%
  if (Heat<100)LCDSpace(1); //added space as place holder for <100
  lcd.print(Heat,0);        //Display output%
 showString(PSTR("% ")); 
  
  lcd.setCursor(10,0);
  PrintTemp(Temp);
} 

void NoBoil(){ 
}

void HopAdd(byte HopAdd){
  lcd.setCursor(0,0);
 showString(PSTR("Hops   "));
  if(HopAdd<9)LCDSpace(1);
  lcd.print(HopAdd+1);
  lcd.setCursor(1,1);
  LCDSpace(6); 
}

void Raffreddamento() {
  lcd.clear();
  lcd.setCursor(0, 0);
 showString(PSTR(" START  COOLING "));
  LCD_Procedo();
}

void LCDWhirlpool() {
  lcd.clear();
  lcd.setCursor(0, 0);
 showString(PSTR("START  WHIRLPOOL"));
  LCD_Procedo();
  
}void Menu_3(){
  lcd.clear();
  lcd.setCursor(0,0);
 showString(PSTR(" CONFIGURATION  "));
  delay(750);
}
void Menu_3_1(){
  lcd.setCursor(0,0);
  //lcd.print(F("P.I.D.PARAMETERS"));
 showString(PSTR("   PID -- PWM   "));
}     
void Menu_3_1_x(byte i){
  lcd.setCursor(0,1);
  printMem(PIDName, i);
//  lcd.print(PIDName[i]);
}  
void PidSet(int pidSet, byte i){
  lcd.setCursor(9, 1);
  if (i > 0) LCDSpace(2);
  
  if (i > 0 && i <= 6) {
    if (i < 6) LCDSpace(1);
    
    if (i  < 4)           pidSet = pidSet - 100;
    if (i == 4 || i == 5) pidSet = pidSet * 250;
    
    FormatNumeri(pidSet, -1);  
  }
  
  if(i >= 7){
    float OffSet = pidSet;
    if (i == 7) OffSet = (OffSet - 50.0) / 10.0;
    if (i == 8) OffSet =  OffSet / 10.0;
    
    FormatNumeri(OffSet, 0);
    lcd.print(OffSet);
    return;
  }
  
  if (i == 0) {
    lcd.setCursor(7, 1);
    if (pidSet == 0)showString(PSTR(" Electric"));
    else            showString(PSTR("      Gas"));
  } else             lcd.print(pidSet);

  if (i == 6)showString(PSTR("%"));
}

void Menu_3_2(){
  lcd.setCursor(0,0);
 showString(PSTR("UNIT  PARAMETERS"));
}     

void Menu_3_2_x(byte i){
  lcd.setCursor(0,1);

  printMem(unitName, i); // Necessary casts and dereferencing, just copy.
}
void UnitSet(byte unitSet, byte i){
  
  switch(i){
     
    case(0):// Scala Temp
        lcd.setCursor(15,1);
        lcd.write((byte)0);  
        break;
      
    case(1)://Sensore
      lcd.setCursor(9,1);
      if (unitSet == 0)showString(PSTR(" Inside"));
      else            showString(PSTR("Outside"));
      break;
      
    case( 2):
    case( 3): // Temperatura di Ebollizione
    case(10):
    case(11): //Temperatura Fermo Pompa
      lcd.setCursor(12,1);
      if (unitSet<100)LCDSpace(1);
      lcd.print(unitSet);
      lcd.write((byte)0);
      break;
    
    case(4):// Durata Ciclo Pompa
      lcd.setCursor(13,1);
      if (unitSet<10)LCDSpace(1);
      lcd.print(unitSet);
     showString(PSTR("'"));
      break;
    
    case(5)://Durata Pausa Pompa
      lcd.setCursor(14,1);
      lcd.print(unitSet);
     showString(PSTR("'"));
      break;
    
    default:
      lcd.setCursor(13,1);
      if (unitSet==0) showString(PSTR("Off"));
      if (unitSet==1) showString(PSTR(" On"));
      break;
      
    case(12)://Pipe
      lcd.setCursor(9,1);
      if (unitSet==0) showString(PSTR("Passive"));
      else  showString(PSTR(" Active"));
      break;

    case(13):
    case(14):
    case(15):
      lcd.setCursor(13,1);
      if (unitSet==0) showString(PSTR(" No"));
      if (unitSet==1) showString(PSTR("Yes"));
      break;
    
    case(16): //Iodio
      if (unitSet==0){
        lcd.setCursor(9,1);
         showString(PSTR("    Off"));
      }else CountDown(unitSet*60,9,1,1);
      break;
      
    case(17):
      if (unitSet == 0)  showString(PSTR("    Off"));
      if (unitSet == 1)showString(PSTR("   Cold"));
      if (unitSet == 2)showString(PSTR("    Hot"));
      break;
  }  
}


void Menu_3_3(){
  lcd.setCursor(0,0);
 showString(PSTR(" SET AUTOMATION "));
}     
void Menu_3_3_x(byte Stage){
  lcd.setCursor(0,1);
  //lcd.print(stageName[Stage]);
  char buffer[30];  
  strcpy_P(buffer, (char*)pgm_read_word(&(stageName[Stage]))); // Necessary casts and dereferencing, just copy.
    
  lcd.print(buffer);
}
void StageSet(float Temp){
  lcd.setCursor(10,1);
  PrintTemp(Temp);
}

void TimeSet(int Time){
  lcd.setCursor(9,1);
  if (Time<10)LCDSpace(5);
  if (Time>=10 && Time<100)LCDSpace(4);
  if (Time>=100)LCDSpace(3);
  lcd.print(Time);   
 showString(PSTR("'"));
}


void Menu_3_3_8(){
  lcd.setCursor(0,1);
 showString(PSTR("Number of Hops"));
} 
void NumHops(byte SetNumHops){
  lcd.setCursor(14,1);
  if(SetNumHops<10)LCDSpace(1);
  lcd.print(SetNumHops);   
}

void Menu_3_3_9(){
  lcd.setCursor(0,1);
 showString(PSTR("Boil        "));
} 

void Menu_3_3_10(byte SetHop){
  lcd.setCursor(0,1);
 showString(PSTR("Hops     "));
  if(SetHop<10)LCDSpace(1);
  lcd.print(SetHop);

  
} 
void TimeHops(int Time){
  lcd.setCursor(12,1);
  if (Time<10)LCDSpace(2);;
  if (Time>=10 && Time<100)LCDSpace(1);
  lcd.print(Time);   
 showString(PSTR("'"));
}

void Menu_3_4(){
  lcd.setCursor(0,0);
 showString(PSTR(" MANAGE RECIPES "));
  LCDClear(1);
}     
void Menu_3_4_1(){
  lcd.setCursor(0,1);
 showString(PSTR("  Load Setting  "));
}
void Menu_3_4_2(){
  lcd.setCursor(0,1);
 showString(PSTR("  Save Setting  "));
}
void Menu_3_4_3(){
  lcd.setCursor(0,1);
 showString(PSTR(" Delete Setting "));
}
void Menu_3_4_4(){
  lcd.setCursor(0,1);
 showString(PSTR(" Initialization "));
}
/*
void CaricaRicetta(byte Ricetta){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(F("RECIPE  "));
  if (Ricetta<10)lcd.print(F("0"));
  lcd.print(Ricetta);
  LCD_Procedo();
}
*/
void NoRecipe(){
  lcd.setCursor(0,1);
 showString(PSTR("- NO  SETTINGS -"));
  Buzzer(3,50);
  delay(2500);
}

void Ricetta(byte numRicetta, byte Tipo){
  //Tipo 0 = Load
  //Tipo 1 = Save
  
  lcd.setCursor(1,1);
  if (numRicetta<10)showString(PSTR("0"));
  lcd.print(numRicetta);
 showString(PSTR("> "));
}

void LCD_NomeRicetta(byte pos, byte Lettera){
  lcd.setCursor(pos+5,1);
  lcd.print((char)Lettera);
}


void LeggoRicetta(byte Ricetta){
  lcd.clear();
  
  lcd.setCursor(0,0);
 showString(PSTR("  Setting  "));
  if (Ricetta<10) showString(PSTR("0"));
  lcd.print(Ricetta);
  lcd.setCursor(1,1);
  showString(PSTR("Setting Loaded"));
  Menu_3_4();
}


void SalvataggioRicetta(byte Ricetta){
  lcd.clear();

  lcd.setCursor(0,0);
  showString(PSTR("  SETTING  "));
  if (Ricetta<10) showString(PSTR("0"));
  lcd.print(Ricetta);
  LCD_Procedo();
}

void SalvaRicetta(){
  Buzzer(5,35);
  
  lcd.setCursor(0,1);
  showString(PSTR(" Setting  Saved "));
  delay(2000);
  Menu_3_4();
}

void CancelloRicetta(byte Ricetta){
  lcd.clear();

  lcd.setCursor(0,0);
  showString(PSTR("  SETTING  "));
  if (Ricetta<10) showString(PSTR("0"));
  lcd.print(Ricetta);
  LCD_Procedo();
}

void Cancellazione(byte Ricetta){
  lcd.clear();
  
  lcd.setCursor(3,0);
  showString(PSTR("SETTING "));

  if (Ricetta<10) showString(PSTR("0"));
  lcd.print(Ricetta);
  LCDSpace(3);
  lcd.setCursor(3,1);
  showString(PSTR("  DELETED"));
  delay(1500);
  Menu_3_4();
}


void Inizializzazione(){
  lcd.setCursor(0,0);
  showString(PSTR(" Initialization "));
  LCD_Procedo();
}
void Inizializza(){
  lcd.clear();
  
  lcd.setCursor(2,1);
  showString(PSTR("EEPROM Ready"));
  Buzzer(3,75);
  delay(1500);

  Menu_3_4();
}

void MemoriaPiena(){
  lcd.clear();
  
  lcd.setCursor(2,1);
  showString(PSTR("FULL  MEMORY"));
  Buzzer(3,125);
  delay(2500);
}

void Menu_3_5(){
  lcd.setCursor(0,0);
  showString(PSTR("     CREDIT     "));
}     

void Credits(){
  lcd.clear();
  
  Intestazione();
  delay(1500);
  
  display_lcd(0,0,"Original   Idea:",750);
  display_lcd(0,1,"Stephen Mathison",2000);

}

#if TestMemoria == true
void Menu_4(){
    lcd.clear();
    lcd.setCursor(4,0);
     showString(PSTR("TEST RAM"));
}
void Menu_4_1(){
    lcd.clear();
    lcd.setCursor(1,0);
     showString(PSTR("Memoria Libera"));
    lcd.setCursor(4,1);
  
    if (freeRam()<100&&freeRam()>=10)LCDSpace(1);
    if (freeRam()<10)LCDSpace(2);
    lcd.print(freeRam());
    LCDSpace(1);
     showString(PSTR("byte"));
  
    delay(3500);
    lcd.clear();
}
#endif

void Pause_Stage(float Temp, int Time){
  lcd.setCursor(0,0);
   showString(PSTR("--- In Pause ---" ));
  
  lcd.setCursor(1,1);
  PrintTemp(Temp);
  
  CountDown(Time,8,1,1);
}

void prompt_for_water (){
  lcd.setCursor(0,0);
   showString(PSTR("  Water Added?  "));
  LCD_OkEsci();
}

void Resume(){
  lcd.setCursor(0,0);
   showString(PSTR("Resume  Process?"));
  LCD_OkEsci();
}

void CntDwn(int Time){
  CountDown(Time,8,1,1);
}

void PausaPompa(float Temp, int Time){  
  lcd.setCursor(0,0);
   showString(PSTR("-- Pump  Rest --"));

  lcd.setCursor(1,1);
  PrintTemp(Temp);
  
  CountDown(Time,8,1,1);
}

void Iodine(float Temp, int Time){
  lcd.setCursor(0,0);
  LCDSpace(1);
  PrintTemp(Temp);
  LCDSpace(2);
  CountDown(Time,9,0,1);
  
  lcd.setCursor(1,1);
   showString(PSTR("IODINE"));
  lcd.setCursor(7,1);
   showString(PSTR(" Ok  -- "));
}

void End(){
  Buzzer(3,250);
  lcd.setCursor(0,0);
   showString(PSTR("     Brewing    "));
  lcd.setCursor(0,1);
   showString(PSTR("    Finished!   "));
  
  Buzzer(1,3000);
  delay(2500);
}

void PumpPrime(){
  lcd.setCursor(0,1);
   showString(PSTR("   Pump Prime   "));  // priming the pump
}  

void ledHeatON(){
  lcd.setCursor(0,1);
  lcd.write(6);
}

void ledHeatStatus(boolean mheat){
  lcd.setCursor(0,1);
  if (mheat)lcd.write(5);
  else LCDSpace(1);
}

void ledPumpON(){
  lcd.setCursor(15,1);
  lcd.write(4);
}

void ledPumpStatus(boolean mpump){
  lcd.setCursor(15,1);
  if (mpump)lcd.write(3);
  else LCDSpace(1);
}

void ArdBir(){
  #if StartSprite == true
    Presentazione(0,0);
  #endif
  
  #if Sprite == true
    ArdBir1(4,0);
  #endif
}

void PartenzaRitardata(){
  lcd.setCursor(0,0);
   showString(PSTR("  Delay Start?  "));
  lcd.setCursor(0,1);
  LCDSpace(9);
   showString(PSTR("No Yes")); 
}

void ImpostaTempo(unsigned long Time){
  lcd.setCursor(0,0);
   showString(PSTR(" Delay  "));
  
  CountDown(Time*60,8,0,2);
  
  lcd.setCursor(0,1);
   showString(PSTR(" --  -- Quit Ok "));
}

void StartDelay(unsigned long Tempo){
  lcd.setCursor(0,0);
   showString(PSTR("To be started in"));
  CountDown(Tempo,4,1,2);
}

void TemperaturaRaggiunta(){
  LCDClear(0);
  lcd.setCursor(0,0);
   showString(PSTR(" Temp.  Reached "));
  lcd.setCursor(8,1);
   showString(PSTR("Yes -- "));
}

void ImpostaWhirlpool(unsigned long Time) {
  lcd.setCursor(0, 1);
   showString(PSTR("Timing "));
 
  CountDown(Time * 60, 7, 2, 2);
}

