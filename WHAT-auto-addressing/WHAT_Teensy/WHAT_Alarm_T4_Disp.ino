 #define USED_EEPROM_VALUE 0x7B    // This value is stored in EEPROM_USED_FLAG_LOCATION to show this is not a fresh installation. 

/* Teensy 4.0 Display board WHAT_Alarm_Teensy_0.pcb */

/*
 * This is a test of the "Datalink Analyzer" hardware.
 * It will allow verification of the following:
 *    1) VKey LED (and supply voltage delivered to DB25-14) toggles at 1Hz
 *    2) J1939 message (Sensor Supply Voltage from SA 0x25) is transmitted on CAN0 at 1Hz
 *          - Monitor shows:  Sending test message on CAN0
 *    3) J1939 message (Sensor Supply Voltage from SA 0x25) is transmitted on CAN1 at 1Hz
 *          - Monitor shows:  Sending test message on CAN1
 *    3) Analog output voltage from MAX509 on all 4 channels steps: CH1 ++;  CH2 --; CH3=CH3+2;CH4=CH4-2;
  *          - Monitor shows:  Writing AOut Channel: x  Writing 8-bit Value: x
 *    4) Tie analog out to analog in to read the MAX output voltage and send as Sensor Supply Voltage data bytes.
 *          - Monitor shows:  AnalogIN1: xxx  AnalogIN2: xxx  AnalogIN3: xxx  AnalogIN4: xxx  
 *    5) Touch Screen X/Y written to Monitor
 *          - Monitor shows:  Touchscreen pressed at    X:     Y: 
 */   
 //    This software is broken into these sections for all areas:
 //    / ************ Setup for Display Backlight ********************** /
 //    / ************  Setup for Display Buttons  ********************** /
 //    / **************** Setup for VKey LED *************************** /
 //    / ************    Setup for Analog IN    ************************ /
 //    / ************    Setup for Temperature Sensor ****************** /
 //    / ************ Setup for Analog Out (MAX509) ******************** /
 //    / ************    Setup for Zigbee Comm        ****************** /
 //    / ****************   Setup for CAN0   *************************** /
 //    / ****************   Setup for CAN1   *************************** /
 //    / ************  Setup for Digital Out 5-8  ********************** /
 //    / ************      Setup for W.H.A.T      ********************** /

// / **************** Setup for VKey LED *************************** /
#define TESTLEDPIN 5
#define TESTLEDFLASHRATE 500

/// ************ Setup for Display Backlight ********************** /
//#define DISPLAYBACKLIGHT 7    // Defined in Display_ili9341_t3.h
//#define DISPLAYFLASHRATE 2000 // Defined in Display_ili9341_t3.h
#include "z_Display_ili9341_t3.h" // This makes the display easier to use by allowing the definintion of different display screens with different buttons. 

 //    / ************      Setup for W.H.A.T      ********************** /
bool WHAT_NetworkButton = false;
uint8_t WHAT_NetworkLight = 0;
#include "z_WHAT.h"  // Wireless zigbee communications.
//#define ZIGBEE_CS   4
#define WHAT_CHIP_SELECT 4   // Hardware chip select pin
Mrf24j mrf(WHAT_CHIP_SELECT);
WHAT_Packet WHAT_Tx_Packet;  // Make a temporary transmit packet

const char WHAT_Am_I[] PROGMEM  = {"WHAT Alarm Display"};
const char WHAT_abilities[] PROGMEM  = {"4xAI, 4xDI, 4xDO, 1xRGB, 1xLED, 0xFET_LOW, 0xFET_HI, 1xDISP "};

// / ************  Setup for Display Buttons  ********************** /
#include "z_display_main.h"   // this handles the primary display for this software.
#include "z_KeypadEntry.h"  // Allows number pad entry of values.

#include "z_RemoteConnection.h"   // Allows network device identification, selection and pairing.

// ************ Setup for Digital Chip Selects  ******************/
// !!! AS THESE SECTIONS ARE INCLUDED, REMOVE THESE DEFINITIONS AND ASSOCIATED PIN INITIALIZATIONS !!!!
#define SD_CARD_CS  19
#define External_CS 5

// ************ Setup for Analog IN  ******************/
#define AIN0_PIN  A0  
#define AIN1_PIN  A1
#define AIN2_PIN  A2
#define AIN3_PIN  A3

#define AtoDforMAXv 724 // Capture the highest AtoD count for an input.
#define VoltsAtMaxv 483 // Input voltage when the AtoDforMAXv counts was read.
#define J39ValueAtMaxv 965 // This is the J1939 value (.005v/count) representing the voltage used for AtoDforMAXv
#define AIN_SampleRate 500 // Update rate for the analog inputs.

 //    / ************    Setup for Temperature Sensor ****************** /
#define TMPTR_MAX_A2D_VOLTS 3.18
#define TMPTR_MAX_A2D_COUNTS 1024

#define TMPTR1_PIN A4  //LM61
#define C_TO_F ((9.0/5.0)+32.0)
float tmprtr_DegC_local_1 = 999.9;
float tmprtr_DegF_local_1 = 999.9;

//// ************ Setup for Analog Out (MAX509) ******************/
#define MAX509CHIPSELECT 3
#define MAX509SPICLOCK  13
#define MAX509SPIMOSI   11
#define MAX500_Rate 50 // Time between evaluations of analog outputs.
//
//// ************  Setup for Digital Out 5-8  ********************** /
//#define AN8_PIN   A8 //22
//#define AN9_PIN   A9  //23
//#define AN12_PIN   A12 //31
//#define AN13_PIN   A13 //32

// *************************************************************/
// *************************************************************/
// *************************************************************/
// *************************************************************/
void setup() 
{
    Serial.begin(115200);
// de-activate all chip selects:
  pinMode(TFT_CS,OUTPUT);       // Display chip select.
  digitalWrite(TFT_CS,HIGH);
  pinMode(Touch_CS,OUTPUT);       // Touchscreen chip select.
  digitalWrite(Touch_CS,HIGH);
  pinMode(TIRQ_PIN,INPUT_PULLUP);       // Display chip select.
  //digitalWrite(TIRQ_PIN,HIGH);
  pinMode(SD_CARD_CS,OUTPUT);       // SD Card on the display chip select.
  digitalWrite(SD_CARD_CS,HIGH);
  pinMode(MAX509CHIPSELECT,OUTPUT);       // SD Card on the display chip select.
  digitalWrite(MAX509CHIPSELECT,HIGH);
  pinMode(WHAT_CHIP_SELECT,OUTPUT);       // MRF24J40MA Zigbee communications chip select.
  digitalWrite(WHAT_CHIP_SELECT,HIGH);
  pinMode(External_CS,OUTPUT);       // External SPI port chip select.
  digitalWrite(External_CS,HIGH);

// ************ Setup for Analog Out (MAX509) ****************** /
// ************ Setup for Analog IN           ****************** /
// ************  Setup for Digital Out 5-8   ******************* /
// ************ Setup for TOUCH part of Touchscreen  *********** /
  //pinMode(DISPLAYBACKLIGHT,OUTPUT);
  //digitalWrite(DISPLAYBACKLIGHT,HIGH);
  Display_begin(); //(Display_ili9341_t3.c) Allows a main menu screen with touchable buttons.
  Setup_MAIN_Display();   //(Z_Display_main.ino) creates the main menu display.
  gbl_backlightBrightness = 100;  // Turn on the display backlight to indicate life at startup.
  digitalWrite(TFT_BACKLIGHT_PIN,HIGH);
  
 //    / ************      Setup for W.H.A.T      ********************** /
  WHAT_begin(/*bool *_network_button */ &WHAT_NetworkButton , /*bool *_network_light */&WHAT_NetworkLight);
  Setup_RemoteConnect();   //(RemoteConnection.ino) If already connected, re-esablishes remote connection.  Creates a seperate page.

 Setup_KEYPAD_Display();   //(KeypadEnrty.c) Allows numbers to be entered using a keypad.
 Setup_Tmptr();        //(This file) enables pin for temperature reading.

 drawGrid();
}
// *************************************************************/
// *************************************************************/
// *************************************************************/
// *************************************************************/
void loop() {
  // put your main code here, to run repeatedly:
  Display_Loop();       //(Display_ili9341_t3.c) Allows a main menu screen with touchable buttons.
  Loop_MAIN_Display();  //(z_Display_main.ino) Determine which screen is shown and monitors buttons on that screen.
  Loop_Tmptr();         //(This routine) reads the current temperature from board mounted sensor.
  //Loop_SendData();      //(This routine) Sends accumulated data all in one message.
  KEYPAD_Display_Loop();   //(KeypadEnrty.c) Allows numbers to be entered using a keypad.
  Loop_WHAT(); // WHAT.ino -  Monitors and communicates for Wireless Home Automation and Telemetry
  Loop_Network_Button_Light();
}
// *************************************************************/
// *************************************************************/
// *************************************************************/
// *************************************************************/

void InstructionsPress (uint8_t _buttonNumber)
{
  // Nothing to do when the instruction button is pressed.
}

// *************************************************************/
// *************************************************************/
// Show_btn_tmprtr_DegF_local(/*Color of the button*/ILI9341_GREEN, /*Color of the text*/ILI9341_WHITE, /*line1 Text*/ String "Power is",/*line2 Text*/ String "  ON ");
// *********** tmprtr_DegF_local Button *******************
#define _BTN_tmprtr_DegF_local_X (max_X_coordinate - MAIN_DISPLAY_BUTTON_W)  //"X" coordinate of Top Left Corner of button
#define _BTN_tmprtr_DegF_local_Y 0     //"Y" coordinate of Top Left Corner of button.  This button will be at the top of the screen.
#define _BTN_tmprtr_DegF_local_W MAIN_DISPLAY_BUTTON_W
#define _BTN_tmprtr_DegF_local_H (MAIN_DISPLAY_BUTTON_H/2)
#define _BTN_tmprtr_DegF_local_TEXTSIZE 2
#define _BTN_tmprtr_DegF_local_LINE1TEXT_X (_BTN_tmprtr_DegF_local_X + 2)
#define _BTN_tmprtr_DegF_local_LINE1TEXT_Y (_BTN_tmprtr_DegF_local_Y + 2)
#define _BTN_tmprtr_DegF_local_LINE2TEXT_X (_BTN_tmprtr_DegF_local_LINE1TEXT_X)
#define _BTN_tmprtr_DegF_local_LINE2TEXT_Y _BTN_tmprtr_DegF_local_LINE1TEXT_Y
   //This will re-draw the button whenever called.
void Show_btn_tmprtr_DegF_local(/*Color of the button*/uint16_t _buttonColor, /*Color of the text*/uint16_t _textColor, /*line1 Text*/ String _line1Text,/*line2 Text*/ String _line2Text)
{
     tft.fillRect(_BTN_tmprtr_DegF_local_X, _BTN_tmprtr_DegF_local_Y,_BTN_tmprtr_DegF_local_W, _BTN_tmprtr_DegF_local_H, _buttonColor); //A red box, for now.
     tft.setCursor(_BTN_tmprtr_DegF_local_LINE1TEXT_X, _BTN_tmprtr_DegF_local_LINE1TEXT_Y);   // Move the cursor to this location for printing.
     tft.setTextSize(_BTN_tmprtr_DegF_local_TEXTSIZE);   
     tft.setTextColor(_textColor);            
     tft.print(_line1Text);                         
     tft.setCursor(_BTN_tmprtr_DegF_local_LINE2TEXT_X, _BTN_tmprtr_DegF_local_LINE2TEXT_Y);              // Move the cursor to this location for printing.
     tft.print(_line2Text);       // Indicate which display was activated.    
}
// **********
void btn_tmprtr_DegF_local(void)
{
}
void Setup_Tmptr(void)
{
  pinMode(TMPTR1_PIN,INPUT);
}
void Loop_Tmptr(void)
{
  /*
   *  This is for an LM61.  Vo = (+10 mV/°C x T °C) + 600 mV.
   *                       Tmptr = (Vo-600mV)/10mV/°C 
   *                       Vo = (5v/A2D_5V)* rawA2Dcounts  
   *                       so Tmptr(°C) = ((rawA2Dcounts * (5v/A2D_5V))-600mV)/10
   *  The datasheet also has a table of values.
   *  -30C = 0.3v       100C = 1.6v
   *  This can be used in the MAP function.
   *  
   *  global definition:   #define TMPTR_MAX_A2D_VOLTS 4.95
   *  global definition:   #define TMPTR_MAX_A2D_COUNTS 345774.0
   *  #define MIN_LM61_TEMPERATURE -30
   *  #define VOLTS_AT_MIN_TEMP   0.3
   *  #define MAX_LM61_TEMPERATURE 100
   *  #define VOLTS_AT_MAX_TEMP   1.6
   */

  #define _TMPTR_DELAY 1000  
  static double tmptr_delay = millis();
  
  if(tmptr_delay <= millis())
  {
    tmptr_delay = millis() + _TMPTR_DELAY; // Reset the timer.

    uint16_t _rawA2Dcounts = analogRead(TMPTR1_PIN);

   //tmprtr_DegC_local_1 =_rawA2Dcounts;
    //Serial.print("Tmptr: counts:");Serial.println(_rawA2Dcounts);
    tmprtr_DegC_local_1 = ((((float)_rawA2Dcounts * (TMPTR_MAX_A2D_VOLTS/TMPTR_MAX_A2D_COUNTS))-.6)/.01);
    //tmprtr_DegC_local_1 = ((_rawA2Dcounts * (TMPTR_MAX_A2D_VOLTS/TMPTR_MAX_A2D_COUNTS)));
    //Serial.print("DegC:");Serial.println(tmprtr_DegC_local_1);

    //tmprtr_DegF_local_1 = tmprtr_DegC_local_1 * C_TO_F;
    tmprtr_DegF_local_1 = ((tmprtr_DegC_local_1 * (9.0/5.0)) + 32.0);
   //Serial.print("DegF:");Serial.println(tmprtr_DegF_local_1);

    if(gbl_ActiveMenuNumber == gbl_MAIN_Display)
    {
      String _line2_Text = ((String)tmprtr_DegF_local_1 + "oF");
      Show_btn_tmprtr_DegF_local(/*Color of the button*/ILI9341_BLUE, /*Color of the text*/ILI9341_WHITE, /*line1 Text*/ " ",/*line2 Text*/ _line2_Text);
    }
  }
}



// *************************************************************/
// *************************************************************/
// *************************************************************/
  
//// ****************************************************************************************************
//void Test_MAX509_AnalogOut(void)
//{
//  static double MAX509Timer = millis();
//  static uint8_t Analog1 = 0;   // MAX 509 is an 8 bit device.
//  static uint8_t Analog2 = 0;
//  static uint8_t Analog3 = 0;
//  static uint8_t Analog4 = 0;
//  
//  if(millis()>=MAX509Timer)
//  {
//    MAX509Timer = MAX509Timer + MAX500_Rate;  // set the timer for the next expiration
//    // Step the outputs at different rates. allow rollovers.
//    Analog1 ++;     //increment channel 1
//      Max509Write(1, Analog1);
//    Analog2 --;     // decrement channel 2
//      Max509Write(2, Analog2);
//    Analog3 = Analog3 + 2;  // 2 step the next channel
//      Max509Write(3, Analog3);
//    Analog4 = Analog4 - 2;  // 2 step the next channel
//      Max509Write(4, Analog4);
//  }
//}

//// ************  Setup for Digital Out 5-8  ********************** /
//void Test_5_to_8 (void)
//{
//  pinMode(AN8_PIN,OUTPUT);
//  pinMode(AN9_PIN,OUTPUT);
//  pinMode(AN12_PIN,INPUT);
//  pinMode(AN13_PIN,INPUT);
//
//  #define BUTTONUPDATE 500 // Refresh the value on the display this often.
//  #define DI5_RATE 5000
//  #define DI6_RATE 6000
//
//  static double t_DI5_Timer = millis();
//  static double t_DI6_Timer = millis();
//  static double t_button_refresh_timer = millis();
//  uint16_t RawAtoDInput;
//  
//  // Toggle the DI5 pin at a defined rate  
//  if(t_DI5_Timer <= millis())
//  {
//    uint8_t Pinvalue = !(digitalRead(AN8_PIN));
//    t_DI5_Timer = millis() + DI5_RATE;
//    digitalWrite(AN8_PIN, Pinvalue);
//    if(Pinvalue){MainScreenButtons[0][button_AN8].Line2Text = "HIGH  ";}else{MainScreenButtons[0][button_AN8].Line2Text = " LOW  ";}
//  }
//  // Toggle the DI6 pin at a defined rate  
//  if(t_DI6_Timer <= millis())
//  {
//    uint8_t Pinvalue = !(digitalRead(AN9_PIN));
//    t_DI6_Timer = millis() + DI6_RATE;
//    digitalWrite(AN9_PIN, Pinvalue);
//    if(Pinvalue){MainScreenButtons[0][button_AN9].Line2Text = "HIGH  ";}else{MainScreenButtons[0][button_AN9].Line2Text = " LOW  ";}
//  }
//
//  // Display the voltage measured on the DI7 input (Set by connecting D15 to it)
//  if(t_button_refresh_timer <= millis())
//  {
//    t_button_refresh_timer = millis() + BUTTONUPDATE;  // reset the timer.
//    //RawAtoDInput = analogRead(AN12_PIN);
//    RawAtoDInput = map(analogRead(AN12_PIN),0,AtoDforMAXv,0,VoltsAtMaxv);
//    Serial.print("Analog 12 ");Serial.print((((float)RawAtoDInput)/50) ,2);
//    //MainScreenButtons[0][button_AN12].Line2Text = " " + String(RawAtoDInput ,2)+ "V";
//    MainScreenButtons[0][button_AN12].Line2Text = " " + String((((float)RawAtoDInput)/50) ,2)+ "V";
//
//    //RawAtoDInput = analogRead(AN13_PIN);
//    RawAtoDInput = map(analogRead(AN13_PIN),0,AtoDforMAXv,0,VoltsAtMaxv);
//    Serial.print("Analog 13 ");Serial.println((((float)RawAtoDInput)/50) ,2);
//    //MainScreenButtons[0][button_AN13].Line2Text = " " + String(RawAtoDInput ,2)+ "V";
//    MainScreenButtons[0][button_AN13].Line2Text = " " + String((((float)RawAtoDInput)/50) ,2)+ "V";
//
//    MainScreenButtons[0][button_AN8].NeedRefresh = 1;
//    MainScreenButtons[0][button_AN9].NeedRefresh = 1;
//    MainScreenButtons[0][button_AN12].NeedRefresh = 1;
//    MainScreenButtons[0][button_AN13].NeedRefresh = 1;
//  }//end if time to update
//}// End function
//


//// ****************************************************************************************************
//void MAXSpiByte(byte Value)
//{ // MAX509ChipSelect, MAX509SPIClock, MAX509SPIMOSI,
//  //BitBanged SPI out
//  uint8_t mask;
//  //pinMode (ChipSelect,OUTPUT);
//  pinMode (MAX509SPICLOCK,OUTPUT);
//  pinMode (MAX509SPIMOSI,OUTPUT);
//
// for (mask = 0b10000000;mask>0;mask>>=1)
// {
//   digitalWrite(MAX509SPICLOCK,LOW);
//   delayMicroseconds(1);
//   if (Value & mask)
//   {   // if bitwise AND resolves to true
//    digitalWrite(MAX509SPIMOSI,HIGH); // Send 1
//   }
//   else
//   {    // if butwise AND resolves to false
//    digitalWrite(MAX509SPIMOSI,LOW); // Send 0
//   }
//   delayMicroseconds(1);
//   digitalWrite(MAX509SPICLOCK,HIGH); // Clock in the bit
// }
// digitalWrite(MAX509SPICLOCK,LOW);
//}
//// ****************************************************************************************************
//boolean Max509Write (byte AO_Channel, byte AO_Value)
//{// MAX509ChipSelect, MAX509SPIClock, MAX509SPIMOSI,
//
//  //MAX 509 SPI Controlled 8-bit DAC
//  switch(AO_Channel)
//  {
//    case 1:
//      AO_Channel = 0x03;    // A1 to Out
//      break;
//    case 2:
//      AO_Channel = 0x07;    // A2 to Out
//      break;
//    case 3:
//      AO_Channel = 0x0B;    // A3 to Out
//      break;
//    case 4:
//      AO_Channel = 0x0F;    // A4 to Out
//      break;
//    default:     // only values 1-4 are valid.
//        return false;
//  }
//    Serial.print("Writing AOut Channel: ");Serial.print(AO_Channel);Serial.print("\t Writing 8-bit Value: ");Serial.println(AO_Value);
//    pinMode(MAX509CHIPSELECT,OUTPUT);
//    digitalWrite(MAX509CHIPSELECT,LOW);
//    MAXSpiByte(AO_Channel);
//    MAXSpiByte(AO_Value);
//    digitalWrite(MAX509CHIPSELECT,HIGH);
//    return true;
//}
//// ****************************************************************************************************
//void Test_AnalogIn()
//{
//  static double t_AIN_Sample = millis(); // Update delay timer.
//
//  if(millis()>=t_AIN_Sample)
//  {
//  t_AIN_Sample = t_AIN_Sample + AIN_SampleRate; // reset the delay timer.
//    
//  uint16_t RawAtoDInput;
//
//  pinMode(AIN0_PIN,INPUT);  // Rev0 hardware has problems with AIN1Pin
//  pinMode(AIN1_PIN,INPUT);  // Rev0 hardware has problems with AIN2Pin
//  pinMode(AIN2_PIN,INPUT);
//  pinMode(AIN3_PIN,INPUT);
//
//  RawAtoDInput = analogRead(AIN0_PIN);
//  //RawAtoDInput = map(analogRead(AIN0_PIN),0,AtoDforMAXv,0,J39ValueAtMaxv);
//  Serial.print("AnalogIN0: ");Serial.print(RawAtoDInput);Serial.print("\t");
//  //MainScreenButtons[0][button_AN0].Line2Text = String(RawAtoDInput);
//  MainScreenButtons[0][button_AN0].Line2Text = " " + String((((float)RawAtoDInput)/100) ,2)+ "V";
//
//  txmsg0.buf[1] = highByte(RawAtoDInput);
//  txmsg0.buf[0] = lowByte(RawAtoDInput);
//  txmsg1.buf[1] = highByte(RawAtoDInput);
//  txmsg1.buf[0] = lowByte(RawAtoDInput);
//
//  //RawAtoDInput = analogRead(AIN1_PIN);
//  RawAtoDInput = map(analogRead(AIN1_PIN),0,AtoDforMAXv,0,J39ValueAtMaxv);
//  MainScreenButtons[0][button_AN1].Line2Text = " " + String((((float)RawAtoDInput)/100) ,2)+ "V";
//  //MainScreenButtons[0][button_AN1].Line2Text = String(RawAtoDInput);
//  txmsg0.buf[3] = highByte(RawAtoDInput);
//  txmsg0.buf[2] = lowByte(RawAtoDInput);
//  txmsg1.buf[3] = highByte(RawAtoDInput);
//  txmsg1.buf[2] = lowByte(RawAtoDInput);
// 
//  //RawAtoDInput = analogRead(AIN2_PIN);
//  RawAtoDInput = map(analogRead(AIN2_PIN),0,AtoDforMAXv,0,J39ValueAtMaxv);
//  MainScreenButtons[0][button_AN2].Line2Text = " " + String((((float)RawAtoDInput)/100) ,2)+ "V";
//  //MainScreenButtons[0][button_AN2].Line2Text = String(RawAtoDInput);
//  txmsg0.buf[5] = highByte(RawAtoDInput);
//  txmsg0.buf[4] = lowByte(RawAtoDInput);
//  txmsg1.buf[5] = highByte(RawAtoDInput);
//  txmsg1.buf[4] = lowByte(RawAtoDInput);
//  
//  //RawAtoDInput = analogRead(AIN3_PIN);
//  RawAtoDInput = map(analogRead(AIN3_PIN),0,AtoDforMAXv,0,J39ValueAtMaxv);
//  Serial.print("AnalogIN3: ");Serial.print(RawAtoDInput);Serial.println("\t");
//  MainScreenButtons[0][button_AN3].Line2Text = " " + String((((float)RawAtoDInput)/100) ,2)+ "V";
//  //MainScreenButtons[0][button_AN3].Line2Text = " " + String(RawAtoDInput);
//  txmsg0.buf[7] = highByte(RawAtoDInput);
//  txmsg0.buf[6] = lowByte(RawAtoDInput);
//  txmsg1.buf[7] = highByte(RawAtoDInput);
//  txmsg1.buf[6] = lowByte(RawAtoDInput);
//  }
//
//  // Update the display:
//  #define ANALOG_IN_DISPLAY_UPDATE_RATE 250
//  static double t_An_In_Disp_timer = millis(); 
//  if (t_An_In_Disp_timer <= millis())
//  {
//    t_An_In_Disp_timer = millis() + ANALOG_IN_DISPLAY_UPDATE_RATE;
//  
//    MainScreenButtons[0][button_AN0].NeedRefresh = 1;
//    MainScreenButtons[0][button_AN1].NeedRefresh = 1;
//    MainScreenButtons[0][button_AN2].NeedRefresh = 1;
//    MainScreenButtons[0][button_AN3].NeedRefresh = 1;
//  }
//}

//void  UpdateInstructions(void)
//{
//  static double t_UpdateInstructions = millis();
//
//  if(t_UpdateInstructions <= millis())
//  {
//    tft.setCursor(MainScreenButtons[0][button_instruction].X + 3,MainScreenButtons[0][button_instruction].Y + 3);
//    tft.setTextSize(2);
//    tft.setTextColor(ILI9341_YELLOW);
//    tft.print("AN0>AOut1,AN1>AOut2");
//    tft.setCursor(MainScreenButtons[0][button_instruction].X + 3,MainScreenButtons[0][button_instruction].Y + DISPLAY_LINE2_Y_OFFSET);
//    tft.print("AN2>AOut3,AN3>AOut4");
//    tft.setCursor(MainScreenButtons[0][button_instruction].X + 3,MainScreenButtons[0][button_instruction].Y + 2*DISPLAY_LINE2_Y_OFFSET);
//    tft.print("AN8>AN12,AN9>AN13");
//    tft.setCursor(MainScreenButtons[0][button_instruction].X + 3,MainScreenButtons[0][button_instruction].Y + 3*DISPLAY_LINE2_Y_OFFSET);
//    tft.print(" Can0->CAN1 + Term ");
//  }
//}

/*
void  Test_Digital(void)
{
  #define UPDATERATE 1000
  static double t_Timer = millis();
  static bool b_FailPin15=false,b_FailPin16=false,b_FailPin17=false,b_FailPin18=false;  // Set thee to 1 if a pin fails SHORTED high or low.

  if(t_Timer>=t_Timer)
  {
   t_Timer = t_Timer + UPDATERATE;
    
   // Set all the pins to inputs to check the pull-up resistors.
   pinMode(AN8_PIN,INPUT);
   pinMode(AN9_PIN,INPUT);
   pinMode(AN12_PIN,INPUT);
   pinMode(AN13_PIN,INPUT);

   if(AN8_PIN==LOW)
   { // Verify the pull-up is installed
     Serial.println("Digital Pin #5 (DB25-15) does not have a pull-up resistor");
   }
   else
   { // Resitor in place.  Verify the associated pin also has a resistor in place.
      if((AN12_PIN==LOW)
        {
           Serial.println("Digital Pin #5 (DB25-17) does not have a pull-up resistor");
        }
        // Proced with the test even if the pin is missing its pull-up.
        if(b_FailPin15 == false)
         {// Only conduct this test if it has not previously failed.
          pinMode(AN8_PIN,OUTPUT);
          digitalWrite(AN8_PIN,HIGH);
          if(AN12_PIN == HIGH)
            { // Should have already been high, but if Pull-up was missing, it would not be.
             digitalWrite(AN8_PIN,LOW);
              if(AN12_PIN == HIGH)
              { // If this pin is still high, something is wrong.
                pinMode(AN8_PIN,INPUT); // Turn the pin back to an input to reduce likelihood of damage.
                Serial.println("Connection between DB25-15 and DB25-17 is incorrect");
                b_FailPin15 == true;
                b_FailPin17 == true;
              }
            }
          }
        
       }
      
   if(AN9_PIN==LOW)
   { // Verify the pull-up is installed
     Serial.println("Digital Pin #5 (DB25-16) does not have a pull-up resistor");
   }
   else
   { // Resitor in place.  Verify the associated pin also has a resistor in place.
      if((AN13_PIN==LOW)
        {
           Serial.println("Digital Pin #5 (DB25-18) does not have a pull-up resistor");
        }
        // Proced with the test even if the pin is missing its pull-up.
        if(b_FailPin16 == false)
         {// Only conduct this test if it has not previously failed.
          pinMode(AN9_PIN,OUTPUT);
          digitalWrite(AN9_PIN,HIGH);
          if(AN13_PIN == HIGH)
            { // Should have already been high, but if Pull-up was missing, it would not be.
             digitalWrite(AN9_PIN,LOW);
              if(AN13_PIN == HIGH)
              { // If this pin is still high, something is wrong.
                pinMode(AN9_PIN,INPUT); // Turn the pin back to an input to reduce likelihood of damage.
                Serial.println("Connection between DB25-16 and DB25-18 is incorrect");
                b_FailPin16 == true;
                b_FailPin18 == true;
              }
            }
          }
        
       }
   }
}
*/

void Loop_Network_Button_Light(void)    // checks the RAW button reserved for networking, and outputs the light state.
{
  /*  This does not make sense at first, however by operating the network button and light
   *   through variables, the Zigbee is passed a pointer to the variables, and the actual 
   *   state of the light and button can be either hardwired, on a touchscreen, or even 
   *   completely remote through the Zigbee connection!  The variables are the virtual state
   *   of the input and output no matter what form that I/O takes.

   */
  static uint8_t _LastNetworkLight = 255;   // initialize the value as invalid so it will get the initial color updated.

//Network button
#ifdef __DISPLAY_ILI9341_T3__    // If there is a display defined, handle this routine differently
  #define _NETWORK_BUTTON_UPDATE_RATE 100  // this will update the network button even if its state does not change.
  static uint64_t t_network_button_update = millis();
  String _line2Text = "        ";

   if(t_network_button_update <= millis())
  {
    t_network_button_update = millis()+ _NETWORK_BUTTON_UPDATE_RATE;
    _LastNetworkLight = 255;
  btn_NETWORK();  //(z_Display_main.ino) checks the state of the virtual pushbutton on the touchscreen and feeds that to the button variable. 
  //Serial.println(WHAT_NetworkButton);
  }
#else      
  WHAT_NetworkButton = digitalRead(b_BUTTON_PIN);   // Read the state of the physical button designated for networking, and put it in the holding variable.
#endif
   
// Network Light   
   if(gbl_ActiveMenuNumber == gbl_MAIN_Display)
    {
      if(_LastNetworkLight != WHAT_NetworkLight )
      {
        _LastNetworkLight=WHAT_NetworkLight;
        switch(WHAT_NetworkLight)
        {
          case WHAT_NETWORK_NONE:
            //digitalWrite(b_RED_RGB_PIN,HIGH);   //Light off.
            //digitalWrite(b_GREEN_RGB_PIN,HIGH); 
            //digitalWrite(b_BLUE_RGB_PIN,HIGH);
            Show_btn_NETWORK(/*Color of the button*/ILI9341_DARKGREY, /*Color of the text*/ILI9341_BLACK, /*line1 Text*/ "Network",/*line2 Text*/ "  OFF   ");
            break;
          
         case WHAT_NETWORK_ERROR:
            //digitalWrite(b_RED_RGB_PIN,LOW);    // Red light.
            //digitalWrite(b_GREEN_RGB_PIN,HIGH); 
            //digitalWrite(b_BLUE_RGB_PIN,HIGH);
            Show_btn_NETWORK(/*Color of the button*/ILI9341_RED, /*Color of the text*/ILI9341_BLACK, /*line1 Text*/ "Network",/*line2 Text*/ "FAILED");
            break;
     
        case WHAT_NETWORK_ERROR_OFF:
            //digitalWrite(b_RED_RGB_PIN,HIGH);  //Light off (as part of a flashing light).
            //digitalWrite(b_GREEN_RGB_PIN,HIGH); 
            //digitalWrite(b_BLUE_RGB_PIN,HIGH);
            Show_btn_NETWORK(/*Color of the button*/ILI9341_DARK_RED, /*Color of the text*/ILI9341_BLACK, /*line1 Text*/ "Network",/*line2 Text*/ "FAILED");
            break;
              
        case WHAT_NETWORK_SEARCH_OFF:
            //digitalWrite(b_RED_RGB_PIN,HIGH); //Light off (as part of a flashing light).
            //digitalWrite(b_GREEN_RGB_PIN,HIGH); 
            //digitalWrite(b_BLUE_RGB_PIN,HIGH);
            Show_btn_NETWORK(/*Color of the button*/ILI9341_NAVY, /*Color of the text*/ILI9341_BLACK, /*line1 Text*/ "Network",/*line2 Text*/ " Search");
            break;
           case WHAT_NETWORK_SEARCH_ON:
            //digitalWrite(b_RED_RGB_PIN,HIGH); //Blue Light when a network is seen.
            //digitalWrite(b_GREEN_RGB_PIN,HIGH); 
            //digitalWrite(b_BLUE_RGB_PIN,LOW);
            Show_btn_NETWORK(/*Color of the button*/ILI9341_BLUE, /*Color of the text*/ILI9341_BLACK, /*line1 Text*/ "Network",/*line2 Text*/ " Search");
            break;
    
        case WHAT_NETWORK_SEEN_OFF:
            //digitalWrite(b_RED_RGB_PIN,HIGH); //Light off (as part of a flashing light).
            //digitalWrite(b_GREEN_RGB_PIN,HIGH); 
            //digitalWrite(b_BLUE_RGB_PIN,HIGH);
            Show_btn_NETWORK(/*Color of the button*/ILI9341_NAVY, /*Color of the text*/ILI9341_BLACK, /*line1 Text*/ "Network",/*line2 Text*/ "AVAILABLE");
            break;
           case WHAT_NETWORK_SEEN_ON:
            //digitalWrite(b_RED_RGB_PIN,HIGH); //Blue Light when a network is seen.
            //digitalWrite(b_GREEN_RGB_PIN,HIGH); 
            //digitalWrite(b_BLUE_RGB_PIN,LOW);
            Show_btn_NETWORK(/*Color of the button*/ILI9341_BLUE, /*Color of the text*/ILI9341_BLACK, /*line1 Text*/ "ONLINE",/*line2 Text*/ "AVAILABLE");
            break;
      
          case WHAT_NETWORK_CREATING_OFF:
            //digitalWrite(b_RED_RGB_PIN,HIGH);   //Solid blue light when creating the network
            //digitalWrite(b_GREEN_RGB_PIN,HIGH); 
            //digitalWrite(b_BLUE_RGB_PIN,HIGH);
            _line2Text = " " + String(myPANID,HEX) + " ";
            Show_btn_NETWORK(/*Color of the button*/ILI9341_NAVY, /*Color of the text*/ILI9341_BLACK, /*line1 Text*/ "Network",/*line2 Text*/_line2Text );
            break;
      
        case WHAT_NETWORK_CREATING_ON:
            //digitalWrite(b_RED_RGB_PIN,HIGH);  //Solid blue light when creating the network
            //digitalWrite(b_GREEN_RGB_PIN,HIGH); 
            //digitalWrite(b_BLUE_RGB_PIN,LOW);
            _line2Text = " " + String(myPANID,HEX) + " ";
            Show_btn_NETWORK(/*Color of the button*/ILI9341_BLUE, /*Color of the text*/ILI9341_BLACK, /*line1 Text*/ "Network",/*line2 Text*/ _line2Text);
            break;
    
        case WHAT_NETWORK_JOINING_OFF:
            //digitalWrite(b_RED_RGB_PIN,HIGH);  //Solid blue light when creating the network
            //digitalWrite(b_GREEN_RGB_PIN,HIGH); 
            //digitalWrite(b_BLUE_RGB_PIN,HIGH);
            _line2Text = " " + String(myPANID,HEX) + " ";
            Show_btn_NETWORK(/*Color of the button*/ILI9341_DARKGREY, /*Color of the text*/ILI9341_BLACK, /*line1 Text*/ "Joining",/*line2 Text*/ _line2Text);
            break;
     
        case WHAT_NETWORK_JOINING_ON:
            //digitalWrite(b_RED_RGB_PIN,HIGH);  //Solid GREEN light when joining the network
            //digitalWrite(b_GREEN_RGB_PIN,LOW); 
            //digitalWrite(b_BLUE_RGB_PIN,HIGH);
            _line2Text = " " + String(myPANID,HEX) + " ";
            Show_btn_NETWORK(/*Color of the button*/ILI9341_GREEN, /*Color of the text*/ILI9341_BLACK, /*line1 Text*/ "Joining",/*line2 Text*/ _line2Text);
            break;
    
        case WHAT_NETWORK_CREATED:
            //digitalWrite(b_RED_RGB_PIN,HIGH);  //Solid CYAN when creating the network
            //digitalWrite(b_GREEN_RGB_PIN,LOW); 
            //digitalWrite(b_BLUE_RGB_PIN,LOW);
            _line2Text = " " + String(myPANID,HEX) + " ";
            Show_btn_NETWORK(/*Color of the button*/ILI9341_CYAN, /*Color of the text*/ILI9341_BLACK, /*line1 Text*/ "Made Net",/*line2 Text*/ _line2Text);
            break;
    
        case WHAT_NETWORK_JOINED:
            //digitalWrite(b_RED_RGB_PIN,HIGH);  //Solid GREEN light when joining the network
            //digitalWrite(b_GREEN_RGB_PIN,LOW); 
            //digitalWrite(b_BLUE_RGB_PIN,HIGH);
            _line2Text = " " + String(myPANID,HEX) + " ";
            Show_btn_NETWORK(/*Color of the button*/ILI9341_DARKGREEN, /*Color of the text*/ILI9341_BLACK, /*line1 Text*/ " Online ",/*line2 Text*/ _line2Text);
            break;
    
        case WHAT_CHILD_REQUEST_OFF:
            //digitalWrite(b_RED_RGB_PIN,HIGH);  //Solid GREEN light when joining the network
            //digitalWrite(b_GREEN_RGB_PIN,HIGH); 
            //digitalWrite(b_BLUE_RGB_PIN,LOW);
            Show_btn_NETWORK(/*Color of the button*/ILI9341_NAVY, /*Color of the text*/ILI9341_WHITE, /*line1 Text*/ " CHILD ",/*line2 Text*/ "Request");
            break;
    
       case WHAT_CHILD_REQUEST_ON:
            //digitalWrite(b_RED_RGB_PIN,HIGH);  //Solid GREEN light when joining the network
            //digitalWrite(b_GREEN_RGB_PIN,HIGH); 
            //digitalWrite(b_BLUE_RGB_PIN,LOW);
            Show_btn_NETWORK(/*Color of the button*/ILI9341_BLUE, /*Color of the text*/ILI9341_WHITE, /*line1 Text*/ " CHILD ",/*line2 Text*/ "Request");
            break;
          
         case WHAT_NEW_CHILD_ALLOWED_OFF:
            //digitalWrite(b_RED_RGB_PIN,HIGH);  //Solid GREEN light when joining the network
            //digitalWrite(b_GREEN_RGB_PIN,HIGH); 
            //digitalWrite(b_BLUE_RGB_PIN,HIGH);
            Show_btn_NETWORK(/*Color of the button*/ILI9341_GREEN, /*Color of the text*/ILI9341_BLACK, /*line1 Text*/ " CHILD ",/*line2 Text*/ "Accepted");
            break;
    
         case WHAT_NEW_CHILD_ALLOWED_ON:
            //digitalWrite(b_RED_RGB_PIN,HIGH);  //Solid GREEN light when joining the network
            //digitalWrite(b_GREEN_RGB_PIN,LOW); 
            //digitalWrite(b_BLUE_RGB_PIN,HIGH);
            Show_btn_NETWORK(/*Color of the button*/ILI9341_GREEN, /*Color of the text*/ILI9341_BLACK, /*line1 Text*/ " CHILD ",/*line2 Text*/ "Accepted");
            break;
    
         case WHAT_LINKING_OFF:
            //digitalWrite(b_RED_RGB_PIN,HIGH);  //Solid GREEN light when joining the network
            //digitalWrite(b_GREEN_RGB_PIN,LOW); 
            //digitalWrite(b_BLUE_RGB_PIN,HIGH);
            Show_btn_NETWORK(/*Color of the button*/ILI9341_DARKGREEN, /*Color of the text*/ILI9341_WHITE, /*line1 Text*/ "Linking",/*line2 Text*/ "Pending");
            break;
    
         case WHAT_LINKING_ON:
            //digitalWrite(b_RED_RGB_PIN,HIGH);  //Solid GREEN light when joining the network
            //digitalWrite(b_GREEN_RGB_PIN,HIGH); 
            //digitalWrite(b_BLUE_RGB_PIN,LOW);
            Show_btn_NETWORK(/*Color of the button*/ILI9341_NAVY, /*Color of the text*/ILI9341_WHITE, /*line1 Text*/ "Linking",/*line2 Text*/ "Pending");
            break;
    
         case WHAT_LINKED:
            //digitalWrit(b_RED_RGB_PIN,HIGH);  //Solid GREEN light when joining the network
            //digitalWrite(b_GREEN_RGB_PIN,HIGH); 
            //digitalWrite(b_BLUE_RGB_PIN,HIGH);
            Show_btn_NETWORK(/*Color of the button*/ILI9341_DARKGREEN, /*Color of the text*/ILI9341_WHITE, /*line1 Text*/ " Linking ",/*line2 Text*/ "Accepted");
            break;
    
         case WHAT_LINKFAILED:
            //digitalWrite(b_RED_RGB_PIN,HIGH);  //Solid GREEN light when joining the network
            //digitalWrite(b_GREEN_RGB_PIN,HIGH); 
            //digitalWrite(b_BLUE_RGB_PIN,HIGH);
            Show_btn_NETWORK(/*Color of the button*/ILI9341_ORANGE, /*Color of the text*/ILI9341_BLACK, /*line1 Text*/ " Linking ",/*line2 Text*/ " Denied ");
            break;
    
         case WHAT_NET_ALIVE:
            break;
             
         default:
            Serial.print("Invalid Light State : 0x");Serial.println(WHAT_NetworkLight,HEX);
            break;   
        }
    }
    if(WHAT_NetworkLight == WHAT_NET_ALIVE)
    {
        #define _ALIVE_LONG 1000
        #define _ALIVE_SHORT 100
        static double _alive_timer = millis();
        static bool _b_lastAlive = false;

        if(_alive_timer <= millis())
        {
          if(_b_lastAlive)
          {
            _alive_timer = millis() + _ALIVE_SHORT;
            // light on for short time.
            //digitalWrite(b_RED_RGB_PIN,HIGH);  //Solid GREEN light when joining the network
            //digitalWrite(b_GREEN_RGB_PIN,HIGH); 
            //digitalWrite(b_BLUE_RGB_PIN,LOW);
            _line2Text = " " + String(myPANID,HEX) + " ";
            Show_btn_NETWORK(/*Color of the button*/ILI9341_BLUE, /*Color of the text*/ILI9341_BLACK, /*line1 Text*/ " NETWORK ",/*line2 Text*/ _line2Text);
           }
           else
           {
             _alive_timer = millis() + _ALIVE_LONG;
            // light off for long time.
            //digitalWrite(b_RED_RGB_PIN,HIGH);  //Solid GREEN light when joining the network
            //digitalWrite(b_GREEN_RGB_PIN,HIGH); 
            //digitalWrite(b_BLUE_RGB_PIN,HIGH);
            Show_btn_NETWORK(/*Color of the button*/ILI9341_NAVY, /*Color of the text*/ILI9341_BLACK, /*line1 Text*/ " NETWORK ",/*line2 Text*/ " Alive ");
           }
            _b_lastAlive = ! _b_lastAlive;
        }
    }

    
  }
}
