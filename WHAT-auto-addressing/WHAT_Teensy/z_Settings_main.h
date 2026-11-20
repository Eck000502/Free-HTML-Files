#ifndef __Z_SETTINGS_MAIN__
#define __Z_SETTINGS_MAIN__

#define _Z_SETTINGS_MAIN_SOFTWARE_VERSION 220205
//#define BOX_HARDWARE_VERSION TEENSY4_0
//#define HARNESS_VERSION 21XXXX
// Main display

// --------   Copy these to your main program  --------
//#include "z_settings_main.h"   // Provides setup options for the display
//  Setup_MAIN_Display();   //(Z_settings_main.ino) creates the main menu display.
//  Loop_MAIN_Display(); //(Z_settings_main.ino) Determine which screen is shown and monitors buttons on that screen.


#include "z_Display_ili9341_t3.h" // This makes the display easier to use by allowing the definintion of different display screens with different buttons. 

// *****Copy these to your main routine.*****

// *********** Prototypes  ************************* //

// Button locations for this display
//#define MAIN_AUTO_X 2
//#define MAIN_AUTO_W 45
//#define MAIN_MAN_X 51
//#define MAIN_MAN_W 180
//#define MAIN_THIS_settings_NUMBER 0
#define MAIN_SETTINGS_SCREEN_COLOR  ILI9341_GREEN // This will be the color taken by the screen when the MAIN button is pressed.
/* The settings screen will be single row parameters */
#define MAIN_settings_COLUMN1_X 16
#define MAIN_settings_ROW1_Y 50
#define MAIN_settings_BUTTON_H 20
#define MAIN_settings_BUTTON_HSPACE 10
#define MAIN_settings_BUTTON_W 204
#define MAIN_settings_BUTTON_WSPACE 10
//#define MAIN_settings_COLUMN2_X (MAIN_settings_COLUMN1_X + MAIN_settings_BUTTON_W + MAIN_settings_BUTTON_WSPACE)
#define MAIN_settings_ROW2_Y (MAIN_settings_ROW1_Y + MAIN_settings_BUTTON_H + MAIN_settings_BUTTON_HSPACE)
#define MAIN_settings_ROW3_Y (MAIN_settings_ROW1_Y + (2*MAIN_settings_BUTTON_H)+ (2*MAIN_settings_BUTTON_HSPACE))
#define MAIN_settings_ROW4_Y (MAIN_settings_ROW1_Y + (3*MAIN_settings_BUTTON_H)+ (3*MAIN_settings_BUTTON_HSPACE))
#define MAIN_settings_ROW5_Y (MAIN_settings_ROW1_Y + (4*MAIN_settings_BUTTON_H)+ (4*MAIN_settings_BUTTON_HSPACE))
#define MAIN_settings_ROW6_Y (MAIN_settings_ROW1_Y + (5*MAIN_settings_BUTTON_H)+ (5*MAIN_settings_BUTTON_HSPACE))
#define MAIN_settings_ROW7_Y (MAIN_settings_ROW1_Y + (6*MAIN_settings_BUTTON_H)+ (6*MAIN_settings_BUTTON_HSPACE))
#define MAIN_settings_ROW8_Y (MAIN_settings_ROW1_Y + (7*MAIN_settings_BUTTON_H)+ (7*MAIN_settings_BUTTON_HSPACE))

//// *********** General settings and home screen button information  ********************
//#define MAIN_settings_REFRESH_RATE 500  // mS between checking if display needs an update
//double MAIN_Diplay_RefreshTimer = millis();


/*THIS IS DEFINED IN MAIN SO OTHER ROUTINES CAN USE IT
// **********************    Display Colors  ************************************************* 
//THIS IS DEFINED IN MAIN SO OTHER ROUTINES CAN USE IT
*/

uint8_t gbl_MAIN_Settings = 0;


#endif
