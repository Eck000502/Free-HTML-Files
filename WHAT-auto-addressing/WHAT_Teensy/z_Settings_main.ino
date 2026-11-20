/*  z_Settings_Main.ino
 * 
 *   This has a button (placed in z_Main_Display.in0) that allows customization of some features.
 *   
*/

#include "z_settings_main.h"   // Provides setup options for the display

// ******************************************************************************
// ******************************************************************************
// ******************************************************************************
// ******************************************************************************
/*********** Add individual SETUP entries from each simulation HERE ************************/
void Setup_MAIN_Settings(void)
{
  gbl_MAIN_Settings = MakeNewDisplay(MAIN_SETTINGS_SCREEN_COLOR,&DrawMainSettingsScreen);  // (Settings.inc)
  Serial.printf("Registered display %d as -Settings-\n", gbl_MAIN_Settings);
}

/*********** Add individual LOOP entries from each simulation HERE ************************/
void Loop_MAIN_Settings(void)
{
    if(gbl_ActiveMenuNumber == gbl_MAIN_Settings)
    {
     //MAIN_Refresh();
      btn_OFF_BacklightValue();
      btn_ON_BacklightValue();

      btn_GoToMainMenu();  // See if the home button was pressed.
    }
}

// Show_btn_SETTINGS(/*Color of the button*/ILI9341_GREEN, /*Color of the text*/ILI9341_WHITE, /*line1 Text*/ String "Power is",/*line2 Text*/ String "  ON ");
// *********** Network Button *******************
#define _BTN_SETTINGS_X (max_X_coordinate - MAIN_DISPLAY_BUTTON_W)  //"X" coordinate of Top Left Corner of button
#define _BTN_SETTINGS_Y (max_Y_coordinate - (MAIN_DISPLAY_BUTTON_H/2))  //"Y" coordinate of Top Left Corner of button
#define _BTN_SETTINGS_W MAIN_DISPLAY_BUTTON_W
#define _BTN_SETTINGS_H (MAIN_DISPLAY_BUTTON_H/2)
#define _BTN_SETTINGS_TEXTSIZE 2
#define _BTN_SETTINGS_LINE1TEXT_X (_BTN_SETTINGS_X + 2)
#define _BTN_SETTINGS_LINE1TEXT_Y (_BTN_SETTINGS_Y + 2)
#define _BTN_SETTINGS_LINE2TEXT_X _BTN_SETTINGS_LINE1TEXT_X
#define _BTN_SETTINGS_LINE2TEXT_Y _BTN_SETTINGS_LINE1TEXT_Y
//#define _BTN_SETTINGS_LINE2TEXT_X (_BTN_SETTINGS_LINE1TEXT_X)
//#define _BTN_SETTINGS_LINE2TEXT_Y (_BTN_SETTINGS_LINE1TEXT_Y +(_BTN_SETTINGS_H/2)+ 2)
   //This will re-draw the button whenever called.
void Show_btn_SETTINGS(/*Color of the button*/uint16_t _buttonColor, /*Color of the text*/uint16_t _textColor, /*line1 Text*/ String _line1Text,/*line2 Text*/ String _line2Text)
{
     tft.fillRect(_BTN_SETTINGS_X, _BTN_SETTINGS_Y,_BTN_SETTINGS_W, _BTN_SETTINGS_H, _buttonColor); //A red box, for now.
     tft.setCursor(_BTN_SETTINGS_LINE1TEXT_X, _BTN_SETTINGS_LINE1TEXT_Y);   // Move the cursor to this location for printing.
     tft.setTextSize(_BTN_SETTINGS_TEXTSIZE);   
     tft.setTextColor(_textColor);            
     tft.print(_line1Text);                         
     tft.setCursor(_BTN_SETTINGS_LINE2TEXT_X, _BTN_SETTINGS_LINE2TEXT_Y);              // Move the cursor to this location for printing.
     tft.print(_line2Text);       // Indicate which display was activated.    
}
// **********
void btn_SETTINGS(void)
{
  if(Check_new_touch(_BTN_SETTINGS_X,_BTN_SETTINGS_Y,_BTN_SETTINGS_W,_BTN_SETTINGS_H))
   { // The display area with this button has been touched this program cycle.
      gbl_ActiveMenuNumber =gbl_MAIN_Settings;
   }
}


// *********** Backlight Button *******************
// Show_btn_OFF_BacklightValue(/*Color of the button*/ILI9341_GREEN, /*Color of the text*/ILI9341_WHITE, /*line1 Text*/ String "Power is",/*line2 Text*/ String "  ON ");
#define _BTN_OFF_BacklightValue_X MAIN_settings_COLUMN1_X  //"X" coordinate of Top Left Corner of button
#define _BTN_OFF_BacklightValue_Y MAIN_settings_ROW1_Y     //"Y" coordinate of Top Left Corner of button
#define _BTN_OFF_BacklightValue_W MAIN_settings_BUTTON_W
#define _BTN_OFF_BacklightValue_H MAIN_settings_BUTTON_H
#define _BTN_OFF_BacklightValue_TEXTSIZE 2
#define _BTN_OFF_BacklightValue_LINE1TEXT_X (_BTN_OFF_BacklightValue_X + 2)
#define _BTN_OFF_BacklightValue_LINE1TEXT_Y (_BTN_OFF_BacklightValue_Y + 2)
#define _BTN_OFF_BacklightValue_LINE2TEXT_X (_BTN_OFF_BacklightValue_X + _BTN_OFF_BacklightValue_W - 47)
#define _BTN_OFF_BacklightValue_LINE2TEXT_Y _BTN_OFF_BacklightValue_LINE1TEXT_Y
   //This will re-draw the button whenever called.
void Show_btn_OFF_BacklightValue(/*Color of the button*/uint16_t _buttonColor, /*Color of the text*/uint16_t _textColor, /*line1 Text*/ String _line1Text,/*line2 Text*/ String _line2Text)
{
     tft.fillRect(_BTN_OFF_BacklightValue_X, _BTN_OFF_BacklightValue_Y,_BTN_OFF_BacklightValue_W, _BTN_OFF_BacklightValue_H, _buttonColor); //A red box, for now.
     tft.setCursor(_BTN_OFF_BacklightValue_LINE1TEXT_X, _BTN_OFF_BacklightValue_LINE1TEXT_Y);   // Move the cursor to this location for printing.
     tft.setTextSize(_BTN_OFF_BacklightValue_TEXTSIZE);   
     tft.setTextColor(_textColor);            
     tft.print(_line1Text);                         
     tft.setCursor(_BTN_OFF_BacklightValue_LINE2TEXT_X, _BTN_OFF_BacklightValue_LINE2TEXT_Y);              // Move the cursor to this location for printing.
     tft.print(_line2Text);       // Indicate which display was activated.    
}
// **********
void btn_OFF_BacklightValue(void)
{
  if(Check_new_touch(_BTN_OFF_BacklightValue_X,_BTN_OFF_BacklightValue_Y,_BTN_OFF_BacklightValue_W,_BTN_OFF_BacklightValue_H))
   { // The display area with this button has been touched this program cycle.
     KeyPad_Invocation(/*Hexi or Deci*/ DECIMAL,/*Number type*/ KEYPAD_UINT8, /*Variable being entered*/ &gbl_MinBacklight);
   }
}

// *********** Backlight Button *******************
// Show_btn_ON_BacklightValue(/*Color of the button*/ILI9341_GREEN, /*Color of the text*/ILI9341_WHITE, /*line1 Text*/ String "Power is",/*line2 Text*/ String "  ON ");
#define _BTN_ON_BacklightValue_X MAIN_settings_COLUMN1_X  //"X" coordinate of Top Left Corner of button
#define _BTN_ON_BacklightValue_Y MAIN_settings_ROW2_Y    //"Y" coordinate of Top Left Corner of button
#define _BTN_ON_BacklightValue_W MAIN_settings_BUTTON_W
#define _BTN_ON_BacklightValue_H MAIN_settings_BUTTON_H
#define _BTN_ON_BacklightValue_TEXTSIZE 2
#define _BTN_ON_BacklightValue_LINE1TEXT_X (_BTN_ON_BacklightValue_X + 2)
#define _BTN_ON_BacklightValue_LINE1TEXT_Y (_BTN_ON_BacklightValue_Y + 2)
#define _BTN_ON_BacklightValue_LINE2TEXT_X (_BTN_ON_BacklightValue_X + _BTN_ON_BacklightValue_W - 58)
#define _BTN_ON_BacklightValue_LINE2TEXT_Y _BTN_ON_BacklightValue_LINE1TEXT_Y
   //This will re-draw the button whenever called.
void Show_btn_ON_BacklightValue(/*Color of the button*/uint16_t _buttonColor, /*Color of the text*/uint16_t _textColor, /*line1 Text*/ String _line1Text,/*line2 Text*/ String _line2Text)
{
     tft.fillRect(_BTN_ON_BacklightValue_X, _BTN_ON_BacklightValue_Y,_BTN_ON_BacklightValue_W, _BTN_ON_BacklightValue_H, _buttonColor); //A red box, for now.
     tft.setCursor(_BTN_ON_BacklightValue_LINE1TEXT_X, _BTN_ON_BacklightValue_LINE1TEXT_Y);   // Move the cursor to this location for printing.
     tft.setTextSize(_BTN_ON_BacklightValue_TEXTSIZE);   
     tft.setTextColor(_textColor);            
     tft.print(_line1Text);                         
     tft.setCursor(_BTN_ON_BacklightValue_LINE2TEXT_X, _BTN_ON_BacklightValue_LINE2TEXT_Y);              // Move the cursor to this location for printing.
     tft.print(_line2Text);       // Indicate which display was activated.    
}
// **********
void btn_ON_BacklightValue(void)
{
  if(Check_new_touch(_BTN_ON_BacklightValue_X,_BTN_ON_BacklightValue_Y,_BTN_ON_BacklightValue_W,_BTN_ON_BacklightValue_H))
   { // The display area with this button has been touched this program cycle.
    //KeyPad_Invocation(/*Hexi or Deci*/ DECIMAL,/*Number type*/ KEYPAD_UINT8, /*Variable being entered*/ &gbl_MaxBacklight);
    KeyPad_Invocation(/*Hexi or Deci*/ DECIMAL,/*Number type*/ KEYPAD_UINT8, /*Variable being entered*/ &gbl_MaxBacklight);
   }
}


// ******************************************************************************
// ******************************************************************************
// ******************************************************************************
// ******************************************************************************
void DrawMainSettingsScreen(void)
{
  //uint8_t TemporaryValue = 10.1;
  //String _line2Text = " " + String(TemporaryValue,0)+(char)247 +"C";
  String _line2Text = " " + String(float(gbl_MinBacklight),0)+"%";
   Show_btn_OFF_BacklightValue(/*Color of the button*/ILI9341_YELLOW, /*Color of the text*/ILI9341_BLACK, /*line1 Text*/ "Backlight Off:",/*line2 Text*/ _line2Text);
    _line2Text = " " + String(float(gbl_MaxBacklight),0)+"%";
  Show_btn_ON_BacklightValue(/*Color of the button*/ILI9341_YELLOW, /*Color of the text*/ILI9341_BLACK, /*line1 Text*/ "Backlight On:",/*line2 Text*/ _line2Text);
  Show_btn_GoToMainMenu();
}

//void MAIN_Refresh(void)
//{
//  #define MAIN_DISPLAY_REFRESH_RATE 1000
//  static double MAIN_Display_RefreshTimer = millis();
//  if(MAIN_Display_RefreshTimer <= millis())
//  {
//    MAIN_Display_RefreshTimer = millis() + MAIN_DISPLAY_REFRESH_RATE; // Reset the refresh timer.
//  }// End if refresh timer expired  
//}// End function


 
