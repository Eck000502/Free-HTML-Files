#ifndef __WHAT_REMOTE_CONNECTION__
#define __WHAT_REMOTE_CONNECTION__

#define _WHAT_REMOTE_CONNECTION_SOFTWARE_VERSION 220314
//#define BOX_HARDWARE_VERSION TEENSY4_0
//#define HARNESS_VERSION 21XXXX
// Allows network device identification, selection and pairing.

// --------   Copy these to your main program  --------
//#include "z_RemoteConnection.h"   // Allows network device identification, selection and pairing.
//  Setup_RemoteConnect();   //(RemoteConnection.ino) If already connected, re-esablishes remote connection.  Creates a seperate page.
//  Loop_RemoteConnect(); //(RemoteConnection.ino) Evaluates remote connection button status.
// --------   Copy these to your main program  --------

#include "z_Display_ili9341_t3.h" // This makes the display easier to use by allowing the definintion of different display screens with different buttons. 

void Show_btn_MAIN_REMOTE_SETTINGS(/*Color of the button*/uint16_t _buttonColor, /*Color of the text*/uint16_t _textColor, /*line1 Text*/ String _line1Text,/*line2 Text*/ String _line2Text);
#define MAIN_REMOTE_SETTINGS_SCREEN_COLOR  ILI9341_GREEN // This will be the color taken by the screen when the MAIN button is pressed.
/* The settings screen will be single row parameters */
#define MAIN_REMOTE_SETTINGS_COLUMN1_X 16
#define MAIN_REMOTE_SETTINGS_ROW1_Y 50
#define MAIN_REMOTE_SETTINGS_BUTTON_H 20
#define MAIN_REMOTE_SETTINGS_BUTTON_HSPACE 10
#define MAIN_REMOTE_SETTINGS_BUTTON_W 204
#define MAIN_REMOTE_SETTINGS_BUTTON_WSPACE 10
//#define MAIN_REMOTE_SETTINGS_COLUMN2_X (MAIN_REMOTE_SETTINGS_COLUMN1_X + MAIN_REMOTE_SETTINGS_BUTTON_W + MAIN_REMOTE_SETTINGS_BUTTON_WSPACE)
#define MAIN_REMOTE_SETTINGS_ROW2_Y (MAIN_REMOTE_SETTINGS_ROW1_Y + MAIN_REMOTE_SETTINGS_BUTTON_H + MAIN_REMOTE_SETTINGS_BUTTON_HSPACE)
#define MAIN_REMOTE_SETTINGS_ROW3_Y (MAIN_REMOTE_SETTINGS_ROW1_Y + (2*MAIN_REMOTE_SETTINGS_BUTTON_H)+ (2*MAIN_REMOTE_SETTINGS_BUTTON_HSPACE))
#define MAIN_REMOTE_SETTINGS_ROW4_Y (MAIN_REMOTE_SETTINGS_ROW1_Y + (3*MAIN_REMOTE_SETTINGS_BUTTON_H)+ (3*MAIN_REMOTE_SETTINGS_BUTTON_HSPACE))
#define MAIN_REMOTE_SETTINGS_ROW5_Y (MAIN_REMOTE_SETTINGS_ROW1_Y + (4*MAIN_REMOTE_SETTINGS_BUTTON_H)+ (4*MAIN_REMOTE_SETTINGS_BUTTON_HSPACE))
#define MAIN_REMOTE_SETTINGS_ROW6_Y (MAIN_REMOTE_SETTINGS_ROW1_Y + (5*MAIN_REMOTE_SETTINGS_BUTTON_H)+ (5*MAIN_REMOTE_SETTINGS_BUTTON_HSPACE))
#define MAIN_REMOTE_SETTINGS_ROW7_Y (MAIN_REMOTE_SETTINGS_ROW1_Y + (6*MAIN_REMOTE_SETTINGS_BUTTON_H)+ (6*MAIN_REMOTE_SETTINGS_BUTTON_HSPACE))
#define MAIN_REMOTE_SETTINGS_ROW8_Y (MAIN_REMOTE_SETTINGS_ROW1_Y + (7*MAIN_REMOTE_SETTINGS_BUTTON_H)+ (7*MAIN_REMOTE_SETTINGS_BUTTON_HSPACE))

//// *********** General settings and home screen button information  ********************
//#define MAIN_REMOTE_SETTINGS_REFRESH_RATE 500  // mS between checking if display needs an update
//double MAIN_Diplay_RefreshTimer = millis();


/*THIS IS DEFINED IN MAIN SO OTHER ROUTINES CAN USE IT
// **********************    Display Colors  ************************************************* 
//THIS IS DEFINED IN MAIN SO OTHER ROUTINES CAN USE IT
*/

uint8_t gbl_RemoteConnect = 0;


#endif
