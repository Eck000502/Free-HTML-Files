
#ifndef __WHAT_h
#define __WHAT_h
#define WHAT_SOFTWARE_VERSION 191231
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
//#define AUTO_ADDRESSING true    // If this is commented out, auto-addressing is disabled.
//#define RX_MONITOR true     // If this is uncommented, recieved messages will be displayed on the serial monitor.
//#define TX_MONITOR true     // If this is uncommented, recieved messages will be displayed on the serial monitor.
//#define RETRANSMIT true     // This will quickly allow/prevent retransmission of messages (based on criteria).
//#define CHILD_MONITOR true   // Gives serial monitor text during linking process. 
#define DX_ONLY true        // Gives more detailed information about raw recieved messages.

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
//   if(_LastNetworkStateColor != _WHAT_network_StateColor )
//   {
//     _LastNetworkStateColor = _WHAT_network_StateColor;
//     switch(_WHAT_network_StateColor)
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
//         Serial.print("Invalid Light State : 0x");Serial.println(_WHAT_network_StateColor,HEX);
//         _WHAT_network_StateColor = WHAT_LIGHT_OFF;
//         break;   
//     }
//     Show_btn_NETWORK(/*Color of the button*/ILI9341_DARKGREEN, /*Color of the text*/ILI9341_BLACK, /*line1 Text*/ " Online ",/*line2 Text*/ String(myPANID));
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
  WHAT_LIGHT_BLUE
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
  WHAT_LINKING          // This is only used if z_WHAT_FullLink.h is included.

};

bool _WHAT_NetworkButton = 0;      // Given to WHAT.ino to indicate the state of the physical or virtual netowrk button.
WHAT_State _WHAT_network_State = WHAT_STARTUP;   // Obtained from WHAT.ino:  Indicates the current network status for displaying to operator.
WHAT_LedColor _WHAT_network_StateColor = WHAT_LIGHT_OFF;    //Controlled in WHAT.ino to give a color indication to show device state.

//  **********************   Network Button and lights   **********************  //
// Uncomment ONE of these to determine what type of device this is.
#define WHAT_PERMANENT 0x41  // The intent is for this device to be a permanent member of this network.
//#define WHAT_TEMPORARY  // This device will only be part of the network for x days,and must re-new its membership.
//#define WHAT_PART_TIME  // This device will come and go, but stays a permanent member of the network 
//#define WHAT_LEASE      // This device will come and go, but must re-new its lease periodically.
#define TX_BUFFER_DEPTH 3 //This will have a dramatic effect on Dynamic memory, so try to keep it small.
#define RX_BUFFER_DEPTH 3 //This will have a dramatic effect on Dynamic memory, so try to keep it small.

uint16_t myParentAddress = 0xFFFF; // when I am adopted, I will record the parent's address

#ifdef AUTO_ADDRESSING

#define ChildMaxIndex 15
uint16_t _Offered_PANID = 0xFF;          // When joining a network, this is the PANID of the network being joined.
uint16_t _Offered_SourceAddress = 0xFF;  // when  joining a network, this is the lowest new address offered.
uint16_t _Offered_Parent = 0xFF; 
uint16_t _Offered_Latchkey = 0xFF;      // this is the latchkey offered by the parent with the lowest offered address.


uint16_t  _WHAT_Child_Used = 0xFF;

struct ChildType
{
  uint16_t ChildAddress = 0xFFFF;
  uint16_t LatchKeyValue = 0xFFFF;
  uint8_t  RelationshipType = 0;
  uint16_t  Security = 0xFFFF;
}myChildren[ChildMaxIndex];// each device can have up to 15 children.// each device can have up to 15 children.

#ifdef  AUTO_ADDRESSING
// myChildren[0] is used by THIS device to hold its own information
#define mySourceAddress myChildren[0].ChildAddress   // This is saved in and loaded from EEPROM
#define myLatchKeyNumber myChildren[0].LatchKeyValue  // A secret number shared between parent and child nodes.
#define mySecurity myChildren[0].Security
#endif  //AUTO_ADDRESSING

#ifndef AUTO_ADDRESSING
  uint16_t  myPANID = 0xFFFF;
  uint16_t  mySourceAddress = 0xFFFF;
#endif    // AUTO_ADDRESSING
bool f_hasChildren = false;   // quick check (during message forwarding) to see if this device has children.
                              // This device does not need to forward any messages if it does not have children.
                              // See WHAT.ino/RX_WHAT (after switch)
uint16_t myMask = 0x000F;     // this mask is re-calculated when a source address is obtained.  it can be used to remove children from an address comparison, making sure the message should be routed through this device.
#endif  // If Autoaddressing

uint16_t myPANID = 0xFFFF;     // This is saved in and loaded from EEPROM
#ifndef AUTO_ADDRESSING
uint16_t mySourceAddress = 0xFFFF;     // This is saved in and loaded from EEPROM
#endif

#ifndef ArrayElementSize
  #define ArrayElementSize(x) (sizeof(x)/sizeof(x[0]))
#endif
//bool * _network_button = &WHAT_NetworkButton;     // This will mirror the button styate in the main program.
//WHAT_State * _network_state = &_WHAT_network_State;    // This will mirror the network light words in the main program.
//WHAT_LedColor * _network_light_color = &_WHAT_network_StateColor;  // This will mirror the Light color to the main program.

//void * _Loop_Network_Button_Light();
#define GLB_T_NETWORKALIVE_RESET 30000;
double gbl_t_NetworkAlive = millis()+5000;  // Timeout - when messages on this PANID arrive, the reset this.
bool f_gbl_t_NetworkAlive = true;      // Flag that other network nodes have been seen using THIS PANID

double _flash_timer = millis();
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
WHAT_Packet TxQueue[TX_BUFFER_DEPTH];
uint8_t TxQueue_PendIndex = 0;
uint8_t TxQueue_SentIndex = 0;
//WHAT_Packet WHAT_Tx_Packet;

WHAT_Packet WHAT_Rx_Packet;

//MID reception table
#define NUM_MAX_MESSAGE_REGISTRATIONS 32
uint8_t WHATRxIndex = 0;     // tracks the number of entries used in the WHAT Rx Array.  "RegisterWHATMessage" increments this value to indicate the number of messages watched.
struct WHATCallback
{
  uint16_t MessageID;
  void (* callback)(WHAT_Packet, uint8_t); // Function to be called if this message arrives. 
}WHATRxMessage[NUM_MAX_MESSAGE_REGISTRATIONS];  // incomming CAN messages will be compared to this list of messages.

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
