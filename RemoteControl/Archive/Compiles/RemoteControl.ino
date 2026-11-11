#define USED_EEPROM_VALUE 0x7C   // This value is stored in EEPROM_USED_FLAG_LOCATION to show this is not a fresh installation. 

/* W.H.A.T.
 *  REV: 200117
 *  ** Remote_WHAT **
 *  - MRF24J
 *  - Hardware: 

  This is focused on pairing 2 devices.
  
 *
 *  History:
 *  
 */

//#define TEENSY_ALARM_BOARD  1
#define NANO_REMOTE_BOARD  1


#include "z_WHAT.h"  // Wireless zigbee communications.
#include "z_WHAT_Command.h"      // zigbee communications definitions.
//#include "z_WHAT_FullLink.h"   // -FULL LINK routine- as part of the WHAT system.

#ifdef NANO_REMOTE_BOARD
   #define WHAT_CHIP_SELECT 10   // Hardware chip select pin for hardware 220319
#endif
//bool _WHAT_NetworkButton = false;      // Given to WHAT.ino to indicate the state of the physical or virtual netowrk button.
//WHAT_State _WHAT_network_State = WHAT_STARTUP;   // Obtained from WHAT.ino:  Indicates the current network status for displaying to operator.
//WHAT_LedColor _WHAT_network_StateColor = WHAT_LIGHT_OFF;    //Controlled in WHAT.ino to give a color indication to show device state.
//#define WHAT_CHIP_SELECT 7   // Hardware chip select pin for OLDER BOARDS (only a couple of boards)
Mrf24j mrf(WHAT_CHIP_SELECT);
WHAT_Packet WHAT_Tx_Packet;  // Make a temporary transmit packet

const char WHAT_Am_I[] PROGMEM  = {"WHAT Generic IO"};
const char WHAT_abilities[] PROGMEM  = {"3xAI, 3xDI, 3xDO, 1xRGB, 0xLED, 3xFET_LOW, 0xFET_HI "};

/*
/ **********************  NEO Pixels   *************************** /
// For led chips like Neopixels, which have a data line, ground, and power, you just
// need to define NEOPIXEL_DATA_PIN.  For led chipsets that are SPI based (four wires - data, clock,
// ground, and power), like the LPD8806 define both NEOPIXEL_DATA_PIN and CLOCK_PIN
#include <FastLED.h>
#define NEOPIXEL_DATA_PIN 5
// Define the array of leds
CRGB leds[1];    // LED0 is the one closest to the NANO.

#define NEO_COLOR_OFF  1,1,1    // Grey (MAX31856 Type 0000)
#define NEO_COLOR_RED  100,0,0  // Purple (MAX31856 Type 0001)
#define NEO_COLOR_GREEN  0,100,0    // Black  (MAX31856 Type 0010)
#define NEO_COLOR_BLUE  0,0,100  // Yellow (MAX31856 Type 0011)

const CRGB lightcolor[] PROGMEM= {CRGB(NEO_COLOR_OFF),
                                   CRGB(NEO_COLOR_RED),
                                   CRGB(NEO_COLOR_GREEN),
                                   CRGB(NEO_COLOR_BLUE)
                                  };
*/

// ************* Input/Output pin assignments *******************************
// **************************************************************************
// **************************************************************************
#ifdef NANO_REMOTE_BOARD
  //#define TMPTR1_PIN A7  //LM61
  #define o_FET1_PIN A0  //Active High Low side drive FET
  #define o_FET2_PIN A1 
  #define o_FET3_PIN A2 
  #define ANALOG1_PIN A3  // Analog input
  #define ANALOG2_PIN A4  
  #define ANALOG3_PIN A5
  #define DIGITAL1_PIN A3 // the analog inputs can also be used as digital.
  #define DIGITAL2_PIN A4
  #define DIGITAL3_PIN A5
  #define DIGITALxtra1 6
  #define DIGITALxtra2 7
  #define DIGITALxtra3 8
  #define DIGITALxtra4 9
  #define BATTERYMEASUREPIN A6
  //#define b_BUTTON_LIGHT_PIN A5
  #define b_BUTTON_PIN 2

  // ************* Onboard LEDs **@define****************************
  #define b_RED_RGB_PIN 5   // Remote zigbee hardware
  #define b_GREEN_RGB_PIN 4
  #define b_BLUE_RGB_PIN 3
  // ************* AtoD Calibration *******************************
 // #define MAX_A2D_VOLTS 0.833 // Call getSupplyVoltage routine to calculate this! 
  #define MAX_A2D_COUNTS 175.0
  //#define MAX_A2D_VOLTS 5.0   // Call getSupplyVoltage routine to calculate this! 
  //#define MAX_A2D_COUNTS 1023.0
  float analog_in1 = 999.9;
  float analog_in2 = 999.9;
  float analog_in3 = 999.9;
  bool b_o_FET1 = LOW;
  bool b_o_FET2 = LOW;
  bool b_o_FET3 = LOW;

  //    / ************    Setup for Temperature Sensor ****************** /
  //#define TMPTR_MAX_A2D_VOLTS 3.18    // Call getSupplyVoltage routine to calculate this!  // Teensy4.0 values
  //#define TMPTR_MAX_A2D_COUNTS 1024   // Teensy values
 // #define TMPTR_MAX_A2D_VOLTS 4.616      // Call getSupplyVoltage routine to calculate this! // Nano Values
  #define TMPTR_MAX_A2D_COUNTS 1023.0   // Nano Values

  #define TMPTR1_PIN A7  //LM61
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

  // ************* Supply voltage measurement *******************************
  float gbl_Volts = 0.0;  // Measured supply (battery) voltage.
  bool gbl_b_belowVthreshold = true; // Indicates batteries are discharging - no solar or charging voltage present.
  bool gbl_b_belowVshutdown = false; // When supply voltage drops below this, all outputs are turned off to conserve the battery.
 // #define VOLTAGECAL_A2D 940    //AtoD counts when the input voltage is at "VOLTAGECAL_VOLT" (below)
//  #define VOLTAGECAL_VOLT 15.0  // Voltage that creates the AtoD counts for "VOLTAGECAL_A2D" (above)
  #define A2D_to_VOLTS (getSupplyVoltage()/1024)
  #define VOLTAGETHRESHOLD 12.7 // Below this voltage, the batteries are discharging - no solar or charging voltage present.
//  #define VOLTAGESHUTOFF 11.0   // When supply voltage drops below this, all outputs are turned off to conserve the battery.
#endif

// *********************************************************************
// *********************************************************************
// *********************************************************************
void setup() {
  Serial.begin(115200);
  // put your setup code here, to run once:
  pinMode(BATTERYMEASUREPIN,INPUT);
  Setup_LEDs(); // LEDs are used in network setup.
  Setup_Button_Light(); // This is IF the button/light is installed.
   Loop_Network_Button_Light(); //(this routine) monitors the pushbutton.
//  WHAT_begin(/*bool *_network_button */ _WHAT_NetworkButton , /*bool *_network_light */_WHAT_network_State, /*uint_8 *NetLightColor*/_WHAT_network_StateColor);
 
  WHAT_begin();
  Serial.println("WHAT_Began");
  //Setup_ZigBee_Rx();

  // If this device will allow "full links... "
#ifdef __WHAT_FULLLINK_h
  WHAT_LINK_begin();
#endif

  //**********************  NEO Pixels   ***************************/
  //FastLED.addLeds<WS2811, NEOPIXEL_DATA_PIN, GRB>(leds, 1);   //****NOTE *** NEOPixels are GRB (NOT RGB)!!!!
  //leds[0] = lightcolor[0];
  //FastLED.show(); // Lamp test.  Show each possible color for an instant.
   
// ******** Local setup routines ********************** //  
  Setup_Tmptr();
  //Setup_Analog_In();
  //Setup_FET_Out();
  Setup_COMM(); // Register messages and other setup in there.
  #ifdef DX_ONLY
  Serial.print(F("_WHAT_network_StateColor:")); Serial.println(_WHAT_network_StateColor,DEC);
  Serial.print(F("_WHAT_network_State:")); Serial.println(_WHAT_network_State,DEC);
  #endif

}

// *********************************************************************
// *********************************************************************
// *********************************************************************
void loop() {
  // put your main code here, to run repeatedly:
  Loop_Network_Button_Light();    // checks the RAW button reserved for networking, and outputs the light state.
  Loop_WHAT(); // WHAT.ino -  Monitors and communicates for Wireless Home Automation and Telemetry
  //checkVoltage(); //(this routine)  Measures the voltage powering this device.
  Loop_Tmptr();
  Loop_SendData();
  // If this device will allow "full links... "
#ifdef __WHAT_FULLLINK_h
  Loop_WHAT_LINK();
#endif

//  Loop_MonitorButtonLight();
  //PeriodicTx();
//  slowUpdate();
}
// *********************************************************************
// ***************  Setup Routines *************************************
// *********************************************************************

void slowUpdate(void)
{
  static double _slowUpdateTimer = millis();

  if (_slowUpdateTimer <= millis())
  {
    #ifdef DX_ONLY
    Serial.print(F("_WHAT_network_State: ")); Serial.println(_WHAT_network_State,DEC);
    Serial.print(F("_WHAT_network_StateColor:")); Serial.println(_WHAT_network_StateColor,DEC);
    #endif
    _slowUpdateTimer = millis() + 5000;
  }

}


void Setup_Tmptr(void)
{
  pinMode(TMPTR1_PIN,INPUT);
}
/*
 * // *************
void Setup_Analog_In(void)
{
  pinMode(ANALOG1_PIN,INPUT);
  pinMode(ANALOG2_PIN,INPUT);
  pinMode(ANALOG3_PIN,INPUT);
}
// *************
void Setup_FET_Out(void)
{
  pinMode(o_FET1_PIN,OUTPUT);
  digitalWrite(o_FET1_PIN,LOW);  // FETs are active HIGH
  pinMode(o_FET2_PIN,OUTPUT);
  digitalWrite(o_FET2_PIN,LOW);  // FETs are active HIGH
  pinMode(o_FET3_PIN,OUTPUT);
  digitalWrite(o_FET3_PIN,LOW);  // FETs are active HIGH
}
*/
// *************
void Setup_LEDs(void)
{
  pinMode(b_RED_RGB_PIN,OUTPUT);
  pinMode(b_GREEN_RGB_PIN,OUTPUT);
  pinMode(b_BLUE_RGB_PIN,OUTPUT);
  digitalWrite(b_RED_RGB_PIN,HIGH);  // Lights are active LOW
  digitalWrite(b_GREEN_RGB_PIN,HIGH);
  digitalWrite(b_BLUE_RGB_PIN,HIGH);
}
// *************
void  Setup_Button_Light(void)
{
//  pinMode(b_BUTTON_LIGHT_PIN,OUTPUT);
//  digitalWrite(b_BUTTON_LIGHT_PIN,LOW); // Lights are active HIGH
  pinMode(b_BUTTON_PIN,INPUT_PULLUP);
}
// *************
void Setup_COMM(void)
{
  //RegisterZIGMessage(CMD_COLOR0n_04, &RxLightControl);
  //RegisterZIGMessage(CMD_FET01_04, &RxFETControl);
  //RegisterWHATMessage(CMD_COLOR0n_04, &RxLightControl);
  //RegisterWHATMessage(CMD_FET01_04, &RxFETControl);
  //RegisterWHATMessage(CMD_DIG01_04, &RxDigitalInput);
  
}
// *********************************************************************
// ***************  Loop Routines **************************************
// *********************************************************************
void Loop_Network_Button_Light(void)    // checks the RAW button reserved for networking, and outputs the light state.
{
  /*  This does not make sense at first, however by operating the network button and light
   *   through variables, the Zigbee is passed a pointer to the variables, and the actual 
   *   state of the light and button can be either hardwired, on a touchscreen, or even 
   *   completely remote through the Zigbee connection!  The variables are the virtual state
   *   of the input and output no matter what form that I/O takes.

   */

  static uint8_t _LastNetworkStateColor = WHAT_LIGHT_OFF;   // initialize the value as invalid so it will get the initial color updated.
#ifdef __DISPLAY_ILI9341_T3__    // If there is a display defined, handle this routine differently
  #define _NETWORK_BUTTON_UPDATE_RATE 1000  // this will update the network button even if its state does not change.
  static uint64_t t_network_button_update = millis();
  String _line2Text = "        ";

   if(t_network_button_update <= millis())
  {
    t_network_button_update = millis()+ _NETWORK_BUTTON_UPDATE_RATE;
    _LastNetworkStateColor = WHAT_LIGHT_OFF;
  }
  
  btn_NETWORK();  //(z_Display_main.ino) checks the state of the virtual pushbutton on the touchscreen and feeds that to the button variable. 
  //Serial.println("Virtual Network Button");

#else      
  _WHAT_NetworkButton = !digitalRead(b_BUTTON_PIN);   // Button is active LOW.  Read the state of the physical button designated for networking, and put it in the holding variable.
  //Serial.println("Physical Network Button");
#endif

  if(_LastNetworkStateColor != _WHAT_network_StateColor )
  {
    _LastNetworkStateColor = _WHAT_network_StateColor;
    switch(_WHAT_network_StateColor)
    {
      case WHAT_LIGHT_OFF:
        digitalWrite(b_RED_RGB_PIN,HIGH);   //Light off.
        digitalWrite(b_GREEN_RGB_PIN,HIGH); 
        digitalWrite(b_BLUE_RGB_PIN,HIGH);
        // Show_btn_NETWORK(/*Color of the button*/ILI9341_DARKGREY, /*Color of the text*/ILI9341_BLACK, /*line1 Text*/ "Network",/*line2 Text*/ "  OFF   ");
        break;
      
     case WHAT_LIGHT_RED:
        digitalWrite(b_RED_RGB_PIN,LOW);    // Red light.
        digitalWrite(b_GREEN_RGB_PIN,HIGH); 
        digitalWrite(b_BLUE_RGB_PIN,HIGH);
//        Show_btn_NETWORK(/*Color of the button*/ILI9341_RED, /*Color of the text*/ILI9341_BLACK, /*line1 Text*/ "Network",/*line2 Text*/ "FAILED");
        break;
 
    case WHAT_LIGHT_GREEN:
        digitalWrite(b_RED_RGB_PIN,HIGH);  //Light Green
        digitalWrite(b_GREEN_RGB_PIN,LOW); 
        digitalWrite(b_BLUE_RGB_PIN,HIGH);
        // Show_btn_NETWORK(/*Color of the button*/ILI9341_DARK_RED, /*Color of the text*/ILI9341_BLACK, /*line1 Text*/ "Network",/*line2 Text*/ "PENDING");
        break;
 
    case WHAT_LIGHT_BLUE:
        digitalWrite(b_RED_RGB_PIN,HIGH);  //Light Blue
        digitalWrite(b_GREEN_RGB_PIN,HIGH); 
        digitalWrite(b_BLUE_RGB_PIN,LOW);
        // Show_btn_NETWORK(/*Color of the button*/ILI9341_DARK_RED, /*Color of the text*/ILI9341_BLACK, /*line1 Text*/ "Network",/*line2 Text*/ "PENDING");
        break;
         
     default:
        Serial.print("Invalid Light State : 0x");Serial.println(_WHAT_network_StateColor,HEX);
        _WHAT_network_StateColor = WHAT_LIGHT_OFF;
        break;   
    }

  }
}
// *********************************************************************
// void checkVoltage(void)
// {
//   #define VOLTAGEDELAYRATE 1000
//   static double t_VOLTAGEDelay = millis();

//   if(t_VOLTAGEDelay <= millis())
//   {
//     t_VOLTAGEDelay = millis() + VOLTAGEDELAYRATE;
//     gbl_Volts = (analogRead(BATTERYMEASUREPIN) * A2D_to_VOLTS);
//       //Serial.print(volts);Serial.print("  ");
//       //Serial.println(analogRead(BATTERYMEASUREPIN));
//     if(gbl_Volts>VOLTAGETHRESHOLD)
//     {
//       //gbl_t_light_on_time = gbl_t_light_on_reset;
//       gbl_b_belowVthreshold = false;
//     }
//     else
//     {
//       gbl_b_belowVthreshold = true; // Indicate to the rest of the program that the timer should be active.
//     }
//     if(gbl_Volts<VOLTAGESHUTOFF)
//     {
//       gbl_b_belowVshutdown = true;    //
//       //gbl_purpleLight_Percent = 0;  // this takes the place of gbl_Light_ON
//       //gbl_buttonLight_Percent = 0;  // this is the brightness of the button backlight.        
//       //gbl_floorLight_Percent = 0;
//       //gbl_b_buttonBlinking = false;
//     }
//     else
//     {
//        gbl_b_belowVshutdown = false;  //If above the shutdown threshold, change the flag
//     }
//       //Serial.print("Voltage:");Serial.println(gbl_Volts);
//   }
// }

float getSupplyVoltage() {
    // Read 1.1V reference against AVcc
    // set the reference to Vcc and the multiplexer to the 1.1V internal bandgap
    #if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__)
        ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
    #elif defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega32U4RUM__)
        ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
        ADCSRB = 0; // Ensure MUX5 is cleared
    #endif

    delay(2); // Wait for Vref to settle
    ADCSRA |= _BV(ADSC); // Start conversion
    while (bit_is_set(ADCSRA, ADSC)); // Measuring...

    uint8_t low  = ADCL; // Must read ADCL first - it locks the registers
    uint8_t high = ADCH; // Unlocks both

    long result = (high << 8) | low;

    // Calculate Vcc (in Volts); 1125300 = 1.1V * 1023 * 1000
    // (You can tweak 1125300 slightly if your chip's specific internal 1.1V is actually 1.09V or 1.11V)
    float vcc = 1125300.0 / result; 
    return vcc / 1000.0; // Return Vcc in Volts
}

// *********************************************************************
void Loop_Tmptr(void)
{
  // This is for an LM61.  Vo = (+10 mV/°C x T °C) + 600 mV.
  //                       Tmptr = (Vo-600mV)/10mV/°C 
  //                       Vo = (5v/A2D_5V)* rawA2Dcounts  
  //                       so Tmptr(°C) = ((rawA2Dcounts * (5v/A2D_5V))-600mV)/10
  //  The datasheet also has a table of values.
  //  -30C = 0.3v       100C = 1.6v
  // This can be used in the MAP function.
  
 // ************ Setup for Temperature Sensor ****************** 
//#define TMPTR_MAX_A2D_VOLTS 4.5    // Call getSupplyVoltage routine to calculate this! // Nano Values
//#define TMPTR_MAX_A2D_COUNTS 500.0 // Nano Values
//#define TMPTR1_PIN A7              // LM61

// *********************************************************************
    #define TMPTR_DELAY 10000
    static unsigned long tmptr_delay = millis(); // Use unsigned long for millis()

    if(millis() >= tmptr_delay) 
    {
      
        tmptr_delay = millis() + TMPTR_DELAY; // Reset the timer

        // 1. Read the raw ADC value first
        uint16_t _rawA2Dcounts = analogRead(TMPTR1_PIN);
        Serial.print("Raw A2D:"); Serial.println(_rawA2Dcounts);

        // 2. Convert raw counts to Volts (using float math)
        float volts = (_rawA2Dcounts / TMPTR_MAX_A2D_COUNTS) * getSupplyVoltage();

        // 3. Convert Volts to Celsius based on LM61 formula: Voc = (10mV * C) + 600mV
        // Tmptr(°C) = (Volts - 0.6V) / 0.01V
        tmprtr_DegC_local_1 = (volts - 0.6) / 0.01;

        // 4. Convert Celsius to Fahrenheit correctly
        tmprtr_DegF_local_1 = (tmprtr_DegC_local_1 * 9.0 / 5.0) + 32.0;

        Serial.print("DegF:"); Serial.println(tmprtr_DegF_local_1);
        Serial.print("Supply Voltage:"); Serial.println(getSupplyVoltage());
    }
}


// ***************
void Loop_Analog_In(void)
{
  #define ANALOG_DELAY 1000  
  static double analog1_delay = millis();
  static double analog2_delay = millis() + 100;
  static double analog3_delay = millis() + 200;
  
  if(analog1_delay <= millis())
  {
  analog1_delay = millis() + ANALOG_DELAY;  // Reset the delay timer for this channel
  analog_in1 = map(analogRead(ANALOG1_PIN),0,MAX_A2D_COUNTS,0,getSupplyVoltage());
  // Stores Analog 1 voltage as volts for later broadcast.
  } 
  if(analog2_delay <= millis())
  {
  analog2_delay = millis() + ANALOG_DELAY;  // Reset the delay timer for this channel
  analog_in2 = map(analogRead(ANALOG2_PIN),0,MAX_A2D_COUNTS,0,getSupplyVoltage());
  // Stores Analog 2 voltage as volts for later broadcast.
  } 
  if(analog3_delay <= millis())
  {
 analog3_delay = millis() + ANALOG_DELAY;  // Reset the delay timer for this channel
  analog_in3 = map(analogRead(ANALOG3_PIN),0,MAX_A2D_COUNTS,0,getSupplyVoltage());
  // Stores Analog 3 voltage as volts for later broadcast.
  } 
}
// ***************
void Loop_FET_Out(void)
{
  #define o_FET_DELAY 1000  
  static double o_FET1_delay = millis();
  static double o_FET2_delay = millis() + 100;
  static double o_FET3_delay = millis() + 200;
  
  if(o_FET1_delay <= millis())
  {
  o_FET1_delay = millis() + o_FET_DELAY;  // Reset the delay timer for this channel
  digitalWrite(o_FET1_PIN,b_o_FET1);
  } 
  if(o_FET2_delay <= millis())
  {
  o_FET2_delay = millis() + o_FET_DELAY;  // Reset the delay timer for this channel
  digitalWrite(o_FET2_PIN,b_o_FET2);
  } 
  if(o_FET3_delay <= millis())
  {
  o_FET3_delay = millis() + o_FET_DELAY;  // Reset the delay timer for this channel
  digitalWrite(o_FET3_PIN,b_o_FET3);
  } 
}

// ***************
void Loop_MonitorButtonLight(void)
{
  // This will send an update of the button state every second, and immediately when it changes state.
  static bool _lastButtonState = false;
  #define _BUTTON_DELAY 5000 
  static double _button_delay = millis();
  #define _THIS_BUTTON_MASK 0b00000001
  //bool _thisButton = digitalRead(b_BUTTON_PIN);
  bool _thisButton = (analogRead(b_BUTTON_PIN)<500);
  //Serial.println(_thisButton);

  //digitalWrite(b_BUTTON_LIGHT_PIN, !_thisButton); // Reflect the button status on the light.
  if(_lastButtonState != _thisButton)
  { // The button state has changed, so update the message immediately
    _button_delay = millis(); // Reset the delay so this will update immediately.
    Serial.println("NewButton");
  }
  if(_button_delay <= millis())
  { // If it is time, send the state of the button as a stand alone message.
    _button_delay = millis()+_BUTTON_DELAY; // Reset the delay so this will update immediately.
    uint8_t _index =0;
    WHAT_Tx_Packet.payload[_index++] = 4;
    WHAT_Tx_Packet.payload[_index++] = CMD_DIG01_04;
    WHAT_Tx_Packet.payload[_index++] = _THIS_BUTTON_MASK;
    WHAT_Tx_Packet.payload[_index++] = _thisButton;
    
    WHAT_TransmitPacket(WHAT_Tx_Packet);
    digitalWrite(b_RED_RGB_PIN,!(bool)(_thisButton));

    //Serial.println("Tx Button");
    //mrf.send16(GLOBAL_DESTINATION, _payload);
  }
  _lastButtonState = _thisButton;
  //digitalWrite(b_BUTTON_LIGHT_PIN,_thisButton);
}

// ***************
void Loop_SendData(void)
{
  #define TRANSMIT_DELAY 50000
  static double _transmit_delay = millis();

  if(_transmit_delay <= millis())
  {
    WHAT_Packet _WHAT_Tx_Packet;  // Make a temporary transmit packet
    _WHAT_Tx_Packet.WHAT_DATA_MESSAGE; 
    _WHAT_Tx_Packet.pan_id = myPANID;
    _WHAT_Tx_Packet.destination = 0xFFFF;
    _WHAT_Tx_Packet.sourceaddress = mySourceAddress;
    uint8_t _index = 0;
    _transmit_delay = millis() + TRANSMIT_DELAY;  // reset the timer.
    // Serial.print("Channel:");Serial.println(mrf.read_long(MRF_RFCON0));
    //Build payload
    #define PAYLOAD_SIZE 4
    _WHAT_Tx_Packet.payload[_index++] = PAYLOAD_SIZE; // The total payload length is captured in the first byte of the payload.
 
    // Send the temperature from the local sensor:
    _WHAT_Tx_Packet.payload[_index++] = 3;        // Length of the temperature message
    _WHAT_Tx_Packet.payload[_index++] = TEMPERATURE01_DEG_C_03;  //MID 
    _WHAT_Tx_Packet.payload[_index++] = (int8_t)tmprtr_DegC_local_1;   // Data Value - DegC as a SIGNED INTEGER
    Serial.print("TxTemperature ");Serial.println(tmprtr_DegC_local_1);
    Serial.print("DegF:");Serial.println(tmprtr_DegF_local_1);
/*     
    // Send Analog In 1 (order of messages does not matter)
    _WHAT_Tx_Packet.payload[_index++] = 6;                    // Length of analog in1
    _WHAT_Tx_Packet.payload[_index++] = REPLY_ANALOG01_06;  //MID 
    float2byte.myFloat = analog_in1;
    _WHAT_Tx_Packet.payload[_index++] = float2byte.myBytes[0];   // Data Value
    _WHAT_Tx_Packet.payload[_index++] = float2byte.myBytes[1];   // Data Value
    _WHAT_Tx_Packet.payload[_index++] = float2byte.myBytes[2];   // Data Value
    _WHAT_Tx_Packet.payload[_index++] = float2byte.myBytes[3];   // Data Value

    // Send Analog In 2 (order of messages does not matter)
    _WHAT_Tx_Packet.payload[_index++] = 6;                    // Length of analog in1
    _WHAT_Tx_Packet.payload[_index++] = REPLY_ANALOG01_06;  //MID 
    float2byte.myFloat = analog_in2;
    _WHAT_Tx_Packet.payload[_index++] = float2byte.myBytes[0];   // Data Value
    _WHAT_Tx_Packet.payload[_index++] = float2byte.myBytes[1];   // Data Value
    _WHAT_Tx_Packet.payload[_index++] = float2byte.myBytes[2];   // Data Value
    _WHAT_Tx_Packet.payload[_index++] = float2byte.myBytes[3];   // Data Value
    
    // Send Analog In 3 (order of messages does not matter)
    _WHAT_Tx_Packet.payload[_index++] = 6;                    // Length of analog in1
    _WHAT_Tx_Packet.payload[_index++] = REPLY_ANALOG01_06;  //MID 
    float2byte.myFloat = analog_in3;
    _WHAT_Tx_Packet.payload[_index++] = float2byte.myBytes[0];   // Data Value
    _WHAT_Tx_Packet.payload[_index++] = float2byte.myBytes[1];   // Data Value
    _WHAT_Tx_Packet.payload[_index++] = float2byte.myBytes[2];   // Data Value
    _WHAT_Tx_Packet.payload[_index++] = float2byte.myBytes[3];   // Data Value

   // Send digital information (pack bits into a single byte)
    _WHAT_Tx_Packet.payload[_index++] = 3;            // Length of analog in1
    _WHAT_Tx_Packet.payload[_index++] = REPLY_DIG01_03;  //MID 
    _WHAT_Tx_Packet.payload[_index++] = (digitalRead(DIGITAL1_PIN)+(digitalRead(DIGITAL2_PIN)<<1)+(digitalRead(DIGITAL3_PIN)<<2) + (digitalRead(DIGITAL3_PIN)<<2));

   // Send FET state as part of the transmission (pack bits into a single byte)
    _WHAT_Tx_Packet.payload[_index++] = 3;            // Length of analog in1
    _WHAT_Tx_Packet.payload[_index++] = REPLY_FET01_03;  //MID 
    _WHAT_Tx_Packet.payload[_index++] = (b_o_FET1+(b_o_FET2<<1)+(b_o_FET3<<2));

    // Send LED state (pack bits into a single byte)
    _WHAT_Tx_Packet.payload[_index++] = 4;            // Length of analog in1
    _WHAT_Tx_Packet.payload[_index++] = REPLY_COLOR01_0n;  //MID 
    _WHAT_Tx_Packet.payload[_index++] = 01;  // Number of LEDs
    _WHAT_Tx_Packet.payload[_index++] = 01;  // THIS LED number
    // This is all-on for each color, so if this color is active, set all its bits ON in the 16 bit color, otherwise, turn those bits off. 
    _WHAT_Tx_Packet.payload[_index++] = (((!digitalRead(b_RED_RGB_PIN)*0b11111)<<12) + ((!digitalRead(b_GREEN_RGB_PIN)*0b111111)<<5) + ((!digitalRead(b_BLUE_RGB_PIN)*0b11111)));  // LED number
    _WHAT_Tx_Packet.payload[_index++] = 0xFF;
*/
    // Now that the payload is filled, send the message.
    WHAT_TransmitPacket(_WHAT_Tx_Packet);
    Serial.println("sent message");
  }
}
/*
void PeriodicTx (void)
{
  #define BCAST_RATE 5000
  static double _Broadcast_Time = millis();
  static uint8_t _datavalue = 0;
  
  if(_Broadcast_Time <= millis())
  {
    _Broadcast_Time = millis() + BCAST_RATE;
    uint8_t _index =0;
    WHAT_Tx_Packet.payload[_index++] = 3;
    WHAT_Tx_Packet.payload[_index++] = _datavalue++;
    WHAT_Tx_Packet.payload[_index++] = 0xFF;
    WHAT_TransmitPacket(WHAT_Tx_Packet);
    //Serial.println(Mrf24j_Read(MRF24J_BBREG2),HEX);

    Serial.println("Transmitting");

    uint8_t _payload[4];
    
    _payload[0] = 3;
    _payload[1] = NETWORK_BUTTON_02;
    _payload[2] = _datavalue;
    _payload[3] = 0xFF;
    mrf.send16(GLOBAL_DESTINATION, _payload);

  }
}
*/
// *********************************************************************
// ***************  WHAT Rx Routines *********************************
// *********************************************************************
/*
//void RxLightControl (ZIG_Packet _ZigBee_Rx_Packet,uint8_t _index)
void RxLightControl (WHAT_Packet _WHAT_Rx_Packet,uint8_t _index)
{
  // This is a Light control MID
  // Structure is #bytes, MID, LED number, 16Bit color, LED number, 16Bit color,...
  uint8_t _bytes_left = _WHAT_Rx_Packet.payload[COMMAND_NUM_BYTES] - 2;  // 
  uint8_t _commandIndex = 0; // This will track which light entry is being evaluated while the command is evaluated.
  uint8_t _LED_Number = _WHAT_Rx_Packet.payload[COMMAND_DB1];
  uint16_t _LED_COLOR = _WHAT_Rx_Packet.payload[COMMAND_DB2]*0x100 + _WHAT_Rx_Packet.payload[COMMAND_DB3];

  Serial.println("Rx LightControl");

  // This hardware only supports 1 LED (number 1) with red, green, blue and annode pins.
  while(_bytes_left >1)
  { // Step thru all the commanded LED numbers and see if any target LED #1 (for this hardware)
    if(_LED_Number == 1)
    {
      // This singles out each color, and if ANY portion of that color is active, the LED turns on, since I am not PWMing the pins.
      digitalWrite(b_RED_RGB_PIN,!((_LED_COLOR & 0b1111100000000000) > 0b0000010000000000)); // If red portion of color has any value, turn it on.
      digitalWrite(b_GREEN_RGB_PIN,!(((_LED_COLOR & 0b0000011111100000) < 0b0000100000000000) & ((_LED_COLOR & 0b0000011111100000) > 0b0000000000010000)));
      digitalWrite(b_BLUE_RGB_PIN,!((_LED_COLOR & 0b0000000000011111) > 0b0000000000010000));
    }

    _bytes_left -=2;
    if(_bytes_left>0)
    {
    _commandIndex +=2;
    _LED_Number = _WHAT_Rx_Packet.payload[COMMAND_DB1 + _commandIndex];
    _LED_COLOR = _WHAT_Rx_Packet.payload[COMMAND_DB2 + _commandIndex]*0x100 + _WHAT_Rx_Packet.payload[COMMAND_DB3 + _commandIndex];
    }
  }
}

//void RxFETControl (ZIG_Packet _ZigBee_Rx_Packet,uint8_t _index)
void RxFETControl (WHAT_Packet _WHAT_Rx_Packet,uint8_t _index)
{
  Serial.println("Rx FET");

  // This is a FET control MID
  b_o_FET1 = (_WHAT_Rx_Packet.payload[COMMAND_DB1] && 0b00000001);     // FETs are active LOW.
  b_o_FET2 = (_WHAT_Rx_Packet.payload[COMMAND_DB1] && 0b00000010)>>1;
  b_o_FET3 = (_WHAT_Rx_Packet.payload[COMMAND_DB1] && 0b00000100)>>2;
}

void RxDigitalInput(WHAT_Packet _WHAT_Rx_Packet,uint8_t _index)
{
    // This is specifically written to act upon the remote digital input,
    //  which for this project will be DIGITAL 0.
    #define _BUTTON_MASK 0b00000001
    Serial.println("DigitalControl");
    Serial.print(" _INDEX : ");Serial.println(_index);
    Serial.print(" payload[_index] : ");Serial.println(_WHAT_Rx_Packet.payload[_index],HEX);
    Serial.print(" payload[_index +1] : ");Serial.println(_WHAT_Rx_Packet.payload[_index+1],HEX);
    Serial.print(" payload[_index +2] : ");Serial.println(_WHAT_Rx_Packet.payload[_index+2],HEX);
    Serial.print(" payload[_index +3] : ");Serial.println(_WHAT_Rx_Packet.payload[_index+3],HEX);
    Serial.print(" payload[_index +4] : ");Serial.println(_WHAT_Rx_Packet.payload[_index+4],HEX);
    
    if((_WHAT_Rx_Packet.payload[_index+3] & _BUTTON_MASK))
    {
      digitalWrite(o_FET1_PIN,HIGH);
    }
    else
    {
      digitalWrite(o_FET1_PIN,LOW);
    }
}
*/
