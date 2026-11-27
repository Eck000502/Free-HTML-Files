
#ifndef __WHAT_h
#define __WHAT_h
#define WHAT_SOFTWARE_VERSION 26112701
// This revision requires library to set BBREG2 or it will not work. 

//#include "z_WHAT.h"  // Wireless zigbee communications.
//WHAT_begin(bool *_network_button, uint8_t *_network_light); // WHAT.ino -  Configures and autoaddresses Wireless Home Automation and Telemetry
//Loop_WHAT(void) // WHAT.ino -  Monitors and communicates for Wireless Home Automation and Telemetry
/* Define these in the main program: */
//#define WHAT_CHIP_SELECT 7   // Hardware chip select pin
//Mrf24j mrf(WHAT_CHIP_SELECT);
//WHAT_Packet WHAT_Tx_Packet;  // Make a temporary transmit packet
/* Define these in the main program: */

//#define WHAT_CHIP_SELECT ZIGBEE_CS   // Hardware chip select pin
#define EEPROM_WHAT_LOCATION 0x6 // redefine if this EEPROM address is used by other routines.
//#define EEPROM_WHAT_SHOW_VALUES true // At the end of reading and writing, this prints all EEPROM values to the monitor.
#define AUTO_ADDRESSING true    // If this is commented out, auto-addressing is disabled.
// Serial monitor switches for the WHAT infrastructure prints - commented out in the
// WHAT_RemoteControl build (Gary's call, carried over from the Nov 24, 2026 quieting
// of this folder) so the application's own serial output is not buried. The
// WHAT_Remote folder's copies keep these ON for bench work. The adopted base's
// infrastructure prints are gated under these switches in this folder's copies.
//#define RX_MONITOR true     // If this is uncommented, recieved messages will be displayed on the serial monitor.
//#define TX_MONITOR true     // If this is uncommented, recieved messages will be displayed on the serial monitor.
//#define RETRANSMIT true     // This will quickly allow/prevent retransmission of messages (based on criteria).
//#define CHILD_MONITOR true   // Gives serial monitor text during linking process.
//#define DX_ONLY true        // Gives more detailed information about raw recieved messages.
//#define MATCH_MONITOR true  // PROPOSED (Nov 25, 2026): per-message match confirmations (pan / destination / MID match and compare lines). Off at Gary's request for a cleaner serial log; the raw packet dump (DX_ONLY) and the join narrative (CHILD_MONITOR) stay on.

#include <mrf24j_G.h> // *****************  trying to remove this from the final project.
#include "z_WHAT_Command.h"


/*
 * Allows Zigbee communications.
 * Easy message reception can be handled through a registration process.
 *  RegisterZIGMessage (uint16_t PANID_Mask, uint16_t PANID_Value, uint16_t Dest_Mask, uint16_t Dest_Value, uint16_t Source_Mask, uint16_t Source_Value, void(*callback)(ZIG_Packet))
 *  the "callback" is the function that is called when the specified message arrives. 

 * I pulled the useful information from the mrf24jmodule library into this file.
 * Without needing the library, this file initializes the mrf24j module, and will establish "WHAT" communications
 * including:
 *    - Network creation
 *    - Joining to network (with auto-addressing)
 *    - child/parent protocols
 *    - MessageID registration, monitoring, reception and callback.

*/


// * EXAMPLE Button/Light Interface Routine.
// *  A version of this should be placed in your MAIN program.
// *  The networking architecture requires that the user physically accept
// *  the addition of new devices, which gives a semblance of security.
// *  
//void Loop_Network_Button_Light(void)    // checks the RAW button reserved for networking, and outputs the light state.
//{
//  /*  This does not make sense at first, however by operating the network button and light
//   *   through variables, the Zigbee is passed a pointer to the variables, and the actual 
//   *   state of the light and button can be either hardwired, on a touchscreen, or even 
//   *   completely remote through the Zigbee connection!  The variables are the virtual state
//   *   of the input and output no matter what form that I/O takes.
//
//   */
//  static uint8_t _LastNetworkLight = 255;   // initialize the value as invalid so it will get the initial color updated.
//  
//  //WHAT_NetworkButton = digitalRead(b_BUTTON_PIN);   // Read the state of the button designated for networking, and put it in the holding variable.
//  btn_NETWORK();
//
//  //digitalWrite(b_BUTTON_LIGHT_PIN,WHAT_NetworkLight); //make the physical light reflect the state of the network light value.
//
//   if(_st_lastNetworkStateColor != gblMailbox.networkStateColor )
//   {
//     _st_lastNetworkStateColor = gblMailbox.networkStateColor;
//     switch(gblMailbox.networkStateColor)
//     {
//       case WHAT_LIGHT_OFF:
//         digitalWrite(b_RED_RGB_PIN,HIGH);   //Light off.
//         digitalWrite(b_GREEN_RGB_PIN,HIGH); 
//         digitalWrite(b_BLUE_RGB_PIN,HIGH);
//         // Show_btn_NETWORK(/*Color of the button*/ILI9341_DARKGREY, /*Color of the text*/ILI9341_BLACK, /*line1 Text*/ "Network",/*line2 Text*/ "  OFF   ");
//         break;
      
//      case WHAT_LIGHT_RED:
//         digitalWrite(b_RED_RGB_PIN,LOW);    // Red light.
//         digitalWrite(b_GREEN_RGB_PIN,HIGH); 
//         digitalWrite(b_BLUE_RGB_PIN,HIGH);
// //        Show_btn_NETWORK(/*Color of the button*/ILI9341_RED, /*Color of the text*/ILI9341_BLACK, /*line1 Text*/ "Network",/*line2 Text*/ "FAILED");
//         break;
 
//     case WHAT_LIGHT_GREEN:
//         digitalWrite(b_RED_RGB_PIN,HIGH);  //Light Green
//         digitalWrite(b_GREEN_RGB_PIN,LOW); 
//         digitalWrite(b_BLUE_RGB_PIN,HIGH);
//         // Show_btn_NETWORK(/*Color of the button*/ILI9341_DARK_RED, /*Color of the text*/ILI9341_BLACK, /*line1 Text*/ "Network",/*line2 Text*/ "PENDING");
//         break;
 
//     case WHAT_LIGHT_BLUE:
//         digitalWrite(b_RED_RGB_PIN,HIGH);  //Light Blue
//         digitalWrite(b_GREEN_RGB_PIN,HIGH); 
//         digitalWrite(b_BLUE_RGB_PIN,LOW);
//         // Show_btn_NETWORK(/*Color of the button*/ILI9341_DARK_RED, /*Color of the text*/ILI9341_BLACK, /*line1 Text*/ "Network",/*line2 Text*/ "PENDING");
//         break;
         
//      default:
//         Serial.print("Invalid Light State : 0x");Serial.println(gblMailbox.networkStateColor,HEX);
//         gblMailbox.networkStateColor = WHAT_LIGHT_OFF;
//         break;   
//     }
//     Show_btn_NETWORK(/*Color of the button*/ILI9341_DARKGREEN, /*Color of the text*/ILI9341_BLACK, /*line1 Text*/ " Online ",/*line2 Text*/ String(gblNetwork.panID));
//         break;


#include <SPI.h>
#include "z_WHAT_Command.h" //All command data bytes used with WHAT
#include "z_mrf24j_Registers.h"

//  **********************   Network Button and lights   **********************  //
//#define NETWORK_LIGHT  A5
//#define NETWORK_BUTTON A4

enum WHAT_LedColor{
  WHAT_LIGHT_OFF,
  WHAT_LIGHT_RED,
  WHAT_LIGHT_GREEN,
  WHAT_LIGHT_BLUE,
  WHAT_LIGHT_WHITE   // PROPOSED (Nov 25, 2026): all three channels on. A preemie that powered on with its network button held shows steady WHITE while searching - "I am available to replace a missing unit."
};

enum WHAT_State{

  WHAT_STARTUP,
  WHAT_PRE_INVALID,
  CHECK_PREEMIE_RESET,
  WHAT_RESET_PREMIE_INITIATED,
  WHAT_RESET_PREMIE_PENDING,
  WHAT_RESET_PREMIE_AWAITING_ACTIVATION,

  WHAT_PREEMIE_RESTORED,
  WHAT_PRE_PREEMIE_NETWORK_SEARCH,  // Looking to see if a network is seen.
  WHAT_PREEMIE_NETWORK_SEARCH,    // Looking to see if a network is seen.
  WHAT_NETWORK_SEEN,      // An existing network is seen, but no actions have been taken to join it yet.
  WHAT_NETWORK_SEEN_WAIT, // Preemie sees an existing network, and is collecting offered child addresses while waiting for net button press.
  WHAT_NETWORK_CREATING,  // Solid BLUE light while creating the network.
  WHAT_NETWORK_CREATED,   // This device created the network and is the Master Parent Node 0000
  WHAT_PRE_NETWORK_ALLOW_PREEMIE_04,
  WHAT_NETWORK_JOINING,   // This device is joining the network.
  WHAT_PRE_NETWORK_ABORT,
  WHAT_NETWORK_ABORT,
  WHAT_PRE_CONFIRM_NEW_CHILD,
  WHAT_CONFIRM_NEW_CHILD,    
  WHAT_NEW_CHILD_CONFIRMED,


  /*When a device is a part of a network: */
  WHAT_PRE_NEW_CHILD_REQ,
  WHAT_NEW_CHILD_REQ,     // A device is asking to be a child of this device.
  WHAT_NEW_CHILD_ALLOWED, // A device is asking to be a child of this device.
  WHAT_PRE_NEW_CHILD_OFFER, // Sending an offered child address.
  WHAT_NEW_CHILD_OFFER,

   /*Forming Network values */
  WHAT_PRE_NEW_NETWORK_ACK,
 WHAT_NETWORK_NONE,      // Network has timed out.
  WHAT_PRE_NETWORK_ERROR,
  WHAT_NETWORK_ERROR,     // Network Connection Error
  WHAT_NETWORK_ERROR_WAIT,  // Holds the light blinking red for a few seconds.
  WHAT_PRE_NET_ALIVE,
  WHAT_NET_ALIVE,         // A device is connected to an active network.
  WHAT_LINKING,         // This is only used if z_WHAT_FullLink.h is included.

  // PROPOSED (Nov 25, 2026): appended at the END of the enum so every existing
  // state's number - which Gary reads in the serial logs - stays unchanged.
  WHAT_REPLACE_INTENT_CHECK,        // A preemie powered on with its button held: validating the hold before declaring replacement intent.
  WHAT_RESET_PREMIE_AWAITING_CONFIRM  // A leaver has sent ABANDON_CHILD_05 and is waiting (bounded retries) for the parent's REPLY_ABANDON_CONFIRM_03 before wiping itself.

};

// Group: WHAT_Mailbox (instance gblMailbox) - the app <-> WHAT core interface.
// Only the application layer and the WHAT core touch these, each in one direction.
struct WHAT_Mailbox
{
  bool networkButton = 0;                    // App writes, WHAT reads: state of the physical or virtual network button.
  WHAT_State networkState = WHAT_STARTUP;    // WHAT writes, app may display: current network status.
  WHAT_LedColor networkStateColor = WHAT_LIGHT_OFF; // WHAT writes, app renders: network light color (RGB pins, or a screen section).
  bool childLost = false;                    // WHAT writes, app may display: PROPOSED (Nov 25, 2026) - at least one of this device's children has been silent past WHAT_CHILD_LOST_MS. The core blinks the network light RED while this is set; an app with a richer display may show more.
  uint16_t childLostAddress = 0xFFFF;        // WHAT writes, app may display: PROPOSED (Nov 25, 2026) - the indicated lost child (a trunk child outranks fill children, then the longest-lost). This is the address a replacement offer carries if a replacement preemie is accepted; the indication itself authorizes nothing (Gary's ruling, Nov 25, 2026 - there is no button-erase).
};
WHAT_Mailbox gblMailbox;

//  **********************   Network Button and lights   **********************  //
// Uncomment ONE of these to determine what type of device this is.
#define WHAT_PERMANENT 0x41  // The intent is for this device to be a permanent member of this network.
//#define WHAT_TEMPORARY  // This device will only be part of the network for x days,and must re-new its membership.
//#define WHAT_PART_TIME  // This device will come and go, but stays a permanent member of the network 
//#define WHAT_LEASE      // This device will come and go, but must re-new its lease periodically.
#define TX_BUFFER_DEPTH 3 //This will have a dramatic effect on Dynamic memory, so try to keep it small.
#define RX_BUFFER_DEPTH 3 //This will have a dramatic effect on Dynamic memory, so try to keep it small.


#ifdef AUTO_ADDRESSING

#define ChildMaxIndex 15
// Group: WHAT_Joining (instance gblJoining) - an offer in progress while a preemie joins.
// These only have meaning during joining; reset them together when a join ends or times out.
struct WHAT_Joining
{
  uint16_t panID = 0xFF;          // PANID of the network being joined.
  uint16_t sourceAddress = 0xFF;  // Lowest new address offered.
  uint16_t parent = 0xFF;         // Parent offering that address.
  uint32_t latchkey = 0xFFFFFFFF; // Latchkey offered by the parent with the lowest offered address. (32-bit since STEP 1, Nov 24, 2026.)
  uint16_t tempID = 0xFFFF;       // STEP 1 (Nov 24, 2026): random per-attempt ID binding one join attempt - on a preemie, MY id, generated when a search begins, echoed back to me in the allow / offers / accept.
  uint16_t heardTempID = 0xFFFF;  // STEP 1 TIEBREAK (Nov 24, 2026): the temp ID of the preemie whose search (or alert) was last heard. Members echo THIS in their alert / allow / propose and check it against the accept; a searching preemie compares it against its own tempID to decide which of them founds a network.
  uint16_t joinCooldownTempID = 0xFFFF;  // JOIN COOLDOWN (Nov 27, 2026, Gary): the join attempt (temp ID) this member is cooling down from - armed when a join engagement ends with no join (see z_WHAT.ino WHAT_NET_ALIVE gate).
  uint32_t joinCooldownUntil = 0;        // JOIN COOLDOWN (Nov 27, 2026): millis() deadline of the cooldown, 0 = none. While it runs, WHAT_NET_ALIVE ignores that attempt's searches so the lamps rest between failed attempts; a button press or a heard allow clears it at once.
  uint32_t confirmNonce = 0;      // STEP 1 (Nov 24, 2026): preemie-side only - the nonce sent in ACCEPT_PARENT_06, against which the parent's NETWORK_JOIN_COMPLETE_03 proof is checked.
  uint16_t trunkCandidate = 0xFFFF; // PROPOSED (Nov 25, 2026): member-side - the trunk address being claim-checked / offered in this join attempt (0xFFFF = none).
  uint16_t ackSourceAddress = 0xFFFF; // PROPOSED (Nov 25, 2026): source address of the most recent REPLY_ACKNOWLEDGE_03. (Founding ignores it; the trunk claim check matches it against trunkCandidate.)
  bool trunkClaimPending = false;   // PROPOSED (Nov 25, 2026): a trunk ACK claim check is waiting out its silence window.
  bool trunkClaimPassed = false;    // PROPOSED (Nov 25, 2026): the silence window expired with no reply - the trunk address is free to offer.
  double trunkClaimDeadline = 0;    // PROPOSED (Nov 25, 2026): when the claim check's silence window ends (millis()-based, like the other timers).
  bool replaceIntent = false;       // PROPOSED (Nov 25, 2026): PREEMIE side - this device powered on with its network button held, so its searches carry the replacement flag and its light shows WHITE. Cleared when it joins a network or founds one.
  bool heardReplaceIntent = false;  // PROPOSED (Nov 25, 2026): MEMBER side - the searching preemie whose attempt is in flight declared replacement intent (latched by Rx_REQUEST_JOIN_NETWORK_03 from the search's flags byte).
  bool replaceOffer = false;        // PROPOSED (Nov 25, 2026): MEMBER side - the offer currently being made is a replacement offer for the lost child in replaceSlot, not an ordinary fill / trunk offer.
  uint8_t replaceSlot = 0xFF;       // PROPOSED (Nov 25, 2026): MEMBER side - the child-table slot whose lost record a replacement offer would supersede at commit.
  bool replaceOfferReceived = false; // PROPOSED (Nov 25, 2026): PREEMIE side - a replacement offer has been taken (it outranks ordinary offers, which are then ignored for this attempt).
};
WHAT_Joining gblJoining;
// NOTE (Nov 2026 rename pass): _WHAT_Child_Used is no longer a global - it is a local
// variable at its one call site in z_WHAT.ino (Gary's decision).

struct ChildType
{
  uint16_t ChildAddress = 0xFFFF;
  uint32_t LatchKeyValue = 0xFFFFFFFF;  // 32-bit since STEP 1 (Nov 24, 2026) - was a 16-bit value derived from millis().
  uint8_t  RelationshipType = 0;
  uint16_t  Security = 0xFFFF;
};

// gblNetwork.children[0] is used by THIS device to hold its own information
#define mySourceAddress gblNetwork.children[0].ChildAddress   // This is saved in and loaded from EEPROM
#define myLatchKeyNumber gblNetwork.children[0].LatchKeyValue  // A secret number shared between parent and child nodes.
#define mySecurity gblNetwork.children[0].Security

// Group: WHAT_Network (instance gblNetwork) - who I am, who my family is. Saved to EEPROM field-by-field.
// (The macros above point into gblNetwork.children[0] - this device holds its own info there.)
struct WHAT_Network
{
  uint16_t panID = 0xFFFF;          // This is saved in and loaded from EEPROM
  uint16_t parentAddress = 0xFFFF;  // When I am adopted, I will record the parent's address
  ChildType children[ChildMaxIndex]; // each device can have up to 15 children. Index 0 is THIS device.
  uint32_t childLastHeard[ChildMaxIndex]; // PROPOSED (Nov 25, 2026): millis() of the last traffic heard from each child's subtree (slot 0 unused). RAM ONLY - never stored to EEPROM: a power cycle must not resurrect a stale "heard". WHAT_begin stamps restored children with the boot time, so each gets a full lost interval to reappear before it can be flagged.
  bool hasChildren = false;         // quick check (during message forwarding) to see if this device has children.
                                    // This device does not need to forward any messages if it does not have children.
                                    // See WHAT.ino/RX_WHAT (after switch)
};
WHAT_Network gblNetwork;

// STEP 1 (Nov 24, 2026): the liveness heartbeat interval. Every member guarantees the
// network at least one transmission per interval (WHATHouseKeeping sends a
// NETWORK_HEARTBEAT_03 if the device has been transmit-quiet for the whole interval).
// The network-alive timeout below is DERIVED from this (Gary's rule: 2 x heartbeat) -
// one missed heartbeat is tolerated, two flip the alive light from green to blue.
#define WHAT_HEARTBEAT_INTERVAL_MS 10000

// PROPOSED (Nov 25, 2026): a child counts as LOST when no traffic from its
// subtree has been heard for this long. DERIVED from the heartbeat interval by
// the same rule as the network-alive timeout (Gary, Nov 24, 2026: 2 x
// heartbeat) - every member transmits at least once per heartbeat interval, so
// two silent intervals mean the child is gone or out of range, never merely
// quiet. A lost child that is heard again is simply back: its record and
// latchkey were never touched.
#define WHAT_CHILD_LOST_MS (2 * WHAT_HEARTBEAT_INTERVAL_MS)

// The network-alive deadline stays a standalone global, NOT a member of gblNetwork:
// its value is millis()+20000 - a runtime value, not a compile-time constant. Putting it
// inside the struct would make the whole struct dynamically initialized, and the compiler
// then keeps a second template copy of the children array in RAM (+105 bytes on a Nano).
double gbl_t_NetworkAlive = millis()+(2 * WHAT_HEARTBEAT_INTERVAL_MS);  // Timeout - when messages on this PANID arrive, reset this.
bool f_gbl_t_NetworkAlive = true;           // Flag that other network nodes have been seen using THIS PANID
double gbl_t_NextHeartbeat = millis()+WHAT_HEARTBEAT_INTERVAL_MS;  // STEP 1: deadline for this device's next guaranteed transmission - re-armed by WHAT_TransmitPacket on every send, watched by WHATHouseKeeping. Standalone for the same reason as gbl_t_NetworkAlive above.

// (gblNetwork.panID, gblNetwork.parentAddress, gblNetwork.hasChildren and the network-alive timer/flag moved into
//  the WHAT_Network group defined just below the macros that follow - Nov 2026 rename pass.)
// NOTE: the global myMask that used to be declared here is commented out (Gary, Nov 2026):
// it appeared unused - the retransmit code in z_WHAT.ino declares its own local myMask.
//uint16_t myMask = 0x000F;     // this mask is re-calculated when a source address is obtained.  it can be used to remove children from an address comparison, making sure the message should be routed through this device.

#endif  // If Autoaddressing

#ifndef ArrayElementSize
  #define ArrayElementSize(x) (sizeof(x)/sizeof(x[0]))
#endif
//bool * _network_button = &WHAT_NetworkButton;     // This will mirror the button styate in the main program.
//WHAT_State * _network_state = &gblMailbox.networkState;    // This will mirror the network light words in the main program.
//WHAT_LedColor * _network_light_color = &gblMailbox.networkStateColor;  // This will mirror the Light color to the main program.

//void * _Loop_Network_Button_Light();
#define GLB_T_NETWORKALIVE_RESET (2 * WHAT_HEARTBEAT_INTERVAL_MS)  // Gary's rule (Nov 24, 2026): the alive timeout is twice the heartbeat interval, never a free-standing number.
// (gbl_t_NetworkAlive / f_gbl_t_NetworkAlive moved into gblNetwork - Nov 2026 rename pass.)

double gbl_t_flash = millis();   // Network-light blink timing, shared by the light routines in z_WHAT.ino.
//#define _NETWORK_PRESSED *WHAT_network_button = true;
//#define _NETWORK_RELEASED *WHAT_network_button = false;

//union
//{
//   // This union lets a value be converted for transmit and reception of floating point values (AS 4 bytes).
//   // This is a re-usable variable definition.
//   // you can put in either a floating point number (referred to as float2byte.myFloat = some floating point value)
//   // and then you can use the converted version as an array of bytes: (convertedbyte[?] = float2byte.mybyte[?])
//  float myFloat;
//  uint8_t myBytes[4];
//} float2byte;

// *****************************
#define MASK_RXIF 0b0001000
#define GLOBAL_PANID   0xFFFF    // Used with J1939 Callback registration to verify a matching PGN.
#define GLOBAL_DESTINATION 0xFFFF //
#define GLOBAL_MASK    0xFFFF

#define NUM_PAYLOAD_BYTES 32   // Number of bytes that will be sent in the message
// Frame types (ANDed with ZIG_ADR_FRAMECTRL)
#define MASK_FC_BEACON      0b0000000000000000           // ANDed with FRAMECONTROL to identify the recieved frame is just an acknowledgement
#define MASK_FC_DATA        0b0000000000000001            // ANDed with FRAMECONTROL to identify the recieved frame is just an acknowledgement
#define MASK_FC_ACKFRAME    0b0000000000000010            // ANDed with FRAMECONTROL to identify the recieved frame is just an acknowledgement
#define MASK_FC_MAC_CMD     0b0000000000000011            // ANDed with FRAMECONTROL to identify the recieved frame is just an acknowledgement
// All other frame type values are reserved.
#define MASK_FC_SECURITY    0b0000000000001000
#define MASK_FC_NO_SECURITY 0b0000000000000000   
#define MASK_FC_PENDING     0b0000000000010000   
#define MASK_FC_NOT_PENDING 0b0000000000000000   
#define MASK_FC_ACK_REQ     0b0000000000100000
#define MASK_FC_NO_ACK_REQ  0b0000000000000000
#define MASK_FC_INTRAPAN    0b0000000001000000
#define MASK_FC_DEST64BIT   0b0000110000000000
#define MASK_FC_DEST16BIT   0b0000100000000000
#define MASK_FC_NODEST      0b0000000000000000
#define MASK_FC_SRC64BIT    0b1100000000000000
#define MASK_FC_SRC16BIT    0b1000000000000000
#define MASK_FC_NOSRC       0b0000000000000000

#define MASK_FC_SOURCEMODE  0b0000011000000000

// Usage for this would be:  _WHAT_Tx_Packet.WHAT_MAC_MESSAGE;
#define WHAT_MAC_MESSAGE frame_control = (MASK_FC_MAC_CMD | MASK_FC_NO_SECURITY | MASK_FC_NOT_PENDING | MASK_FC_NO_ACK_REQ | MASK_FC_INTRAPAN | MASK_FC_DEST16BIT | MASK_FC_SRC16BIT)  // Default:DataFrame,NoSecurity,NoPendingData,NoAck,Intra-PAN,16BitDest, 16BitSource.
// Usage for this would be:  _WHAT_Tx_Packet.WHAT_DATA_MESSAGE;
#define WHAT_DATA_MESSAGE frame_control = (MASK_FC_DATA | MASK_FC_NO_SECURITY | MASK_FC_NOT_PENDING | MASK_FC_NO_ACK_REQ | MASK_FC_INTRAPAN | MASK_FC_DEST16BIT | MASK_FC_SRC16BIT)  // Default:DataFrame,NoSecurity,NoPendingData,NoAck,Intra-PAN,16BitDest, 16BitSource.
// Usage for this would be:  _WHAT_Tx_Packet.WHAT_BEACON_MESSAGE;
#define WHAT_BEACON_MESSAGE frame_control = (MASK_FC_BEACON | MASK_FC_NO_SECURITY | MASK_FC_NOT_PENDING | MASK_FC_NO_ACK_REQ | MASK_FC_INTRAPAN | MASK_FC_DEST16BIT | MASK_FC_SRC16BIT)  // Default:DataFrame,NoSecurity,NoPendingData,NoAck,Intra-PAN,16BitDest, 16BitSource.


struct WHAT_Packet
{
  uint8_t frame_length = 5; //FrameControl,Seq#,PANID,Dest,Source
  uint8_t header_length = 9;  // FrameControl,SequenceNum,PANID,Dest,Source,
  uint16_t frame_control = (MASK_FC_DATA | MASK_FC_NO_SECURITY | MASK_FC_NOT_PENDING | MASK_FC_NO_ACK_REQ | MASK_FC_INTRAPAN | MASK_FC_DEST16BIT | MASK_FC_SRC16BIT);  // Default:DataFrame,NoSecurity,NoPendingData,NoAck,Intra-PAN,16BitDest, 16BitSource.
              //   Bits:  15-14   13-12  11-10  9-7    6      5    4        3      2-0
              //        SourceMode RES DestMode RES IntraPAN Ack pending Security FrameType        Res   
//  uint16_t frame_control = 0b00100010001000001    ;  // Default:DataFrame,NoSecurity,NoPendingData,NoAck,Intra-PAN,16BitDest, 16BitSource.
//              //   Bits:  15-14   13-12  11-10  9-7    6      5    4        3      2-0
//              //        SourceMode RES DestMode RES IntraPAN Ack pending Security FrameType        Res   
//  uint16_t frame_control = 0x8861    ;  // Default:DataFrame,NoSecurity,NoPendingData,NoAck,Intra-PAN,16BitDest, 16BitSource.
//              //   Bits:  15-14   13-12  11-10  9-7    6      5    4        3      2-0
//              //        SourceMode RES DestMode RES IntraPAN Ack pending Security FrameType        Res   
  uint8_t sequence_number = 0;   
  uint16_t pan_id = 0xFFFF;
  uint16_t destination = 0xFFFF;
  uint16_t sourceaddress = 0xFFFF;
  uint8_t payload[NUM_PAYLOAD_BYTES]; 
  uint8_t lqi = 0;
  uint8_t rssi = 0;
};
//#define NumWHATTxMessages TX_BUFFER_DEPTH  // Make this muber bigger if a lot of different messages are being sent asynchronously.
// Group: WHAT_Queues (instance gblQueues) - packets waiting to send, and the packet in hand.
struct WHAT_Queues
{
  WHAT_Packet tx[TX_BUFFER_DEPTH];
  uint8_t txPendIndex = 0;
  uint8_t txSentIndex = 0;
  WHAT_Packet rxPacket;
};
WHAT_Queues gblQueues;
//WHAT_Packet WHAT_Tx_Packet;

//MID reception table
#define NUM_MAX_MESSAGE_REGISTRATIONS 32
struct WHATCallback
{
  uint16_t MessageID;
  void (* callback)(WHAT_Packet, uint8_t); // Function to be called if this message arrives. 
};
// Group: WHAT_Registry (instance gblRegistry) - who to call when a message arrives.
struct WHAT_Registry
{
  WHATCallback message[NUM_MAX_MESSAGE_REGISTRATIONS];  // incoming messages will be compared to this list of messages.
  uint8_t index = 0;        // tracks the number of entries used in the message array.  "RegisterWHATMessage" increments this value.
  uint8_t reusableSlot = 0xFF; // One registry slot whose MessageID/callback is swapped as needed (was gblRegistry.reusableSlot, in z_WHAT.ino).
};
WHAT_Registry gblRegistry;

// 888888888888888888 Prototypes 88888888888888888888888888
// Public:
//void Mrf24j_begin (uint16_t _myPANID = 0xFFFF, uint16_t _mySourceAddress = 0xFFFF);
//uint8_t RegisterWHATMessage (uint16_t _MessageID,void(*callback)(WHAT_Packet ,uint8_t));
//void Mrf24j_reset (void);
//uint16_t Mrf24j_get_pan(void);
//void Mrf24j_set_pan(uint16_t _panid);
//void Mrf24j_set_SourceAddress(uint16_t _mySourceAddress);
//void WHAT_TransmitPacket (WHAT_Packet _WHAT_Tx_Packet);
//uint16_t Mrf24j_get_SourceAddress(void);  
//void Mrf24j_set_promiscuous(boolean _enabled);
//void Mrf24j_set_channel(uint8_t _channel);
//void Mrf24j_set_palna(bool _enabled);
//uint8_t Mrf24j_Read(uint16_t _address);
//void Mrf24j_Write(uint16_t _address,uint8_t _data);

// Private
//void RX_WHAT (void);
//void WHAT_Buffer_RX(void);
//void  WHAT_EEPROM(void);
//bool preemie_check_restore(void);
//uint8_t RegisterWHATMessage (uint16_t _MessageID, void(*callback)(WHAT_Packet , uint8_t));
//void Rx_REQUEST_JOIN_NETWORK_03 (WHAT_Packet _WHAT_Rx_Packet, uint8_t _index);

// 888888888888888888 Prototypes 88888888888888888888888888

#define ZIG_WAKE_TIMEOUT 1000
//double WAKE_Timer = millis(); // Global virtual Key Timer
//bool WAKE_ON = false;         // Actual state of the virtual Key that can be monitored by all simulations.

#define WHAT_ADR_RxSTART 0x300
#define WHAT_ADR_PACKETLENGTH WHAT_ADR_RxSTART
#define WHAT_ADR_HEADERLENGTH WHAT_ADR_RxSTART + 1
#define WHAT_ADR_FRAMECTRL  WHAT_ADR_RxSTART + 2
#define WHAT_ADR_FRAMECTRH  WHAT_ADR_RxSTART + 3
#define WHAT_ADR_PANIDL    WHAT_ADR_RxSTART + 4
#define WHAT_ADR_PANIDH    WHAT_ADR_RxSTART + 5
#define WHAT_ADR_DESTL     WHAT_ADR_RxSTART + 6
#define WHAT_ADR_DESTH     WHAT_ADR_RxSTART + 7
#define WHAT_ADR_SOURCEL   WHAT_ADR_RxSTART + 8
#define WHAT_ADR_SOURCEH   WHAT_ADR_RxSTART + 9
//#define WHAT_CMD_DATAL WHAT_ADR_RxSTART + 10
//#define WHAT_CMD_DATAH WHAT_ADR_RxSTART + 11
#define WHAT_ADR_PAYLOADSTART WHAT_ADR_RxSTART + 10
#define WHAT_ADR_PAYLOADLENGTH WHAT_ADR_PAYLOADSTART

#define COMMAND_NUM_BYTES _index
#define COMMAND_MID (_index + 1)
#define COMMAND_DB1 (_index + 2)
#define COMMAND_DB2 (_index + 3)
#define COMMAND_DB3 (_index + 4)
#define COMMAND_DB4 (_index + 5)
#define COMMAND_DB5 (_index + 6)
#define COMMAND_DB6 (_index + 7)
#define COMMAND_DB7 (_index + 8)

#include "z_WHAT_mrf24j.h"
#endif
