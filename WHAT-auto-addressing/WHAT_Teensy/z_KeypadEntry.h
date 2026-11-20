#ifndef __KeypadEntry_h
#define __KeypadEntry_h

#define KEYPAD_SOFTWARE_VERSION 210314
//#define BOX_HARDWARE_VERSION 6.0a
//#define HARNESS_VERSION 19XXXX


//#include "z_KeypadEntry.h"  // Allows number pad entry of values.
//Setup_KEYPAD_Display();   //(KeypadEnrty.c) Allows numbers to be entered using a keypad.
//KEYPAD_Display_Loop();   //(KeypadEnrty.c) Allows numbers to be entered using a keypad.

/*  KeypadEntry.ino
 * 
 * Displays a keypad that can be used to enter parameter values.
 * Allows float (with +/- and Decimal point), or HEX entries.
 * decimal/hex Mode is pre-selected when the routine is called.
 * 
 */
 //    KeyPad_Invocation(/*Hexi or Deci*/ DECIMAL,/*Number type*/ KEYPAD_FLOAT, /*Variable being entered*/ &myVariable)

/* 
 *  Known Errors:
 *  
 */

/*
 * This uses the function from display.ino:
 *    MyButtonNumber = RegisterDisplay(DisplayNumber, &ButtonPressCallback);  // Create a button that, when pressed, calls the passed callback function
 * 
 * "&ButtonPressCallback" is the function that will be called when the button is pressed.
 * MyButtonNumber will be an index to an array of structures that contains:
 *      Structure name is "MainScreenButtons[MenuNumber][ButtonNumber]"
 *      "X" location of the top left corner of the button
 *      "Y" location of the top left corner of the button
 *      "W"idth of the button
 *      "H"eight of the button
 *      "COLOR"          // Color to be used for this object.
 *      String Line1Text     // The words that go on the first line of the button
 *      String Line2Text     // The words that go on the second line of the button
 *      bool NeedRefresh     // Set this to 1 to signal the display routine that this object needs to be refreshed. (will be cleared when refreshed)
 *      "callback" or the name of the function to run when the button is pressed.
 * 
*/

#include <ILI9341_t3.h>   // Include this here because it has definitions for the screen colors.

// **********************    Display Colors  *************************************************/ 
#define KEYPAD_SCREENCOLOR ILI9341_DARKGREEN
#define KEYPAD_ERROR_COLOR   ILI9341_RED      // Color of the Main Screen KEYPAD button if KEYPAD is overridden have an error
#define KEYPAD_INC_DEC_COLOR  ILI9341_LIGHTGREY      /* 192, 192, 192 */
//#define KEYPAD_Orange     ILI9341_ORANGE      /* 255, 165,   0 */

#if (SCREEN_ROTATION == 0) | (SCREEN_ROTATION == 2) // Landscape
  #define KP_BUTTON_WIDTH 50
  #define KP_HEXIDECIMAL_HEIGHT 19
  #define KP_BUTTON_HEIGHT (35-(f_hexidecimal_value*KP_HEXIDECIMAL_HEIGHT))
  #define KP_BUTTON_SPACEX 15
  #define KP_BUTTON_SPACEY 15
  
  #define KP_NUMBER_OFFSETX 20
  #define KP_NUM_OFF 11
  #define KP_NUMBER_OFFSETY (10 - (f_hexidecimal_value*KP_NUM_OFF))
#endif
#if (SCREEN_ROTATION == 1) | (SCREEN_ROTATION == 3) //Portrait
  #define KP_BUTTON_WIDTH 50
  #define KP_HEXIDECIMAL_HEIGHT 19
  #define KP_BUTTON_HEIGHT (50-(f_hexidecimal_value*KP_HEXIDECIMAL_HEIGHT))
  #define KP_BUTTON_SPACEX 15
  #define KP_BUTTON_SPACEY 15
  
  #define KP_NUMBER_OFFSETX 20
  #define KP_NUM_OFF 11
  #define KP_NUMBER_OFFSETY (20 - (f_hexidecimal_value*KP_NUM_OFF))
#endif


 #define KP_BTN_KEY_TEXTSIZE 2
#define KEYPAD_AUTO_COLOR     ILI9341_GREEN
#define KEYPAD_OVERRIDE_COLOR   ILI9341_ORANGE //0xE6C7      // Darker yellow    // ILI9341_YELLOW
#define KEYPAD_VALUE_COLOR  ILI9341_YELLOW
#define KEYPAD_VALUE_BACKGROUND  ILI9341_DARKGREEN
#define KEYPAD_TEXT_COLOR   ILI9341_LIGHTGREY
#define KEYPAD_TEXT_SIZE 2
#define DECIMAL 0
#define HEXIDECIMAL 1

enum _KEYPAD_DATATYPE_
{
  KEYPAD_FLOAT = 1,
  KEYPAD_UINT8 = 2,
  KEYPAD_UINT16 = 3,
  KEYPAD_UINT32 = 4,
  KEYPAD_UINT64 = 5,
} keypad_uints;
// Button locations for this display

// *********** General settings and home screen button information  ********************
uint8_t KEYPAD_ButtonNumber;   // Captures the main screen button number assigned when this routine registers with the main menu display.
uint8_t KEYPAD_DisplayNum;     // Menu number assigned to this routine to allow the use of "Display.ino" infrastructure.

//enum _KEYPAD_DATATYPE_ keypad_uints = KEYPAD_FLOAT;  // Allows different types of numbers to be converted.
uint8_t keypad_previous_menu = 0;
uint8_t ABOUT_KEYPAD_DisplayNum;

void * ptr_keypad_value;
void * ptr_keypad_value_initial;
float keypad_value_initial;    // This will hold the value when this function is entered so hitting Home can restore the value.
float keypad_working_value;   // holds the new value being typed in .

// ****************************************
/* typedef struct keypad_type_s 
{
    void* data;
    void  (*f)(void* data);
} generic_type_t;

void f_int(void* _data)
{
    int* data = (int*)_data;
    // Do something
}

int main()
{
   int data1 = 10;
   // Generic variable initialization
   generic_type_t gv;
   gv.data = &data1;
   gv.f = f_int;

   // Calling an appropriate function
   gv.f(gv.data);
*/

bool f_floatvalue = false;
bool f_hexidecimal_value = false;
bool f_decimal_entry = false;
float DecimalEntry;           // If "f_decimal_entry = true", this will hold the multiplier for the final value.
uint64_t keypad_last_value;

// ****** Function Prototypes ******
void KeyPad_Invocation(/*Hexi or Deci*/ bool _hex,/*Number type*/ enum _KEYPAD_DATATYPE_ _myDatatype, /*Variable being entered*/void * _ptr_keypad_value);


#endif
