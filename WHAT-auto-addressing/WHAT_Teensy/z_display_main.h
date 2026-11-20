#ifndef __Z_DISPLAY_MAIN__
#define __Z_DISPLAY_MAIN__

#define Z_DISPLAY_MAIN_SOFTWARE_VERSION 211202
//#define BOX_HARDWARE_VERSION TEENSY4_0
//#define HARNESS_VERSION 21XXXX
// Main display

// --------   Copy these to your main program  --------
//#include z_display_main.h   // this handles the primary display for this software.
//  Setup_MAIN_Display();   //(Z_Display_main.ino) creates the main menu display.
//  Loop_MAIN_Display(); //(Z_Display_main.ino) Determine which screen is shown and monitors buttons on that screen.


#include "z_Display_ili9341_t3.h" // This makes the display easier to use by allowing the definintion of different display screens with different buttons. 

// *****Copy these to your main routine.*****

// *********** Prototypes  ************************* //
void Show_btn_NETWORK(/*Color of the button*/uint16_t _buttonColor, /*Color of the text*/uint16_t _textColor, /*line1 Text*/ String _line1Text,/*line2 Text*/ String _line2Text);

// Button locations for this display
//#define MAIN_AUTO_X 2
//#define MAIN_AUTO_W 45
//#define MAIN_MAN_X 51
//#define MAIN_MAN_W 180
//#define MAIN_THIS_DISPLAY_NUMBER 0
#define MAIN_SCREEN_COLOR  ILI9341_DARKGREEN // This will be the color taken by the screen when the MAIN button is pressed.
#define MAIN_DISPLAY_COLUMN1_X 18
#define MAIN_DISPLAY_ROW1_Y 50
#define MAIN_DISPLAY_BUTTON_H 40
#define MAIN_DISPLAY_BUTTON_HSPACE 10
#define MAIN_DISPLAY_BUTTON_W 100
#define MAIN_DISPLAY_BUTTON_WSPACE 10
#define MAIN_DISPLAY_COLUMN2_X (MAIN_DISPLAY_COLUMN1_X + MAIN_DISPLAY_BUTTON_W + MAIN_DISPLAY_BUTTON_WSPACE)
#define MAIN_DISPLAY_ROW2_Y (MAIN_DISPLAY_ROW1_Y + MAIN_DISPLAY_BUTTON_H + MAIN_DISPLAY_BUTTON_HSPACE)
#define MAIN_DISPLAY_ROW3_Y (MAIN_DISPLAY_ROW1_Y + (2*MAIN_DISPLAY_BUTTON_H)+ (2*MAIN_DISPLAY_BUTTON_HSPACE))
#define MAIN_DISPLAY_ROW4_Y (MAIN_DISPLAY_ROW1_Y + (3*MAIN_DISPLAY_BUTTON_H)+ (3*MAIN_DISPLAY_BUTTON_HSPACE))

//// *********** General settings and home screen button information  ********************
//#define MAIN_DISPLAY_REFRESH_RATE 500  // mS between checking if display needs an update
//double MAIN_Diplay_RefreshTimer = millis();
bool * _App_WHAT_NetworkButton = &WHAT_NetworkButton;   // Local reference to App Virtual Button State.
uint8_t * _App_WHAT_NetworkLight = &WHAT_NetworkLight;  // Local reference to App Virtual Light State.


/*THIS IS DEFINED IN MAIN SO OTHER ROUTINES CAN USE IT
// **********************    Display Colors  ************************************************* 
#define MAIN_ERROR_COLOR   ILI9341_RED    // Color of the Main Screen MAIN button if MAIN is overridden have an error
#define MAIN_CONFLICT_COLOR ILI9341_ORANGE // Another VGT has been seen on the datalink
#define MAIN_RECOVERY_COLOR ILI9341_YELLOW // Another VGT was on the datalink during the LAST key cycle, but not during THIS key cycle.
#define MAIN_ACTIVE_COLOR ILI9341_GREEN
#define MAIN_NOT_ACTIVE_COLOR ILI9341_LIGHTGREY  
#define MAIN_MAINBUTTON_CONFLICT_COLOR ILI9341_LIGHTGREY
#define MAIN_250k_BAUD_COLOR ILI9341_GREEN
#define MAIN_500k_BAUD_COLOR 0x2D44  //ILI9341_DARKGREEN
#define MAIN_1MEG_BAUD_COLOR ILI9341_OLIVE
#define MAIN_MAINBUTTON_RECOVERY_COLOR ILI9341_YELLOW
#define MAIN_INC_DEC_COLOR  ILI9341_LIGHTGREY      // 192, 192, 192 
#define MAIN_AUTO_COLOR     ILI9341_GREEN
#define MAIN_OVERRIDE_COLOR   0xE6C7      // Darker yellow    // ILI9341_YELLOW
uint8_t MAIN_DisplayNum;     // Menu number assigned to this routine to allow the use of "Display.ino" infrastructure.
//THIS IS DEFINED IN MAIN SO OTHER ROUTINES CAN USE IT
*/

uint8_t gbl_MAIN_Display = 0;
bool pause_Test = false;


#endif
