
#include "z_KeypadEntry.h"  // Allows number pad entry of values.

// ****************************************************************************************** //   
// ****************************************************************************************** //   
// ****************************************************************************************** //   
// ****************************************************************************************** //   
// *********** Add individual SETUP entries from each simulation HERE *********************** //

void Setup_KEYPAD_Display(void)
{
  KEYPAD_DisplayNum = MakeNewDisplay(KEYPAD_SCREENCOLOR,&ShowDisplay_KEYPAD);
  Serial.printf("Registered display %d as -Keypad- \n", KEYPAD_DisplayNum);
}

void KEYPAD_Display_Loop(void)
{
  if(gbl_ActiveMenuNumber == KEYPAD_DisplayNum)
  {
//    #define _KEYPAD_LOOP_RATE 100
//    double _t_KEYPAD_loop = millis();
//if(_t_KEYPAD_loop<=millis())
//    {
//        _t_KEYPAD_loop = millis()+_KEYPAD_LOOP_RATE;
        Loop_KEYPADButtons();// Keypad.ino
//    }
  }
}

// ****************************************************************************************** //   
// ****************************************************************************************** //   
// ****************************************************************************************** //   
#if (SCREEN_ROTATION == 0) | (SCREEN_ROTATION == 2) // Landscape
  #define KP_COLUMN1 60
#endif
#if (SCREEN_ROTATION == 1) | (SCREEN_ROTATION == 3) //Portrait
    #define KP_COLUMN1 30
#endif
#define KP_COLUMN2 (KP_COLUMN1 + KP_BUTTON_WIDTH + KP_BUTTON_SPACEX)
#define KP_COLUMN3 (KP_COLUMN2 + KP_BUTTON_WIDTH + KP_BUTTON_SPACEX)
#define KP_ROW1 50
#define KP_ROW2 (KP_ROW1 + KP_BUTTON_HEIGHT + KP_BUTTON_SPACEY)
#define KP_ROW3 (KP_ROW2 + KP_BUTTON_HEIGHT + KP_BUTTON_SPACEY)
#define KP_ROW4 (KP_ROW3 + KP_BUTTON_HEIGHT + KP_BUTTON_SPACEY)
#define KP_ROW5 (KP_ROW4 + KP_BUTTON_HEIGHT + KP_BUTTON_SPACEY)
#define KP_ROW6 (KP_ROW5 + KP_BUTTON_HEIGHT + KP_BUTTON_SPACEY)

// ****************************************************************************************** //   
// ****************************************************************************************** //   
// ****************************************************************************************** //   
void KeyPad_Invocation(/*Hexi or Deci*/ bool _hex,/*Number type*/ enum _KEYPAD_DATATYPE_ _myDatatype, /*Variable being entered*/void * _ptr_keypad_value)
{
  Display_ClearScreen(KEYPAD_SCREENCOLOR);
  // Any number value can call this, and pass the current value to it.
  // The pointer is of type "void" so it can be ANY type.  The enum must identify the type to avoid errors.
  f_hexidecimal_value = _hex;  // Calling routine can request initial display in decimal or hexidecimal.
  f_floatvalue = (bool)(_myDatatype == KEYPAD_FLOAT);  // This only allows the decimal point to show for floating point numbers.
 
  keypad_uints = _myDatatype; // Capture the incoming TYPE of data (since the void pointer does not tell it).
  ptr_keypad_value = _ptr_keypad_value;    // make a copy of the pointer to the data to be changed.
  float * _temp = (float *)_ptr_keypad_value;
  keypad_value_initial = *_temp;    // Hold the value that existed when this display was envoked.
  Serial.println(keypad_value_initial);
  keypad_working_value = keypad_value_initial;    // Also make a working copy of that value.
 
  keypad_previous_menu = gbl_ActiveMenuNumber;  // This menu number can be used to return to the calling menu.

  new_Touch = false;  // new_Touch will be reset to false every program loop.
  gbl_ActiveMenuNumber = KEYPAD_DisplayNum;  // this points the touchscreen routine to register touchpoints for THIS simulation
  LoadDefaultKEYPADButtons(_hex);
  displayKeypadValue();
}

void ShowDisplay_KEYPAD(void)
{
  new_Touch = false;  // new_Touch will be reset to false every program loop.
  Display_ClearScreen(KEYPAD_SCREENCOLOR);

  LoadDefaultKEYPADButtons(DECIMAL);         // Loads different values for KEYPAD buttons (locations and colors)
}


// **************************************************************************************************************************
//                                        BUTTON HANDLER
// **************************************************************************************************************************

void Update_keypad_Value (uint8_t _buttonNumber)
{
    //Serial.println(keypad_working_value);
    // All of the keypad buttons are registered to call this.
    // This must take any previous value, shift it by 10 or 0x10. 
    if(keypad_working_value<99999)
    { // Do not increase the value to 6 digits
      if(f_hexidecimal_value)
      { // Display is showing hexidecimal values.
        keypad_working_value *= 0x10;    // Shift left by 1 hexidecimal digit
        if (_buttonNumber < 16) // If the "0" button is pressed, this is the value 16, and it is already added by shifting the displayed value.
          {
            keypad_working_value += _buttonNumber;
          }
      }
      else 
      {    
        if( ! f_decimal_entry)
        { // The decimal is not being used yet.
          keypad_working_value *= 10;   // Shift left by 1 decimal digit
          if (_buttonNumber < 10) // If the "0" button is pressed, this is the value 16, and it is already added by shifting the displayed value.
            {
              keypad_working_value += _buttonNumber;
            }
        }
        else
        { // This is a decimal entry value.  Shift the multiplier and add this value.
          keypad_working_value += ((float)_buttonNumber * DecimalEntry);
          DecimalEntry *= 0.1;    // Another decimal position to the right.
        }
      }
      displayKeypadValue();
    }
}

void  displayKeypadValue(void)
{
  #define _KEYPADVALUE_X KP_COLUMN1 + 10
  #define _KEYPADVALUE_Y 10
  #define _KEYPADVALUE_H 30
  #define _KEYPADVALUE_W 150
  #define _KEYPADVALUE_COLOR ILI9341_LIGHTGREY
  #define _KEYPADVALUE_TEXT_COLOR ILI9341_DARKGREY
  #define _KEYPADVALUE_DEC_COLOR ILI9341_DARKCYAN
  #define _KEYPADVALUE_HEX_COLOR ILI9341_DARKGREEN
  #define _KEYPADVALUE_TEXT_SIZE 3
  char _printableValue[20];
  
  // Blank the location where the previous value was displayed.
  tft.fillRect(_KEYPADVALUE_X, _KEYPADVALUE_Y,_KEYPADVALUE_W, _KEYPADVALUE_H, ColorInversion^_KEYPADVALUE_COLOR); //A RED box, for now.
  tft.setTextSize(_KEYPADVALUE_TEXT_SIZE);   
  tft.setCursor(_KEYPADVALUE_X + 5, _KEYPADVALUE_Y + 5);  // Move the cursor to this location for printing.

  //if (f_negative_number) {tft.print("-");}   // Indicate which display was activated.    

    if(f_hexidecimal_value)
    {
      tft.setTextColor(_KEYPADVALUE_HEX_COLOR);            
      sprintf (_printableValue,"0x%4x",(uint16_t)keypad_working_value);
    }
    else
    {
      tft.setTextColor(_KEYPADVALUE_DEC_COLOR);            
      if(f_floatvalue)
      {
        sprintf (_printableValue,"%.1f",keypad_working_value);
      }
      else 
      {
        sprintf (_printableValue,"%ld",(uint32_t)keypad_working_value);    
      }
    }
    tft.print(_printableValue);                     // Indicate which display was activated.    
}

void Update_keypad_Plus_Minus (uint8_t _buttonNumber)
{
  // this allows negative numbers
  // Toggle the negative flag.
  //When the value is accepted, if this bit is set, the number becomes 2s compliment.
   keypad_working_value *= -1; 
   displayKeypadValue();
}

void Update_keypad_Decimal (uint8_t _buttonNumber)
{
  // this allows negative numbers
  // Toggle the negative flag.
    if(!f_hexidecimal_value)
    {
      if (DecimalEntry == 0) DecimalEntry = 0.1;
    }
  displayKeypadValue();
}
void Update_keypad_Clear (uint8_t _buttonNumber)
{
  // this allows negative numbers
  // Toggle the negative flag.
    keypad_working_value = 0;
    DecimalEntry = 0;
    displayKeypadValue();
}
void Update_keypad_HexDec (uint8_t _buttonNumber)
{
  // this allows negative numbers
  // Toggle the negative flag.
  f_hexidecimal_value = (!f_hexidecimal_value);
  if(f_hexidecimal_value)
    {
    LoadDefaultKEYPADButtons(f_hexidecimal_value);
    DecimalEntry = 0;
    }
    displayKeypadValue();
}


//void KeypadInterruptHome(uint8_t _ButtonNumber)
//{
//  *ptr_keypad_value = keypad_value_initial; // When the Home key is pressed, restore the previous value that was sent here.
//  gbl_ActiveMenuNumber = keypad_previous_menu;  // this points the touchscreen routine to register touchpoints for THIS simulation
////ShowHomeScreen(gbl_ActiveMenuNumber);        // Return to the previous menu.
//}

// Show_btn_KEY1(/*Color of the button*/ILI9341_GREEN, /*Color of the text*/ILI9341_WHITE, /*line1 Text*/ String "Power is",/*line2 Text*/ String "  ON ");
#define _BTN_KEY1_X KP_COLUMN1    //"X" coordinate of Top Left Corner of button
#define _BTN_KEY1_Y KP_ROW1       //"Y" coordinate of Top Left Corner of button
#define _BTN_KEY1_W KP_BUTTON_WIDTH
#define _BTN_KEY1_H KP_BUTTON_HEIGHT
//#define _BTN_KEY1_COLOR ILI9341_DARKGREEN
//#define _BTN_KEY1_TEXTCOLOR BLACK
#define _BTN_KEY1_TEXTSIZE 2
#define _BTN_KEY1_LINE1TEXT_X KP_NUMBER_OFFSETX
#define _BTN_KEY1_LINE1TEXT_Y KP_NUMBER_OFFSETY
  //This will re-draw the button whenever called.
void Show_btn_KEY1(/*Color of the button*/uint16_t _buttonColor, /*Color of the text*/uint16_t _textColor, /*line1 Text*/ String _line1Text)
{
    tft.fillRect(_BTN_KEY1_X, _BTN_KEY1_Y,KP_BUTTON_WIDTH, KP_BUTTON_HEIGHT, _buttonColor); //A red box, for now.
    tft.setCursor(_BTN_KEY1_X+KP_NUMBER_OFFSETX,_BTN_KEY1_Y+KP_NUMBER_OFFSETY);   // Move the cursor to this location for printing.
    tft.setTextSize(KP_BTN_KEY_TEXTSIZE);   
    tft.setTextColor(_textColor);            
    tft.print(_line1Text);                         
}
void btn_KEY1(void)
{
  if(Check_new_touch(_BTN_KEY1_X,_BTN_KEY1_Y,KP_BUTTON_WIDTH,KP_BUTTON_HEIGHT))
  { // The display area with this button has been touched this program cycle.
    Update_keypad_Value (1);
  }
}

// Show_btn_KEY2(/*Color of the button*/ILI9341_GREEN, /*Color of the text*/ILI9341_WHITE, /*line1 Text*/ String "Power is",/*line2 Text*/ String "  ON ");
// *********** Main Screeen button to change to Cold Boot menu *******************
#define _BTN_KEY2_X KP_COLUMN2   //"X" coordinate of Top Left Corner of button
#define _BTN_KEY2_Y KP_ROW1     //"Y" coordinate of Top Left Corner of button
  //This will re-draw the button whenever called.
void Show_btn_KEY2(/*Color of the button*/uint16_t _buttonColor, /*Color of the text*/uint16_t _textColor, /*line1 Text*/ String _line1Text)
{
    tft.fillRect(_BTN_KEY2_X, _BTN_KEY2_Y,KP_BUTTON_WIDTH, KP_BUTTON_HEIGHT, _buttonColor); //A red box, for now.
    tft.setCursor(_BTN_KEY2_X+KP_NUMBER_OFFSETX,_BTN_KEY2_Y+KP_NUMBER_OFFSETY);   // Move the cursor to this location for printing.
    tft.setTextSize(KP_BTN_KEY_TEXTSIZE);   
    tft.setTextColor(_textColor);            
    tft.print(_line1Text);                         
}
void btn_KEY2(void)
{
  if(Check_new_touch(_BTN_KEY2_X,_BTN_KEY2_Y,KP_BUTTON_WIDTH,KP_BUTTON_HEIGHT))
  { // The display area with this button has been touched this program cycle.
    Update_keypad_Value (2);
  }
}

// Show_btn_KEY3(/*Color of the button*/ILI9341_GREEN, /*Color of the text*/ILI9341_WHITE, /*line1 Text*/ String "Power is",/*line2 Text*/ String "  ON ");
// *********** Main Screeen button to change to Cold Boot menu *******************
#define _BTN_KEY3_X KP_COLUMN3    //"X" coordinate of Top Left Corner of button
#define _BTN_KEY3_Y KP_ROW1       //"Y" coordinate of Top Left Corner of button
  //This will re-draw the button whenever called.
void Show_btn_KEY3(/*Color of the button*/uint16_t _buttonColor, /*Color of the text*/uint16_t _textColor, /*line1 Text*/ String _line1Text)
{
    tft.fillRect(_BTN_KEY3_X, _BTN_KEY3_Y,KP_BUTTON_WIDTH, KP_BUTTON_HEIGHT, _buttonColor); //A red box, for now.
    tft.setCursor(_BTN_KEY3_X+KP_NUMBER_OFFSETX,_BTN_KEY3_Y+KP_NUMBER_OFFSETY);   // Move the cursor to this location for printing.
    tft.setTextSize(KP_BTN_KEY_TEXTSIZE);   
    tft.setTextColor(_textColor);            
    tft.print(_line1Text);                         
}
void btn_KEY3(void)
{
  if(Check_new_touch(_BTN_KEY3_X,_BTN_KEY3_Y,KP_BUTTON_WIDTH,KP_BUTTON_HEIGHT))
  { // The display area with this button has been touched this program cycle.
    Update_keypad_Value (3);
  }
}

// Show_btn_KEY4(/*Color of the button*/ILI9341_GREEN, /*Color of the text*/ILI9341_WHITE, /*line1 Text*/ String "Power is",/*line2 Text*/ String "  ON ");
// *********** Main Screeen button to change to Cold Boot menu *******************
#define _BTN_KEY4_X KP_COLUMN1    //"X" coordinate of Top Left Corner of button
#define _BTN_KEY4_Y KP_ROW2       //"Y" coordinate of Top Left Corner of button
  //This will re-draw the button whenever called.
void Show_btn_KEY4(/*Color of the button*/uint16_t _buttonColor, /*Color of the text*/uint16_t _textColor, /*line1 Text*/ String _line1Text)
{
    tft.fillRect(_BTN_KEY4_X, _BTN_KEY4_Y,KP_BUTTON_WIDTH, KP_BUTTON_HEIGHT, _buttonColor); //A red box, for now.
    tft.setCursor(_BTN_KEY4_X+KP_NUMBER_OFFSETX,_BTN_KEY4_Y+KP_NUMBER_OFFSETY);   // Move the cursor to this location for printing.
    tft.setTextSize(KP_BTN_KEY_TEXTSIZE);   
    tft.setTextColor(_textColor);            
    tft.print(_line1Text);                         
}
void btn_KEY4(void)
{
  if(Check_new_touch(_BTN_KEY4_X,_BTN_KEY4_Y,KP_BUTTON_WIDTH,KP_BUTTON_HEIGHT))
  { // The display area with this button has been touched this program cycle.
    Update_keypad_Value (4);
  }
}

// Show_btn_KEY5(/*Color of the button*/ILI9341_GREEN, /*Color of the text*/ILI9341_WHITE, /*line1 Text*/ String "Power is",/*line2 Text*/ String "  ON ");
// *********** Main Screeen button to change to Cold Boot menu *******************
#define _BTN_KEY5_X KP_COLUMN2    //"X" coordinate of Top Left Corner of button
#define _BTN_KEY5_Y KP_ROW2       //"Y" coordinate of Top Left Corner of button
  //This will re-draw the button whenever called.
void Show_btn_KEY5(/*Color of the button*/uint16_t _buttonColor, /*Color of the text*/uint16_t _textColor, /*line1 Text*/ String _line1Text)
{
    tft.fillRect(_BTN_KEY5_X, _BTN_KEY5_Y,KP_BUTTON_WIDTH, KP_BUTTON_HEIGHT, _buttonColor); //A red box, for now.
    tft.setCursor(_BTN_KEY5_X+KP_NUMBER_OFFSETX,_BTN_KEY5_Y+KP_NUMBER_OFFSETY);   // Move the cursor to this location for printing.
    tft.setTextSize(KP_BTN_KEY_TEXTSIZE);   
    tft.setTextColor(_textColor);            
    tft.print(_line1Text);                         
}
void btn_KEY5(void)
{
  if(Check_new_touch(_BTN_KEY5_X,_BTN_KEY5_Y,KP_BUTTON_WIDTH,KP_BUTTON_HEIGHT))
  { // The display area with this button has been touched this program cycle.
    Update_keypad_Value (5);
  }
}

// Show_btn_KEY6(/*Color of the button*/ILI9341_GREEN, /*Color of the text*/ILI9341_WHITE, /*line1 Text*/ String "Power is",/*line2 Text*/ String "  ON ");
// *********** Main Screeen button to change to Cold Boot menu *******************
#define _BTN_KEY6_X KP_COLUMN3    //"X" coordinate of Top Left Corner of button
#define _BTN_KEY6_Y KP_ROW2       //"Y" coordinate of Top Left Corner of button
  //This will re-draw the button whenever called.
void Show_btn_KEY6(/*Color of the button*/uint16_t _buttonColor, /*Color of the text*/uint16_t _textColor, /*line1 Text*/ String _line1Text)
{
    tft.fillRect(_BTN_KEY6_X, _BTN_KEY6_Y,KP_BUTTON_WIDTH, KP_BUTTON_HEIGHT, _buttonColor); //A red box, for now.
    tft.setCursor(_BTN_KEY6_X+KP_NUMBER_OFFSETX,_BTN_KEY6_Y+KP_NUMBER_OFFSETY);   // Move the cursor to this location for printing.
    tft.setTextSize(KP_BTN_KEY_TEXTSIZE);   
    tft.setTextColor(_textColor);            
    tft.print(_line1Text);                         
}
void btn_KEY6(void)
{
  if(Check_new_touch(_BTN_KEY6_X,_BTN_KEY6_Y,KP_BUTTON_WIDTH,KP_BUTTON_HEIGHT))
  { // The display area with this button has been touched this program cycle.
    Update_keypad_Value (6);
  }
}

// Show_btn_KEY7(/*Color of the button*/ILI9341_GREEN, /*Color of the text*/ILI9341_WHITE, /*line1 Text*/ String "Power is",/*line2 Text*/ String "  ON ");
// *********** Main Screeen button to change to Cold Boot menu *******************
#define _BTN_KEY7_X KP_COLUMN1    //"X" coordinate of Top Left Corner of button
#define _BTN_KEY7_Y KP_ROW3       //"Y" coordinate of Top Left Corner of button
  //This will re-draw the button whenever called.
void Show_btn_KEY7(/*Color of the button*/uint16_t _buttonColor, /*Color of the text*/uint16_t _textColor, /*line1 Text*/ String _line1Text)
{
    tft.fillRect(_BTN_KEY7_X, _BTN_KEY7_Y,KP_BUTTON_WIDTH, KP_BUTTON_HEIGHT, _buttonColor); //A red box, for now.
    tft.setCursor(_BTN_KEY7_X+KP_NUMBER_OFFSETX,_BTN_KEY7_Y+KP_NUMBER_OFFSETY);   // Move the cursor to this location for printing.
    tft.setTextSize(KP_BTN_KEY_TEXTSIZE);   
    tft.setTextColor(_textColor);            
    tft.print(_line1Text);                         
}
void btn_KEY7(void)
{
  if(Check_new_touch(_BTN_KEY7_X,_BTN_KEY7_Y,KP_BUTTON_WIDTH,KP_BUTTON_HEIGHT))
  { // The display area with this button has been touched this program cycle.
    Update_keypad_Value (7);
  }
}


// Show_btn_KEY8(/*Color of the button*/ILI9341_GREEN, /*Color of the text*/ILI9341_WHITE, /*line1 Text*/ String "Power is",/*line2 Text*/ String "  ON ");
// *********** Main Screeen button to change to Cold Boot menu *******************
#define _BTN_KEY8_X KP_COLUMN2    //"X" coordinate of Top Left Corner of button
#define _BTN_KEY8_Y KP_ROW3       //"Y" coordinate of Top Left Corner of button
  //This will re-draw the button whenever called.
void Show_btn_KEY8(/*Color of the button*/uint16_t _buttonColor, /*Color of the text*/uint16_t _textColor, /*line1 Text*/ String _line1Text)
{
    tft.fillRect(_BTN_KEY8_X, _BTN_KEY8_Y,KP_BUTTON_WIDTH, KP_BUTTON_HEIGHT, _buttonColor); //A red box, for now.
    tft.setCursor(_BTN_KEY8_X+KP_NUMBER_OFFSETX,_BTN_KEY8_Y+KP_NUMBER_OFFSETY);   // Move the cursor to this location for printing.
    tft.setTextSize(KP_BTN_KEY_TEXTSIZE);   
    tft.setTextColor(_textColor);            
    tft.print(_line1Text);                         
}
void btn_KEY8(void)
{
  if(Check_new_touch(_BTN_KEY8_X,_BTN_KEY8_Y,KP_BUTTON_WIDTH,KP_BUTTON_HEIGHT))
  { // The display area with this button has been touched this program cycle.
    Update_keypad_Value (8);
  }
}
// Show_btn_KEY9(/*Color of the button*/ILI9341_GREEN, /*Color of the text*/ILI9341_WHITE, /*line1 Text*/ String "Power is",/*line2 Text*/ String "  ON ");
// *********** Main Screeen button to change to Cold Boot menu *******************
#define _BTN_KEY9_X KP_COLUMN3  //"X" coordinate of Top Left Corner of button
#define _BTN_KEY9_Y KP_ROW3  //"Y" coordinate of Top Left Corner of button
  //This will re-draw the button whenever called.
void Show_btn_KEY9(/*Color of the button*/uint16_t _buttonColor, /*Color of the text*/uint16_t _textColor, /*line1 Text*/ String _line1Text)
{
    tft.fillRect(_BTN_KEY9_X, _BTN_KEY9_Y,KP_BUTTON_WIDTH, KP_BUTTON_HEIGHT, _buttonColor); //A red box, for now.
    tft.setCursor(_BTN_KEY9_X+KP_NUMBER_OFFSETX,_BTN_KEY9_Y+KP_NUMBER_OFFSETY);   // Move the cursor to this location for printing.
    tft.setTextSize(KP_BTN_KEY_TEXTSIZE);   
    tft.setTextColor(_textColor);            
    tft.print(_line1Text);                         
}
void btn_KEY9(void)
{
  if(Check_new_touch(_BTN_KEY9_X,_BTN_KEY9_Y,KP_BUTTON_WIDTH,KP_BUTTON_HEIGHT))
  { // The display area with this button has been touched this program cycle.
    Update_keypad_Value (9);
  }
}
// Show_btn_KEYA(/*Color of the button*/ILI9341_GREEN, /*Color of the text*/ILI9341_WHITE, /*line1 Text*/ String "Power is",/*line2 Text*/ String "  ON ");
// *********** Main Screeen button to change to Cold Boot menu *******************
#define _BTN_KEYA_X KP_COLUMN1  //"X" coordinate of Top Left Corner of button
#define _BTN_KEYA_Y KP_ROW4  //"Y" coordinate of Top Left Corner of button
  //This will re-draw the button whenever called.
void Show_btn_KEYA(/*Color of the button*/uint16_t _buttonColor, /*Color of the text*/uint16_t _textColor, /*line1 Text*/ String _line1Text)
{
    tft.fillRect(_BTN_KEYA_X, _BTN_KEYA_Y,KP_BUTTON_WIDTH, KP_BUTTON_HEIGHT, _buttonColor); //A red box, for now.
    tft.setCursor(_BTN_KEYA_X+KP_NUMBER_OFFSETX,_BTN_KEYA_Y+KP_NUMBER_OFFSETY);   // Move the cursor to this location for printing.
    tft.setTextSize(KP_BTN_KEY_TEXTSIZE);   
    tft.setTextColor(_textColor);            
    tft.print(_line1Text);                         
}
void btn_KEYA(void)
{
  if(Check_new_touch(_BTN_KEYA_X,_BTN_KEYA_Y,KP_BUTTON_WIDTH,KP_BUTTON_HEIGHT))
  { // The display area with this button has been touched this program cycle.
    Update_keypad_Value (10);
  }
}
// Show_btn_KEYB(/*Color of the button*/ILI9341_GREEN, /*Color of the text*/ILI9341_WHITE, /*line1 Text*/ String "Power is",/*line2 Text*/ String "  ON ");
// *********** Main Screeen button to change to Cold Boot menu *******************
#define _BTN_KEYB_X KP_COLUMN2  //"X" coordinate of Top Left Corner of button
#define _BTN_KEYB_Y KP_ROW4  //"Y" coordinate of Top Left Corner of button
  //This will re-draw the button whenever called.
void Show_btn_KEYB(/*Color of the button*/uint16_t _buttonColor, /*Color of the text*/uint16_t _textColor, /*line1 Text*/ String _line1Text)
{
    tft.fillRect(_BTN_KEYB_X, _BTN_KEYB_Y,KP_BUTTON_WIDTH, KP_BUTTON_HEIGHT, _buttonColor); //A red box, for now.
    tft.setCursor(_BTN_KEYB_X+KP_NUMBER_OFFSETX,_BTN_KEYB_Y+KP_NUMBER_OFFSETY);   // Move the cursor to this location for printing.
    tft.setTextSize(KP_BTN_KEY_TEXTSIZE);   
    tft.setTextColor(_textColor);            
    tft.print(_line1Text);                         
}
void btn_KEYB(void)
{
  if(Check_new_touch(_BTN_KEYB_X,_BTN_KEYB_Y,KP_BUTTON_WIDTH,KP_BUTTON_HEIGHT))
  { // The display area with this button has been touched this program cycle.
    Update_keypad_Value (11);
  }
}
// Show_btn_KEYC(/*Color of the button*/ILI9341_GREEN, /*Color of the text*/ILI9341_WHITE, /*line1 Text*/ String "Power is",/*line2 Text*/ String "  ON ");
// *********** Main Screeen button to change to Cold Boot menu *******************
#define _BTN_KEYC_X KP_COLUMN3  //"X" coordinate of Top Left Corner of button
#define _BTN_KEYC_Y KP_ROW4  //"Y" coordinate of Top Left Corner of button
  //This will re-draw the button whenever called.
void Show_btn_KEYC(/*Color of the button*/uint16_t _buttonColor, /*Color of the text*/uint16_t _textColor, /*line1 Text*/ String _line1Text)
{
    tft.fillRect(_BTN_KEYC_X, _BTN_KEYC_Y,KP_BUTTON_WIDTH, KP_BUTTON_HEIGHT, _buttonColor); //A red box, for now.
    tft.setCursor(_BTN_KEYC_X+KP_NUMBER_OFFSETX,_BTN_KEYC_Y+KP_NUMBER_OFFSETY);   // Move the cursor to this location for printing.
    tft.setTextSize(KP_BTN_KEY_TEXTSIZE);   
    tft.setTextColor(_textColor);            
    tft.print(_line1Text);                         
}
void btn_KEYC(void)
{
  if(Check_new_touch(_BTN_KEYC_X,_BTN_KEYC_Y,KP_BUTTON_WIDTH,KP_BUTTON_HEIGHT))
  { // The display area with this button has been touched this program cycle.
    Update_keypad_Value (12);
  }
}
// Show_btn_KEYD(/*Color of the button*/ILI9341_GREEN, /*Color of the text*/ILI9341_WHITE, /*line1 Text*/ String "Power is",/*line2 Text*/ String "  ON ");
// *********** Main Screeen button to change to Cold Boot menu *******************
#define _BTN_KEYD_X KP_COLUMN1  //"X" coordinate of Top Left Corner of button
#define _BTN_KEYD_Y KP_ROW5  //"Y" coordinate of Top Left Corner of button
  //This will re-draw the button whenever called.
void Show_btn_KEYD(/*Color of the button*/uint16_t _buttonColor, /*Color of the text*/uint16_t _textColor, /*line1 Text*/ String _line1Text)
{
    tft.fillRect(_BTN_KEYD_X, _BTN_KEYD_Y,KP_BUTTON_WIDTH, KP_BUTTON_HEIGHT, _buttonColor); //A red box, for now.
    tft.setCursor(_BTN_KEYD_X+KP_NUMBER_OFFSETX,_BTN_KEYD_Y+KP_NUMBER_OFFSETY);   // Move the cursor to this location for printing.
    tft.setTextSize(KP_BTN_KEY_TEXTSIZE);   
    tft.setTextColor(_textColor);            
    tft.print(_line1Text);                         
}
void btn_KEYD(void)
{
  if(Check_new_touch(_BTN_KEYD_X,_BTN_KEYD_Y,KP_BUTTON_WIDTH,KP_BUTTON_HEIGHT))
  { // The display area with this button has been touched this program cycle.
    Update_keypad_Value (13);
  }
}
// Show_btn_KEYE(/*Color of the button*/ILI9341_GREEN, /*Color of the text*/ILI9341_WHITE, /*line1 Text*/ String "Power is",/*line2 Text*/ String "  ON ");
// *********** Main Screeen button to change to Cold Boot menu *******************
#define _BTN_KEYE_X KP_COLUMN2  //"X" coordinate of Top Left Corner of button
#define _BTN_KEYE_Y KP_ROW5  //"Y" coordinate of Top Left Corner of button
  //This will re-draw the button whenever called.
void Show_btn_KEYE(/*Color of the button*/uint16_t _buttonColor, /*Color of the text*/uint16_t _textColor, /*line1 Text*/ String _line1Text)
{
    tft.fillRect(_BTN_KEYE_X, _BTN_KEYE_Y,KP_BUTTON_WIDTH, KP_BUTTON_HEIGHT, _buttonColor); //A red box, for now.
    tft.setCursor(_BTN_KEYE_X+KP_NUMBER_OFFSETX,_BTN_KEYE_Y+KP_NUMBER_OFFSETY);   // Move the cursor to this location for printing.
    tft.setTextSize(KP_BTN_KEY_TEXTSIZE);   
    tft.setTextColor(_textColor);            
    tft.print(_line1Text);                         
}
void btn_KEYE(void)
{
  if(Check_new_touch(_BTN_KEYE_X,_BTN_KEYE_Y,KP_BUTTON_WIDTH,KP_BUTTON_HEIGHT))
  { // The display area with this button has been touched this program cycle.
    Update_keypad_Value (14);
  }
}
// Show_btn_KEYF(/*Color of the button*/ILI9341_GREEN, /*Color of the text*/ILI9341_WHITE, /*line1 Text*/ String "Power is",/*line2 Text*/ String "  ON ");
// *********** Main Screeen button to change to Cold Boot menu *******************
#define _BTN_KEYF_X KP_COLUMN3  //"X" coordinate of Top Left Corner of button
#define _BTN_KEYF_Y KP_ROW5  //"Y" coordinate of Top Left Corner of button
  //This will re-draw the button whenever called.
void Show_btn_KEYF(/*Color of the button*/uint16_t _buttonColor, /*Color of the text*/uint16_t _textColor, /*line1 Text*/ String _line1Text)
{
    tft.fillRect(_BTN_KEYF_X, _BTN_KEYF_Y,KP_BUTTON_WIDTH, KP_BUTTON_HEIGHT, _buttonColor); //A red box, for now.
    tft.setCursor(_BTN_KEYF_X+KP_NUMBER_OFFSETX,_BTN_KEYF_Y+KP_NUMBER_OFFSETY);   // Move the cursor to this location for printing.
    tft.setTextSize(KP_BTN_KEY_TEXTSIZE);   
    tft.setTextColor(_textColor);            
    tft.print(_line1Text);                         
}
void btn_KEYF(void)
{
  if(Check_new_touch(_BTN_KEYF_X,_BTN_KEYF_Y,KP_BUTTON_WIDTH,KP_BUTTON_HEIGHT))
  { // The display area with this button has been touched this program cycle.
    Update_keypad_Value (15);
  }
}

// Show_btn_KEY0(/*Color of the button*/ILI9341_GREEN, /*Color of the text*/ILI9341_WHITE, /*line1 Text*/ String "Power is",/*line2 Text*/ String "  ON ");
  //This will re-draw the button whenever called.
void Show_btn_KEY0(/*Color of the button*/uint16_t _buttonColor, /*Color of the text*/uint16_t _textColor, /*line1 Text*/ String _line1Text)
{
    uint16_t thiscolumn= KP_COLUMN2;
    uint16_t thisrow= KP_ROW4;
    if(f_hexidecimal_value){thisrow = KP_ROW6;}
    
    tft.fillRect(thiscolumn, thisrow,KP_BUTTON_WIDTH, KP_BUTTON_HEIGHT, _buttonColor); //A red box, for now.
    tft.setCursor(thiscolumn + KP_NUMBER_OFFSETX,thisrow + KP_NUMBER_OFFSETY);   // Move the cursor to this location for printing.
    tft.setTextSize(KP_BTN_KEY_TEXTSIZE);   
    tft.setTextColor(_textColor);            
    tft.print(_line1Text);                         
}
void btn_KEY0(void)
{
    uint16_t thiscolumn= KP_COLUMN2;
    uint16_t thisrow= KP_ROW4;
    if(f_hexidecimal_value){thisrow = KP_ROW6;}

  if(Check_new_touch(thiscolumn,thisrow,KP_BUTTON_WIDTH,KP_BUTTON_HEIGHT))
  { // The display area with this button has been touched this program cycle.
    Update_keypad_Value (0);
  }
}

// Show_btn_ENTER(/*Color of the button*/ILI9341_GREEN, /*Color of the text*/ILI9341_WHITE, /*line1 Text*/ String "Power is",/*line2 Text*/ String "  ON ");
  //This will re-draw the button whenever called.
void Show_btn_ENTER(/*Color of the button*/uint16_t _buttonColor, /*Color of the text*/uint16_t _textColor, /*line1 Text*/ String _line1Text)
{
    uint16_t thiscolumn= KP_COLUMN3;
    uint16_t thisrow= KP_ROW4;
    if(f_hexidecimal_value){thisrow = KP_ROW6;}

    tft.fillRect(thiscolumn, thisrow,KP_BUTTON_WIDTH, KP_BUTTON_HEIGHT, _buttonColor); //A red box, for now.
    tft.setCursor(thiscolumn + KP_NUMBER_OFFSETX -13,thisrow + KP_NUMBER_OFFSETY);   // Move the cursor to this location for printing.
    tft.setTextSize(KP_BTN_KEY_TEXTSIZE);   
    tft.setTextColor(_textColor);            
    tft.print(_line1Text);                         
}
void btn_ENTER(void)
{
    uint16_t thiscolumn= KP_COLUMN3;
    uint16_t thisrow= KP_ROW4;
    if(f_hexidecimal_value){thisrow = KP_ROW6;}
    //Serial.println(thisrow);

  if(Check_new_touch(thiscolumn,thisrow,KP_BUTTON_WIDTH,KP_BUTTON_HEIGHT))
  { // The display area with this button has been touched this program cycle.
  // Pressing "enter" loads the accumulated keypad value back to the variable.
  // It must be converted back to the original variable type before writing the new value back.
  switch (keypad_uints)
  {
    case KEYPAD_FLOAT:
        {
        float * _temp = (float *)ptr_keypad_value; // Make a "float" type of pointer and equate it to the value passed into this function (which was a "void" type)
        *_temp = keypad_working_value;  // The working value is a floating point, so it need not be typecast.
        }
        break;

    case KEYPAD_UINT8:
        {
        uint8_t * _temp = (uint8_t *)ptr_keypad_value; // Make a "float" type of pointer and equate it to the value passed into this function (which was a "void" type)
        *_temp = (uint8_t) keypad_working_value;  // The working value is a floating point, so it need not be typecast.
        }
        break;
    case KEYPAD_UINT16:
       {
        uint16_t * _temp = (uint16_t *)ptr_keypad_value; // Make a "float" type of pointer and equate it to the value passed into this function (which was a "void" type)
        *_temp = (uint16_t) keypad_working_value;  // The working value is a floating point, so it need not be typecast.
       }
        break;
    case KEYPAD_UINT32:
        {
        uint32_t * _temp = (uint32_t *)ptr_keypad_value; // Make a "float" type of pointer and equate it to the value passed into this function (which was a "void" type)
        *_temp = (uint32_t) keypad_working_value;  // The working value is a floating point, so it need not be typecast.
        }
        break;
    case KEYPAD_UINT64:
        {
          uint64_t * _temp = (uint64_t *)ptr_keypad_value; // Make a "float" type of pointer and equate it to the value passed into this function (which was a "void" type)
        *_temp = (uint64_t) keypad_working_value;  // The working value is a floating point, so it need not be typecast.
        }
        break;
   } // End of switch

    gbl_ActiveMenuNumber = keypad_previous_menu;
    //new_Touch = false;  // (Display_ILI9341_t3.ino) new_Touch will be reset to false to prevent re-use..
    //showDisplay[gbl_ActiveMenuNumber]();  // This is an example of how to change to a different screen:

  } // End of if touched.
}// End of function.

// Show_btn_KEY_CLR(/*Color of the button*/ILI9341_GREEN, /*Color of the text*/ILI9341_WHITE, /*line1 Text*/ String "Power is",/*line2 Text*/ String "  ON ");
  //This will re-draw the button whenever called.
void Show_btn_KEY_CLR(/*Color of the button*/uint16_t _buttonColor, /*Color of the text*/uint16_t _textColor, /*line1 Text*/ String _line1Text)
{
    uint16_t thiscolumn= KP_COLUMN1;
    uint16_t thisrow= KP_ROW4;
    if(f_hexidecimal_value){thisrow = KP_ROW6;}

    tft.fillRect(thiscolumn, thisrow,KP_BUTTON_WIDTH, KP_BUTTON_HEIGHT, _buttonColor); //A red box, for now.
    tft.setCursor(thiscolumn + KP_NUMBER_OFFSETX-13,thisrow + KP_NUMBER_OFFSETY);   // Move the cursor to this location for printing.
    tft.setTextSize(KP_BTN_KEY_TEXTSIZE);   
    tft.setTextColor(_textColor);            
    tft.print(_line1Text);                         
}
void btn_KEY_CLR(void)
{
    uint16_t thiscolumn= KP_COLUMN1;
    uint16_t thisrow= KP_ROW4;
    if(f_hexidecimal_value){thisrow = KP_ROW6;}

  if(Check_new_touch(thiscolumn,thisrow,KP_BUTTON_WIDTH,KP_BUTTON_HEIGHT))
  { // The display area with this button has been touched this program cycle.
    keypad_working_value = 0;    // Clear the value
    displayKeypadValue();
  }
}

void LoadDefaultKEYPADButtons(bool _hex_or_dec)
{
  Serial.println("Showing Keypad Numbers");
  Show_btn_KEY1(/*Color of the button*/ILI9341_GREEN, /*Color of the text*/ILI9341_BLACK, /*line1 Text*/ "1" );
  Show_btn_KEY2(/*Color of the button*/ILI9341_GREEN, /*Color of the text*/ILI9341_BLACK, /*line1 Text*/ "2" );
  Show_btn_KEY3(/*Color of the button*/ILI9341_GREEN, /*Color of the text*/ILI9341_BLACK, /*line1 Text*/ "3" );
  Show_btn_KEY4(/*Color of the button*/ILI9341_GREEN, /*Color of the text*/ILI9341_BLACK, /*line1 Text*/ "4" );
  Show_btn_KEY5(/*Color of the button*/ILI9341_GREEN, /*Color of the text*/ILI9341_BLACK, /*line1 Text*/ "5" );
  Show_btn_KEY6(/*Color of the button*/ILI9341_GREEN, /*Color of the text*/ILI9341_BLACK, /*line1 Text*/ "6" );
  Show_btn_KEY7(/*Color of the button*/ILI9341_GREEN, /*Color of the text*/ILI9341_BLACK, /*line1 Text*/ "7" );
  Show_btn_KEY8(/*Color of the button*/ILI9341_GREEN, /*Color of the text*/ILI9341_BLACK, /*line1 Text*/ "8" );
  Show_btn_KEY9(/*Color of the button*/ILI9341_GREEN, /*Color of the text*/ILI9341_BLACK, /*line1 Text*/ "9" );
  Show_btn_KEY0(/*Color of the button*/ILI9341_GREEN, /*Color of the text*/ILI9341_BLACK, /*line1 Text*/ "0" );
  Show_btn_KEY_CLR(/*Color of the button*/ILI9341_GREEN, /*Color of the text*/ILI9341_BLUE, /*line1 Text*/ "CLR" );
  Show_btn_ENTER(/*Color of the button*/ILI9341_GREEN, /*Color of the text*/ILI9341_BLUE, /*line1 Text*/ "Entr" );
  if(f_hexidecimal_value)
  {
    Show_btn_KEYA(/*Color of the button*/ILI9341_GREEN, /*Color of the text*/ILI9341_BLACK, /*line1 Text*/ "A" );
    Show_btn_KEYB(/*Color of the button*/ILI9341_GREEN, /*Color of the text*/ILI9341_BLACK, /*line1 Text*/ "B" );
    Show_btn_KEYC(/*Color of the button*/ILI9341_GREEN, /*Color of the text*/ILI9341_BLACK, /*line1 Text*/ "C" );
    Show_btn_KEYD(/*Color of the button*/ILI9341_GREEN, /*Color of the text*/ILI9341_BLACK, /*line1 Text*/ "D" );
    Show_btn_KEYE(/*Color of the button*/ILI9341_GREEN, /*Color of the text*/ILI9341_BLACK, /*line1 Text*/ "E" );
    Show_btn_KEYF(/*Color of the button*/ILI9341_GREEN, /*Color of the text*/ILI9341_BLACK, /*line1 Text*/ "F" );
  }


  //if(_hex_or_dec)
  //{
  //Show_btn_KEYA(/*Color of the button*/ILI9341_GREEN, /*Color of the text*/ILI9341_BLACK, /*line1 Text*/ "A" );
  //Show_btn_KEYB(/*Color of the button*/ILI9341_GREEN, /*Color of the text*/ILI9341_BLACK, /*line1 Text*/ "B" );
  //}
  //if(f_decimal_entry)
  //{
  //  Showbtn_DECIMAL(/*Color of the button*/ILI9341_GREEN, /*Color of the text*/ILI9341_BLACK, /*line1 Text*/ "." );
  //}
  //else
  //(
  //  Showbtn_DECIMAL(/*Color of the button*/ILI9341_LIGHTGRAY, /*Color of the text*/ILI9341_DARKGRAY, /*line1 Text*/ " " );
  //}
  displayKeypadValue();
}

// ****************************************************************************************** //   
// *********** Add individual LOOP entries from each simulation HERE ************************ //
void Loop_KEYPADButtons(void)
{
  btn_KEY1();
  btn_KEY2();
  btn_KEY3();
  btn_KEY4();
  btn_KEY5();
  btn_KEY6();
  btn_KEY7();
  btn_KEY8();
  btn_KEY9();
  btn_KEY_CLR();
  btn_KEY0();
  btn_ENTER();
  if(f_hexidecimal_value)
  {
    btn_KEYA();
    btn_KEYB();
    btn_KEYC();
    btn_KEYD();
    btn_KEYE();
    btn_KEYF();
  }
}
