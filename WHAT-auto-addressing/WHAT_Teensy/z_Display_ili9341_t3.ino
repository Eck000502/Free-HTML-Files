
#include "z_Display_ili9341_t3.h" // This makes the display easier to use by allowing the definintion of different display screens with different buttons. 

void Display_begin(void) // Allows a main menu screen with touchable buttons.
{
   InitTFT();  // Identify the TFT controller type and take control of the display
}

void Display_Loop(void)  // Display_NB_T.ino  Allows a main menu screen with touchable buttons.
{
  _backlight_control();   // this routine - allows PWM control of backlight brightness
   #define T_MAIN_REFRESH 100      // Only check the screen press every 100mS
   static double t_main_refresh = millis();
   if(t_main_refresh <= millis())
   {
     t_main_refresh = millis()+T_MAIN_REFRESH;
     ReadTouch();
     CheckForDisplayUpdates();
   }
}

//bool Checktouch(/*BTN_FET1_X*/uint16_t _X,/*BTN_FET1_Y*/uint16_t _Y,/*BTN_FET1_W*/uint16_t _W,/*BTN_FET1_H*/uint16_t _H)
//bool Checktouch(uint16_t _X, uint16_t _Y, uint16_t _W, uint16_t _H)
bool Checktouch(uint16_t my_x,uint16_t my_y,uint16_t my_w,uint16_t my_h)
{
  if ((TouchPoint.z<3000) & (TouchPoint.z>800))
  { //touchpoint will be less than 0xFFFF if it is currently being touched.
    //Serial.println(TouchPoint.z);
    if((TouchPoint.x >= my_x) && (TouchPoint.x <= my_x + my_w))
      {
      if((TouchPoint.y >= my_y) && (TouchPoint.y <= (my_y + my_h)))
        { //This touch spot is coincident with this button number.  Execute the callback.
          //Serial.println("T");
          return true;  // touched point is inside the given coordinates
        }
          //Serial.println("F1");
        return false; // touched point is OUTside the "Y" coordinates
      }
          //Serial.println("F2");
      return false; // touched point is OUTside the "X" coordinates
   }
          //Serial.println("F3");
   return false; // the screen was NOT touched
}

//bool Check_new_touch(/*BTN_FET1_X*/uint16_t _X,/*BTN_FET1_Y*/uint16_t _Y,/*BTN_FET1_W*/uint16_t _W,/*BTN_FET1_H*/uint16_t _H)
//bool Check_new_touch(uint16_t _X,uint16_t _Y,uint16_t _W,uint16_t _H)
bool Check_new_touch(uint16_t _my_x,uint16_t _my_y,uint16_t _my_w,uint16_t _my_h)
{
  bool _ret = (Checktouch (_my_x,_my_y,_my_w,_my_h) & new_Touch);
  if (_ret){new_Touch = false;Serial.println("NT");}  // if this returns a 1, then the touchpoint was used, and the touch is no longer new.
  return _ret;
}

// ****************************************************************************************************
/*
 * Purpose:  This must be called in the main loop  
 *           A delay is incorporated here.
 *           The touchscreen is polled periodically. If it has been touched, 
 *           the array of registered buttons will be compared to the touchpoint
 *           and if a match is found, the callback is executed for that button.
 *           
 * Inputs:  Touch   Created by the touchscreen construction process.
 * 
 *          TouchscreenCal    A global structure holding the minimum and maximum values of
 *                            the X and Y values returned from the touchscreen that allows
 *                            re-liniarization of TouchPoint to reflect the bitmap coordinates.
 *          TOUCHRATE     Refresh rate for polling the touchscreen
 * 
 * Outputs: TouchPoint    A global structure that holds the X/Y coordinates where the touchscreen was touched.
 *          Returns       true/false ti indicate if the screen is currently being activated.
 * 
 * 
 */
void ReadTouch(void)
{
   static double t_TouchDelay = millis();
   uint16_t TempX;    // The X and Y touchpoints are reversed (with respect to the display writing), so this will hold the X while they are swapped.
   //new_Touch = false;  // new_Touch will be reset to false every program loop.
   static bool _last_Button = false;
    if(t_TouchDelay <= millis())
    {
     t_TouchDelay = millis() + TOUCHRATE;  
     TouchPoint = Touch.getPoint();

      if ((TouchPoint.z<3000) & (TouchPoint.z>800))
      {
      //Serial.printf ("X:%d  Y:%d   Z:%d ",TouchPoint.x,TouchPoint.y,TouchPoint.z);
      //Serial.println ();
        //Serial.println(TouchPoint.z);
        t_BacklightTimeout = millis() + BACKLIGHT_TIMEOUT_RATE;   // Whenever the screen is touched, reset the backlight timer.

        if(gbl_backlightBrightness>0) // if the backlight is currently ON....
        {
          if(gbl_waking_touch == false) // Only process the touch if it was NOT the one that woke up the display.
          {
             if(! _last_Button) new_Touch = true;  //once the last button is RELEASED, new_Touch will be reset.
             _last_Button = true;

            //TouchPoint = Touch.getPoint();
  
            //TempX = map(TouchPoint.x,TouchscreenCal.X + TouchscreenCal.W ,TouchscreenCal.X,max_Y_coordinate,0);
            TempX = map(TouchPoint.x,TS_MAXX ,TS_MINX,max_Y_coordinate,0);
            //TouchPoint.x = map(TouchPoint.y,TouchscreenCal.Y,TouchscreenCal.Y + TouchscreenCal.H,max_X_coordinate,0);   // Re-map the touch points to allign with the bitmap (Note the touch X and Y xalues are swapped.
            TouchPoint.x = map(TouchPoint.y,TS_MINY,TS_MAXY,max_X_coordinate,0);   // Re-map the touch points to allign with the bitmap (Note the touch X and Y xalues are swapped.
            constrain(TouchPoint.x,0,max_X_coordinate);
            TouchPoint.y = TempX;   // The X/Y orientation now matches the pixels.
            constrain(TouchPoint.y,0,max_Y_coordinate);
            //Serial.print("Pressure = ");
            //Serial.print(TouchPoint.z);
            //Serial.print(", x = ");
            //Serial.print(TouchPoint.x);
            //Serial.print(", y = ");
            //Serial.print(TouchPoint.y);
            //Serial.println();
  
            } // End if waking touch
        }// End if backlight is on
      }// End if touched
      else
      {
        TouchPoint.z = 0;
        TouchPoint.x = 0;
        TouchPoint.y = 0;
        _last_Button = false;
        gbl_waking_touch = false;     // When the touch that woke the display is released, allow future pressed to be registered as actual presses.
      }
  }// end if delay has expired
}
// ****************************************************************************************************
/*
 * Purpose:  Monitors the backlight and turns it off if the touchscreen has not been touched in a while.
 *  
 *        
 * Inputs:   none
 * 
 * Outputs: Displayed button will be updated.
 *          .NeedRefresh will be set to False.
 */
void CheckForDisplayUpdates(void)
{
  if (t_Display_UpdateTimer < millis()  )
  {
    t_Display_UpdateTimer = millis() + DISPLAY_UPDATE_RATE;

    if(t_BacklightTimeout <millis())
    {
     gbl_backlightBrightness= gbl_MinBacklight;                // Turn off the backlight when the timer expires.
     gbl_waking_touch = true;                       // The next touch will wake the display, and should not count as a button press.
    }
    else
    {
        gbl_backlightBrightness= gbl_MaxBacklight;   // Turn on the backlight if the timer is not expired.
    }
  }
  static uint8_t LastActiveMenuNumber = 0xFF;

  if(LastActiveMenuNumber != gbl_ActiveMenuNumber)
  {
    LastActiveMenuNumber = gbl_ActiveMenuNumber;
    Display_ClearScreen(/*Screencolor*/showDisplay[gbl_ActiveMenuNumber].COLOR);
    (* showDisplay[gbl_ActiveMenuNumber].callback)();
    //if(gbl_ActiveMenuNumber != 0)Show_btn_GoToMainMenu();      // Show the Home Button if on some screen other than the home screen.
  }
}

void InitTFT(void)
{
  // put your setup code here, to run once:
// ************ Setup for TOUCH part of Touchscreen  ******************/
  Touch.begin();
  Touch.setRotation(SCREEN_ROTATION);// the rotation for the display is different than the touchpanel 

//    / ********* Setup for DISPLAY part of Touchscreen  **************** /
  tft.begin();
  // origin = left,top landscape (USB left upper)
  tft.setRotation(SCREEN_ROTATION+1); // the rotation for the display is different than the touchpanel 
//  tft.drawRect(FRAME_X, FRAME_Y, FRAME_W, FRAME_H, ILI9341_BLACK);

  //ScreenColor[gbl_ActiveMenuNumber] = ColorInversion^BACKGROUND; // THIS MUST BE THE FIRST REGISTRATION FOR THE DISPLAY
  
  pinMode(TFT_BACKLIGHT_PIN,OUTPUT);    // backlight for the display
  digitalWrite(TFT_BACKLIGHT_PIN,HIGH); // Turn the backlight on.
  gbl_backlightBrightness = 100;        // Turn on the backlight to full value on power up.
 
//    / ********* Setup for DISPLAY part of Touchscreen  **************** /
//  tft.drawRect(FRAME_X, FRAME_Y, FRAME_W, FRAME_H, ILI9341_BLACK);
  max_X_coordinate = tft.width();
  max_Y_coordinate = tft.height();
  Serial.print("TFT size is "); Serial.print(max_X_coordinate); Serial.print("x"); Serial.println(max_Y_coordinate);
 
  tft.fillScreen(ColorInversion^ILI9341_BLUE);
  #if SCREEN_ROTATION == 2
  tft.setCursor(((max_Y_coordinate/2)-40) , (max_X_coordinate/2));   // Move the cursor to this location for printing.
  #else
   tft.setCursor(((max_X_coordinate/2)-40) , (max_Y_coordinate/2));   // Move the cursor to this location for printing.
  #endif 
  tft.setTextSize(2);   
  tft.setTextColor(ILI9341_WHITE);            
  tft.print("Initializing...");                         
}

void Display_ClearScreen (uint16_t _screencolor)
{
  //pinMode(TFT_BACKLIGHT_PIN,OUTPUT);    // backlight for the display
  //digitalWrite(TFT_BACKLIGHT_PIN,HIGH); // Turn the backlight on.

  tft.fillScreen(ColorInversion^_screencolor);
}

//****************************************************************************************************
// ****************************************************************************************************
/*
 * Purpose: PWM control of backlight brightness
 * Inputs:  Microsecond counter 
 * 
 * Outputs: digital control of FET controlling the backlight.
 */
void _backlight_control(void)   // this routine - allows PWM control of backlight brightness
{
  #define _BACKLIGHT_PWM_RATE 100
  static double _backlight_timer = micros();
  static uint8_t  _backlight_PWM_Count = 0;

  if(_backlight_timer <= micros())
  {
    _backlight_timer = micros() + _BACKLIGHT_PWM_RATE; // reset the backlight delay timer.
    _backlight_PWM_Count++;
    if (_backlight_PWM_Count>99)
     {
        _backlight_PWM_Count = 0;
     }
    if(_backlight_PWM_Count >= gbl_backlightBrightness )
    {
      digitalWrite(TFT_BACKLIGHT_PIN,LOW);
    }
    else
    {
      digitalWrite(TFT_BACKLIGHT_PIN,HIGH);
    }
  }
}

//****************************************************************************************************
// ****************************************************************************************************
/*
 * Purpose:  Superflous routine that puts an X/Y grid on the screen  
 *           Each gridline is spaced evenly allowing measurements of pixels.
 * Inputs:  #define 
 * 
 * Outputs: SerialMonitor text indicating button that was pressed.
 *          Pressed button is filled with a white rectangle.
 */

void  drawGrid(void)
{
#define GRIDLINE_COLOR ILI9341_LIGHTGREY

  // Display test.  Draw grid.
  for(uint16_t x=0;x<max_X_coordinate;x=x+20)
  {
    tft.drawFastVLine(x,0,max_Y_coordinate,GRIDLINE_COLOR);
  }
  for(uint16_t y=0;y<=max_Y_coordinate;y=y+20)
  {
   tft.drawFastHLine(0,y,max_X_coordinate,GRIDLINE_COLOR);
  }
}

// Show_btn_GoToMainMenu(/*Color of the button*/ILI9341_GREEN, /*Color of the text*/ILI9341_WHITE, /*line1 Text*/ String "Power is",/*line2 Text*/ String "  ON ");
// *********** Button sto change to Cold Boot menu *******************
#define HOME_BUTTON_X (max_X_coordinate - HOME_BUTTON_W)  // Sets the close button to the upper right corner
#define HOME_BUTTON_Y 0                                    // Sets the close button to the upper right corner      
#define HOME_BUTTON_W 30 // This will be subtracted from the max screen width
#define HOME_BUTTON_H 30 // This will be added to the 0 row (at the top)
#define HOME_BUTTON_COLOR ILI9341_RED

void Show_btn_GoToMainMenu(void)
{
  //tft.writeRect(_BTN_GoToMainMenu_X, _BTN_GoToMainMenu_X, 30, 30, (uint16_t*)Home);
  tft.fillRect(HOME_BUTTON_X, HOME_BUTTON_Y,HOME_BUTTON_W, HOME_BUTTON_H, HOME_BUTTON_COLOR); //A RED box, for now.
}
// **********
void btn_GoToMainMenu()
{
  if(Check_new_touch(HOME_BUTTON_X,HOME_BUTTON_Y,HOME_BUTTON_W,HOME_BUTTON_H))
  { // The display area with this button has been touched this program cycle.
    gbl_ActiveMenuNumber = 0;
  }
}
// ****************************************************************************************************
/*
 * Purpose:  This function should be called in the setup routine for any sub-function 
 *           that desires a seperate menu activation.  The order in which 
 *           they call RegisterDisplay determines their menu number.
 *           ** The main reason for this is so you can return to a previous menu.  This will be a set of callbacks.
 * Inputs:
 *          none
 *                        
 * Outputs:
 *        uint8_t gbl_ActiveMenuNumber    The MainScreenButtons[gbl_ActiveMenuNumber][][] index will be returned by the function so the 
 *                                registering routine will be able to reference the assigned 
 *                                menu number
 */
uint8_t MakeNewDisplay(uint16_t _screencolor,void (*_menudisplay)(void))
{
    static uint8_t numDisplays = 0;
   if((numDisplays) >= NUM_DIFFERENT_MENUS)
    {
      Serial.println(F("Maximum number of display sreens reached.  This button was NOT registered."));
      return 0xFF; // do not change the screen beyond the number of allowed buttons.
    }
  showDisplay[numDisplays].callback = _menudisplay;  // This adds the new screen to the list of screens.
  showDisplay[numDisplays].COLOR = _screencolor;
  numDisplays++;  // Add a new display page.
  return (numDisplays -1); // return the current page number.
}
