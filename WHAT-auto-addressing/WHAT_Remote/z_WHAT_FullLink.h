#ifndef __WHAT_FULLLINK_h
#define __WHAT_FULLLINK_h
#define WHAT_FULLLINK_VERSION 260916

/*
#include "z_WHAT_FullLink.h"   // -This routine- as part of the WHAT system.
  // If this device will allow "full links... "
#ifdef __WHAT_FULLLINK_h
  WHAT_LINK_begin();  // Setup the specific IO in this file.
#endif
#ifdef __WHAT_FULLLINK_h
  Loop_WHAT_LINK();
#endif

 Define these in the main program: 
  #define WHAT_CHIP_SELECT 7   // Hardware chip select pin
   WHAT_Packet WHAT_Tx_Packet;  // Make a temporary transmit packet
 Define these in the main program: 
#include "z_WHAT.h"  // Wireless zigbee communications.

*/

/*
This allows smaller devices to use teh NETWORK button to full-link to other small devices.
This is not necessary for larger devices.
for instance, WHAT_Remote hardware only has 3 inputs and 3 outputs, and no  display.
  How would you indicate that Input 1 of Board 1 should control Output 1 of Board 2?
  By adding full link capability, you can HOLD the network button on one WHAT_Remote, then acknowledge the link by 
  pressing the network button on the second WHAT_Remote, and instantally:
    Input 1 of Device 1  controls OUTPUT 1 of device 2
    Input 2 of Device 1  controls OUTPUT 2 of device 2
    Input 3 of Device 1  controls OUTPUT 3 of device 2
      AND
    Input 1 of Device 2  controls OUTPUT 1 of device 1
    Input 2 of Device 2  controls OUTPUT 2 of device 1
    Input 3 of Device 2  controls OUTPUT 3 of device 1

This allows consumers to just buy 2 WHAT_Remote boards, with NO central controller, to instantly be useable.
Then as additional devices are added, control can be re-arranged or expanded.
      
*/

#define LINK_MONITOR  1

// These are only going to be DIGITAL values.
#define NUM_OUTPUTS 3   // (Limit to 8) Each OUPUT can hold a link to being controlled by an INPUT.
#define NUM_INPUTS  3   // (Limit to 8) The inputs really just broadcast their value.  It is up to the recieving output to monitor this input.
#define numWHATLinks 16  // Number of IDs that this device automatically ties recieved inputs and outputs.


#define _WHAT_NEW_LINK_HELD_TIME_DELAY  5000  // Time the network button must be held while in NetAlive before initiating a link
#define _WHAT_NEW_Link_TIME_DELAY 60000 // Only stay in new child mode for 30 seconds if not accepted earlier.
#define _WHAT_LINK_INPUT_UPDATE_RATE 50   // minimum milliseconds between message

// Group: WHAT_LinkIO (instance gblLinkIO) - FullLink's input/output pin and value tables.
struct WHAT_LinkIO
{
  uint8_t inputPin[NUM_INPUTS];     // Pin number
  uint8_t inputValue[NUM_INPUTS];   // Value read for the given pin.
  uint8_t outputPin[NUM_OUTPUTS];   // Pin number
  uint8_t outputValue[NUM_OUTPUTS]; // Value for the given pin.
};
WHAT_LinkIO gblLinkIO;

// 888888888888888888 Prototypes 88888888888888888888888888
// Public:
//void  mrf.init(void);
//void Mrf24j_begin (uint16_t gblNetwork.panID, uint16_t mySourceAddress);

// Private

// 888888888888888888 Prototypes 88888888888888888888888888

//#define ZIG_WAKE_TIMEOUT 1000
//double WAKE_Timer = millis(); // Global virtual Key Timer
//bool WAKE_ON = false;         // Actual state of the virtual Key that can be monitored by all simulations.


enum WHAT_LINK_State{
  WHAT_NO_LINK,
  WHAT_PRE_LINKING_INITIATED,
  WHAT_LINKING_INITIATED,
  WHAT_PRE_LINKING_WAITING,
  WHAT_PRE_LINK_REQUESTED,    // Another device might want to link with this device.
  WHAT_LINK_REQUESTED,    // Another device might want to link with this device.
  WHAT_REMOVE_LINK,        // the device this is already linked to is requesting a link again.  If the button is pressed, remove the link.
  WHAT_LINK_COMPLETED
};

// Group: WHAT_Link (instance gblLink) - FullLink state.
struct WHAT_Link
{
  WHAT_LINK_State state = WHAT_NO_LINK;
  uint16_t alreadyLinked = 0xFF;
  uint16_t links[numWHATLinks];
  uint8_t commandedOutputs = 0;  // Captures the payload value of a received input message. (was gblLink.commandedOutputs)
  uint8_t reusableSlot = 0xFF;   // Registered once, then the callback can be changed. (was gblLink.reusableSlot)
};
WHAT_Link gblLink;
//bool * _network_button = &WHAT_NetworkButton;     // This will mirror the button styate in the main program.
//WHAT_State * _network_state = &gblMailbox.networkState;    // This will mirror the network light words in the main program.
//WHAT_LedColor * _network_light_color = &gblMailbox.networkStateColor;  // This will mirror the Light color to the main program.

// ***************************
// TOC Global Variables
// (gblLink.reusableSlot and gblLink.commandedOutputs moved into gblLink - Nov 2026 rename pass.)

// Group: WHAT_LinkRxFlags (instance gblLinkRxFlags) - FullLink's "a message arrived" flags.
// Member names are the command names from z_WHAT_Command.h, same rule as gblRxFlags.
struct WHAT_LinkRxFlags
{
  bool f_REQUEST_FULL_LINK_03 = false;
  bool f_ACCEPT_FULL_LINK_03 = false;
  bool f_REJECT_FULL_LINK_03 = false;
  bool f_REMOVE_FULL_LINK_03 = false;
  bool f_CMD_DIG01_04 = false;
};
WHAT_LinkRxFlags gblLinkRxFlags;



#endif
