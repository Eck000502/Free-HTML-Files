/*  Display_main.ino
 * 
 *   This sets the buttons and information display for the Datalink analyzer.
 *   This version was adapted to use the hard coded buttons.
 *   
*/

#include "z_display_main.h"   // this handles the primary display for this software.
#include "z_settings_main.h"   // Provides setup options for the display

// The regular display routine takes a large amount of program memory and 
// RAM.  This version hardcodes the button coordinates, and does not have (or need)
// automatic button registration and placement.

// In order to accomplish this, the ReadTouch() function should be called in the main loop and before any buttons, 
// and any buttons should review the most recent X/Y coordinates (TouchPoint.x,TouchPoint.y) to identify
// touches to that particular button.

// This makes function calls available to the main program:
//  uint8_t MyMenuNumber = MakeNewDisplay();  // Create a whole new set of buttons.

//  bool Checktouch(/*BTN_FET1_X*/uint8_t _X,/*BTN_FET1_Y*/uint8_t _Y,/*BTN_FET1_H*/uint8_t _H,/*BTN_FET1_W*/uint8_t _W);
//  bool Check_new_touch(/*BTN_FET1_X*/uint8_t _X,/*BTN_FET1_Y*/uint8_t _Y,/*BTN_FET1_H*/uint8_t _H,/*BTN_FET1_W*/uint8_t _W);
//  void Show_Button(/*Color of the button*/uint16_t _buttonColor, /*Color of the text*/uint16_t _textColor, /*line1 Text*/ String _line1Text,/*line2 Text*/ String _line2Text);

// Checktouch will return true if the screen is currently being touched.   
//      TouchPoint.x and TouchPoint.y hold the X/Y coordinates of the touched point.

// Check_new_touch will only be true when the display is just touched for the first time. 
//      This will not become true again until the operator stops touching the display.   
//      TouchPoint.x and TouchPoint.y hold the X/Y coordinates of the touched point.


// ******************************************************************************
// ******************************************************************************
// ******************************************************************************
// ******************************************************************************
/*********** Add individual SETUP entries from each simulation HERE ************************/
void Setup_MAIN_Display(void)
{
  //RegisterJ39Message (3, MASK_SA_ONLY, J39_ENGINE_SA, ZDM_Rx_EngineSeen); // Engine seen on datalink
  //RegisterJ39Message (3, 00000000, 00000000, ZDM_Rx_Calterm); // Engine Speed on PUBLIC or CAN1 or CAN4

  gbl_MAIN_Display = MakeNewDisplay(MAIN_SCREEN_COLOR,&MAIN_DrawMainScreen);  // Create a whole new set of buttons.
  Serial.printf("Registered display %d as -Main Display- \n", gbl_MAIN_Display);

  Setup_MAIN_Settings();  //(z_Main_settings.ino) intialize the settings screen along with this screen, because this screen uses it.

  MAIN_DrawMainScreen(); // (This Routine) this will be the main screen, so it is displayed immediately.
}

/*********** Add individual LOOP entries from each simulation HERE ************************/
void Loop_MAIN_Display(void)
{
    if(gbl_ActiveMenuNumber == gbl_MAIN_Display)
    {
     //MAIN_Refresh();
      btn_NETWORK();
      Loop_Network_Button_Light();  // This routine.  Updates the Network light and button state.
     
      btn_SETTINGS(); //(z_settings_main.ino)
 
      //btn_GoToMainMenu();  // This IS the main menu, so do not use this button.
    }
    Loop_MAIN_Settings();   //(z_settings_main.ino)
    Loop_RemoteConnect(); //(RemoteConnection.ino) Evaluates remote connection screen.
}

// Show_btn_Network(/*Color of the button*/ILI9341_GREEN, /*Color of the text*/ILI9341_WHITE, /*line1 Text*/ String "Power is",/*line2 Text*/ String "  ON ");
// *********** Network Button *******************
//#define _BTN_NETWORK_X MAIN_DISPLAY_COLUMN1_X  //"X" coordinate of Top Left Corner of button
//#define _BTN_NETWORK_Y MAIN_DISPLAY_ROW1_Y     //"Y" coordinate of Top Left Corner of button
#define _BTN_NETWORK_X 0  //"X" coordinate of Top Left Corner of button
#define _BTN_NETWORK_Y 0  //"Y" coordinate of Top Left Corner of button
#define _BTN_NETWORK_W MAIN_DISPLAY_BUTTON_W
#define _BTN_NETWORK_H MAIN_DISPLAY_BUTTON_H
#define _BTN_NETWORK_TEXTSIZE 2
#define _BTN_NETWORK_LINE1TEXT_X (_BTN_NETWORK_X + 2)
#define _BTN_NETWORK_LINE1TEXT_Y (_BTN_NETWORK_Y + 2)
#define _BTN_NETWORK_LINE2TEXT_X (_BTN_NETWORK_LINE1TEXT_X)
#define _BTN_NETWORK_LINE2TEXT_Y (_BTN_NETWORK_LINE1TEXT_Y +(_BTN_NETWORK_H/2)+ 2)
   //This will re-draw the button whenever called.
void Show_btn_NETWORK(/*Color of the button*/uint16_t _buttonColor, /*Color of the text*/uint16_t _textColor, /*line1 Text*/ String _line1Text,/*line2 Text*/ String _line2Text)
{
     tft.fillRect(_BTN_NETWORK_X, _BTN_NETWORK_Y,_BTN_NETWORK_W, _BTN_NETWORK_H, _buttonColor); //A red box, for now.
     tft.setCursor(_BTN_NETWORK_LINE1TEXT_X, _BTN_NETWORK_LINE1TEXT_Y);   // Move the cursor to this location for printing.
     tft.setTextSize(_BTN_NETWORK_TEXTSIZE);   
     tft.setTextColor(_textColor);            
     tft.print(_line1Text);                         
     tft.setCursor(_BTN_NETWORK_LINE2TEXT_X, _BTN_NETWORK_LINE2TEXT_Y);              // Move the cursor to this location for printing.
     tft.print(_line2Text);       // Indicate which display was activated.    
}
// **********
void btn_NETWORK(void)
{
  /*
   * Error....
   * when touching the screen somewhere OTHER than the button location, the 
   * vlaue of button becomes 0, but when releaseing the button, the value does not go to 0.
   * 
   */

  WHAT_NetworkButton = Checktouch(_BTN_NETWORK_X,_BTN_NETWORK_Y,_BTN_NETWORK_W,_BTN_NETWORK_H);
  //Serial.println(Check_new_touch(_BTN_NETWORK_X,_BTN_NETWORK_Y,_BTN_NETWORK_W,_BTN_NETWORK_H));
}


// ******************************************************************************
// ******************************************************************************
// ******************************************************************************
// ******************************************************************************
void MAIN_DrawMainScreen(void)
{
  Show_btn_NETWORK(/*Color of the button*/ILI9341_GREEN, /*Color of the text*/ILI9341_WHITE, /*line1 Text*/ "Network",/*line2 Text*/ "Button");
  Show_btn_SETTINGS(/*Color of the button*/ILI9341_DARKGREY, /*Color of the text*/ILI9341_YELLOW, /*line1 Text*/ "        ",/*line2 Text*/ "Settings");
  Show_btn_REMOTE_SET(/*Color of the button*/ILI9341_DARKGREY, /*Color of the text*/ILI9341_YELLOW, /*line1 Text*/ "        ",/*line2 Text*/ " Remote ");
 
 // (z_Settings_main.ino) 
  //Show_btn_GoToMainMenu();
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


 
