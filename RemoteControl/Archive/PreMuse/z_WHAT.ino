#define WHAT_NETWORK_VERSION 220329
/*
   Allows WHAT communications.
   Easy message reception can be handled through a registration process.
    void RegisterWHATMessage(uint16_t _MessageID,void(*callback)(ZIG_Packet ,uint8_t))
      - Exact match (or global (0xFFFF)) is required for PANID and Destination Address.
      - Messgage length is captured, then _MessageID is compared to list
      - If _MessageID matches, the associated callback is executed.
      - an "Index" value is passed to the callback, allowing more than one MessageID per
        Zigbee packet to be used.
// ***************************
// TableOfContents:
// TOC Globals and definitions
// TOC Initialize the MRF24J
// TOC Restore PREEMIE state
// TOC EEPROM initialization
// TOC PREEMIE EEPROM Values
// TOC EEPROM Storage
// TOC EEPROM retrieval
// TOC SETUP
// TOC  Register WHAT messages for reception
// TOC LOOP
//TOC Evaluate WHAT network state
// TOC PREEMIE operations
// TOC    Request Joining Network
// TOC    Creating a new network
// TOC    Avoiding network conflicts
// TOC    Network identified, waiting to be allowed
// TOC Adult operations
// TOC    Child requesting addition to network
// TOC    New child allowed to be added to network
// TOC    This Device attempting to link to another device
// TOC    Another device is attempting to link to a device
// TOC    Normal Adult operations
// TOC Recieve MRF24J buffer
// TOC Recieve raw MRF24J messages
// TOC Indicate the network is alive

// TOC Individual Recieve messages
      //TOC  WHAT_Buffer_RX returns TRUE if there is a new message in the recieve buffer.
// TOC Identify the message type and act accordingly
  // TOC Rx Beacon message
  // TOC Rx Data message
         //TOC THIS is where the matched message activates the callback.
  // TOC Rx Acknowledge message
  // TOC Rx MAC message (network creation/modification)
// TOC Handle the recieve payload

// TOC Individual Transmit messages
// TOC Check if the network is alive and give its status to the network status and light.
// TOC Handle Light color based on network status

// TOC Put new message into buffer
// TOC Transmit Message from buffer
// TOC Re-Transmit message
// ***************************

  TODO:
    - Amber Alert - Child loses direct contact with parent.
    - Link/unlink one or more devices

  Incomplete verifications:
    - Re-transmit message scheme testing
    - Message sent flag and transmit buffer... Prevent message re-broadcast from interferring with transmissions from this device.
    - verify all addresses can be assigned. (oddity seen around re-assigning address 1 multiple times)
    - Verify child removal (on parent and child side)
    - Verify full depth of payload and abiilty to locate MIDs through full depth of payload.
    Rev 260906
      Changed most operating modes to a state machine (Switch/case)
      Changed light control to OFF/RED/GREEN/BLUE.
      Changed lights to have a function call for color and flashing with delaytimes

    Rev 200224
      revised view:  (copied to message reception area)
      1) If this device does not have any children, no need to forward ANY messages.
          - any recieved message would be from a different parentation line.
          - Later implimentation of "AmberAlert" when child loses parent will allow it to re-address.
          - targeted and global messages should come from parent, though direct messages will still be handled.
          
      2)  If this device DOES have children:
          - if message is destination specific, verify destination is in this devices lineage:
            for example, this address is 0123.  message arrives for DA 0223, do NOT forward. (sibling)
                         this address is 0123.  message arrives for DA 0243, do NOT forward. (cousin)
                         this address is 0123.  message arrives for DA 1243, do NOT forward. (nephew)
                         this address is 0123.  message arrives for DA 1224, do NOT forward. (great-nephew)
                         this address is 0123.  message arrives for DA 1123, ALWAYS forward. (to my child)
                         this address is 0123.  message arrives for DA 0023, CHECK source address.... 
                                                      only forward if source is THIS device's child.(to parent)
                         this address is 0123.  message arrives for DA 1323, CHECK source address.... 
                                                      only forward if source is THIS device's child.(to parent)
              To summarize, only forward destination specific messages if they are from THIS devices child.          
           - if destination is global, ensure message source address includes this child's lineage... 
            for example, this address is 0123.  message arrives from SA 0223, do NOT forward. (sibling)
                         this address is 0123.  message arrives from SA 0243, do NOT forward. (cousin)
                         this address is 0123.  message arrives from SA 1243, do NOT forward. (nephew)
                         this address is 0123.  message arrives from SA 1224, do NOT forward. (great-nephew)
                         this address is 0123.  message arrives from SA 0023, DO forward (from parent, to children)
                         this address is 0123.  message arrives from SA 1323, DO forward (from children, to parent)
              To summarize, only forward global destination messages if they are from THIS devices child, or this device's parent.
          - Combining these,  always forward messages with children's source address, destination address, OR global destination from parent.         
 
    Rev 200223
        EEPROM, auto-addressing (No child removal), Manual re-preemie 
      Sketch uses 10620 bytes (34%) of program storage space. Maximum is 30720 bytes.
      Global variables use 538 bytes (26%) of dynamic memory, leaving 1510 bytes for local variables. Maximum is 2048 bytes.

    Rev 200216
      Measured size.  
        No auto addressing/reporting: 
          Sketch uses 6218 bytes (20%) of program storage space. Maximum is 30720 bytes.
          Global variables use 413 bytes (20%) of dynamic memory, leaving 1635 bytes for local variables. Maximum is 2048 bytes.
        auto addressing only
          Sketch uses 12334 bytes (40%) of program storage space. Maximum is 30720 bytes.
          Global variables use 563 bytes (27%) of dynamic memory, leaving 1485 bytes for local variables. Maximum is 2048 bytes.
        Both Addressing and reporting
          Sketch uses 12556 bytes (60%) of program storage space. Maximum is 30720 bytes.
          Global variables use 565 bytes (27%) of dynamic memory, leaving 1483 bytes for local variables. Maximum is 2048 bytes.

    Rev 200213
      Implimenting EEPROM and preemie recovery.
       - once a device has been connected to a network, it can be restored to its preemie state
       - DURING POWER-UP  **HOLD** the network button.
       - When the Blue Light starts a fast blink, release the network button.
       - the blue light will start blinking slower.
       - Press the network button again momentarily, and preemie mode will be entered.
       - the device will start the slow-blink/join request.

    Rev 200210

    Rev 200209
      Previous revision was too complicated.
      This version takes 34% of program memory and 57% of dynamic memory (on Nano).
      THis version will use message reception flags to try to keep most actions in the 
      "switch"es.
      Also, this uses "MAC Commands" for all the network configuration messages.
        This just requires a different Frame Control setting, which I handle with 3 new macros:
            - WHAT_MAC_MESSAGE    for network creation and modification functions
            - WHAT_DATA_MESSAGE   for normal WHAT related data
            - WHAT_BEACON_MESSAGE for a yet-to-be-implimented Beacon.
      
    Rev 200203
      At power on, preemie sends message: REQUEST_JOIN_NETWORK_03
      Preemie has 2 possible paths:
        1) Preemie sees another REQUEST_JOIN_NETWORK_03 (another preemie), so it creates a network:
                - Send REQUEST_ACK_03 and wait for _5_ seconds.
                - if REPLY_ACKNOWLEDGE_03 recieved, change PANID and try again.
                - if timeout, use proposed PANID and source address 0000.
        2) Preemie sees a NETWORK_ALERT_PREEMIE_03 message:
                - increases BLUE blink rate and waits for NETWORK_ALLOW_PREEMIE_04 (created by a network member's network button activation.
        3) Preemie sees a NETWORK_ALLOW_PREEMIE_04 message (ONLY after seeing a NETWORK_ALERT_PREEMIE_03 message):
                - Along with NETWORK_ALLOW_PREEMIE_04 come PROPOSE_CHILD_NUMBER_07 messages.
                - Preemie keeps the lowest parent/child address offered and joins network after _5_ seconds.
                - sends ACCEPT_PARENT_06 message.
      Adult has a couple of network creation options.
        1) sees REQUEST_JOIN_NETWORK_03
              - sends NETWORK_ALERT_PREEMIE_03 and if it has a valid child slot available sends PROPOSE_CHILD_NUMBER_07
              - changes to WHAT_NEW_CHILD_REQ state.
        2) sees NETWORK_ALERT_PREEMIE_03
              - Monitors its network button.  If pressed, sends NETWORK_ALLOW_PREEMIE_04 and PROPOSE_CHILD_NUMBER_07
        3) sees NETWORK_ALLOW_PREEMIE_04 (with or without seeing REQUEST_JOIN_NETWORK_03)
              - sends PROPOSE_CHILD_NUMBER_07
     Adult in WHAT_NEW_CHILD_REQ state
        1) monitors its own network button,and (if pressed):
            - sends NETWORK_ALLOW_PREEMIE_04
            - Changes to WHAT_NEW_CHILD_OFFER state.
        2) monitors network messages for NETWORK_ALLOW_PREEMIE_04
            - changes to WHAT_NEW_CHILD_OFFER state.
        3) monitors network for CHOOSE_NETWORK_05
            - if chosen network number does not match this PANID, return to WHAT_PRE_NET_ALIVE state 
        
     Adult in WHAT_NEW_CHILD_OFFER
        1) Monitors network for ACCEPT_PARENT_06
           - If the accepted parent source matches this source:
              o Verify recieved child address is valid and available.
              o Add this child - Record latchkey value, and increment the child index.
              o return to WHAT_PRE_NET_ALIVE state
        2) monitors network for CHOOSE_NETWORK_05
            - if chosen network number does not match this PANID, return to WHAT_PRE_NET_ALIVE state 

    Other always monitored network messages:


    Rev 200202
      - preemie repeats REQUEST_JOIN_NETWORK_03 1/second until WHAT_PRE_NETWORK_ALLOW_PREEMIE_04 before recording proposed child addressses.
      - Once WHAT_PRE_NETWORK_ALLOW_PREEMIE_04 is seen, preemie changes to that PANID, and only accepts
      - When
            Rx_PROPOSE_CHILD_NUMBER_07 messages from the allowing PAN


    Rev 200201
 *    *** NOTE  ALL Multibyte MESSAGE IDs (MIDs) that are larger than 0xEF -MUST- be highbyte/Lowbyte order.
      This allows the recieving routine to evalueate the byte and if it sees it is F0 or larger, it will pull the next byte as its low byte.


*/
/*#include <WHAT.h>*/
#include <EEPROM.h>
#include "z_WHAT_mrf24j.h"
#include "z_WHAT.h"
// *********************************************************************
// *********************************************************************
// *********************************************************************

// TOC Globals and definitions

// These are variables that can be (carefully) re-used within this program
// This is done to reduce the amount of dynamic memory used.
double WHAT_Reusable_Timer1 = millis();
double WHAT_Reusable_Timer2 = millis();
uint8_t WHAT_Reusable_8t_1  = 0xFF;
uint16_t WHAT_Reusable_16t_1  = 0xFFFF;
uint16_t WHAT_Reusable_16t_2  = 0xFFFF;
uint16_t WHAT_Reusable_16t_3  = 0xFFFF;
uint16_t WHAT_Reusable_16t_4  = 0xFFFF;

uint8_t WHAT_Reusable_Message1 = 0xFF; //This is registered once, then the callback can be changed
//When connecting to the network, several messages are only valid
//uint8_t WHAT_Reusable_Message2 = 0xFF; //at certian times, so this can be changed to avoid each of them taking
//an entry in the recieved message table.  All of the 1-time, 1-at-a-time use messages
//can be assigned to this same message table entry.

// Rx Message flags
bool f_ResetToPremiee = false;                // Holding the network button at power on activates this.
bool f_Rx_REQUEST_JOIN_NETWORK_03 = false;    // Preemie asking to join network
bool f_Rx_NETWORK_ALERT_PREEMIE_03 = false;   // Notice from another device that a preemie is trying to join
bool f_Rx_CHOOSE_NETWORK_05  = false;         // Notice to other networks that a network has accepted this preemie
bool f_Rx_NETWORK_ALLOW_PREEMIE_04 = false;   // Notice to this preemie that it may accept its parent.
bool f_Rx_PROPOSE_CHILD_NUMBER_05 = false;    // Preemie asking the network for a child number
bool f_Rx_ACCEPT_PARENT_06 = false;
bool f_Rx_NETWORK_JOIN_COMPLETE_03 = false;   // Preeemie accepted a proposed parent 
bool f_Rx_NETWORK_JOIN_ABORT_03 = false;      // User abort of the acceptance process.
bool f_Rx_REQUEST_ACK_03 = false;             // Send out a request for any existing network with my proposed PANID to respond.
bool f_Rx_REPLY_ACKNOWLEDGE_03 = false;       // While a new network is forming, requests existing network to claim its PANID

// TOC SETUP
void WHAT_begin(void)
//void WHAT_begin(bool & _WHAT_NetworkButton, WHAT_State & _WHAT_network_State, WHAT_LedColor & _WHAT_network_StateColor)
{
  // ****************   Setup for WHAT   ***************************/
  // Register common ZigBee recieved messages
  //void RegisterZIGMessage (uint16_t _MessageID,void(*callback)()); // Messages must match this PANID and destination address or be global.

  //WHAT_Tx_Packet.header_length = 9;
  //WHAT_Tx_Packet.frame_length = WHAT_Tx_Packet.header_length + 1; //FrameControl,Seq#,PANID,Dest,Source
  //WHAT_Tx_Packet.frame_control = 0b100010001000001    ;  // Default:DataFrame,NoSecurity,NoPendingData,NoAck,Intra-PAN,16BitDest, 16BitSource.
  //            //   Bits:  15-14   13-12  11-10  9-7    6      5    4        3      2-0
  //            //        SourceMode RES DestMode RES IntraPAN Ack pending Security FrameType        Res
  //WHAT_Tx_Packet.sequence_number = 0;
  //WHAT_Tx_Packet.pan_id = myPANID;
  //WHAT_Tx_Packet.destination = 0xFFFF;
  //WHAT_Tx_Packet.sourceaddress = mySourceAddress;

  // setup the network button and lights.
  //pinMode(NETWORK_LIGHT,OUTPUT);
  //digitalWrite(NETWORK_LIGHT,LOW); // Lights are active HIGH
  //pinMode(NETWORK_BUTTON,INPUT_PULLUP);
  //Setup_Button_Light(); // This is IF the button/light is installed.
  //_WHAT_network_State = _WHAT_network_State;   // Main application is responsible for monitoring the network button and light (in case of LCD displays, etc...)
  //_WHAT_NetworkButton = _WHAT_NetworkButton; // Main application is responsible for monitoring the network button and light (in case of LCD displays, etc...)
  //_WHAT_network_StateColor = _WHAT_network_StateColor;
  //_Loop_WHAT_NetworkButton_Light = Loop_WHAT_NetworkButton_Light; // Function pointer for lamp updates.
  //pinMode(b_RED_RGB_PIN,OUTPUT);        // Main application is responsible for additional lights, if desired.
  //pinMode(b_GREEN_RGB_PIN,OUTPUT);
  //pinMode(b_BLUE_RGB_PIN,OUTPUT);
  //digitalWrite(b_RED_RGB_PIN,HIGH);  // Lights are active LOW
  //digitalWrite(b_GREEN_RGB_PIN,HIGH);
  //digitalWrite(b_BLUE_RGB_PIN,HIGH);

#ifdef AUTO_ADDRESSING  
  WHAT_EEPROM();  // restore any WHAT parameters from EEPROM (Source address/PANID, etc...)

  _WHAT_network_State = WHAT_STARTUP;
  Serial.print(F("Network State set to 'WHAT_STARTUP' "));Serial.println(_WHAT_network_State,DEC);

  Serial.println(F("PANID\tmySourceAddress\tmyParentAddress\tmyLatchkey\t_WHAT_network_State"));
  Serial.print(myPANID,HEX);Serial.print(" \t");Serial.print(mySourceAddress,HEX);Serial.print(" \t\t");Serial.print(myParentAddress,HEX);Serial.print(" \t");Serial.print(myLatchKeyNumber,HEX);Serial.print(" \t\t\t");Serial.println(_WHAT_network_State,HEX);
  #ifdef __WHAT_FULLLINK_h
    Serial.print(F("Links: 0x"));
    for(uint8_t _i=0; _i<numWHATLinks; _i++)
    {
      Serial.print(WHATLinks[_i],HEX);Serial.print(" \t0x");
    }
  #endif  //__WHAT_FULLLINK_h
#endif  //AUTO_ADDRESSING
  Serial.println(" ");
  mrf.init();
  Mrf24j_begin (myPANID, mySourceAddress);
  // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// TOC  Register WHAT messages for reception
#ifdef  AUTO_ADDRESSING
  Serial.println(F("WHAT Auto-addressing enabled"));
  RegisterWHATMessage (REQUEST_JOIN_NETWORK_03, &Rx_REQUEST_JOIN_NETWORK_03); // This is an entry that will not change.
  RegisterWHATMessage (NETWORK_ALERT_PREEMIE_03, &Rx_NETWORK_ALERT_PREEMIE_03); // This is an entry that will not change.
  RegisterWHATMessage (PROPOSE_CHILD_NUMBER_05, &Rx_PROPOSE_CHILD_NUMBER_05); // This is an entry that will not change.
  RegisterWHATMessage (ABANDON_CHILD_05, &Rx_ABANDON_CHILD_05); // This is an entry that will not change.
 
   // Trying to make these as re-usable messages.
  //RegisterWHATMessage (NETWORK_ALERT_PREEMIE_03, &Rx_NETWORK_ALERT_PREEMIE_03); // This is an entry that will not change.
  //RegisterWHATMessage (NETWORK_ALLOW_PREEMIE_04, &Rx_NETWORK_ALLOW_PREEMIE_04); // This is an entry that will not change.
  WHAT_Reusable_Message1 = RegisterWHATMessage (0xFFFF, &holdingFunction); // This entry can be re-used during network adoption.
      #ifdef RX_MONITOR
      Serial.print(F("WHAT_Reusable_Message1 :"));Serial.println(WHAT_Reusable_Message1);
      #endif
#endif  //AUTO_ADDRESSING
  _WHAT_NetworkButton = LOW;
   _WHAT_network_State = WHAT_STARTUP;
   _WHAT_network_StateColor = WHAT_LIGHT_OFF;

#ifdef AUTO_ADDRESSING
  // Check to see if this device has any children.
  f_hasChildren = false;
  for(uint8_t _i=0;_i<ChildMaxIndex;_i++)
  {
    if(myChildren[_i].ChildAddress<0xFFFF)
    {
      f_hasChildren = true;
      #ifdef RX_MONITOR
      Serial.print(F("Has Children:"));Serial.println(_i);
      #endif
   } 
  }
#endif
}
// *********************************************************************
// *********************************************************************
// *********************************************************************
// TOC LOOP
void Loop_WHAT(void)
{
  Mrf24j_Loop();  //Physical Layer 

#ifdef AUTO_ADDRESSING
   WHAT_StateCheck(); // This is only invoked when the device has created or connected to a network.
#endif 
   WHATHouseKeeping();

  // put your main code here, to run repeatedly:
  Mrf24j_Loop();  //Physical Layer 
  RX_WHAT();
  Mrf24j_Loop();  //Physical Layer 


}

// *************************************************************************
// TOC Recieve raw MRF24J messages
void RX_WHAT (void)
{
  #define NumBufferMessages 8 
  static uint8_t _RxbufferIndex = 0;
  if (WHAT_Buffer_RX(WHAT_Rx_Packet,_RxbufferIndex) )
  { //TOC  WHAT_Buffer_RX returns TRUE if there is a new message in the recieve buffer.
    Serial.print("\nRx WHAT");

       #ifdef DX_ONLY
        Serial.print("\nGBL_rx_index: ");Serial.print(GBL_rx_index);
        Serial.print("\t_RxbufferIndex: ");Serial.print(_RxbufferIndex);
        Serial.println(F("\nLength\tCONTROL\tRx pan_id\tdestination\tsource"));
        Serial.print(WHAT_Rx_Packet.frame_length,DEC);Serial.print("\t  ");
        Serial.print(WHAT_Rx_Packet.frame_control,HEX);Serial.print("\t  ");
        Serial.print(WHAT_Rx_Packet.pan_id,HEX);Serial.print("\t\t\t  ");
        Serial.print(WHAT_Rx_Packet.destination,HEX);Serial.print("\t  ");
        Serial.println(WHAT_Rx_Packet.sourceaddress,HEX);
    // read the payload
        Serial.print(F("Adr:"));Serial.print(WHAT_ADR_PAYLOADSTART,HEX);Serial.print(F(":"));
        uint8_t _payloadSize = WHAT_Rx_Packet.payload[0];
        Serial.print(F("\t_payloadSize:"));Serial.println(_payloadSize);
        for (uint8_t _i = 0; _i <= _payloadSize; _i++) 
        {
            //WHAT_Rx_Packet.payload[_i] = Mrf24j_Read(WHAT_ADR_PAYLOADSTART + _i);
              Serial.print(F("0x"));Serial.print(WHAT_Rx_Packet.payload[_i],HEX);Serial.print(F(", "));
        }
        Serial.print("FCS: ");Serial.print(WHAT_Rx_Packet.payload[_payloadSize+2],HEX);Serial.print(WHAT_Rx_Packet.payload[_payloadSize+1],HEX);;
        Serial.print("\tlqi: ");Serial.print(WHAT_Rx_Packet.lqi,HEX);
        Serial.print("\trssi: ");Serial.println(WHAT_Rx_Packet.rssi,HEX);
        Serial.print("\n");

      #endif
// TOC Indicate the network is alive 
   if((WHAT_Rx_Packet.pan_id == myPANID) | (WHAT_Rx_Packet.pan_id == GLOBAL_PANID))
    {
        gbl_t_NetworkAlive = millis() + GLB_T_NETWORKALIVE_RESET; // This allows an indication of any other devices on the network.
        f_gbl_t_NetworkAlive = true;      // Flag that no other network nodes have been seen
    }

// TOC Identify the message type and act accordingly
    switch (WHAT_Rx_Packet.frame_control & 0x07)
    {
// TOC Rx Beacon message
      case MASK_FC_BEACON: //this is a beacon frame.  These are not currently supported.
        Serial.println(F("Beacon"));
      break;

// TOC Rx Data message
      case MASK_FC_DATA: //this is a data frame.  Extract and use the data.
        Serial.println(F("Data"));
        //Serial.print("MRF_INTSTAT : ");Serial.println(_mrf_intstat);
        //WHAT_Rx_Packet.packet_length = Mrf24j_Read(WHAT_ADR_PACKETLENGTH);
        //Serial.print("Address "); Serial.print(WHAT_ADR_RxSTART,HEX); Serial.print(" (length) : ");Serial.println(_length);
        Serial.print("pan_id : 0x"); Serial.println(WHAT_Rx_Packet.pan_id,HEX);
        Serial.print("myPANID : 0x"); Serial.println(myPANID,HEX);
        
        Serial.print("destination : 0x"); Serial.println(WHAT_Rx_Packet.destination,HEX);
        Serial.print("sourceaddress :"); Serial.println(WHAT_Rx_Packet.sourceaddress,HEX);
        //_length= WHAT_Rx_Packet.frame_length - _ZIGBEE_HEADER_LENGTH;
          //Serial.println ("A message Arrived. ");
          if ((WHAT_Rx_Packet.pan_id == myPANID) | (WHAT_Rx_Packet.pan_id == GLOBAL_PANID))
          { // The recieved packet is using the PAN ID of this network or has a global PAN or is configuring a network
            #ifdef DX_ONLY
                Serial.println(F("Data PAN ID Match"));
            #endif
            //Serial.print("Rx Destination: 0x");Serial.println(WHAT_Rx_Packet.destination,HEX);
            //Serial.print("MySource: 0x");Serial.println(mySourceAddress,HEX);
            if ((WHAT_Rx_Packet.destination == mySourceAddress) | (WHAT_Rx_Packet.pan_id == GLOBAL_DESTINATION))
            { // The recieved packet is targeting this device, or is general communications
              #ifdef DX_ONLY
                      Serial.println("Destination Match");
              #endif
              WHAT_PayloadHandler();
            } // End If this message is targeting this device

          } // End if this message is on this PAN
        break;
// TOC Rx Acknowledge message
      case MASK_FC_ACKFRAME:  // Default to ACKs, though they are not currently being handled.
        Serial.println(F("Ack"));
        break;
// TOC Rx MAC message (network creation/modification)
      case MASK_FC_MAC_CMD: // MAC command... Used in network creation.
        Serial.println(F("MAC Command"));
        #ifdef RX_MONITOR
          //Serial.println(F("PANID DESTINATION SOURCE f_hasChildren"));
          //Serial.print(WHAT_Rx_Packet.pan_id,HEX);Serial.print(" ");Serial.print(WHAT_Rx_Packet.destination,HEX);Serial.print(" ");Serial.print(f,HEX);Serial.print(" ");Serial.println(f_hasChildren);
        #endif
        if(mySourceAddress == 0xFFFF)
        {
              WHAT_PayloadHandler();
        }
        else
        {
          //Once this device is part of a network, establish more message acceptance critera
          if ((WHAT_Rx_Packet.pan_id == myPANID) | (WHAT_Rx_Packet.pan_id == GLOBAL_PANID))
          { // The recieved packet is using the PAN ID of this network or has a global PAN or is configuring a network
            Serial.print(F("match: WHAT_Rx_Packet.pan_id:0X"));Serial.println(WHAT_Rx_Packet.pan_id,HEX);
            //Serial.print("Rx Destination: 0x");Serial.println(WHAT_Rx_Packet.destination,HEX);
            //Serial.print("MySource: 0x");Serial.println(mySourceAddress,HEX);
            if ((WHAT_Rx_Packet.destination == mySourceAddress) | (WHAT_Rx_Packet.destination == GLOBAL_DESTINATION))
            { // The recieved packet is targeting this device, or is general communications
              Serial.println("Destination Match");
              WHAT_PayloadHandler();
            } // End If this message is targeting this device
          }
          else
          {
            Serial.print("No Destination Match: ");Serial.println(WHAT_Rx_Packet.pan_id);
          }
        }
        break;

      default:
        Serial.print(F("Reserved recieved message frame: 0x"));Serial.println(WHAT_Rx_Packet.frame_control & 0x07);
        break;
    }// End Switch
// TOC Re-Transmit message
//    #ifdef RETRANSMIT
    // Forward message (if necessary)
/*    Rev 200224
      revised view:  (copied to message reception area)
      1) If this device does not have any children, no need to forward ANY messages.
          - any recieved message would be from a different parentation line or this devices parent.
          - Later implimentation of "AmberAlert" when child loses parent will allow it to re-address, so all childrent should communicate direclty with parents.
          - targeted and global messages should come from parent, though direct messages will still be conveyed through lineage.
          
      2)  If this device DOES have children:
          - if message is destination specific, verify destination is in this devices lineage:
            for example, this address is 0123.  message arrives for DA 0223, do NOT forward. (sibling)
                         this address is 0123.  message arrives for DA 0243, do NOT forward. (cousin)
                         this address is 0123.  message arrives for DA 1243, do NOT forward. (nephew)
                         this address is 0123.  message arrives for DA 1224, do NOT forward. (great-nephew)
                         this address is 0123.  message arrives for DA 1123, ALWAYS forward. (to my child)
                         this address is 0123.  message arrives for DA 0023, CHECK source address.... 
                                                      only forward if source is THIS device's child.(to parent)
                         this address is 0123.  message arrives for DA 1323, CHECK source address.... 
                                                      only forward if SOURCE is THIS device's child.(to parent)
              To summarize, only forward destination specific messages if they are FROM THIS devices child.          
           - if destination is global, ensure message SOURCE address includes this child's lineage... 
            for example, this address is 0123.  message arrives from SA 0223, do NOT forward. (sibling)
                         this address is 0123.  message arrives from SA 0243, do NOT forward. (cousin)
                         this address is 0123.  message arrives from SA 1243, do NOT forward. (nephew)
                         this address is 0123.  message arrives from SA 1224, do NOT forward. (great-nephew)
                         this address is 0123.  message arrives from SA 0023, DO forward (from parent, to children)
                         this address is 0123.  message arrives from SA 1323, DO forward (from children, to parent)
              To summarize, only forward global destination messages if they are from THIS devices child, or this device's parent.
          - Combining these, always forward messages with children's source address, destination address, OR 
                global destination from THIS DEVICE's parent.         
 */
#ifdef AUTO_ADDRESSING
     if ((WHAT_Rx_Packet.pan_id == myPANID) & (WHAT_Rx_Packet.destination != mySourceAddress) & f_hasChildren) 
     {  // Only consider re-transmit if the PANID matches, THIS device was not targeted, and this device has children.

        bool _retransmit_message = false;
        if(WHAT_Rx_Packet.destination == GLOBAL_DESTINATION) 
        {
           if(WHAT_Rx_Packet.sourceaddress == myParentAddress) _retransmit_message = true;
        }

        if(!_retransmit_message)// If already transmitting, no need to evaluate the source and destination...
        {   // source or destination of grand children and great grand children also need be considered....
            // mask to isolate this child's children and grandchildren in the destination  and source address.
            // This is multiplying a comparison with a number.  A false comparison resolves to 0 and when multiplied, removes the term from the equation.
            uint16_t myMask = ( ((mySourceAddress > 0x00) * 0x0F) | ((mySourceAddress > 0x0F) * 0x0FF) | ((mySourceAddress > 0xFF) * 0xFFF) );
            Serial.print("Mask:0x");Serial.println(myMask);
             //Apply the mask to the incoming source address to verify that the lineage matches this device before sending this message.
            uint16_t _temporary = WHAT_Rx_Packet.sourceaddress & myMask;
            if(_temporary == mySourceAddress) _retransmit_message = true; //This message came FROM one of my (grand)children. 
            _temporary = WHAT_Rx_Packet.destination & myMask;
            if(_temporary == mySourceAddress) _retransmit_message = true; //This message is going TO one of my (grand)children. 
        }
        if(!_retransmit_message)// If already transmitting, no need to evaluate the children...
        {// Not re-transmitting.  see if any of the children are targeted.
          for (uint8_t _i=1;_i<ChildMaxIndex;_i++)
          { //see if the arriving souce address or destination address matches any of the children of this device. 
            if(WHAT_Rx_Packet.destination == myChildren[_i].ChildAddress) _retransmit_message = true;
            if(WHAT_Rx_Packet.sourceaddress == myChildren[_i].ChildAddress) _retransmit_message = true;
          }
        }
        if(_retransmit_message)
        { // re transmit this message.
            // My initial thoughts were to use the MSB as an indicator for if the message was 
            // traveling inbound or outbound, but I think the use of the source and destination
            // eliminate the need for that.  That might allow twice as many source addresses to be used.
            WHAT_Packet _WHAT_Tx_Packet;  // Make a temporary transmit packet
            _WHAT_Tx_Packet = WHAT_Rx_Packet; // Make it the same as the incoming message.
            #ifdef RX_MONITOR
            Serial.println("Retransmitting");
            #endif
        }
    }

    #ifdef RX_MONITOR
    else
    {
      //Serial.println(F("Not Retransmitted.  PANID DESTINATION SOURCE f_hasChildren"));
      //Serial.print(WHAT_Rx_Packet.pan_id,HEX);Serial.print(" ");Serial.print(WHAT_Rx_Packet.destination,HEX);Serial.print(" ");Serial.print(mySourceAddress,HEX);Serial.print(" ");Serial.println(f_hasChildren);
    }
    #endif  // RX_MONITOR
#endif  //AUTO_ADDRESSING 
 //  #endif  // RETRANSMIT

    _RxbufferIndex++;
    // FIX: Enforce proper zero-indexed circular buffer bounds
    if(_RxbufferIndex >= RX_BUFFER_DEPTH ){ 
        _RxbufferIndex = 0; 
    }
  }// End if a message has arrived
}// End Function

// TOC Handle the recieve payload
void WHAT_PayloadHandler(void)
{// The PANID, SOURCE and DESTINATION have been verified that this routine
 //   is permitted to sift through the payload and act upon the MIDs
 //   this works on the global variable WHAT_Rx_Packet that is buffered seperately.
 //   MIDs are compared to the registered list and when matches are found, callbacks are executed.
 
     uint8_t _length = WHAT_Rx_Packet.payload[0];  // Total payload length.
    //Serial.print("Rx payload size:");Serial.println(_length);
    uint8_t _index = 1;
    while (_index < (_length - 1))
    { // Read the first command from the packet.
      uint8_t _NumBytes = WHAT_Rx_Packet.payload[_index];  // each command starts with its total data length.
      //Serial.print("_NumBytes:");Serial.println(_NumBytes);
      //uint16_t _thisMessageID = (WHAT_Rx_Packet.payload[_index + 1] *0x100) + WHAT_Rx_Packet.payload[_index + 2]; // Message IDs uniquely identify message contents
      uint16_t _thisMessageID = WHAT_Rx_Packet.payload[_index + 1]; // Message IDs uniquely identify message contents
      if (_thisMessageID > 0xEF) {
        _thisMessageID = _thisMessageID * 0x100 + WHAT_Rx_Packet.payload[_index + 2];
      }
      for (uint8_t _i = 0; _i <= WHATRxIndex; _i++)
      { // Search the registered messages and see if they want this messageID.
        if ( WHATRxMessage[_i]. MessageID ==  _thisMessageID)
        {
         #ifdef DX_ONLY
          Serial.print(F("MID Match. Index:"));Serial.print(_i); Serial.print("MID:");Serial.println(_thisMessageID, HEX);
          Serial.print(F("_WHAT_network_State:")); Serial.println(_WHAT_network_State, DEC);
          #endif  
         (* WHATRxMessage[_i].callback)(WHAT_Rx_Packet, _index);
         //TOC THIS is where the recieved message activates the callback function.

        }
        #ifdef DX_ONLY
        else
        {
          Serial.print(_thisMessageID,HEX);Serial.print("<>");Serial.println(WHATRxMessage[_i]. MessageID,HEX);
        }
        #endif
      }// End search registered messages for a match.
      _index += (_NumBytes); // Move the index to the next message (if there is one)
      //Serial.print("_index:");Serial.println(_index);
    } // End While Index<length
}


// *************************************************************************
// *************************************************************************
void  WHATHouseKeeping(void)
{
  if(mySourceAddress < 0xFFFF)
  {
   // This device has been assigned a souce address, so is part of a network.
   // Items here can be done as routine maintainence
  // TOC Check if the network is alive and give its status to the network status and light.
     if( gbl_t_NetworkAlive <= millis())   // This is reset in     // TOC Recieve MRF24J buffer
     {
       gbl_t_NetworkAlive = millis() + GLB_T_NETWORKALIVE_RESET; // go aheand and reset the counter here so this check is done less often.
       f_gbl_t_NetworkAlive = false;      // Flag that no other network nodes have been seen
      }
  }
}

// *************************************************************************
// *************************************************************************
// TOC  Register WHAT messages for reception
uint8_t RegisterWHATMessage (uint16_t _MessageID, void(*callback)(WHAT_Packet , uint8_t))
{
  if(WHATRxIndex>=NUM_MAX_MESSAGE_REGISTRATIONS)
  {Serial.println(F("Message Reception buffer Overflow.  Some messages may not be recieved."));}
  uint8_t ret = WHATRxIndex; // this is the table entry index for THIS message.  This allows entries to be changed.
  WHATRxMessage[WHATRxIndex].MessageID = _MessageID;
  WHATRxMessage[WHATRxIndex].callback = callback;    // Function to be called if this message arrives.
  WHATRxIndex++;
  return ret;
}

// *************************************************************************
// TOC Put new message into buffer
void WHAT_TransmitPacket (WHAT_Packet _WHAT_Tx_Packet)
{
  // the transmit packet is placed in the buffer to be transmitted.
      TxQueue[TxQueue_PendIndex] = _WHAT_Tx_Packet;
      TxQueue_PendIndex++; 
      if(TxQueue_PendIndex >= TX_BUFFER_DEPTH) 
      {
        TxQueue_PendIndex=0;
      }
      //Serial.println(TxQueue_PendIndex);
      //Tx_What_Que(); // go ahead and try to send it now.

  //Mrf24j_Write(MRF24J_TXNCON, MRF24J_TXNTRIG);  // Transmit the packet
  //Serial.println(Mrf24j_Read(MRF24J_TXSTAT),BIN);
}
// *************************************************************************
 #ifdef AUTO_ADDRESSING
 // TOC EEPROM initialization
#define EEPROM_USED_FLAG_LOCATION EEPROM_WHAT_LOCATION
// I moved this to the main page so it is easier to edit.  //#define USED_EEPROM_VALUE 0x7A    // This value is stored in EEPROM_USED_FLAG_LOCATION to show this is not a fresh installation. 
#define EEPROM_myPANIDL_LOCATION  EEPROM_USED_FLAG_LOCATION + 1
#define EEPROM_myPANIDH_LOCATION  EEPROM_myPANIDL_LOCATION + 1
#define EEPROM_mySourceAddressL_LOCATION  EEPROM_myPANIDH_LOCATION + 1
#define EEPROM_mySourceAddressH_LOCATION  EEPROM_mySourceAddressL_LOCATION + 1
#define EEPROM_myParentAddressL_LOCATION  EEPROM_mySourceAddressH_LOCATION + 1
#define EEPROM_myParentAddressH_LOCATION  EEPROM_myParentAddressL_LOCATION + 1
#define EEPROM_myLatchKeyNumberL_LOCATION  EEPROM_myParentAddressH_LOCATION + 1
#define EEPROM_myLatchKeyNumberH_LOCATION  EEPROM_myLatchKeyNumberL_LOCATION + 1
#define EEPROM_preemie_State_LOCATION EEPROM_myLatchKeyNumberH_LOCATION + 1
#define _EEPROM_Child_LOCATION (EEPROM_preemie_State_LOCATION + 1)
#define EEPROM_LINK_LOCATION _EEPROM_Child_LOCATION + 1

void  WHAT_EEPROM(void)
{
  uint8_t _temp = EEPROM.read(EEPROM_USED_FLAG_LOCATION);
    //for(uint8_t _i=EEPROM_USED_FLAG_LOCATION;_i<255;_i++)
    //{ // recover index 0 to the end, even though index 0 actually is used for this device.
    //  Serial.print(EEPROM.read(_i++),HEX);Serial.print("\t");
    //}
    //Serial.print("\n");

  if (_temp == USED_EEPROM_VALUE)
  {
// TOC EEPROM retrieval
    Serial.println(F("Restoring from EEPROM"));
    // Values have already been saved and updated in EEPROM.
    // Retrieve the EEPROM values to be used instead of default values
    myPANID = EEPROM.read(EEPROM_myPANIDL_LOCATION) + (EEPROM.read(EEPROM_myPANIDH_LOCATION) * 0x100);
    //Serial.print(F("EE read myPANID:0x"));Serial.println(myPANID,HEX);
    mySourceAddress = EEPROM.read(EEPROM_mySourceAddressL_LOCATION) + (EEPROM.read(EEPROM_mySourceAddressH_LOCATION) * 0x100);
    //Serial.print(F("EE read mySourceAddress:0x"));Serial.println(mySourceAddress,HEX);

    myParentAddress = EEPROM.read(EEPROM_myParentAddressL_LOCATION) + (EEPROM.read(EEPROM_myParentAddressH_LOCATION) * 0x100);
    //Serial.print(F("EE read myParentAddress:0x"));Serial.println(myParentAddress,HEX);
    myLatchKeyNumber = EEPROM.read(EEPROM_myLatchKeyNumberL_LOCATION) + (EEPROM.read(EEPROM_myLatchKeyNumberH_LOCATION) * 0x100);
    //Serial.print(F("EE read myLatchKeyNumber:0x"));Serial.println(myLatchKeyNumber,HEX);
      /* EEPROM.get( eeAddress, f ) for TEENSY!!!! */ 
      /* but this is listed in another example   value = EEPROM.read(address);  */

    _WHAT_network_State = (WHAT_State)EEPROM.read(EEPROM_preemie_State_LOCATION);
    //uint8_t temp = EEPROM.read(EEPROM_preemie_State_LOCATION);
    //_WHAT_network_State = (en_preemie_states)(temp);

#ifdef __WHAT_FULLLINK_h
    // Retrieve link table.
    uint8_t _EEPROM_Link_Location = EEPROM_LINK_LOCATION;
    for(uint8_t _i=0;_i<numWHATLinks;_i++)
    {
      WHATLinks[_i] = (EEPROM.read(_EEPROM_Link_Location++) + EEPROM.read(_EEPROM_Link_Location++) * 0x100);
      //#ifdef CHILD_MONITOR
      //Serial.print(F("Link EEPROM READ. Adr: 0x"));Serial.print(_EEPROM_Link_Location,HEX);Serial.print(F("\tLink EEPROM. Value: 0x"));Serial.println(WHATLinks[_i],HEX);
     // #endif
    }
#endif
   
    //Serial.print(F("EE read _WHAT_network_State:0x"));Serial.println(_WHAT_network_State);
    // reload all child addresses:
    uint8_t _EEPROM_Child_Location = _EEPROM_Child_LOCATION;
    for(uint8_t _i=1;_i<ChildMaxIndex;_i++)
    { // recover index 0 to the end, even though index 0 actually is used for this device.
      myChildren[_i].ChildAddress = EEPROM.read(_EEPROM_Child_Location++) + (EEPROM.read(_EEPROM_Child_Location++) * 0x100);
      myChildren[_i].LatchKeyValue = EEPROM.read(_EEPROM_Child_Location++) + (EEPROM.read(_EEPROM_Child_Location++) * 0x100);
      myChildren[_i].RelationshipType = EEPROM.read(_EEPROM_Child_Location++);
      myChildren[_i].Security = EEPROM.read(_EEPROM_Child_Location++) + (EEPROM.read(_EEPROM_Child_Location++) * 0x100);
      //Serial.println(_EEPROM_Child_Location);
    }
  }
  else
  {
    WHAT_Default_EEPROM_Values(); // Reset PANID, and all EEPROM stored addresses to 0xFFFF
    WHAT_StoreEEPROM();
  }
}


// TOC EEPROM Storage
void WHAT_StoreEEPROM(void)
{
  Serial.print(F("Saving to EEPROM. Starting Address: 0x"));Serial.println(EEPROM_myPANIDL_LOCATION,HEX);
  //Serial.print("myPANID:");Serial.print(myPANID,HEX);
  EEPROM.write(EEPROM_myPANIDL_LOCATION, lowByte(myPANID)); EEPROM.write(EEPROM_myPANIDH_LOCATION, highByte(myPANID));
  //Serial.print("\t EEmyPANID:");Serial.print((EEPROM.read(EEPROM_myPANIDH_LOCATION)*0x100 + EEPROM.read(EEPROM_myPANIDL_LOCATION)),HEX);
  EEPROM.write(EEPROM_mySourceAddressL_LOCATION, lowByte(mySourceAddress)); EEPROM.write(EEPROM_mySourceAddressH_LOCATION, highByte(mySourceAddress));
  //Serial.print("mySourceAddress:");Serial.println(EEPROM.read(EEPROM_mySourceAddressL_LOCATION) + (EEPROM.read(EEPROM_mySourceAddressL_LOCATION) * 0x100),HEX);
  EEPROM.write(EEPROM_myParentAddressL_LOCATION, lowByte(myParentAddress)); EEPROM.write(EEPROM_myParentAddressH_LOCATION, highByte(myParentAddress));
  //Serial.print("myParentAddress:");Serial.println(EEPROM.read(EEPROM_myParentAddressL_LOCATION) + (EEPROM.read(EEPROM_myParentAddressL_LOCATION) * 0x100),HEX);
  EEPROM.write(EEPROM_myLatchKeyNumberL_LOCATION, lowByte(myLatchKeyNumber)); EEPROM.write(EEPROM_myLatchKeyNumberH_LOCATION, highByte(myLatchKeyNumber));
  //Serial.print("myLatchKeyNumber:");Serial.println(EEPROM.read(EEPROM_myLatchKeyNumberL_LOCATION) + (EEPROM.read(EEPROM_myLatchKeyNumberL_LOCATION) * 0x100),HEX);
//  EEPROM.write(EEPROM_myChildIndex_LOCATION, myChildIndex);
  EEPROM.write(EEPROM_preemie_State_LOCATION, _WHAT_network_State);
  //Serial.print("_WHAT_network_State:");Serial.println(EEPROM.read(EEPROM_preemie_State_LOCATION),HEX);
  EEPROM.write(EEPROM_USED_FLAG_LOCATION, USED_EEPROM_VALUE);
  //Serial.print("Used:");Serial.println(EEPROM.read(EEPROM_USED_FLAG_LOCATION),HEX);

#ifdef __WHAT_FULLLINK_h
  _EEPROM_store_links();
#endif

  _EEPORM_store_children();
}
void _EEPORM_store_children (void)
{ //Having this as a seperate file allows just the children list to be updated.
  // reset all child addresses:
  uint8_t _EEPROM_Child_Location = _EEPROM_Child_LOCATION;
  //Serial.print(F("_EEPROM_Child_Location. Adr: 0x"));Serial.println(_EEPROM_Child_Location,HEX);
  for(uint8_t _i=1;_i<16;_i++)
  { // Store index 0 to the end, even though index 0 actually is used for this device.
    EEPROM.write(_EEPROM_Child_Location++, lowByte(myChildren[_i].ChildAddress)); EEPROM.write(_EEPROM_Child_Location++, highByte(myChildren[_i].ChildAddress));
    EEPROM.write(_EEPROM_Child_Location++, lowByte(myChildren[_i].LatchKeyValue)); EEPROM.write(_EEPROM_Child_Location++, highByte(myChildren[_i].LatchKeyValue));
    EEPROM.write(_EEPROM_Child_Location++, myChildren[_i].RelationshipType); 
    EEPROM.write(_EEPROM_Child_Location++, lowByte(myChildren[_i].Security)); EEPROM.write(_EEPROM_Child_Location++, highByte(myChildren[_i].Security));
    //Serial.println(_EEPROM_Child_Location,HEX);
   }
   Serial.print(F("End _EEPROM_Location. Adr: 0x"));Serial.println(_EEPROM_Child_Location,HEX);
}


void _EEPROM_store_links(void)
{ //Having this as a seperate file allows just the link list to be updated.
#ifdef __WHAT_FULLLINK_h  // This routine should only be used if __WHAT_FULLLINK_h is included.
// Store link table.
  uint8_t _EEPROM_Link_Location = EEPROM_LINK_LOCATION;
  //Serial.print(F("_EEPROM_Link_Location. Adr: 0x"));Serial.println(_EEPROM_Link_Location,HEX);
  for(uint8_t _i=0;_i<numWHATLinks;_i++)
  {
  EEPROM.write(_EEPROM_Link_Location++, lowByte(WHATLinks[_i])); EEPROM.write(_EEPROM_Link_Location++, highByte(WHATLinks[_i]));
  }
#endif
}
 #endif  //AUTO_ADDRESSING  
//void WHAT_SHOW_ALL_USED_EEPROM (void)
//{
//  #ifdef EEPROM_WHAT_SHOW_VALUES
//  Serial.print ("All EEPROM starting at Adr:0x");Serial.println(EEPROM_WHAT_LOCATION,HEX);
//  for(uint8_t _i = EEPROM_WHAT_LOCATION;_i<255;_i++)
//  {
//    Serial.print ("Adr: 0x");Serial.print (_i,HEX);Serial.print (" = ");Serial.println(EEPROM.read(_i),HEX);
//  }
//  #endif
//}

// TOC PREEMIE EEPROM Values
void WHAT_Default_EEPROM_Values(void)
{
  myPANID = 0xFFFF;
  mySourceAddress = 0xFFFF;
  WHAT_set_pan(0xFFFF);
  WHAT_set_SourceAddress(0xFFFF);

#ifdef AUTO_ADDRESSING
  myParentAddress = 0xFFFF;
  myLatchKeyNumber = 0xFFFF;
//  myChildIndex = 0;    // each child will be indexed into the "myChildren" array.
  mySecurity = 0xFFFF;
#ifdef __WHAT_FULLLINK_h
  // Reset all link addressses.
  for(uint8_t _i=0;_i<numWHATLinks;_i++)
  {
    WHATLinks[_i]=0xFFFF;
    //#ifdef CHILD_MONITOR
    //  Serial.print("Resetting Link");Serial.println(WHATLinks[_i],HEX);
    //#endif
  }
#endif  //__WHAT_FULLLINK_h
  // reset all child addresses:
  for(uint8_t _i=1;_i<15;_i++)
  { 
    myChildren[_i].ChildAddress = 0xFFFF;
    myChildren[_i].LatchKeyValue = 0xFFFF;
    myChildren[_i].RelationshipType = 0;
    myChildren[_i].Security = 0xFFFF;
  }
#endif  //AUTO_ADDRESSING
}

// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//#ifdef AUTO_ADDRESSING
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// *  Network creation: - This is all built into WHAT.ino, and enabled by #define AUTO_ADDRESSING

// WHAT_Reusable_Message1 
void holdingFunction(WHAT_Packet _WHAT_Rx_Packet, uint8_t _index)
{
  // The re-usable call has to reserve a function.
  // Different functions will be called based on what is loaded into this message registration
  //f_WHAT_Reusable_Message1 = true;   // Indicate that the message has been recieved.
}

/*
  All new devices are programmed with PANID FFFF and source address FFFF (considered a preemie)
    When a device recognizes it is a preemie (PANID and SA are FFFF), it will periodically send a "REQUEST_JOIN_NETWORK_02" message.

    Rev 200203
      At power on, preemie sends message: REQUEST_JOIN_NETWORK_03
      Preemie has 2 possible paths:
        1) Preemie sees another REQUEST_JOIN_NETWORK_03 (another preemie), so it creates a network:
                - Use millis() lower 16 bits to create a unique PANID.
                - Send REQUEST_ACK_03 and wait for _5_ seconds. (to ensure PANID is not already in use)
                - if REPLY_ACKNOWLEDGE_03 recieved, change PANID and try again.
                - if timeout, use proposed PANID and source address 0000.
        2) Preemie sees a NETWORK_ALERT_PREEMIE_03 message:
                - increases BLUE blink rate and waits for NETWORK_ALLOW_PREEMIE_04 (created by a network member's network button activation.
        3) Preemie sees a NETWORK_ALLOW_PREEMIE_04 message (ONLY after seeing a NETWORK_ALERT_PREEMIE_03 message):
                - Along with NETWORK_ALLOW_PREEMIE_04 come PROPOSE_CHILD_NUMBER_07 messages.
                - Preemie keeps the lowest parent/child address offered and joins network after _5_ seconds.
                - sends ACCEPT_PARENT_06 message.
      Adult has a couple of network creation options.
        1) sees REQUEST_JOIN_NETWORK_03
              - if it has a valid child slot available sends NETWORK_ALERT_PREEMIE_03 and PROPOSE_CHILD_NUMBER_07  
              - changes to WHAT_NEW_CHILD_REQ state.
        2) sees NETWORK_ALERT_PREEMIE_03
              - Monitors its network button.  If pressed, sends NETWORK_ALLOW_PREEMIE_04 and PROPOSE_CHILD_NUMBER_07
        3) sees NETWORK_ALLOW_PREEMIE_04 (with or without seeing REQUEST_JOIN_NETWORK_03)
              - sends PROPOSE_CHILD_NUMBER_07
     Adult in WHAT_NEW_CHILD_REQ state
        1) monitors its own network button,and (if pressed):
            - sends NETWORK_ALLOW_PREEMIE_04
            - Changes to WHAT_NEW_CHILD_OFFER state.
        2) monitors network messages for NETWORK_ALLOW_PREEMIE_04
            - changes to WHAT_NEW_CHILD_OFFER state.
        3) monitors network for CHOOSE_NETWORK_05
            - if chosen network number does not match this PANID, return to WHAT_PRE_NET_ALIVE state 
        
     Adult in WHAT_NEW_CHILD_OFFER
        1) Monitors network for ACCEPT_PARENT_06
           - If the accepted parent source matches THIS source:
              o Verify recieved child address is valid and available.
              o Add this child - Record latchkey value, and increment the child index.
              o return to WHAT_PRE_NET_ALIVE state
        2) monitors network for CHOOSE_NETWORK_05
            - if chosen network number does not match this PANID, return to WHAT_PRE_NET_ALIVE state 

    Other always monitored network messages:

*/
/*
// Rx Message flags
bool f_Rx_REQUEST_JOIN_NETWORK_03 = false;    // Preemie asking to join network
bool f_Rx_NETWORK_ALERT_PREEMIE_03 = false;   // Notice from another device that a preemie is trying to join
bool f_Rx_CHOOSE_NETWORK_05  = false;         // Notice to other networks that a network has accepted this preemie
bool f_Rx_NETWORK_ALLOW_PREEMIE_04 = false;   // Notice to this preemie that it may accept its parent.
bool f_Rx_PROPOSE_CHILD_NUMBER_07 = false;    // Preemie asking the network for a child number
bool f_Rx_NETWORK_JOIN_COMPLETE_03 = false;   // Preeemie accepted a proposed parent 
bool f_Rx_NETWORK_JOIN_ABORT_03 = false;      // User abort of the acceptance process.
bool f_Rx_REPLY_ACKNOWLEDGE_03 = false;       // While a new network is forming, requests existing network to claim its PANID
*/

void _Control_Net_Light(/*LightColor1*/ uint8_t LC1,/*Light Color 1 time*/ double _Time1,/*LightColor2 */ uint8_t LC2,/*Light Color 2 time*/ double _Time2 )
{
  // Consolidates all of the flash timers into one subroutine.
  //static double _flash_timer = millis();
  static uint8_t _last_LC1 = WHAT_LIGHT_OFF;
  static uint8_t _last_LC2 = WHAT_LIGHT_OFF;
  
  if((_last_LC1 != LC1) | (_last_LC2 != LC2))
  {
    _flash_timer = millis();    // If either color command changes, reset the timer.
    _last_LC1 = LC1;
    _last_LC2 = LC2;
  }

  if( _flash_timer <= millis() )
  {
    if(_WHAT_network_StateColor != LC1 ){
    _WHAT_network_StateColor = LC1;
    _flash_timer = millis() + _Time1;
    }
    else
    {
      _WHAT_network_StateColor = LC2;
      _flash_timer = millis() + _Time2;
    }
  }
}

// ******************** Rx Messages
// non-reusable message.  different functions when a preemie.
// TOC Individual Recieve messages
void Rx_REQUEST_JOIN_NETWORK_03 (WHAT_Packet _WHAT_Rx_Packet, uint8_t _index)
{
   f_Rx_REQUEST_JOIN_NETWORK_03 = true;
      #ifdef DX_ONLY
        Serial.println(F("f_Rx_REQUEST_JOIN_NETWORK_03"));
        Serial.print(F("_WHAT_network_State:")); Serial.println(_WHAT_network_State, DEC);
      #endif

}  

// TOC Individual Recieve messages 
void Tx_REQUEST_ACK_03(void)
{        
    //myPANID = (uint16_t) millis();  // Make up a new PANID based on the timer.  This will be a pretty random value.
    WHAT_Packet _WHAT_Tx_Packet;
    _WHAT_Tx_Packet.WHAT_MAC_MESSAGE;
    // Now send the ACK request
    //WHAT_set_pan(myPANID);
    _WHAT_Tx_Packet.pan_id = myPANID;      // Use the desired PANID so only that PANID will reply (if it is already used)
    _WHAT_Tx_Packet.destination = 0xFFFF;  // Send to ANY device with this PANID.
    _WHAT_Tx_Packet.sourceaddress = 0xFFFF;  // no source address claimed yet, in case another network has this address.
    uint8_t _index = 0;
    _WHAT_Tx_Packet.payload[_index++] = 3; //Total payload length (including this byte)
    _WHAT_Tx_Packet.payload[_index++] = 2; //MID length (including this byte)
    _WHAT_Tx_Packet.payload[_index++] = highByte(REQUEST_ACK_03);
    _WHAT_Tx_Packet.payload[_index++] = lowByte(REQUEST_ACK_03);
    //Serial.print(F("Tx_REQUEST_ACK_03: ")); Serial.print(_WHAT_Tx_Packet.payload[0],HEX);Serial.print(_WHAT_Tx_Packet.payload[1],HEX);Serial.print(_WHAT_Tx_Packet.payload[2],HEX);Serial.print(_WHAT_Tx_Packet.payload[3],HEX);
      WHAT_TransmitPacket(_WHAT_Tx_Packet);
        #ifdef CHILD_MONITOR
        Serial.print(F("Asking for ACK on :0x")); Serial.println(myPANID,HEX);
        //Serial.print(F("Undefined Preemie state : ")); Serial.println(_WHAT_network_State,HEX);
      #endif
}

// TOC Individual Recieve messages 
// WHAT_Reusable_Message1 
void Rx_REPLY_ACKNOWLEDGE_03(WHAT_Packet _WHAT_Rx_Packet, uint8_t _index)
{
// Rx Message flags
  f_Rx_REPLY_ACKNOWLEDGE_03 = true;       // While a new network is forming, requests existing network to claim its PANID
    #ifdef CHILD_MONITOR
    Serial.print(F("Recieve ACK from :0x")); Serial.println(_WHAT_Rx_Packet.pan_id);
    //Serial.print(F("Undefined Preemie state : ")); Serial.println(_WHAT_network_State,HEX);
    #endif
}

// &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
// &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
// &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
// &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
#ifdef AUTO_ADDRESSING
// TOC Adult operations 
void WHAT_StateCheck(void)
{
  #define _RQST_WAIT_TIME 60000 //Wait 30 seconds to allow user to push network button.
  #define _RQST_ACK_DELAY 5000  // wait 5 second after requesting an ACK before using the PANID

  #define _WHAT_NEW_CHILD_TIME_DELAY 60000 // Only stay in new child mode for 30 seconds if not accepted earlier.

  bool default_case = true;    // Each routine will mark this as false so if NONE are executed, default will be true.
 //Serial.println("memberloop");
  //static double _flash_timer = millis();
  //static WHAT_State _last_WHAT_network_State = WHAT_STARTUP;

  //TOC Evaluate WHAT network state
//  switch (_WHAT_network_State)
//  {
    /**********************************************************************************/
    /**********************************************************************************/
    /**********************************************************************************/
    // Check if this is a preemie or adult
    // Startup is the power on default
      if(_WHAT_network_State == WHAT_STARTUP )//      case WHAT_STARTUP:
      {
          default_case = false;  // Used to verify that one of the cases executed.
          if( (mySourceAddress < 0xFFFF) | (myParentAddress< 0xFFFF))
          {
            // This device has already been connected to a network and assigned an address.
            // see if the operator is trying to reset it at power up.
            _WHAT_network_State = CHECK_PREEMIE_RESET;
          //Serial.print(F("_WHAT_network_State -> CHECK_PREEMIE_RESET")); Serial.println(_WHAT_network_State);
          }
          else
          {
            // This device has never been connected to a network.
            // Handle it as a preemie.
            _WHAT_network_State = WHAT_PRE_PREEMIE_NETWORK_SEARCH;
          //Serial.print(F("_WHAT_network_State -> WHAT_PRE_PREEMIE_NETWORK_SEARCH")); Serial.println(_WHAT_network_State);
          }
      }//break;

   // ************************************************************************
    // ************************************************************************
    // ************************************************************************
    /*     Check to see if PREEMIE should be restored. */
     // ************************************************************************
    // This should be the predominant state.  Once a network is joined or created,
    // This device will just watch for a few MAC messages.

 // TOC Manual Restore PREEMIE state
    if(_WHAT_network_State == CHECK_PREEMIE_RESET )//    case CHECK_PREEMIE_RESET:
    {
          default_case = false;  // Used to verify that one of the cases executed.
        #ifdef CHILD_MONITOR
         Serial.println(F("Checking for PreemieReset"));
        #endif
        if (_WHAT_NetworkButton == HIGH) // Button is active high
        {
          WHAT_Reusable_Timer1 = millis() + 100;  // NETWORK button must be held for at least this long.
          WHAT_Reusable_Timer2 = millis()+ 10000; // NETWORK button must be released and re-pressed before this time expires.
          _WHAT_network_State = WHAT_RESET_PREMIE_INITIATED;  // Have the network light reflect the potential PREEMIE reset status.
          #ifdef CHILD_MONITOR
            Serial.print(F("_WHAT_network_State: WHAT_RESET_PREMIE_INITIATED ")); Serial.println(_WHAT_network_State);
          #endif

        }
        else // Button is not pressed.
        {
          #ifdef CHILD_MONITOR
          Serial.println(F("PreemieReset NOT initiated"));
          #endif

          if( (mySourceAddress < 0xFFFF) | (myParentAddress< 0xFFFF))
          { // Device is NOT a preemie
          _WHAT_network_State = WHAT_PRE_NET_ALIVE;  //
          //Serial.print(F("_WHAT_network_StateColor:")); Serial.println(_WHAT_network_StateColor,DEC);
          //Serial.print(F("_WHAT_network_State:")); Serial.println(_WHAT_network_State,DEC);
          }
          else
          { // device is a preemie.  try to find a network.
            _WHAT_network_State = WHAT_PRE_PREEMIE_NETWORK_SEARCH;  //
          }

        }
      // TOC Handle Light color based on network status
      // No lights during this check.  if the button is pressed at power on, validate user request to re-preemie.
    }//break;

    if(_WHAT_network_State == WHAT_RESET_PREMIE_INITIATED )//    case WHAT_RESET_PREMIE_INITIATED:   // Button was held at 
    {
          default_case = false;  // Used to verify that one of the cases executed.
      if (_WHAT_NetworkButton == HIGH) // Button is active high
      {
        if(WHAT_Reusable_Timer1 <= millis() )
        { // Button was held for at least the minimum hold time.
          // Transition for waiting for it to be released.
          //WHAT_Reusable_Timer2 <= millis() + 2000;
         #ifdef CHILD_MONITOR
         Serial.print(F("_WHAT_network_State: WHAT_RESET_PREMIE_PENDING ")); Serial.println(WHAT_RESET_PREMIE_PENDING);
        #endif
          _WHAT_network_State = WHAT_RESET_PREMIE_PENDING;

        }
      }
      else
      {
           _WHAT_network_State = WHAT_PRE_NET_ALIVE;  //
      }
    // TOC Handle Light color based on network status
    // Network button held at power on - cycle while waiting for the button to be held long enough.
    // No light during this initial press time.
      //_WHAT_network_StateColor = WHAT_LIGHT_OFF;
      _Control_Net_Light(/*LightColor1*/ WHAT_LIGHT_OFF,/*Light Color 1 time*/ 50,/*LightColor2 */ WHAT_LIGHT_OFF,/*Light Color 2 time*/ 50 );

      }//  break;
    
    if(_WHAT_network_State == WHAT_RESET_PREMIE_PENDING )//     case WHAT_RESET_PREMIE_PENDING:
    {
          default_case = false;  // Used to verify that one of the cases executed.
      // Minimum press time established... the button must now be RELEASED before continuing.
      if (_WHAT_NetworkButton == HIGH) // Button is active high
      { // ensure max timer has not been exceeded while the button is being held.
       if(WHAT_Reusable_Timer2 <= millis() )
        { // Button was never released.
          // The network button is not functioning properly.
          #ifdef CHILD_MONITOR
            Serial.println(F("Network Button was never released. "));
            #endif
           _WHAT_network_State = WHAT_PRE_NETWORK_ERROR;  //
        }
      }
      else
      { // the button was release before the timer expired.  Now it must be pressed again to restore the preemie.
           _WHAT_network_State = WHAT_RESET_PREMIE_AWAITING_ACTIVATION;
        #ifdef CHILD_MONITOR
         Serial.println(F("_WHAT_network_State: WHAT_RESET_PREMIE_AWAITING_ACTIVATION ")); Serial.println(WHAT_RESET_PREMIE_AWAITING_ACTIVATION);
        #endif
      }
    // TOC Handle Light color based on network status
    // Network button held at power on - cycle colors while waiting for the button to be released.
      if( _flash_timer <= millis() ){
        switch( _WHAT_network_StateColor)
        {
          case WHAT_LIGHT_BLUE:
            _WHAT_network_StateColor = WHAT_LIGHT_GREEN;
          break; 
          case WHAT_LIGHT_GREEN:
            _WHAT_network_StateColor = WHAT_LIGHT_RED;
          break; 
          case WHAT_LIGHT_RED:
            _WHAT_network_StateColor = WHAT_LIGHT_OFF;
          break; 
          default:
            _WHAT_network_StateColor = WHAT_LIGHT_BLUE;
          break; 
        }
        _flash_timer = millis() + 200;
      }
    }//  break;
    
    if(_WHAT_network_State == WHAT_RESET_PREMIE_AWAITING_ACTIVATION )//    case WHAT_RESET_PREMIE_AWAITING_ACTIVATION:
    {
          default_case = false;  // Used to verify that one of the cases executed.
      // The button is working - it was pressed and released.  When pressed again, reset to preemie.
      if (_WHAT_NetworkButton == HIGH) // Button is active high
      { //The button was pressed again.  Reset to PREEMIE mode.
        Serial.println(F("Reset Preemie criteria complete"));
        // preemie creation criteria met.   restore to preemie state, and continue with program initialization
        // Send an abandon network message so the parent will release the child slot, then revert to compile time values.
        WHAT_Packet _WHAT_Tx_Packet;  // Make a temporary transmit packet
        _WHAT_Tx_Packet.WHAT_MAC_MESSAGE; 
        _WHAT_Tx_Packet.pan_id = myPANID;
        _WHAT_Tx_Packet.destination = myParentAddress;
        _WHAT_Tx_Packet.sourceaddress = mySourceAddress;
        uint8_t _index = 0;
        _WHAT_Tx_Packet.payload[_index++] = 6; //Total payload length (including this byte)
        _WHAT_Tx_Packet.payload[_index++] = 5; //MID length (including this byte)
        _WHAT_Tx_Packet.payload[_index++] = highByte(ABANDON_CHILD_05);
        _WHAT_Tx_Packet.payload[_index++] = lowByte(ABANDON_CHILD_05);
        _WHAT_Tx_Packet.payload[_index++] = highByte(myLatchKeyNumber);
        _WHAT_Tx_Packet.payload[_index++] = lowByte(myLatchKeyNumber);
          WHAT_TransmitPacket(_WHAT_Tx_Packet);

        WHAT_Default_EEPROM_Values();
        WHAT_StoreEEPROM();
        
        Serial.println(F("Preemie state restored."));
        _WHAT_network_State = WHAT_PREEMIE_RESTORED;  //
        #ifdef CHILD_MONITOR
         Serial.println(F("_WHAT_network_State: WHAT_PREEMIE_RESTORED ")); Serial.println(WHAT_PREEMIE_RESTORED);
        #endif
        WHAT_Reusable_Timer2 = millis()+ 1000; // Give happy indication for a second once restored.
    // TOC Handle Light color based on network status
    // Preemie state is restored. make the light green.
        //_WHAT_network_StateColor = WHAT_LIGHT_GREEN; 
        _Control_Net_Light(/*LightColor1*/ WHAT_LIGHT_GREEN,/*Light Color 1 time*/ 50,/*LightColor2 */ WHAT_LIGHT_GREEN,/*Light Color 2 time*/ 50 );

      }// End if the button was pressed.
      else
      { // If the button is never pressed again, have the restore time out.
      if(WHAT_Reusable_Timer2 <= millis() )
        { // Button was never released.
          // The network button is not functioning properly.
          #ifdef CHILD_MONITOR
          Serial.println(F("Network Button -not- re-pressed"));
          #endif
           _WHAT_network_State = WHAT_PRE_NETWORK_ERROR;  //
        }
      }
    // TOC Handle Light color based on network status
    // Preemie reset started. Pressing again activates.  fast flash green until activated.
      _Control_Net_Light(/*LightColor1*/ WHAT_LIGHT_GREEN,/*Light Color 1 time*/ 50,/*LightColor2 */ WHAT_LIGHT_OFF,/*Light Color 2 time*/ 50 );
    }//  break;

    if(_WHAT_network_State == WHAT_PREEMIE_RESTORED )//     case WHAT_PREEMIE_RESTORED:
    {
          default_case = false;  // Used to verify that one of the cases executed.
      if(WHAT_Reusable_Timer2 <= millis() )
        { // Preemie restoration is complete, and indication was given to user.
           _WHAT_network_State = WHAT_PRE_PREEMIE_NETWORK_SEARCH;  //
        }
    // TOC Handle Light color based on network status
    // Preemie state is restored. Hold the light green as an indication.
        //_WHAT_network_StateColor = WHAT_LIGHT_GREEN; 
        _Control_Net_Light(/*LightColor1*/ WHAT_LIGHT_GREEN,/*Light Color 1 time*/ 50,/*LightColor2 */ WHAT_LIGHT_GREEN,/*Light Color 2 time*/ 50 );
      }//  break;

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
// TOC PREEMIE operations 
      // ++++++++++++   New preemie has not seen a network yet     ++++++++++++
    if(_WHAT_network_State == WHAT_PRE_PREEMIE_NETWORK_SEARCH )//       case WHAT_PRE_PREEMIE_NETWORK_SEARCH:
    {
          default_case = false;  // Used to verify that one of the cases executed.
          //The preemie trying to join the network. Use the reusable message to watch for another network.
          // _WHAT_network_StateColor = WHAT_LIGHT_OFF;
          _Control_Net_Light(/*LightColor1*/ WHAT_LIGHT_OFF,/*Light Color 1 time*/ 50,/*LightColor2 */ WHAT_LIGHT_OFF,/*Light Color 2 time*/ 50 );

           f_Rx_NETWORK_ALERT_PREEMIE_03 = false;   // Clear the message reception flag.
            f_Rx_REQUEST_JOIN_NETWORK_03 = false;    // Clear the message reception flag.
            _flash_timer = millis();
            _WHAT_network_State = WHAT_PREEMIE_NETWORK_SEARCH;  // request joining any network that might be availble.

          #ifdef CHILD_MONITOR
          Serial.print(F("_WHAT_network_State: WHAT_PREEMIE_NETWORK_SEARCH ")); Serial.println(WHAT_PREEMIE_NETWORK_SEARCH);
          #endif
    }//    break;      

    if(_WHAT_network_State == WHAT_PREEMIE_NETWORK_SEARCH )//       case WHAT_PREEMIE_NETWORK_SEARCH:
    {
          default_case = false;  // Used to verify that one of the cases executed.
        // TOC PREEMIE operations 
                // New device.  request joining any network that might be availble.
                // This is a "preemie" - this device is hoping to join a network, and repeatedly sends a request to join.
          if( _flash_timer <= millis() ) // If flash timer expired...
          {
    // TOC    Request Joining Network 
              // Tx.PANID and Tx.SOURCEADDRESS are defaulted to 0xFFFF
              WHAT_Packet _WHAT_Tx_Packet;  // Make a temporary transmit packet
              _WHAT_Tx_Packet.WHAT_MAC_MESSAGE; 
              //WHAT_set_pan(myPANID);
              //WHAT_set_SourceAddress(mySourceAddress);
              _WHAT_Tx_Packet.pan_id = 0xFFFF;
              _WHAT_Tx_Packet.destination = 0xFFFF;
              _WHAT_Tx_Packet.sourceaddress = 0xFFFF;
              uint8_t _index = 0;
              _WHAT_Tx_Packet.payload[_index++] = 4; //Total payload length (including this byte)
              _WHAT_Tx_Packet.payload[_index++] = 3; //MID length (including this byte)
              _WHAT_Tx_Packet.payload[_index++] = highByte(REQUEST_JOIN_NETWORK_03);
              _WHAT_Tx_Packet.payload[_index++] = lowByte(REQUEST_JOIN_NETWORK_03);
              WHAT_TransmitPacket(_WHAT_Tx_Packet);
              #ifdef CHILD_MONITOR
              Serial.println(F("Tx WHAT_PREEMIE_NETWORK_SEARCH ")); Serial.println(millis());
              #endif
 
               // TOC Handle Light color based on network status
            // Preemie state Looking for available network.  Toggle blue on/off while waiting.
            // Toggle occurs at the same time as a new message is transmitted (once per second)
              if(_WHAT_network_StateColor != WHAT_LIGHT_BLUE )
              {
                _WHAT_network_StateColor = WHAT_LIGHT_BLUE;
              }
              else
              {
                _WHAT_network_StateColor = WHAT_LIGHT_OFF;
              }
              _flash_timer = millis() + 2000;
           } // End If Flash timer expired

        // ooo One way out is to form a network ooo
        // TOC    Creating a new network 
            if(f_Rx_REQUEST_JOIN_NETWORK_03)
            { // Another preemie is looking for a network, and this device has not yet joined a network.
              // Form a new network.
              f_Rx_REQUEST_JOIN_NETWORK_03 = false; // clear the message flag when it is used.
              myPANID = (uint16_t) millis();  // Make up a new PANID based on the timer.  This will be a pretty random value.
              WHAT_set_pan(myPANID);
          
              //prepare to recieve an ACK request from the chosen PANID. If other devices respond, then this address is already used.
              //re-configure the reusable message to receive an ACK from an existing network.
              WHATRxMessage[WHAT_Reusable_Message1].MessageID = REPLY_ACKNOWLEDGE_03;
              WHATRxMessage[WHAT_Reusable_Message1].callback = &Rx_REPLY_ACKNOWLEDGE_03; // Message callback set here to collect the responses
             _WHAT_network_StateColor = WHAT_LIGHT_OFF;
              f_Rx_REPLY_ACKNOWLEDGE_03 = false;    // Clear the message flag so only new messages will be used.
              // TOC    Avoiding network conflicts 
                // Now send the ACK request
                // If anything replies, then this network already exists, and a new PANID will be chosen.
                Tx_REQUEST_ACK_03();    // Request an ACK to see if any other device is using this PANID.
                      
                  // Start a delay timer.  If it expires with no ACK reply, this PANID is acceptable.
                WHAT_Reusable_Timer1 = millis() + _RQST_ACK_DELAY;
                Serial.print(F("Creating Network @ ")); Serial.println(myPANID, HEX);
                _WHAT_network_State = WHAT_NETWORK_CREATING;  //Creating network, and delaying to see if this PANID is already used.
            }
                // ooo another way out is to be allowed into an existing network ooo
        // TOC    Network identified, waiting to be allowed 
                 if(f_Rx_NETWORK_ALERT_PREEMIE_03)
                {
                  f_Rx_NETWORK_ALERT_PREEMIE_03 = false; // clear the message flag when it is used.
                  _WHAT_network_State = WHAT_NETWORK_SEEN;
                  #ifdef CHILD_MONITOR
                    Serial.print(F("A Network Seen: ")); //Serial.println(millis(), HEX);
                  #endif
               }
      }//  break;

     // ++++++++++++   Preemie is forming a network      ++++++++++++
     if(_WHAT_network_State == WHAT_NETWORK_CREATING )//     case WHAT_NETWORK_CREATING:
     {
          default_case = false;  // Used to verify that one of the cases executed.
        // TOC    Creating a new network 
      // This device created a network, and sent an ACK request to verify its PANID is not used.
      // wait here for a period of time, and if no reply arrives, then make the network.
      // A reusable message is monitored for this reception. It was registered in "WHAT_PREEMIE_NETWORK_SEARCH".
      //WHATRxMessage[WHAT_Reusable_Message1].MessageID = REPLY_ACKNOWLEDGE_03;
      //WHATRxMessage[WHAT_Reusable_Message1].callback = &Rx_REPLY_ACKNOWLEDGE_03; // Message callback set here to collect the responses
      if( f_Rx_REPLY_ACKNOWLEDGE_03)
        {// Ack seen.  try a different address.
          // As a preemie converting to a network creator, this device will send a
          // request for ACK to see if the PANID it chose is already used.
          // So, this message should NOT be recieved, (because to get to this point,
          //  2 preemies are within range of each other, and they have not been accepted into another
          //  network, and the new PANID is randomly chosen, so it will probably not match.
          // But if it DOES happen, just try a different PANID.
          f_Rx_REPLY_ACKNOWLEDGE_03 = false; // clear the message flag when it is used.
          myPANID = (uint16_t) millis();  // Make up a -new- PANID based on the timer.  This will be a pretty random value.
          WHAT_set_pan(myPANID);
          Tx_REQUEST_ACK_03();    // Request an ACK to see if any other device is using this PANID.
      
          // Start a delay timer.  If it expires with no ACK reply, this PANID is acceptable.
          WHAT_Reusable_Timer1 = millis() + _RQST_ACK_DELAY;
        #ifdef CHILD_MONITOR
        Serial.print(F("Trying new PANID : ")); Serial.println(millis());
        #endif
        }
        if (WHAT_Reusable_Timer1 <= millis())
        {
          //Use the current PANID and leave PREEMIE mode.
          //Serial.print(WHAT_Reusable_Timer1); Serial.print(" <- Timer     millis-> "); Serial.println(millis());
          Serial.print(F("Network Created. PANID: 0x")); Serial.println(myPANID, HEX);
          //WHAT_Default_EEPROM_Values();
          mySourceAddress = 0;  // When forming a new network, start with the address 0x0000
          myParentAddress = 0;  // This is the only device that is its own parent.
          myLatchKeyNumber = (uint16_t) millis();  // This device will make its own latchkey number (which really can not be used, because no one will know it.
          //myChildIndex = 0;             // When forming a new network, this device will start with no children.
          mySecurity = myLatchKeyNumber; // use this device's latchkey number as the base security number.
          WHAT_set_SourceAddress(mySourceAddress);
      
          //Serial.print("Pre-Store PANID:0x"); Serial.println(myPANID);
          WHAT_StoreEEPROM();
          Serial.println(F("EEPROM Stored"));

          //_WHAT_network_State = WHAT_PRE_NET_ALIVE; // Control will transition to the program loop from here.
          WHAT_Reusable_Timer1 = millis()+ 1000; // This will hold the indication of successful creation.
          _WHAT_network_State = WHAT_NETWORK_CREATED; //This device created a network.
        }

        // TOC Handle Light color based on network status
        // This node is creating a network and becoming node 0000.
        // Fast blue blink is the indication.
       _Control_Net_Light(/*LightColor1*/ WHAT_LIGHT_BLUE,/*Light Color 1 time*/ 100,/*LightColor2 */ WHAT_LIGHT_OFF,/*Light Color 2 time*/ 100 );

       }//  break;

      // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  
      // ++++++++++++  Network was created by this device  ++++++++++++
     if(_WHAT_network_State == WHAT_NETWORK_CREATED )//      case  WHAT_NETWORK_CREATED:
     {
           default_case = false;  // Used to verify that one of the cases executed.
          //_WHAT_network_StateColor = WHAT_LIGHT_GREEN;
          _Control_Net_Light(/*LightColor1*/ WHAT_LIGHT_GREEN,/*Light Color 1 time*/ 1000,/*LightColor2 */ WHAT_LIGHT_GREEN,/*Light Color 2 time*/ 1000 );

          if( WHAT_Reusable_Timer1 <= millis() )  //THis is set in "WHAT_NETWORK_CREATING"
          {

            _Control_Net_Light(/*LightColor1*/ WHAT_LIGHT_OFF,/*Light Color 1 time*/ 10,/*LightColor2 */ WHAT_LIGHT_OFF,/*Light Color 2 time*/ 10 );
           _WHAT_network_State = WHAT_PRE_NET_ALIVE;  // After creation has been indicated, go to live operations.
          }
      }//  break;

      // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  
      // ++++++++++++   Preemie is joining a network      ++++++++++++
      if(_WHAT_network_State == WHAT_NETWORK_SEEN )//      case WHAT_NETWORK_SEEN:
      {
          default_case = false;  // Used to verify that one of the cases executed.
        // TOC    Network identified, waiting to be allowed 
              WHAT_Reusable_Timer1 = millis() + 60000; // During this time, collects offered child addresses and waits for network button press.
              #ifdef CHILD_MONITOR
              Serial.println(F("PREEMIE sees network : ")); //Serial.println(millis());
              #endif
              // This is the initial run through for this state.
              // Keep an eye out for offered child slots.
              _Offered_PANID = 0xFFFF; // These are collected in  Rx_PROPOSE_CHILD_NUMBER_07 to
              _Offered_SourceAddress = 0xFFFF; // identify the lowest offered address seen.
              _Offered_Latchkey = 0xFFFF;
              WHATRxMessage[WHAT_Reusable_Message1].MessageID = NETWORK_ALLOW_PREEMIE_04;  // these are prospective parents offering free addresses.
              WHATRxMessage[WHAT_Reusable_Message1].callback = &Rx_NETWORK_ALLOW_PREEMIE_04; // Message callback set here to collect the responses
             _WHAT_network_StateColor = WHAT_LIGHT_OFF;
             #ifdef CHILD_MONITOR
                Serial.print(F("WHAT_Reusable_Message1: ")); Serial.println(WHAT_Reusable_Message1);
                //Serial.print(F("Response Timeout : ")); Serial.println(millis());
              #endif
               f_Rx_NETWORK_ALLOW_PREEMIE_04 = false;   // Clear the message reception flag.

              _WHAT_network_State = WHAT_NETWORK_SEEN_WAIT;
   
      }//   break;

       if(_WHAT_network_State == WHAT_NETWORK_SEEN_WAIT )//     case WHAT_NETWORK_SEEN_WAIT:
       {
          default_case = false;  // Used to verify that one of the cases executed.
            if (WHAT_Reusable_Timer1 <= millis())
            {
              // When the LONG timeout occurs, revert to the request state.
              _WHAT_network_State = WHAT_PRE_PREEMIE_NETWORK_SEARCH;
              #ifdef CHILD_MONITOR
              Serial.print(F("Network Button not pressed : ")); Serial.println(millis());
              #endif
            }
            if(f_Rx_NETWORK_ALLOW_PREEMIE_04)
            {// This preemie has been accepted to a network.
              // Get a parent address from the new network.
              //Serial.println(F("Preemie f_Rx_NETWORK_ALLOW_PREEMIE_04"));
              f_Rx_NETWORK_ALLOW_PREEMIE_04 = false; // clear the message flag when it is used.
              #ifdef CHILD_MONITOR
                Serial.print(F("Offered PANID:0x")); Serial.println(_Offered_PANID,HEX);
                //Serial.print(F("Response Timeout : ")); Serial.println(millis());
              #endif
               if (_Offered_PANID < 0xFFFF) // Only do this if a PANID was offered.
                {
                  // Other network members on this network seeing the NETWORK_ALLOW_PREEMIE_04 will send a proposed child address.
                  // note that the allowing node will send a proposed child number in the same messages as the allow message.
                  //  because of that, the _offered values must be reset prior to being allowed. (usually done in Network seen)
                  _WHAT_network_State = WHAT_PRE_NETWORK_ALLOW_PREEMIE_04; // A valid PAN ID was offered.
                 }
            }

                // If an abort message is recieved, return to the beginning.
                if(f_Rx_NETWORK_JOIN_ABORT_03) 
                {
                  f_Rx_NETWORK_JOIN_ABORT_03 = false; // clear the message flag when it is used.
                  //_WHAT_network_State = WHAT_PRE_REQUEST_JOIN_NETWORK_03; // If nothing offered a child slot, return to being a preemie.
                #ifdef CHILD_MONITOR
                  //Serial.print(F("Offered PANID:0x")); Serial.println(_Offered_PANID);
                  Serial.println(F("f_Rx_NETWORK_JOIN_ABORT_03")); 
                  _WHAT_network_State = WHAT_PRE_NETWORK_ERROR; //Give the lamp color/text that indicates that the connection was aborted.
                #endif
                }
                // TOC Handle Light color based on network status
                // This node is joining an existing network.
                // blue/green alternation is the indication.

       _Control_Net_Light(/*LightColor1*/ WHAT_LIGHT_GREEN,/*Light Color 1 time*/ 100,/*LightColor2 */ WHAT_LIGHT_BLUE,/*Light Color 2 time*/ 100 );

      }// break;
        
      // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  
      // ++++++++++++  Preemie is waiting for a proposed ID    ++++++++++++
       if(_WHAT_network_State == WHAT_PRE_NETWORK_ALLOW_PREEMIE_04 )//       case WHAT_PRE_NETWORK_ALLOW_PREEMIE_04:
       {
          default_case = false;  // Used to verify that one of the cases executed.
          WHAT_Reusable_Timer1 = millis() + 5000;   // Wait this amount of time before accepting the lowest offered source address.
          _WHAT_network_State = WHAT_NETWORK_JOINING;
          Serial.print(F("_WHAT_network_State = WHAT_NETWORK_JOINING")); Serial.println(_WHAT_network_State,DEC);
       }//break;

      // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  
        if(_WHAT_network_State == WHAT_NETWORK_JOINING )//     case WHAT_NETWORK_JOINING:
        {
           default_case = false;  // Used to verify that one of the cases executed.
        // TOC    Accepted to existing netowrk 
            // A button on the network has permitted this preemie to join.
            // This preemie waits to allow several proposals, then responds with acceptance of the lowest proposed address.
            // the re-usable timer set in "WHAT_PRE_NETWORK_ALLOW_PREEMIE_04" will serve as a delay while the child proposal addresses are collected.
              if (WHAT_Reusable_Timer1 <= millis()) //Wait a short time after permitted to join to get more proposed addresses.
              {
                //Serial.println(F("Allow Preemie Timer Expired"));
                //Serial.print(F("Offered PANID,PARENT,CHILD:"));Serial.print(_Offered_PANID,HEX);Serial.print(",");Serial.print(_Offered_Parent,HEX);Serial.print(",");Serial.println(_Offered_SourceAddress,HEX);
               #ifdef CHILD_MONITOR
                Serial.print(F("Joining. Offered PANID:0x")); Serial.println(_Offered_PANID,HEX);
                //Serial.print(F("Response Timeout : ")); Serial.println(millis());
              #endif
               if(_Offered_SourceAddress < 0xFFFF)
                {  
                  myPANID = _Offered_PANID;
                  WHAT_set_pan(myPANID);
                  mySourceAddress = _Offered_SourceAddress;
                  WHAT_set_SourceAddress(mySourceAddress);
                  WHAT_Packet _WHAT_Tx_Packet;  // Make a temporary transmit packet
                  _WHAT_Tx_Packet.WHAT_MAC_MESSAGE;
                  _WHAT_Tx_Packet.pan_id = _Offered_PANID;
                  _WHAT_Tx_Packet.sourceaddress = _Offered_SourceAddress;
                  myParentAddress = _Offered_Parent;
                  myLatchKeyNumber = (uint16_t)millis();  //Create a random latch-key number
                  //myChildIndex = 0;   // Just in case for some reason this was not already 0 (like if this device was removed from another network).
                  // Now send a message to confirm which parent was chosen.
                  //ACCEPT_PARENT_06
                  _WHAT_Tx_Packet.pan_id = myPANID;
                  _WHAT_Tx_Packet.sourceaddress = mySourceAddress;
                  _WHAT_Tx_Packet.destination = myParentAddress;   // Direct this message at the parent that accepted it.
                  // ACCEPT_PARENT_06         0xF005 // 1 byte length, 2 byte command, 2 byte parent, 2 byte latchkey, 1 byte relation type // preemie accepts offer from parent, and proposes latch key value (derived from its timer upon sending)
                  uint8_t _index = 0;
                  _WHAT_Tx_Packet.payload[_index++] = 7; //Total payload length (including this byte)
                  _WHAT_Tx_Packet.payload[_index++] = 6; //MID length (including this byte)
                  _WHAT_Tx_Packet.payload[_index++] = highByte(ACCEPT_PARENT_06);   // 1 byte length, 2 byte command, 2 byte child address, 2 byte latchkey // Any device that has open child slots replies to request (REQUEST_JOIN_NETWORK_02).
                  _WHAT_Tx_Packet.payload[_index++] = lowByte(ACCEPT_PARENT_06);  //in order to support multibyte MIDs, put the high byte first.
                  _WHAT_Tx_Packet.payload[_index++] = highByte(myLatchKeyNumber); //in order to support multibyte MIDs, put the high byte first.
                  _WHAT_Tx_Packet.payload[_index++] = lowByte(myLatchKeyNumber);
                  _WHAT_Tx_Packet.payload[_index++] = WHAT_PERMANENT;           // All devices are permanent at this point.
                      WHAT_TransmitPacket(_WHAT_Tx_Packet);
    
                  //_WHAT_network_State = WHAT_PRE_NET_ALIVE;
                  WHAT_StoreEEPROM();
                  Serial.println(F("Tx Parent Chosen"));
                  Serial.println(F("PANID\tmySourceAddress\tmyLatchKeyNumber\tmyParentAddress"));
                  Serial.print(myPANID,HEX);Serial.print("\t");Serial.print(mySourceAddress,HEX);Serial.print("\t");Serial.print(myLatchKeyNumber,HEX);Serial.print("\t");Serial.println(myParentAddress,HEX);
                  //_WHAT_NetworkButton = LOW;   // indicate that the button is used so it will not roll into the next function.
                  _WHAT_network_State = WHAT_PRE_CONFIRM_NEW_CHILD;  // Network light to indicate someone else's network was sucessfully joined.

                }
                else
                { // No child address was offered. re-start the preemie process.
                  //Serial.println(F("No Source Address Offered"));
                    _WHAT_network_State = WHAT_STARTUP;  //blip of red indication network was not joined.
              #ifdef CHILD_MONITOR
                //Serial.print(F("Offered PANID:0x")); Serial.println(_Offered_PANID);
                Serial.print(F("No child address offered: ")); Serial.println(millis());
              #endif
                }
              }
              //else {Serial.println(WHAT_Reusable_Timer1 - millis());}
         _Control_Net_Light(/*LightColor1*/ WHAT_LIGHT_BLUE,/*Light Color 1 time*/ 10,/*LightColor2 */ WHAT_LIGHT_OFF,/*Light Color 2 time*/ 200 );
  
      }// break;

      // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  
      if(_WHAT_network_State == WHAT_PRE_CONFIRM_NEW_CHILD )//       case WHAT_PRE_CONFIRM_NEW_CHILD:
      {
           default_case = false;  // Used to verify that one of the cases executed.
          WHATRxMessage[WHAT_Reusable_Message1].MessageID = NETWORK_JOIN_COMPLETE_03;
          WHATRxMessage[WHAT_Reusable_Message1].callback = &Rx_NETWORK_JOIN_COMPLETE_03; // Message callback set here to collect the responses
          _WHAT_network_StateColor = WHAT_LIGHT_OFF;
           f_Rx_NETWORK_JOIN_COMPLETE_03 = false;    // Clear the message flag so only new messages will be used.
        WHAT_Reusable_Timer1 <= millis() + 1000;
        _WHAT_network_State = WHAT_CONFIRM_NEW_CHILD;  //blip of red indication network was not joined.

      }//   break;
      // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  
      if(_WHAT_network_State == WHAT_CONFIRM_NEW_CHILD )//      case WHAT_CONFIRM_NEW_CHILD:
      {
           default_case = false;  // Used to verify that one of the cases executed.
          if (WHAT_Reusable_Timer1 <= millis()) //Wait a short time after permitted to join to get more proposed addresses.
          {
            _WHAT_network_State = WHAT_PRE_NETWORK_ABORT;  //blip of red indication network was not joined.
            #ifdef CHILD_MONITOR
            //Serial.print(F("Offered PANID:0x")); Serial.println(_Offered_PANID);
            Serial.print(F("Join timed out : ")); Serial.println(millis());
            #endif
          }
        // If an abort message is recieved, return to the beginning.
          if(f_Rx_NETWORK_JOIN_ABORT_03) 
          {
            f_Rx_NETWORK_JOIN_ABORT_03 = false; // clear the message flag when it is used.
            //_WHAT_network_State = WHAT_PRE_REQUEST_JOIN_NETWORK_03; // If nothing offered a child slot, return to being a preemie.
            _WHAT_network_State = WHAT_PRE_NETWORK_ABORT;  //blip of red indication network was not joined.
            #ifdef CHILD_MONITOR
            //Serial.print(F("Offered PANID:0x")); Serial.println(_Offered_PANID);
            Serial.print(F("Join Abort : ")); Serial.println(millis());
           #endif
          }
          if(f_Rx_NETWORK_JOIN_COMPLETE_03)
          {
            WHAT_Reusable_Timer1 = millis() + 1000;     // Just hold the light red for 1 second to indicate the network join aborted.
            Serial.print(F("Network join confirmed ")); Serial.println(millis());
          // _WHAT_network_StateColor = WHAT_LIGHT_GREEN;    
          _Control_Net_Light(/*LightColor1*/ WHAT_LIGHT_GREEN,/*Light Color 1 time*/ 50,/*LightColor2 */ WHAT_LIGHT_GREEN,/*Light Color 2 time*/ 50 );
          _WHAT_network_State = WHAT_NEW_CHILD_CONFIRMED;  //blip of red indication network was not joined.
           
          }
      }//    break;
      // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  

      if(_WHAT_network_State == WHAT_NEW_CHILD_CONFIRMED )//      case WHAT_NEW_CHILD_CONFIRMED:
      {
           default_case = false;  // Used to verify that one of the cases executed.
         if (WHAT_Reusable_Timer1 <= millis()) //Wait a short time after permitted to join to get more proposed addresses.
          {
           //_WHAT_network_StateColor = WHAT_LIGHT_OFF;
           _Control_Net_Light(/*LightColor1*/ WHAT_LIGHT_OFF,/*Light Color 1 time*/ 50,/*LightColor2 */ WHAT_LIGHT_OFF,/*Light Color 2 time*/ 50 );
    
            _WHAT_network_State = WHAT_STARTUP;  //blip of red indication network was not joined.
            //#ifdef CHILD_MONITOR
            //Serial.print(F("Offered PANID:0x")); Serial.println(_Offered_PANID);
            //#endif
          }
      }//  break;

      // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  
      if(_WHAT_network_State == WHAT_PRE_NETWORK_ABORT )//       case WHAT_PRE_NETWORK_ABORT:
      {
           default_case = false;  // Used to verify that one of the cases executed.
           WHAT_Reusable_Timer1 = millis() + 1000;     // Just hold the light red for 1 second to indicate the network join aborted.
          // _WHAT_network_StateColor = WHAT_LIGHT_RED;    
            _Control_Net_Light(/*LightColor1*/ WHAT_LIGHT_RED,/*Light Color 1 time*/ 50,/*LightColor2 */ WHAT_LIGHT_RED,/*Light Color 2 time*/ 50 );
           _WHAT_network_State = WHAT_NETWORK_ABORT;  //blip of red indication network was not joined.

       }// break;

      // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  
      if(_WHAT_network_State == WHAT_NETWORK_ABORT )//      case WHAT_NETWORK_ABORT:
      {
           default_case = false;  // Used to verify that one of the cases executed.
        if (WHAT_Reusable_Timer1 <= millis()) //Wait a short time after permitted to join to get more proposed addresses.
          {
            //_WHAT_network_StateColor = WHAT_LIGHT_OFF;   
            _Control_Net_Light(/*LightColor1*/ WHAT_LIGHT_OFF,/*Light Color 1 time*/ 50,/*LightColor2 */ WHAT_LIGHT_OFF,/*Light Color 2 time*/ 50 );

            _WHAT_network_State = WHAT_STARTUP;
          }
      }//  break;

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
// TOC    Normal Adult operations 
      if(_WHAT_network_State == WHAT_PRE_NET_ALIVE )//       case WHAT_PRE_NET_ALIVE:
      {
           default_case = false;  // Used to verify that one of the cases executed.
              // Use the reusable message to respond to ACK requests.
              WHATRxMessage[WHAT_Reusable_Message1].MessageID = REQUEST_ACK_03;     // destination requested to reply with ACK.
              WHATRxMessage[WHAT_Reusable_Message1].callback = &Rx_REQUEST_ACK_03; // Message callback set here to collect the responses
              _WHAT_network_StateColor = WHAT_LIGHT_OFF;
               f_Rx_REQUEST_ACK_03 = false;          // Clear the message flag so only new messages will be used.
              f_Rx_REQUEST_JOIN_NETWORK_03 = false; // Clear the message flag so only new messages will be used.
              #ifdef __WHAT_FULLLINK_h
                f_Rx_REQUEST_FULL_LINK_03 = false; // Mark the reception flag as used so it is not duplicated.
              #endif
              _WHAT_network_State = WHAT_NET_ALIVE;
 
              #ifdef CHILD_MONITOR
               Serial.print(F("_WHAT_network_StateColor:")); Serial.println(_WHAT_network_StateColor,DEC);
               Serial.print(F("_WHAT_network_State: WHAT_NET_ALIVE ")); Serial.println(_WHAT_network_State);
              #endif
      }//  break;
/**********************************************************************************/

       if(_WHAT_network_State == WHAT_NET_ALIVE )//      case WHAT_NET_ALIVE:
       {
           default_case = false;  // Used to verify that one of the cases executed.
          // If no network activity is currently underway, monitor network button interactions.
            //#define Network_held WHAT_Reusable_Timer2  //This will not be active while other CASEs are active.
            if (f_Rx_REQUEST_JOIN_NETWORK_03)   // A preemie is attempting to join the network.
            {
                f_Rx_REQUEST_JOIN_NETWORK_03 = false; // Mark the reception flag as used so it is not duplicated.
                _WHAT_network_State = WHAT_PRE_NEW_CHILD_REQ;
                Serial.print(F("_WHAT_network_State: WHAT_PRE_NEW_CHILD_REQ ")); Serial.println(_WHAT_network_State);
            }
       #ifdef __WHAT_FULLLINK_h
            if (_WHAT_NetworkButton == HIGH)   // Holding the network button will attempt to "link" this device with another device
            {
                _WHAT_network_State = WHAT_LINKING; // A holding routine for a linking sequence. Dont do anything else while linking.
                  // This is aslo the ONLY way to get into Linking: Must have a source address<0xFFFF, and must be in "Alive" and the Network button pressed.
            }
            if(f_Rx_REQUEST_FULL_LINK_03)
            {
                _WHAT_network_State = WHAT_LINKING; // A holding routine for a linking sequence. Dont do anything else while linking.
            }
            if(f_Rx_REMOVE_FULL_LINK_03)
            {
                _WHAT_network_State = WHAT_LINKING; // A holding routine for a linking sequence. Dont do anything else while linking.
            }
       #endif
            if (f_Rx_REQUEST_ACK_03)      // A preemie is trying to form a network using THIS network's PANID
            {
                f_Rx_REQUEST_ACK_03 = false; // Mark the reception flag as used so it is not duplicated.
                Tx_REPLY_ACKNOWLEDGE_03(); 
                  // If a device is asking for verification that this network is alive, this is a reply to that request.
            }
            // *****************

           if(f_gbl_t_NetworkAlive )     // Flag that no other network nodes have been seen
           {
            // While in the alive state, blink the light every few seconds.
           _Control_Net_Light(/*LightColor1*/ WHAT_LIGHT_BLUE,/*Light Color 1 time*/ 100,/*LightColor2 */ WHAT_LIGHT_OFF,/*Light Color 2 time*/ 5000 );
           }
           else
           {
           _Control_Net_Light(/*LightColor1*/ WHAT_LIGHT_GREEN,/*Light Color 1 time*/ 100,/*LightColor2 */ WHAT_LIGHT_OFF,/*Light Color 2 time*/ 5000 );
           }
  
      }//break;
              

// ************************************************************************
// ************************************************************************
// ************************************************************************
// ************************************************************************
// TOC    Child requesting addition to network 
       if(_WHAT_network_State == WHAT_PRE_NEW_CHILD_REQ )//    case WHAT_PRE_NEW_CHILD_REQ:
       {
           default_case = false;  // Used to verify that one of the cases executed.
          f_Rx_REQUEST_JOIN_NETWORK_03 = false;
            WHAT_Packet _WHAT_Tx_Packet;    // Create a temporary transmit packet.
            _WHAT_Tx_Packet.WHAT_MAC_MESSAGE;
            _WHAT_Tx_Packet.pan_id = myPANID;
            _WHAT_Tx_Packet.destination = 0xFFFF;   // Send this to all devices.
            _WHAT_Tx_Packet.sourceaddress = mySourceAddress;  // Use the source address of this device.
            uint8_t _index = 0;
            _WHAT_Tx_Packet.payload[_index++] = 4; //Total payload length (including this byte)
            _WHAT_Tx_Packet.payload[_index++] = 3; //MID length (including this byte)
            _WHAT_Tx_Packet.payload[_index++] = highByte(NETWORK_ALERT_PREEMIE_03);
            _WHAT_Tx_Packet.payload[_index++] = lowByte(NETWORK_ALERT_PREEMIE_03);
          WHAT_TransmitPacket(_WHAT_Tx_Packet);
        WHAT_Reusable_Timer1 = millis() + _WHAT_NEW_CHILD_TIME_DELAY; // allow 30 seconds for a network button to be pressed.
       _WHAT_network_State = WHAT_NEW_CHILD_REQ;  // In alll cases change to the new state.
        Serial.print(F("_WHAT_network_State: WHAT_NEW_CHILD_REQ ")); Serial.println(_WHAT_network_State);
       }//break;

// ************************************************************************
      if(_WHAT_network_State == WHAT_NEW_CHILD_REQ )//   case WHAT_NEW_CHILD_REQ:
      {
           default_case = false;  // Used to verify that one of the cases executed.
        // A preemie is trying to join this network.
        // If the network button on this device is pressed, notify the network and child that
        // the operator is allowing it to join.  Also propose a child address (if Available).

        _Control_Net_Light(/*LightColor1*/ WHAT_LIGHT_BLUE,/*Light Color 1 time*/ 500,/*LightColor2 */ WHAT_LIGHT_GREEN,/*Light Color 2 time*/ 100 );
        //Serial.print(F("_WHAT_network_State: WHAT_NEW_CHILD_REQ ")); Serial.println(_WHAT_network_State);
        //_WHAT_network_StateColor = WHAT_LIGHT_GREEN;
         if (_WHAT_NetworkButton == HIGH) // Button is active high.
         {
             WHAT_Packet _WHAT_Tx_Packet;    // Create a temporary transmit packet.
            _WHAT_Tx_Packet.WHAT_MAC_MESSAGE;
            _WHAT_Tx_Packet.pan_id = myPANID;
            _WHAT_Tx_Packet.destination = 0xFFFF;   // Send this to all devices.
            _WHAT_Tx_Packet.sourceaddress = mySourceAddress;  // Use the source address of this device.
            uint8_t _index = 0;
            _WHAT_Tx_Packet.payload[_index++] = 4; //Total payload length (including this byte)
            _WHAT_Tx_Packet.payload[_index++] = 3; //MID length (including this byte)
            _WHAT_Tx_Packet.payload[_index++] = highByte(NETWORK_ALLOW_PREEMIE_04);
            _WHAT_Tx_Packet.payload[_index++] = lowByte(NETWORK_ALLOW_PREEMIE_04);
            _WHAT_Tx_Packet.payload[_index++] = 1;  // This is a value for confirming allowance.
          WHAT_TransmitPacket(_WHAT_Tx_Packet);

            Serial.print(F("_WHAT_NetworkButton Pressed: ")); Serial.println(_WHAT_NetworkButton);
         }
        
         if ((f_Rx_NETWORK_ALLOW_PREEMIE_04) |  (_WHAT_NetworkButton == HIGH))
         {
           // if THIS device or another device allowed a child to join, change to that state.
           _WHAT_Child_Used = isChildAvailable(); // Check and see if a Child location if available.
             if(_WHAT_Child_Used < 0xFF)
             {
             _WHAT_network_State = WHAT_PRE_NEW_CHILD_OFFER; // 
             }
             else
             {
              Serial.println(F("Child not available to offer"));
              _WHAT_network_State = WHAT_PRE_NEW_CHILD_REQ; // 
             }
         }
         if(f_Rx_REQUEST_JOIN_NETWORK_03)
         {
           Serial.println(F("Preemie is still requesting join.  reTx allow."));
          _WHAT_network_State = WHAT_PRE_NEW_CHILD_REQ;
         }
    // TOC    Network identified, waiting to be allowed
         if (WHAT_Reusable_Timer1 <= millis()) //Timer set when a child request is seen.  there is 30seconds to connect.
         { // If the timer expires while in this state, return to normal operations.
           Serial.println(F("\n Allow timer expired"));
           _WHAT_network_State = WHAT_PRE_NET_ALIVE; // Reset to the "normal" state when the timer expires.
         }
      }//break;

// ************************************************************************
      if(_WHAT_network_State == WHAT_PRE_NEW_CHILD_OFFER )//    case WHAT_PRE_NEW_CHILD_OFFER:
      {
            default_case = false;  // Used to verify that one of the cases executed.
         // If this device is already a member of the network,
          //and this or some other device has indicated that it sees the new child, offer an address for the new child.
          WHATRxMessage[WHAT_Reusable_Message1].MessageID = ACCEPT_PARENT_06;   // This message comes once the network button is pressed.
          WHATRxMessage[WHAT_Reusable_Message1].callback = &Rx_ACCEPT_PARENT_06; // Message callback set here to collect the responses
          _WHAT_network_StateColor = WHAT_LIGHT_OFF;
 
          f_Rx_ACCEPT_PARENT_06 = false;   // Clear the message reception flag.
          Serial.println(F("Offering child"));
          // Inform the network that this device has a child spot open while a preemie is trying to join.
          f_Rx_NETWORK_ALLOW_PREEMIE_04 = false; // clear the message flag when it is used.
          WHAT_Reusable_Timer1 = millis() + _WHAT_NEW_CHILD_TIME_DELAY;  //This state will only remain active a short time.
          // Send out a proposal for a child
          Tx_PROPOSE_CHILD_NUMBER_05();
          // There are seperate state machines for preemies and members, each handle the NETWORK_ALLOW_PREEMIE_04 state, but in different state machines.
          _WHAT_network_State = WHAT_NEW_CHILD_OFFER; // Wait to see if child accepts offer.
      }//break;

 // ************************************************************************ 
      if(_WHAT_network_State == WHAT_NEW_CHILD_OFFER )//     case WHAT_NEW_CHILD_OFFER:
      {
           default_case = false;  // Used to verify that one of the cases executed.
// TOC    New child allowed to be added to network 
      // The network has approved for the new child to be added and THIS device has an available child slot.
      // A proposed child address was sent.  The child will now decide which parent it wants.
      
      _Control_Net_Light(/*LightColor1*/ WHAT_LIGHT_GREEN,/*Light Color 1 time*/ 500,/*LightColor2 */ WHAT_LIGHT_BLUE,/*Light Color 2 time*/ 100 );

      if (WHAT_Reusable_Timer1 <= millis()) //Timer set when a network button was pressed.  there is 30seconds to connect.
        {
            Serial.println(F("New Child Timeout"));
            _WHAT_network_State = WHAT_PRE_NET_ALIVE; // Reset to the "normal" state when the timer expires.
        }
        if(f_Rx_ACCEPT_PARENT_06)
        {
          f_Rx_ACCEPT_PARENT_06 = false;    // clear the message flag when it is used.
          // whether the device accepted THIS device as a parent or not, indicate that this state is completed.
          Serial.println(F("New Child Accepted"));
            //Serial.println(F("Child accepted."));
            //Serial.println(F("PANID\tMyChild\tmyLatchkey\tmyRelationship"));
            //Serial.print(myPANID);Serial.print(myChildren[_myChild_available_Index].ChildAddress);Serial.print(myChildren[_myChild_available_Index].LatchKeyValue);Serial.println(myChildren[_myChild_available_Index].RelationshipType);
             WHAT_Packet _WHAT_Tx_Packet;    // Create a temporary transmit packet.
            _WHAT_Tx_Packet.WHAT_MAC_MESSAGE;
            _WHAT_Tx_Packet.pan_id = myPANID;
            _WHAT_Tx_Packet.destination = 0xFFFF;   // Send this to all devices.
            _WHAT_Tx_Packet.sourceaddress = mySourceAddress;  // Use the source address of this device.
             uint8_t _index = 0;
             _WHAT_Tx_Packet.payload[_index++] = 4; //Total payload length (including this byte)
             _WHAT_Tx_Packet.payload[_index++] = 3; //MID length (including this byte)
             _WHAT_Tx_Packet.payload[_index++] = highByte(NETWORK_JOIN_COMPLETE_03);
            _WHAT_Tx_Packet.payload[_index++] = lowByte(NETWORK_JOIN_COMPLETE_03);
           WHAT_TransmitPacket(_WHAT_Tx_Packet);
           Serial.print(F("\nBirth Announcement: "));
           WHAT_Reusable_Timer1 <= millis() + 2000;
          _WHAT_network_State = WHAT_NEW_CHILD_CONFIRMED; // return to normal light operation.
        }
     
      }//break;

// ************************************************************************
      if(_WHAT_network_State == WHAT_PRE_NETWORK_ERROR )//     case WHAT_PRE_NETWORK_ERROR:
      {
            default_case = false;  // Used to verify that one of the cases executed.
       // Something unexpected happened.  Flash the red light for a bit, then return to preemie check.
          #ifdef CHILD_MONITOR
          Serial.println(F("WHAT Network Error: ")); 
          #endif
          WHAT_Reusable_Timer1 = millis() + 5000; // Keep it green for several seconds.
          _WHAT_network_State = WHAT_NETWORK_ERROR;  //blip of red indication network was not joined.
          //_WHAT_network_StateColor = WHAT_LIGHT_RED;    
          _Control_Net_Light(/*LightColor1*/ WHAT_LIGHT_RED,/*Light Color 1 time*/ 50,/*LightColor2 */ WHAT_LIGHT_RED,/*Light Color 2 time*/ 50 );

      }// break;
// ************************************************************************
      if(_WHAT_network_State == WHAT_NETWORK_ERROR )//     case WHAT_NETWORK_ERROR:
      {
           default_case = false;  // Used to verify that one of the cases executed.
        // Something unexpected happened.  Flash the red light for a bit, then return to preemie check.
              if (WHAT_Reusable_Timer1 <= millis()) //Timer set when a network button was pressed.  there is 30seconds to connect.
              {
                  #ifdef CHILD_MONITOR
                  Serial.println(F("Returning to Startup. ")); 
                  #endif
                _WHAT_network_State = WHAT_STARTUP;  //After the red light expires, return to the startup state.
              }
        }//break;
// ************************************************************************
        if(_WHAT_network_State == WHAT_LINKING )//     case WHAT_LINKING:
        {
           default_case = false;  // Used to verify that one of the cases executed.
          #ifdef __WHAT_FULLLINK_h
            WHAT_LinkCheck();
            if(_WHAT_LINK_State == WHAT_NO_LINK)
            {
              _WHAT_network_State = WHAT_NET_ALIVE;
            }
          #endif
          #ifndef __WHAT_FULLLINK_h
            _WHAT_network_State = WHAT_STARTUP;
          #endif
        }//break;

     if(default_case == true)
     {
       //Serial.print(F("Unhandled PREEMIE state:"));Serial.println(_WHAT_network_State);
       // If in an invalid state, revert to being a preemie.
         #ifdef CHILD_MONITOR
         //Serial.print(F("Offered PANID:0x")); Serial.println(_Offered_PANID);
         Serial.print(F("Undefined state : ")); Serial.println(_WHAT_network_State,DEC);
       #endif
       //_WHAT_network_State = WHAT_PRE_REQUEST_JOIN_NETWORK_03; // If nothing offered a child slot, return to being a preemie.
       _WHAT_network_State = WHAT_PRE_NETWORK_ERROR;  //blip of red indication network was not joined.
      }// break;

 //  }// End Switch
      
  // //Look for other messages of interest:
  // if(f_Rx_REQUEST_JOIN_NETWORK_03)
  // {
  //   f_Rx_REQUEST_JOIN_NETWORK_03 = false; // clear the message flag when it is used.
  //   // Send Network Alert.
  //   WHAT_Packet _WHAT_Tx_Packet;  // Make a temporary transmit packet
  //   _WHAT_Tx_Packet.WHAT_MAC_MESSAGE;
  //   // In all cases, if a child address is being proposed, also inform the network that a preemie wants to join.
  //   // In addition to proposing a child, tell the entire network that a child is trying to join:
  //   _WHAT_Tx_Packet.pan_id = myPANID;
  //   _WHAT_Tx_Packet.destination = 0xFFFF;   // Send this to all devices.
  //   _WHAT_Tx_Packet.sourceaddress = mySourceAddress;  // Use the source ddress of this device.
  //   uint8_t _index = 0;
  //   _WHAT_Tx_Packet.payload[_index++] = 5; //Total payload length (including this byte)
  //   _WHAT_Tx_Packet.payload[_index++] = 4; //MID length (including this byte)
  //   _WHAT_Tx_Packet.payload[_index++] = highByte(NETWORK_ALERT_PREEMIE_03);
  //   _WHAT_Tx_Packet.payload[_index++] = lowByte(NETWORK_ALERT_PREEMIE_03);
  //    WHAT_TransmitPacket(_WHAT_Tx_Packet);
     
   
  //  }// end if my proposed address is valid.

  //  if(f_Rx_NETWORK_ALERT_PREEMIE_03)
  //  {
  //     f_Rx_NETWORK_ALERT_PREEMIE_03 = false; // clear the message flag when it is used.
  //     // The network has given an alert that a preemie is trying to join the network.
  //     // During that time, if this device address is <0x0100 it will monitor its network button
  //     // and send an acceptance signal if it is pressed.
  //     //Serial.println(F("Rx_NETWORK_ALERT_PREEMIE_03"));
  //     //WHAT_NETWORK_SEEN = true;
  //     WHAT_Reusable_Timer1 = millis() + _RQST_WAIT_TIME; // Once a network is seen, slow the rate of requests.
  //     //_WHAT_network_State = WHAT_NEW_CHILD_REQ;
  //     _WHAT_network_State = WHAT_NETWORK_CREATING;
  //  }
  // if(f_Rx_CHOOSE_NETWORK_05)
  // {// When a preemie chooses a network, if it is not THIS network, revert to finished state.
  //     //Serial.println(F("Rx_CHOOSE_NETWORK_05"));
  //   f_Rx_CHOOSE_NETWORK_05 = false; // clear the message flag when it is used.
  //   if (_Offered_PANID != myPANID) //Only respond if mature and if the given PANID does not match this one.
  //     {
  //         _WHAT_network_State = WHAT_PRE_NET_ALIVE; // Return to normal if a different PANID is used.
  //                WHAT_Reusable_Timer2 = millis()+ NETWORK_HELD_DELAY; // Reset the network held timer
  //     }
  // }
  //  if(f_Rx_NETWORK_JOIN_COMPLETE_03)
  //  {
  //     //Serial.println(F("Rx_NETWORK_JOIN_COMPLETE_03"));
  //    f_Rx_NETWORK_JOIN_COMPLETE_03 = false; // clear the message flag when it is used.
  //    _WHAT_network_State = WHAT_PRE_NET_ALIVE; // Return to normal if the connection is aborted.
  //    WHAT_Reusable_Timer2 = millis()+ NETWORK_HELD_DELAY; // Reset the network held timer
  //    //_WHAT_network_State = WHAT_NETWORK_CREATED;
  //    //Setup_WHAT_Show_Connection();
  //  }
  //   // If an abort message is recieved, return to the beginning.
  //   if(f_Rx_NETWORK_JOIN_ABORT_03) 
  //   {
  //       //Serial.println(F("Rx_NETWORK_JOIN_ABORT_03"));
  //     f_Rx_NETWORK_JOIN_ABORT_03 = false; // clear the message flag when it is used.
  //     _WHAT_network_State = WHAT_PRE_NET_ALIVE; // Return to normal if the connection is aborted.
  //    WHAT_Reusable_Timer2 = millis()+ NETWORK_HELD_DELAY; // Reset the network held timer
  //     _WHAT_network_State = WHAT_NETWORK_NONE;
  //   }
} // State Machine

 
uint8_t  isChildAvailable(void)
{    
    uint8_t _blankspot = 0xFF;
    uint8_t _WHAT_Child_Empty = 0x0;
   // Child is being requested. see if there is an available slot..
    for (uint8_t _i=0;_i<ChildMaxIndex;_i++)
    {
      #ifdef CHILD_MONITOR
        //Serial.println(myChildren[_i].ChildAddress,HEX);
      #endif
      if(myChildren[_i].ChildAddress == _Offered_SourceAddress) _WHAT_Child_Empty = _i; // This is already a child of this parent.
      if((_blankspot == 0xFF) & (myChildren[_i].ChildAddress == 0xFFFF)) _blankspot = _i; // Keep track of the first blank spot
    }
    if(_WHAT_Child_Empty == 0){ _WHAT_Child_Empty = _blankspot;} // if not already linked, use the blank spot.
    // Now "_WHAT_Child_Used" = either the location of previous link, an empty spot, or 0xFF if neither found,
  #ifdef CHILD_MONITOR
  Serial.print(F("_WHAT_Child_Used value: "));Serial.println(_WHAT_Child_Empty);
  #endif
  return _WHAT_Child_Empty;
}

 // *****************
// TOC Individual Recieve messages
void Rx_NETWORK_ALERT_PREEMIE_03 (WHAT_Packet _WHAT_Rx_Packet, uint8_t _index)
{
  //Serial.println(F("Rx_NETWORK_ALERT_PREEMIE_03"));
  f_Rx_NETWORK_ALERT_PREEMIE_03 = true;   // Notice from another device that a preemie is trying to join
}

// TOC Individual Recieve messages 
// WHAT_Reusable_Message1 
void Rx_NETWORK_ALLOW_PREEMIE_04(WHAT_Packet _WHAT_Rx_Packet, uint8_t _index)
{
// Rx Message flags
  _Offered_PANID = _WHAT_Rx_Packet.pan_id;  //Only accepting PANID from the PANID that sent the allowance indication.
  f_Rx_NETWORK_ALLOW_PREEMIE_04 = true;       // While a new network is forming, requests existing network to claim its PANID
    #ifdef CHILD_MONITOR
    Serial.print(F("Recieve acceptance of child from :0x")); Serial.println(_WHAT_Rx_Packet.sourceaddress);
    //Serial.print(F("Undefined Preemie state : ")); Serial.println(_WHAT_network_State,HEX);
    #endif
}

// *****************
// TOC Individual Recieve messages
// WHAT_Reusable_Message1
void Rx_PROPOSE_CHILD_NUMBER_05(WHAT_Packet _WHAT_Rx_Packet, uint8_t _index)
{ //This device received a proposed child address.
  // If this device has been accepted to a network, keep the lowest numbered proposal.
  f_Rx_PROPOSE_CHILD_NUMBER_05 = true;    // Preemie asking the network for a child number
  //Serial.println(F("Rx_PROPOSE_CHILD_NUMBER_05"));
    if (((_WHAT_Rx_Packet.payload[_index + 3] * 0x100) + (_WHAT_Rx_Packet.payload[_index + 4])) < _Offered_SourceAddress)
    { // Keep the lowest offered child address.
      if( _Offered_PANID == _WHAT_Rx_Packet.pan_id)  //Only accepting PANID from the PANID that sent the allowance indication.
      {
        //_Offered_PANID = _WHAT_Rx_Packet.pan_id;  //Only accepting PANID from the PANID that sent the allowance indication.
        _Offered_SourceAddress = ((_WHAT_Rx_Packet.payload[_index + 3] * 0x100) + _WHAT_Rx_Packet.payload[_index + 4]);
        //_Offered_Latchkey = ((_WHAT_Rx_Packet.payload[_index + 5] * 0x100) + _WHAT_Rx_Packet.payload[_index + 6]);
        _Offered_Parent = WHAT_Rx_Packet.sourceaddress;
        //Serial.print(F("New offered ID: 0x")); Serial.println(_Offered_SourceAddress);
      }
    }
}

// *****************
// TOC Individual Recieve messages
void Rx_ACCEPT_PARENT_06(WHAT_Packet _WHAT_Rx_Packet, uint8_t _index)// 1 byte length, 2 byte command, 2 byte parent, 2 byte latchkey, 1 byte relation type // preemie accepts offer from parent, and proposes latch key value (derived from its timer upon sending)
{
    uint16_t _Rx_ParentAddress = ((_WHAT_Rx_Packet.payload[_index + 3] * 0x100) + _WHAT_Rx_Packet.payload[_index + 4]);
    uint16_t _Rx_Offered_Latchkey = ((_WHAT_Rx_Packet.payload[_index + 5] * 0x100) + _WHAT_Rx_Packet.payload[_index + 6]);
    if (_Rx_ParentAddress == mySourceAddress) 
    { // Child selected THIS device as a parent.
      f_Rx_ACCEPT_PARENT_06 = true;   // Indicate that this message was recieved.
        // Verify the child address matches an open child slot:
      //The algorithn is a little confusing.  Offered child addresses use the address of this device with the child index as the next higher nibble.
      // This requires pulling out the address of THIS device (Parent), and rotating the recieved address so only the index remains.
      // This can be done with rotation.
      //    So "If mySourceAddress == 0x0000, then myChildIndex must not move (meaning the proposed address is 0x000C) (where C is the child index)
      //       "If mySourceAddress >0x0000, then myChildIndex must move right 1 nibble (rotate right 4 times)(meaning the proposed address is 0x00Cp) (where C is the child index, and p is the parent address)
      //       "If mySourceAddress >0x000F, then myChildIndex must move left 2 nibbles (rotate right 8 times)(meaning the proposed address is 0x0C0p) (where C is the child index, and p is the parent address)
      //       "If mySourceAddress >0x00FF, then myChildIndex must move left 3 nibbles (rotate right 12 times)(meaning the proposed address is 0xC00p) (where C is the child index, and p is the parent address)
      // At the end of this process, rotate the recieved source address to the least significant nibble location.
      uint8_t _myChild_available_Index = _WHAT_Rx_Packet.sourceaddress >> ( ((mySourceAddress > 0x0) * 4) + ((mySourceAddress > 0x0F) * 4) + ((mySourceAddress > 0x0FF) * 4) );
      
      //and make sure that index does not already have a child in it, then fill it with this child.... 
      if(myChildren[_myChild_available_Index].ChildAddress == 0xFFFF)
      {
        myChildren[_myChild_available_Index].ChildAddress = _WHAT_Rx_Packet.sourceaddress;
        myChildren[_myChild_available_Index].LatchKeyValue = ((_WHAT_Rx_Packet.payload[_index+4]*0x100) +_WHAT_Rx_Packet.payload[_index+5]);
        myChildren[_myChild_available_Index].RelationshipType = _WHAT_Rx_Packet.payload[_index+6];
        Serial.print(F("Child accepted.  Index:"));Serial.println(_myChild_available_Index);
        Serial.println(F("PANID\tMyChild\tmyLatchkey\tmyRelationship"));
        Serial.print(myPANID,HEX);Serial.print(F("\t"));Serial.print(myChildren[_myChild_available_Index].ChildAddress,HEX);Serial.print(F("\t"));Serial.print(myChildren[_myChild_available_Index].LatchKeyValue,HEX);Serial.print(F("\t"));Serial.println(myChildren[_myChild_available_Index].RelationshipType,HEX);

      // Indicate that the parent accepts the child also!
      // #define NETWORK_JOIN_COMPLETE_03    0xF020   // 1 byte length, 2 byte command  // Sent by new parent (Birth Announcement)
        WHAT_Packet _WHAT_Tx_Packet;  // Make a temporary transmit packet
        _WHAT_Tx_Packet.WHAT_MAC_MESSAGE;
        _WHAT_Tx_Packet.pan_id = myPANID;
        _WHAT_Tx_Packet.destination = myChildren[_myChild_available_Index].ChildAddress;   // Send this to the child.
        _WHAT_Tx_Packet.sourceaddress = mySourceAddress;  // Use the source address of this device.
        uint8_t _index = 0;
        _WHAT_Tx_Packet.payload[_index++] = 4; //include the length of the proposed child address.
        _WHAT_Tx_Packet.payload[_index++] = 3; //MID size (for the 2nd message).
        _WHAT_Tx_Packet.payload[_index++] = highByte(NETWORK_JOIN_COMPLETE_03);// 1 byte length, 2 byte command, 2 byte child address, 2 byte latchkey // Any device that has open child slots replies to request (REQUEST_JOIN_NETWORK_02).
        _WHAT_Tx_Packet.payload[_index++] = lowByte(NETWORK_JOIN_COMPLETE_03); //in order to support multibyte MIDs, put the high byte first.
         WHAT_TransmitPacket(_WHAT_Tx_Packet);

          WHAT_StoreEEPROM();
      }
      else
      {
        Serial.println(F("Child slot was already taken... Child NOT added"));

        WHAT_Packet _WHAT_Tx_Packet;  // Make a temporary transmit packet
        _WHAT_Tx_Packet.WHAT_MAC_MESSAGE;
        _WHAT_Tx_Packet.pan_id = myPANID;
        _WHAT_Tx_Packet.destination = 0xFFFF;   // Send this to all devices.
        _WHAT_Tx_Packet.sourceaddress = mySourceAddress;  // Use the source ddress of this device.
        uint8_t _index = 0;
        _WHAT_Tx_Packet.payload[_index++] = 6; //Total payload length (including this byte)
        _WHAT_Tx_Packet.payload[_index++] = 5; //MID length (including this byte)
        _WHAT_Tx_Packet.payload[_index++] = highByte(ABANDON_CHILD_05);
        _WHAT_Tx_Packet.payload[_index++] = lowByte(ABANDON_CHILD_05);
        _WHAT_Tx_Packet.payload[_index++] = _WHAT_Rx_Packet.payload[_index+4];  // Send back the Latchkey ID as message confirmation
        _WHAT_Tx_Packet.payload[_index++] = _WHAT_Rx_Packet.payload[_index+5];  // Send back the Latchkey ID as message confirmation
            WHAT_TransmitPacket(_WHAT_Tx_Packet);
      }
    }
    else
    {
      // Child did NOT select this device as its parent.
      #ifdef CHILD_MONITOR
        Serial.print(F("Rx Accept Parent not match this device "));
        Serial.print(F("/tRx Parent value "));Serial.println(_Rx_ParentAddress);
        Serial.print(F("/tRx Latchkey value "));Serial.println(_Rx_Offered_Latchkey);
        Serial.println(F("/tChild NOT added "));
      #endif
    }
}

// *****************
// TOC Individual Recieve messages
void Rx_NETWORK_JOIN_COMPLETE_03(WHAT_Packet _WHAT_Rx_Packet, uint8_t _index)
{
  //The parent confirmed the acceptance of the child.  Store the pertinent information.
  //Serial.println(F("Rx_Network    Join Complete"));
// Rx Message flags
  // verify this message was from the proposed parent and the latchkey matches.
  uint16_t _Rx_Offered_Latchkey = ((_WHAT_Rx_Packet.payload[_index + 4] * 0x100) + _WHAT_Rx_Packet.payload[_index + 5]);

  if((_WHAT_Rx_Packet.sourceaddress == myParentAddress ) && (_Rx_Offered_Latchkey == myLatchKeyNumber))
  {
   f_Rx_NETWORK_JOIN_COMPLETE_03 = true;   // Preeemie accepted a proposed parent 

  }

}

// *****************
// TOC Individual Transmit messages
void Tx_PROPOSE_CHILD_NUMBER_05 (void)
{
  // further MID will not be sent if this device does not have any available child slots.
  uint8_t _myChild_available_Index = 0;
  uint8_t _i = 1;
  while((_i < 16) & (_myChild_available_Index ==0))
  {
    // Check each index. If the source address is available (0xFFFF), capture that index, and stop the loop.
    if( myChildren[_i].ChildAddress < 0xFFFF) {_i++;}else{_myChild_available_Index = _i;}
    //Serial.print("Address/_i");Serial.print(myChildren[_i].ChildAddress);Serial.println(_i);
  }
  
  if ((_myChild_available_Index >0) & (_myChild_available_Index < 16)) //if the index is 16, then no slots available.  Address 0 is indistinguishable from parent, and is not allowed.
  {
    // Child slot is available.

    // Create a proposed child address.  This will be the lowest available address.
    //The child address will be in the next higher nibble of the parent's address.
    //Each generation (above the master) will use the next higher nibble, and the open child index.
    //Also note that only 4 generations will fit in a 2 byte number.

    //The algorithn is a little confusing, but if mySourceAddress < 0xF, then the child index must be rotated 1 nibble to the left.
    // This could be done with rotation, but multiplying by 0x0010 serves the same purpose.
    //    So "If mySourceAddress == 0x0000, then myChildIndex must not move (making the proposed address 0x000C) (where C is the child index)
    //       "If mySourceAddress >0x0000, then myChildIndex must move left 1 nibble (rotate left 4 times)(making the proposed address 0x00C0) (where C is the child index)
    //       "If mySourceAddress >0x000F, then myChildIndex must move left 2 nibbles (rotate left 8 times)(making the proposed address 0x0C00) (where C is the child index)
    //       "If mySourceAddress >0x00FF, then myChildIndex must move left 3 nibbles (rotate left 12 times)(making the proposed address 0xC000) (where C is the child index)
    //        At the end of this process, mySourceAddress is logic ORed with myChildIndex becoming the _proposed_child_address.
    //        ** device addresses above 0x3FFF are not allowed.  If this index results in a larger number, it will not offer an address.
    _Offered_SourceAddress = (_myChild_available_Index) << ( ((mySourceAddress > 0x0) * 4) + ((mySourceAddress > 0x0F) * 4) + ((mySourceAddress > 0x0FF) * 4) );
    //Serial.print("Calc child address "); Serial.println(_proposed_child_address,HEX);
    _Offered_SourceAddress |= mySourceAddress; // This incorporates the parent address into the child's address.
    // *******  NOTE:  This _Offered_SourceAddress is checked when recieving ACCEPT_PARENT_06 *********** // 
    //Serial.print("mySource"); Serial.println(mySourceAddress,HEX);
    //Serial.print("_proposed_child_address:0x"); Serial.println(_proposed_child_address,HEX);

    if (_Offered_SourceAddress < 0x4FFF)
    {
  // A valid child address is available.
      WHAT_Packet _WHAT_Tx_Packet;  // Make a temporary transmit packet
      _WHAT_Tx_Packet.WHAT_MAC_MESSAGE;
      _WHAT_Tx_Packet.pan_id = myPANID;
      _WHAT_Tx_Packet.destination = 0xFFFF;   // Send this to all devices.
      _WHAT_Tx_Packet.sourceaddress = mySourceAddress;  // Use the source address of this device.
      uint8_t _index = 0;
      _WHAT_Tx_Packet.payload[_index++] = 6; //include the length of the proposed child address.
      _WHAT_Tx_Packet.payload[_index++] = 5;   //MID size (for the 2nd message).
      _WHAT_Tx_Packet.payload[_index++] = highByte(PROPOSE_CHILD_NUMBER_05);// 1 byte length, 2 byte command, 2 byte child address, 2 byte latchkey // Any device that has open child slots replies to request (REQUEST_JOIN_NETWORK_02).
      _WHAT_Tx_Packet.payload[_index++] = lowByte(PROPOSE_CHILD_NUMBER_05); //in order to support multibyte MIDs, put the high byte first.
      _WHAT_Tx_Packet.payload[_index++] = highByte(_Offered_SourceAddress);//in order to support multibyte MIDs, put the high byte first.
      _WHAT_Tx_Packet.payload[_index++] = lowByte(_Offered_SourceAddress);
      WHAT_TransmitPacket(_WHAT_Tx_Packet);

      WHATRxMessage[WHAT_Reusable_Message1].MessageID = ACCEPT_PARENT_06;  // these are prospective parents offering free addresses.
      WHATRxMessage[WHAT_Reusable_Message1].callback = &Rx_ACCEPT_PARENT_06; // Message callback set here to collect the responses
      _WHAT_network_StateColor = WHAT_LIGHT_OFF;

      f_Rx_ACCEPT_PARENT_06 = false;   // Clear the message reception flag.

      #ifdef CHILD_MONITOR
        Serial.print(F("Tx Propose child address: 0x")); Serial.println(_Offered_SourceAddress,HEX);
      #endif

    }//End if my address proposal will be vaild.
  }// End If I have an available child slot

}

// *****************
// TOC Individual Recieve messages
void Rx_ABANDON_CHILD_05 (WHAT_Packet _WHAT_Rx_Packet, uint8_t _index)
{  // One of this device's children is leaving it.
   // Clear the child information from the index.
   // Moving all the other children would also require giving them new addresses, which 
   // is unnecessary, so this slot will remain open.  New devices can re-use this address.
   uint8_t _i=0;
   while (_i<=ChildMaxIndex)
   {
      if(_WHAT_Rx_Packet.sourceaddress == myChildren[_i].ChildAddress)
      { // the child has been found.  Before blanking, make sure it really was this child by checking the latchkey value.
        if(((_WHAT_Rx_Packet.payload[_index + 3] * 0x100) + _WHAT_Rx_Packet.payload[_index + 4]) == myChildren[_i].LatchKeyValue)
        { // This is the child.  Remove it from the index.
            myChildren[_i].ChildAddress = 0xFFFF;
            myChildren[_i].LatchKeyValue = 0xFFFF;
            myChildren[_i].RelationshipType = 0;
            myChildren[_i].Security = 0xFFFF;
        }//End Child removed
        else
        {// Security breach... incorrect latchkey value.  Note the security breach (not yet implimented)
          WHAT_Packet _WHAT_Tx_Packet;  // Make a temporary transmit packet
          _WHAT_Tx_Packet.WHAT_MAC_MESSAGE;
          _WHAT_Tx_Packet.pan_id = myPANID;
          _WHAT_Tx_Packet.destination = 0xFFFF;   // Send this to all devices.
          _WHAT_Tx_Packet.sourceaddress = mySourceAddress;  // Use the source ddress of this device.
          uint8_t _index = 0;
          _WHAT_Tx_Packet.payload[_index++] = 5; //Total payload length (including this byte)
          _WHAT_Tx_Packet.payload[_index++] = 4; //MID length (including this byte)
          _WHAT_Tx_Packet.payload[_index++] = highByte(Security_Breach_04);
          _WHAT_Tx_Packet.payload[_index++] = lowByte(Security_Breach_04);
          _WHAT_Tx_Packet.payload[_index++] = SB_BUMP;  //(bump)   A parent sees a request to abandon child, but the latchkey value is incorrect.
              WHAT_TransmitPacket(_WHAT_Tx_Packet);
        }// End security breach
       _i = ChildMaxIndex + 1;   // discontinue the search once the child is found by making the index greater than the highest index.
      }// End if child match found.
    _i++; //Check the next entry.
   }// End while looking for child entry.
}

// *****************
// TOC Individual Transmit messages
void Rx_REQUEST_ACK_03 (void)
{
    f_Rx_REQUEST_ACK_03 = true;    // Clear the message flag so only new messages will be used.
      #ifdef CHILD_MONITOR
        Serial.println(F("Rx Request Acknowledgement"));
      #endif

}
// *****************
// TOC Individual Transmit messages
void Tx_REPLY_ACKNOWLEDGE_03 (void)
{   
    WHAT_Packet _WHAT_Tx_Packet;  // Make a temporary transmit packet
    _WHAT_Tx_Packet.WHAT_MAC_MESSAGE;
    _WHAT_Tx_Packet.pan_id = myPANID;
    _WHAT_Tx_Packet.destination = 0xFFFF;   // Send this to all devices.
    _WHAT_Tx_Packet.sourceaddress = mySourceAddress;  // Use the source ddress of this device.
    uint8_t _index = 0;
    _WHAT_Tx_Packet.payload[_index++] = 4; //Total payload length (including this byte)
    _WHAT_Tx_Packet.payload[_index++] = 3; //MID length (including this byte)
    _WHAT_Tx_Packet.payload[_index++] = highByte(REPLY_ACKNOWLEDGE_03);
    _WHAT_Tx_Packet.payload[_index++] = lowByte(REPLY_ACKNOWLEDGE_03);
        WHAT_TransmitPacket(_WHAT_Tx_Packet);
}

void Setup_WHAT_Show_Connection(void)
{
  // This is just an indication that the connection is complete.
  //Serial.print("Connected.   PANID:0x");Serial.println(myPANID,HEX);
  //Serial.print("           Address:0x");Serial.println(mySourceAddress,HEX);
  //Serial.print("            Parent:0x");Serial.println(myParentAddress,HEX);
  //Serial.print("          LatchKey:0x");Serial.println(myLatchKeyNumber,HEX);
  

}

#endif  // If auto-addressing is defined.
