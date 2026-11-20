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
void Setup_RemoteConnect(void)
{
  gbl_RemoteConnect = MakeNewDisplay(MAIN_SETTINGS_SCREEN_COLOR,&DrawRemoteConnectScreen);  // (RemoteConnect.ino)
  Serial.printf("Registered display %d as -Remote Connection- \n", gbl_RemoteConnect);
}

/*********** Add individual LOOP entries from each simulation HERE ************************/
void Loop_RemoteConnect(void)
{
    if(gbl_ActiveMenuNumber == gbl_MAIN_Display)
    {   
      btn_REMOTE_SET(); //(RemoteConnection.ino)  Gives a remote settings configuration button on the main menu.
    }
     else
    {
     if(gbl_ActiveMenuNumber == gbl_RemoteConnect)
      {
       //MAIN_Refresh();
        btn_GoToMainMenu();  // See if the home button was pressed.
      }
    }
}

// Show_btn_REMOTE_SET(/*Color of the button*/ILI9341_GREEN, /*Color of the text*/ILI9341_WHITE, /*line1 Text*/ String "Power is",/*line2 Text*/ String "  ON ");
// *********** Network Button *******************
#define _BTN_REMOTE_SET_X 0  // "X" coordinate of Top Left Corner of button set to the left side of the screen
#define _BTN_REMOTE_SET_Y (max_Y_coordinate - (MAIN_DISPLAY_BUTTON_H/2))  //"Y" coordinate of Top Left Corner of button
#define _BTN_REMOTE_SET_W MAIN_DISPLAY_BUTTON_W
#define _BTN_REMOTE_SET_H (MAIN_DISPLAY_BUTTON_H/2)
#define _BTN_REMOTE_SET_TEXTSIZE 2
#define _BTN_REMOTE_SET_LINE1TEXT_X (_BTN_REMOTE_SET_X + 2)
#define _BTN_REMOTE_SET_LINE1TEXT_Y (_BTN_REMOTE_SET_Y + 2)
#define _BTN_REMOTE_SET_LINE2TEXT_X _BTN_REMOTE_SET_LINE1TEXT_X
#define _BTN_REMOTE_SET_LINE2TEXT_Y _BTN_REMOTE_SET_LINE1TEXT_Y
//#define _BTN_REMOTE_SET_LINE2TEXT_X (_BTN_REMOTE_SET_LINE1TEXT_X)
//#define _BTN_REMOTE_SET_LINE2TEXT_Y (_BTN_REMOTE_SET_LINE1TEXT_Y +(_BTN_REMOTE_SET_H/2)+ 2)
   //This will re-draw the button whenever called.
void Show_btn_REMOTE_SET(/*Color of the button*/uint16_t _buttonColor, /*Color of the text*/uint16_t _textColor, /*line1 Text*/ String _line1Text,/*line2 Text*/ String _line2Text)
{
     tft.fillRect(_BTN_REMOTE_SET_X, _BTN_REMOTE_SET_Y,_BTN_REMOTE_SET_W, _BTN_REMOTE_SET_H, _buttonColor); //A red box, for now.
     tft.setCursor(_BTN_REMOTE_SET_LINE1TEXT_X, _BTN_REMOTE_SET_LINE1TEXT_Y);   // Move the cursor to this location for printing.
     tft.setTextSize(_BTN_REMOTE_SET_TEXTSIZE);   
     tft.setTextColor(_textColor);            
     tft.print(_line1Text);                         
     tft.setCursor(_BTN_REMOTE_SET_LINE2TEXT_X, _BTN_REMOTE_SET_LINE2TEXT_Y);              // Move the cursor to this location for printing.
     tft.print(_line2Text);       // Indicate which display was activated.    
}
// **********
void btn_REMOTE_SET(void)
{
  if(Check_new_touch(_BTN_REMOTE_SET_X,_BTN_REMOTE_SET_Y,_BTN_REMOTE_SET_W,_BTN_REMOTE_SET_H))
   { // The display area with this button has been touched this program cycle.
      gbl_ActiveMenuNumber =gbl_RemoteConnect;
   }
}


// ******************************************************************************
// ******************************************************************************
// ******************************************************************************
// ******************************************************************************
void DrawRemoteConnectScreen(void)
{

  
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


 
