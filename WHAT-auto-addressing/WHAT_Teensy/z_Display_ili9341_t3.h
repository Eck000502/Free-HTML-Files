#ifndef __DISPLAY_ILI9341_T3__
#define __DISPLAY_ILI9341_T3__

#define DISPLAY_SOFTWARE_VERSION 211203
//#define BOX_HARDWARE_VERSION 6.0a
//#define HARNESS_VERSION 19XXXX

// --------   Copy these to your main program  --------
//#include "z_Display_ili9341_t3.h" // This makes the display easier to use by allowing the definintion of different display screens with different buttons. 
//  Display_begin(); //(Display_ili9341_t3.c) Allows a main menu screen with touchable buttons.
//  Display_Loop();  //(Display_ili9341_t3.c) Allows a main menu screen with touchable buttons.
//  void Display_ClearScreen ((uint16_t) _screencolor);  //(Display_ili9341_t3.c) clears the screen and sets to "_screencolor".

#define SCREEN_ROTATION 2 // the objects on screen can be rotated: 0=Landscape,1=Portrait, 2=InvertedLandscape, 3=inverted portriate. 

// Display with NO BUTTONS - TEENSY
// This project is dedicated to the incorporation of the ILI9341_T3 (SPI) display and 
// the XPT2046_Touchscreen on the TEENSY 3.6 platform.

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
//  void Show_btn_GoToMainMenu(void) // This can be used to add a "Home" button in the upper right corner of your display.
//  btn_GoToMainMenu();  // Checks if the home button has been pressed.

// Checktouch will return true if the screen is currently being touched.   
//      TouchPoint.x and TouchPoint.y hold the X/Y coordinates of the touched point.

// Check_new_touch will only be true when the display is just touched for the first time. 
//      This will not become true again until the operator stops touching the display.   
//      TouchPoint.x and TouchPoint.y hold the X/Y coordinates of the touched point.

// *********EXAMPLE BUTTON ROUTINE ***********************
// Show_btn_ManualPower(/*Color of the button*/ILI9341_GREEN, /*Color of the text*/ILI9341_WHITE, /*line1 Text*/ String "Power is",/*line2 Text*/ String "  ON ");
// *********** Pause Button *******************
// #define _BTN_ManualPower_X 75   //"X" coordinate of Top Left Corner of button
// #define _BTN_ManualPower_Y 80   //"X" coordinate of Top Left Corner of button
// #define _BTN_ManualPower_W 90
// #define _BTN_ManualPower_H 40
// #define _BTN_ManualPower_COLOR ILI9341_DARKGREEN
// #define _BTN_ManualPower_TEXTCOLOR BLACK
// #define _BTN_ManualPower_TEXTSIZE 2
// #define _BTN_ManualPower_LINE1TEXT_X (_BTN_ManualPower_X + 2)
// #define _BTN_ManualPower_LINE1TEXT_Y (_BTN_ManualPower_Y + 2)
// #define _BTN_ManualPower_LINE2TEXT_X (_BTN_ManualPower_LINE1TEXT_X)
// #define _BTN_ManualPower_LINE2TEXT_Y (_BTN_ManualPower_LINE1TEXT_Y +(_BTN_ManualPower_H/2)+ 2)
    //This will re-draw the button whenever called.
// void Show_btn_ManualPower(/*Color of the button*/uint16_t _buttonColor, /*Color of the text*/uint16_t _textColor, /*line1 Text*/ String _line1Text,/*line2 Text*/ String _line2Text)
// {
//     tft.fillRect(_BTN_ManualPower_X, _BTN_ManualPower_Y,_BTN_ManualPower_W, _BTN_ManualPower_H, _buttonColor); //A red box, for now.
//     tft.setCursor(_BTN_ManualPower_LINE1TEXT_X, _BTN_ManualPower_LINE1TEXT_Y);   // Move the cursor to this location for printing.
//     tft.setTextSize(_BTN_ManualPower_TEXTSIZE);   
//     tft.setTextColor(_textColor);            
//     tft.print(_BTN_ManualPower_line1Text);                         
//     tft.setCursor(_BTN_ManualPower_LINE2TEXT_X, _BTN_ManualPower_LINE2TEXT_Y);              // Move the cursor to this location for printing.
//     tft.print(_BTN_ManualPower_line1Text);       // Indicate which display was activated.    
// }
  // **********
// void btn_ManualPower()
// {
//   if(Check_new_touch(_BTN_ManualPower_X,_BTN_ManualPower_Y,_BTN_ManualPower_W,_BTN_ManualPower_H))
//   { // The display area with this button has been touched this program cycle.
//     static uint8_t _Previous_Index = 0xFF;
//     if(pause_Test)
//     { // Already paused... release the pause due to this press.
//      script_Step_index = _Previous_Index;
//      pause_Test = false;
//      Show_btn_ManualPower(/*Color of the button*/ILI9341_DARKGREY, /*Color of the text*/ILI9341_BLACK, /*line1 Text*/ String "|| / >",/*line2 Text*/ String "PLAYING")
//     }
//     else
//     {
//       _Previous_Index = script_Step_index;  // Remember where execution stopped.
//       script_Step_index=0xFF;   // This will Stop stepping through the test script.
//       pause_Test = true;
//       Show_btn_ManualPower(/*Color of the button*/ILI9341_RED, /*Color of the text*/ILI9341_BLACK, /*line1 Text*/ String "|| / >",/*line2 Text*/ String "PAUSED")
//     }
//   }
// }
//


// Color definitions
//#define ILI9341_BLACK       0x0000      /*   0,   0,   0 */
//#define ILI9341_NAVY        0x000F      /*   0,   0, 128 */
//#define ILI9341_DARKGREEN   0x03E0      /*   0, 128,   0 */
//#define ILI9341_DARKCYAN    0x03EF      /*   0, 128, 128 */
//#define ILI9341_MAROON      0x7800      /* 128,   0,   0 */
//#define ILI9341_PURPLE      0x780F      /* 128,   0, 128 */
//#define ILI9341_OLIVE       0x7BE0      /* 128, 128,   0 */
//#define ILI9341_LIGHTGREY   0xC618      /* 192, 192, 192 */
//#define ILI9341_DARKGREY    0x7BEF      /* 128, 128, 128 */
//#define ILI9341_BLUE        0x001F      /*   0,   0, 255 */
//#define ILI9341_GREEN       0x07E0      /*   0, 255,   0 */
//#define ILI9341_CYAN        0x07FF      /*   0, 255, 255 */
//#define ILI9341_RED         0xF800      /* 255,   0,   0 */
//#define ILI9341_MAGENTA     0xF81F      /* 255,   0, 255 */
//#define ILI9341_YELLOW      0xFFE0      /* 255, 255,   0 */
//#define ILI9341_WHITE       0xFFFF      /* 255, 255, 255 */
//#define ILI9341_ORANGE      0xFD20      /* 255, 165,   0 */
//#define ILI9341_GREENYELLOW 0xAFE5      /* 173, 255,  47 */
//#define ILI9341_PINK        0xF81F
#define ILI9341_DARK_RED      0x7D00      /* 128,   0,   0 */

//#include "home.c"       // show the house as the return to home button.
#include <Wire.h>
#include <ILI9341_t3.h>
#include <XPT2046_Touchscreen.h>

#define MAIN_MENU_NUMBER 0
uint8_t gbl_ActiveMenuNumber = MAIN_MENU_NUMBER;                 // This is the menu that is currently being displayed.
uint8_t gbl_homescreen = MAIN_MENU_NUMBER;
#define NUM_DIFFERENT_MENUS 15                // Each sub-function can have its own menu. // This must be defined globally on this screen so it can be used on this screeen.

struct IntObject{
  uint16_t COLOR;       // Color to be used for this object.
  void (* callback)(void); // Function to be called if this area of the touch screen is touched. 
};
IntObject showDisplay[NUM_DIFFERENT_MENUS];  

/// ************ Setup for Display Backlight ********************** /
#define TFT_BACKLIGHT_PIN 7
#define TFT_CS 10
#define TFT_DC  9
#define BACKLIGHT_TIMEOUT_RATE (1*60*1000)  // 1 minute * 60Seconds * 1000mS
#define GBL_DIM_VALUE 1
// PWM percent when the backlight is DIMMED.
double t_BacklightTimeout = millis() + BACKLIGHT_TIMEOUT_RATE;   // Allows the backlight to turn off when not used.
bool gbl_waking_touch = false;        // This is set to true when the backlight has gone off, then is re-wakened by a touch.  It is cleared when the touchscreen is released.
uint8_t gbl_backlightBrightness = 100;
uint8_t gbl_MinBacklight = 10;
uint8_t gbl_MaxBacklight = 100;

ILI9341_t3 tft = ILI9341_t3(TFT_CS, TFT_DC);

// ************ Setup for TOUCH part of Touchscreen  ******************/
#define Touch_CS  8
// MOSI=11, MISO=12, SCK=13
#define TIRQ_PIN  2
//XPT2046_Touchscreen Touch(Touch_CS, TIRQ_PIN);  // Param 2 - Touch IRQ Pin - interrupt enabled polling
XPT2046_Touchscreen Touch(Touch_CS);  // Param 2 - Touch IRQ Pin - interrupt enabled polling
TS_Point TouchPoint;   // This will hold the most recently touched position.
bool new_Touch = false;  // This is set to true when the button was released, and back to false on the next program loop.
#define TOUCHRATE 100 // Milliseconds between updates of touchscreen
// This is calibration data for the raw touch data to the screen coordinates
#define TS_MINX 218   // The smallest number register when touching the screen
#define TS_MINY 300   // The smallest number register when touching the screen
#define TS_MAXX 3850   // The largest number register when touching the screen
#define TS_MAXY 3900   // The largest number register when touching the screen
#define TS_MINPRESSURE 10 // minimum "Z" axiz indication to accept that the screen was touched.
#define TS_MAXPRESSURE 1000  // maximum "Z" axiz indication to accept that the screen was touched.

uint16_t  max_X_coordinate = 320;
uint16_t  max_Y_coordinate = 240;
//uint16_t  max_X_coordinate = 240; // Portrait.
//uint16_t  max_Y_coordinate = 320; // Portrait.
uint16_t ColorInversion = 0;  // This value will be XORed with the chosen color.  The TFT type determines if this is all 1s or all 0s.
#define DISPLAY_UPDATE_RATE 250;  // Do not update more frequently than every 1/2 second.    
double t_Display_UpdateTimer = millis();  // This is a global, so if a non-update period is needed, reset this to millis to stop the updates.
//uint8_t ABOUT_Display_ButtonNumber;

#define BACKGROUND ILI9341_BLUE
#define DISPLAY_TEXT_SIZE 2

// ************  Prototypes of Public Functions *************************
//bool Check_new_touch(/*BTN_FET1_X*/uint16_t _X,/*BTN_FET1_Y*/uint16_t _Y,/*BTN_FET1_W*/uint16_t _W,/*BTN_FET1_H*/uint16_t _H);
//bool Checktouch(/*BTN_FET1_X*/uint16_t _X,/*BTN_FET1_Y*/uint16_t _Y,/*BTN_FET1_W*/uint16_t _W,/*BTN_FET1_H*/uint16_t _H);
bool Check_new_touch(uint16_t my_x,uint16_t my_y,uint16_t my_w,uint16_t my_h);
bool Checktouch(uint16_t my_x,uint16_t my_y,uint16_t my_w,uint16_t my_h);
void Show_btn_GoToMainMenu(void);
uint8_t MakeNewDisplay(uint16_t _screencolor,void (*_menudisplay)(void));

#endif
