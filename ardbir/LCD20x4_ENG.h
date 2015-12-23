#include <avr/pgmspace.h>
//char *PIDName[]   ={_use_, "Constant  kP", "Constant  kI", "Constant  kD", "SampleTime  ", "WindowSet ms", "Heat in Boil", "Calibration " , "Hysteresi  "};
//char *stageName[] ={"Mash In   ", "Phytase   ", "Glucanase ", "Protease  ", "\xE2""Amylase  ", "\xE0""Amylase1 ", "\xE0""Amylase2 ", "Mash Out  ", "Boil      ", "Cooling   ", "Whirlpool "};
//char *unitName[]  ={"Set Degrees", "Sensor     ", "Temp Boil  ", "Temp Boil  ", "Pump Cycle ", "Pump Rest  ", "Pmp PreMash", "Pmp on Mash", "Pmp MashOut", "Pmp on Boil", "Pump Stop  ", "Pump Stop  ", "PID Pipe   ", "Skip Add   ", "Skip Remove", "Skip Iodine", "IodineTime " , "Whirlpool "};

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


//byte HeatONOFF[8]    = {B00000, B01010, B01010, B01110, B01110, B01010, B01010, B00000};  // [5] HEAT symbol
//byte RevHeatONOFF[8] = {B11111, B10101, B10101, B10001, B10001, B10101, B10101, B11111};  // [6] reverse HEAT symbol
//byte Language[8]     = {B11111, B00010, B01000, B11111, B00000, B10001, B10101, B11111};  // [7] EN symbol

char buffer[30];  

void printMem(const char* const* array, int entry)
{
  strcpy_P(buffer, (char*)pgm_read_word(&(array[entry]))); // Necessary casts and dereferencing, just copy.
    
  lcd.print(buffer);
}

void displayMem(byte posX , byte posY , const char* const* array, int entry, int Pausa)
{
    strcpy_P(buffer, (char*)pgm_read_word(&(array[entry]))); // Necessary casts and dereferencing, just copy.
    lcd.setCursor(posX, posY);
    lcd.print(buffer);
    delay(Pausa);
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

void PrintTemp(byte PosX, byte PosY, float Temp, byte dec){
  if (PosY<4) lcd.setCursor(PosX,PosY);

  FormatNumeri(Temp, -1);
  //if (Temp<10.0)LCDSpace(2);
  //if (Temp>=10.0 && Temp<100.0)LCDSpace(1);

  lcd.print(Temp, dec);
  ////lcd.write((byte)0);
}

void Clear_2_3(){
  LCDClear(2);
  LCDClear(3);
}

void Version(byte locX, byte locY){
  lcd.setCursor(locX, locY);
  lcd.print(PSTR("2.8.3""\xE0"));
  //lcd.write(7);
}

void Intestazione(){
  lcd.setCursor(0,0);
  lcd.print(PSTR("Open ArdBir "));
  Version(12,0);
}

void LCD_Procedo(){
  lcd.setCursor(1,3);
  lcd.print(PSTR("Continue:  Yes  No "));
}


void LCD_Default(float Temp){
  Intestazione();

  PrintTemp(6,1,Temp,2);

  LCDClear(2);

  lcd.setCursor(1,3);
  lcd.print(PSTR("---  MAN AUTO SETUP"));
}

void CntDwn(int Time){
  CountDown(Time,11,2,2);
}

void Watch(int Time){
  CountDown(Time,6,2,2);
}

void PauseScreen(){
  lcd.setCursor(0,0);
  lcd.print(PSTR("-----  "));
  lcd.setCursor(14,0);
  lcd.print(PSTR(" -----"));

  lcd.setCursor(1,2);
  lcd.print(PSTR("----"));
  lcd.setCursor(14,2);
  lcd.print(PSTR(" ----"));
}

void LCD_QQSO(){
  lcd.setCursor(1,3);
  lcd.print(PSTR("UP* *DWN Skip   Ok"));
}
void LCD_QQxO(){
  lcd.setCursor(1,3);
  lcd.print(PSTR("UP* *DWN  ---   Ok"));
}
void LCD_SGEO(){
  lcd.setCursor(1,3);
  lcd.print(PSTR("Up   Dwn  Quit  Ok"));
}
void LCD_xGEO(){
  lcd.setCursor(1,3);
  lcd.print(PSTR("---  Dwn  Quit  Ok"));
}

void LCD_SxEO(){
  lcd.setCursor(1,3);
  lcd.print(PSTR("Up   ---  Quit  Ok"));
}


void Menu_1(){
  lcd.clear();
  lcd.setCursor(4,0);
  lcd.print(PSTR("MANUAL  MODE"));
}

void Manuale(float Set, float Temp,float TempBoil){
  //lcd.setCursor(1,1);
  PrintTemp(1,1,Temp,2);

  lcd.setCursor(12,1);
  if (Set<100)LCDSpace(1);
  lcd.print(Set);
  //lcd.write(2);

  lcd.setCursor(1,3);
  lcd.print(PSTR("UP* *DWN Heat Pmp"));
}

void Menu_2(){
  lcd.clear();

  lcd.setCursor(3,0);
  lcd.print(PSTR("AUTOMATIC MODE"));
}
void AddMalt(){
  lcd.setCursor(2,2);
  lcd.print(PSTR("    Add Malt    "));

  LCD_Procedo();
}

void Stage(byte Stage, float Set, float Temp){
  lcd.setCursor(0,0);
  lcd.print(PSTR(" AUTO --> "));
  //lcd.print(stageName[Stage]);
  printMem(stageName, Stage);
  

  //lcd.setCursor(1,1);
  PrintTemp(1,1,Temp,2);

  lcd.setCursor(8,1);
  LCDSpace(4);
  if (Set<100)LCDSpace(1);
  lcd.print(Set);
  //lcd.write(2);

  lcd.setCursor(1, 3);
  if (Stage ==  0)               lcd.print(PSTR("UP* *DWN Pause ---"));
  if (Stage  >  0 && Stage  < 8) lcd.print(PSTR("UP* *DWN Pause STP"));
  if (Stage ==  8)               lcd.print(PSTR("UP* *DWN  ---  ---"));
  if (Stage ==  9)               lcd.print(PSTR("UP* *DWN  ---  Pmp"));
  if (Stage == 10)               lcd.print(PSTR("---* *--- ---  Pmp"));
  if (Stage == 11)               lcd.print(PSTR("---* *--- Time Pmp"));
}

void SaltoStep(){
  lcd.setCursor(2,2);
  lcd.print(PSTR("Go to Next Step? "));
  LCD_Procedo();
}

void RemoveMalt(){
  lcd.setCursor(3,2);
  lcd.print(PSTR(" Remove  Malt    "));
  LCD_Procedo();
}

void Temp_Wait(float Temp){
  //lcd.setCursor(1,1);
  PrintTemp(1,1,Temp,2);
}

void Boil(float Heat, float Temp, byte Tipo){
  if (Tipo==1){
    lcd.setCursor(0,0);
    lcd.print(PSTR(" AUTO --> Boil      "));
  }

  //lcd.setCursor(1,1);
  PrintTemp(1,1,Temp,2);

  lcd.setCursor(1,2);
  lcd.print(PSTR("PWM="));    //Display output%
  FormatNumeri(Heat, -1);
  //if (Heat<100 && Heat>=10)LCDSpace(1);
  //if (Heat<10)LCDSpace(2);
  lcd.print(Heat,0);       //Display output%
  lcd.print(PSTR("% "));
}

void NoBoil(){
  lcd.setCursor(1,2);
  LCDSpace(9);
}

void HopAdd(byte HopAdd){
  lcd.setCursor(10,0);
  lcd.print(PSTR("  Hop   "));
  if(HopAdd<10)LCDSpace(1);
  lcd.print(HopAdd+1);
}

void Raffreddamento() {
  lcd.clear();
  lcd.setCursor(3, 1);
  lcd.print(PSTR("START  COOLING"));
  LCD_Procedo();
}

void LCDWhirlpool() {
  lcd.clear();
  lcd.setCursor(2, 1);
  lcd.print(PSTR("START  WHIRLPOOL"));
  LCD_Procedo();
}

void Menu_3(){
  lcd.clear();

  lcd.setCursor(0,0);
  lcd.print(PSTR("     SETUP MENU    "));
}
void Menu_3_1(){
  lcd.setCursor(2,1);
  //lcd.print(PSTR("P.I.D.  Parameters"));
  lcd.print(PSTR("   PID -- PWM   "));

  LCDClear(2);

  LCD_xGEO();
}
void Menu_3_1_x(byte i){
  //display_lcd (1,2,PIDName[i],0);
  displayMem(1,2,PIDName,i,0);
  
  LCD_QQxO();
}

void PidSet(int pidSet, byte i){
  if (i == 0) {
    lcd.setCursor(10, 2);

    if (pidSet == 0) lcd.print(PSTR("  Electric"));
    else             lcd.print(PSTR("       Gas"));
  } else {
    lcd.setCursor(12, 2);
    LCDSpace(2);

    if (i > 0 && i <= 6) {
      if (i < 6) LCDSpace(1);

      if (i  < 4)           pidSet = pidSet - 100;
      if (i == 4 || i == 5) pidSet = pidSet * 250;

      FormatNumeri(pidSet, 0);
    }

    if(i >= 7){
      float OffSet = pidSet;
      if (i == 7) OffSet = (OffSet - 50.0) / 10.0;
      if (i == 8) OffSet =  OffSet / 10.0;

      FormatNumeri(OffSet, -2);
      lcd.print(OffSet);
      return;
    }

    lcd.print(pidSet);

    if (i == 6) lcd.print(PSTR("%"));
  }
}

void Menu_3_2(){
  lcd.setCursor(1,1);
  lcd.print(PSTR(" Unit  Parameters "));

  LCDClear(2);

  LCD_SGEO();
}

void Menu_3_2_x(byte i){
  //display_lcd(1,2,unitName[i],0);
  displayMem(1,2,unitName,i,0);
  LCD_QQxO();
}

void UnitSet(byte unitSet, byte i){
  lcd.setCursor(12,2);
  switch(i){

    case(0):// Scala Temp
      LCDSpace(6);
      //lcd.write((byte)0);
      break;

    case(1)://Sensore
      if (unitSet == 0) lcd.print(PSTR(" Inside"));
      else              lcd.print(PSTR("Outside"));
      break;

    case( 2):
    case( 3): // Temperatura di Ebollizione
    case(10):
    case(11): //Temperatura Fermo Pompa
      LCDSpace(3);
      PrintTemp(9, 9, unitSet, 0);
      break;

    case(4):// Durata Ciclo Pompa
      LCDSpace(4);
      FormatNumeri(unitSet, -2);
      //if (unitSet < 10) LCDSpace(1);
      lcd.print(unitSet);
      lcd.print(PSTR("'"));
      break;

    case(5)://Durata Pausa Pompa
      LCDSpace(5);
      lcd.print(unitSet);
      lcd.print(PSTR("'"));
      break;

 default:
     LCDSpace(4);
      if (unitSet == 0) lcd.print(PSTR("Off"));
      if (unitSet == 1) lcd.print(PSTR(" On"));
      break;

 /*
    case(10):
    case(11):
      LCDSpace(3);
      PrintTemp(9, 9, unitSet, 0);
      break;
 */
    case(12)://Pipe
      if (unitSet == 0) lcd.print(PSTR("Passive"));
      else              lcd.print(PSTR(" Active"));
      break;

    case(13):
    case(14):
    case(15):
      LCDSpace(4);
      if (unitSet == 0) lcd.print(PSTR(" No"));
      if (unitSet == 1) lcd.print(PSTR("Yes"));
      break;

    case(16): //Iodio
      if (unitSet == 0) {
        lcd.setCursor(12, 2);
        lcd.print(PSTR("    Off"));
      } else CountDown(unitSet * 60, 12, 2, 1);
      break;

    case(17):
      if (unitSet == 0) lcd.print(PSTR("    Off"));
      if (unitSet == 1) lcd.print(PSTR("   Cold"));
      if (unitSet == 2) lcd.print(PSTR("    Hot"));
      break;
  }
}


void Menu_3_3(){
  lcd.setCursor(1,1);
  lcd.print(PSTR("  Set Automation  "));
  LCD_SGEO();
}
void Menu_3_3_x(byte Stage){
  //display_lcd(1,2,stageName[Stage],0);
  displayMem(1,2,stageName,Stage,0);
  if (Stage==0||Stage==6||Stage==7)LCD_QQxO();
  else LCD_QQSO();
}

void StageSet(float Temp){
  //lcd.setCursor(12,2);
  PrintTemp(12,2,Temp,2);
}

void TimeSet(int Time){
  lcd.setCursor(12,2);
  FormatNumeri(Time, 2);
  //if (Time<10)LCDSpace(5);
  //if (Time>=10 && Time<100)LCDSpace(4);
  //if (Time>=100)LCDSpace(3);
  lcd.print(Time);
  lcd.print(PSTR("'"));
  LCD_QQxO();
}


void Menu_3_3_8(){
  lcd.setCursor(1,2);
  lcd.print(PSTR("Number of Hops  "));
  LCD_QQxO();
}
void NumHops(byte SetNumHops){
  lcd.setCursor(17,2);
  if(SetNumHops<10)LCDSpace(1);
  lcd.print(SetNumHops);
}

void Menu_3_3_9(){
  lcd.setCursor(1,2);
  lcd.print(PSTR("Boil          "));
  LCD_QQxO();
}

void Menu_3_3_10(byte SetHop){
  lcd.setCursor(1,2);
  lcd.print(PSTR("Hops nmbr("));
  FormatNumeri(SetHop, -2);
  //if (SetHop<10)LCDSpace(1);
  lcd.print(SetHop);
  lcd.print(PSTR(")  "));
  LCD_QQxO();
}

void TimeHops(int Time){
  lcd.setCursor(15,2);
  FormatNumeri(Time, -1);
  //if (Time<10)LCDSpace(2);
  //if (Time>=10 && Time<100)LCDSpace(1);
  lcd.print(Time);
  lcd.print(PSTR("'"));
}


/*
void Menu_3_4(){
  lcd.setCursor(1,1);
  lcd.print(PSTR("  Manage Recipes  "));

  LCDClear(2);

  LCD_SGEO();
}
void Menu_3_4_1(){
  lcd.setCursor(2,2);
  lcd.print(PSTR(" Load Setting "));
  LCD_xGEO();
}
void Menu_3_4_2(){
  lcd.setCursor(3,2);
  lcd.print(PSTR(" Save Setting "));
  LCD_SGEO();
}
void Menu_3_4_3(){
  lcd.setCursor(3,2);
  lcd.print(PSTR("Delete Setting"));
  LCD_SGEO();
}
void Menu_3_4_4(){
  lcd.setCursor(3,2);
  lcd.print(PSTR("Initialization"));
  LCD_SxEO();
}

void NoRecipe(){
  LCDClear(2);

  Buzzer(3,50);

  lcd.setCursor(1,3);
  lcd.print(PSTR("   NO SETTINGS    "));

  delay(1500);
}
*/
void Ricetta(byte numRicetta, byte Tipo){
  //Tipo 0 = Load
  //Tipo 1 = Save

  lcd.setCursor(2,2);
  if (numRicetta<10)lcd.print(PSTR("0"));
  lcd.print(numRicetta);
  lcd.print(PSTR(" > "));

  if (Tipo==0)LCD_Procedo();
  else{
    lcd.setCursor(1,3);
    lcd.print(PSTR("UP* *DWN  Back  Ok"));
  }
}

void LCD_NomeRicetta(byte pos, byte Lettera){
  lcd.setCursor(pos+7,2);
  lcd.print((char)Lettera);
}

void LeggoRicetta(byte Ricetta){
  LCDClear(2);
  Buzzer(2,35);

  lcd.setCursor(1,3);
  lcd.print(PSTR("  Load Setting...  "));
  delay(1500);

  lcd.setCursor(1,3);
  lcd.print(PSTR("Setting "));
  if (Ricetta<10)lcd.print(PSTR("0"));
  lcd.print(Ricetta);
  lcd.print(PSTR(" Loaded "));
  delay(1500);
}


void SalvataggioRicetta(byte Ricetta){
  lcd.setCursor(1,2);
  lcd.print(PSTR(" Save Setting  "));
  if (Ricetta<10)lcd.print(PSTR("0"));
  lcd.print(Ricetta);
  lcd.print(PSTR("?"));

  LCD_Procedo();
}

void SalvaRicetta(){
  LCDClear(2);
  Buzzer(5,35);

  lcd.setCursor(1,3);
  lcd.print(PSTR("    Saving...     "));
  delay(1500);

  lcd.setCursor(1,3);
  lcd.print(PSTR("  Setting  Saved  "));
  delay(1500);
}

void CancelloRicetta(byte Ricetta){
  lcd.setCursor(0,2);
  lcd.print(PSTR(" Delete Setting "));
  if (Ricetta<10)lcd.print(PSTR("0"));
  lcd.print(Ricetta);
  LCDSpace(1);

  LCD_Procedo();
}

void Cancellazione(byte Ricetta){
  LCDClear(2);
  Buzzer(2,35);

  lcd.setCursor(1,3);
  lcd.print(PSTR("  Deleting...     "));
  delay(1500);

  lcd.setCursor(1,3);
  lcd.print(PSTR(" Setting  Deleted "));
  delay(1500);
}

void Inizializzazione(){
  lcd.setCursor(2,2);
  lcd.print(PSTR("  Initialization"));

  LCD_Procedo();
}
void Inizializza(){
  LCDClear(2);

  lcd.setCursor(1,3);
  lcd.print(PSTR(" Initialize EEPROM"));
  delay(1500);

  Buzzer(3,75);
  lcd.setCursor(1,3);
  lcd.print(PSTR("  EEPROM   ready  "));
  delay(1500);
}

void MemoriaPiena(){
  LCDClear(2);

  Buzzer(3,125);

  lcd.setCursor(1,3);
  lcd.print(PSTR("    ATTENZIONE    "));
  delay(1500);

  lcd.setCursor(1,3);
  lcd.print(PSTR("   FULL  MEMORY   "));
  delay(2000);
}


void Menu_3_5(){
  lcd.setCursor(1,1);
  lcd.print(PSTR("     Credits      "));
  LCD_SxEO();
}

void Credits(){
  /*lcd.clear();

  Intestazione();
  delay(1500);

  display_lcd(7,2,"Code :",750);
  display_lcd(5,3,"S.Mathison",2000);
  display_lcd(5,3," M.Wilson ",1750);
  display_lcd(6,3," M.Nevi ",1750);

  Clear_2_3();

  display_lcd(3, 2, "PCB  &  Tests:", 750);
  display_lcd(3, 3, "  Daniel Xan  ", 999);
  display_lcd(3, 3, "Savio Thechnic", 999);
  display_lcd(3, 3, "   A. Tidei   ", 999);
  display_lcd(4, 3, "D. Arzarello", 999);
  display_lcd(4, 3, "L. DiMichele", 999);

  Clear_2_3();

  display_lcd(3,2,"Translations:",750);
  display_lcd(4,3,"A. Moiseyev",999);//Russo
  display_lcd(4,3,"A. Mondejar",999);//Spagnolo
  display_lcd(4,3,"C.M. Macedo",999);//Portoghese 20x4
  display_lcd(3,3,"F.A. Oliveira",999);//Portugues 16x2
  display_lcd(3,3,"G.  Santaella",999);//Portuguese 20x4 (new)
  display_lcd(3,3,"  E. Eduard  ",999);//Norvegese  20x4 (new)

  Clear_2_3();
  */
}


#if TestMemoria == true
void Menu_4() {
    lcd.clear();
    lcd.setCursor(3, 0);
    lcd.print(PSTR("TEST DELLA RAM"));
}
void Menu_4_1() {
    lcd.setCursor(3, 1);
    lcd.print(PSTR("Memoria Libera"));
    lcd.setCursor(6, 2);

    if (freeRam() < 100 && freeRam() >= 10) LCDSpace(1);
    if (freeRam() <  10)                    LCDSpace(2);
    lcd.print(freeRam());
    LCDSpace(2);
    lcd.print(PSTR("byte"));

    LCDClear(3);
    delay(3500);
    lcd.clear();
}
#endif

void Pause_Stage(float Temp, int Time){
  PauseScreen();

  lcd.setCursor(1,1);
  lcd.print(PSTR("     In Pause     " ));

  //lcd.setCursor(7,0);
  PrintTemp(7,0,Temp,2);

  Watch (Time);

  lcd.setCursor(1,3);
  lcd.print(PSTR("---  ---  Exit ---"));
}

void prompt_for_water (){
  lcd.setCursor(2,1);
  lcd.print(PSTR("  Water  Added? "));
  LCD_Procedo();
}

void Resume(){
  lcd.setCursor(2,1);
  lcd.print(PSTR("Resume  Process?"));
  LCD_Procedo();
}

void PausaPompa(float Temp, int Time){
  //lcd.setCursor(1,1);
  PrintTemp(1,1,Temp,2);

  CntDwn(Time);

  lcd.setCursor(1,3);
  lcd.print(PSTR(" -  Pump  Rest  - "));
}

void Iodine(float Temp, int Time){
  PauseScreen();

  lcd.setCursor(1,1);
  lcd.print(PSTR("   IODINE  TEST   " ));

  //lcd.setCursor(7,0);
  PrintTemp(7,0,Temp,2);

  Watch (Time);

  lcd.setCursor(1,3);
  lcd.print(PSTR("---  ---   Ok  ---"));
}

void End(){
  lcd.clear();

  Buzzer(1,3000);

  lcd.setCursor(6,1);
  lcd.print(PSTR("Brewing"));
  lcd.setCursor(6,2);
  lcd.print(PSTR("Finished"));

  delay(2500);
}

void PumpPrime(){
  lcd.setCursor(5,1);
  lcd.print(PSTR("Pump Prime"));  // priming the pump
}

void ledHeatON(){
  lcd.setCursor(0,2);
  //lcd.write(6);
}

void ledHeatStatus(boolean mheat){
  lcd.setCursor(0,2);
  if (mheat) {//lcd.write(5);
  }
  else LCDSpace(1);
}

void ledPumpON(){
  lcd.setCursor(19,2);
  //lcd.write(4);
}

void ledPumpStatus(boolean mpump){
  lcd.setCursor(19,2);
  if (mpump){
  //lcd.write(3);
}
  else LCDSpace(1);
}



void PartenzaRitardata(){
  Clear_2_3();
  lcd.setCursor(4,1);
  lcd.print(PSTR("Delay Start?"));
  lcd.setCursor(13,3);
  lcd.print(PSTR("No Yes"));
}

void ImpostaTempo(unsigned long Time){
  lcd.setCursor(3,1);
  lcd.print(PSTR("Setting Delay "));

  CountDown(Time*60,6,2,2);

  LCD_SGEO();
}

void StartDelay(unsigned long Tempo){
  lcd.setCursor(2,1);
  lcd.print(PSTR("To be started in"));
  CountDown(Tempo,6,2,2);
}

void TemperaturaRaggiunta(){
  LCDClear(2);
  lcd.setCursor(3,2);
  lcd.print(PSTR("Temp.  Reached"));

  lcd.setCursor(1,3);
  lcd.print(PSTR("Continue: Yes --- "));
}

void ImpostaWhirlpool(unsigned long Time) {
  lcd.setCursor(2, 1);
  lcd.print(PSTR("Timing Whirlpool"));

  CountDown(Time * 60, 6, 2, 2);

  LCD_SGEO();
}
