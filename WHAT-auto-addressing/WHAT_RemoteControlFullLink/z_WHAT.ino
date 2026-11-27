#define WHAT_NETWORK_VERSION 26112702
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

// *********************************************************************
// *********************************************************************
// *********************************************************************

// TOC Globals and definitions

// These are variables that can be (carefully) re-used within this program
// This is done to reduce the amount of dynamic memory used.
// Group: WHAT_Scratch (instance gblScratch) - reusable temporaries, shared to save dynamic memory.
// RULE: a scratch value must never be expected to survive a call to another function that uses scratch.
struct WHAT_Scratch
{
  double timer1 = millis();
  double timer2 = millis();
  uint8_t u8_1  = 0xFF;
  uint16_t u16_1  = 0xFFFF;
  uint16_t u16_2  = 0xFFFF;
  uint16_t u16_3  = 0xFFFF;
  uint16_t u16_4  = 0xFFFF;
  uint32_t u32_1  = 0xFFFFFFFF;  // STEP 1 (Nov 24, 2026): 32-bit hand-offs (an accepted child's latchkey / confirm nonce) for the Birth Announcement.
  uint32_t u32_2  = 0xFFFFFFFF;
};
WHAT_Scratch gblScratch;

// *************************************************************************
// STEP 1 (Nov 24, 2026) - Security primitives. One keyed mix is the ONLY
// cryptographic machinery in WHAT (see WHAT_Security_Architecture_2026-11-24.md):
// every proof in the join / abandon / (later) return and re-key flows is
// WHAT_Mix32(latchkey, context, context) - the latchkey itself is transmitted
// exactly once, inside the button-gated join, and never again.
uint32_t WHAT_Mix32(uint32_t _key, uint32_t _a, uint32_t _b)
{
  uint32_t _z = _a ^ _key;
  _z += _b + 0x9E3779B9UL;
  _z = (_z ^ (_z >> 16)) * 0x21F0AAADUL;
  _z = (_z ^ (_z >> 15)) * 0x735A2D97UL;
  _z = (_z ^ (_z >> 15)) + _key;
  return _z;
}

// Randomness pool. There is no hardware RNG on a Nano, so the pool is stirred
// with whatever noise the device lives in: micros() jitter at human- and
// radio-triggered moments, the RSSI/LQI of every packet heard (RX_WHAT stirs
// it), and an analog reading. History-dependent, so a value is never a
// function of uptime alone the way the old (uint16_t)millis() latchkey was.
static uint32_t _st_RandomPool = 0x2545F491UL;
void WHAT_RandomStir(uint32_t _noise)
{
  _st_RandomPool = WHAT_Mix32(_st_RandomPool, _noise, (uint32_t)micros());
}
uint32_t WHAT_Random32(void)
{
  _st_RandomPool = WHAT_Mix32(_st_RandomPool, (uint32_t)micros(), (uint32_t)analogRead(0));
  return _st_RandomPool;
}

// 32-bit payload field helpers (big-endian, matching the highByte-first style
// of the existing 16-bit fields). _index is the ABSOLUTE payload index of the
// field's first byte. In transmit builds that is the running _index counter;
// in receive callbacks (whose _index points at the MID-length byte) the first
// data field of a message is at _index + 3.
void WHAT_PayloadPut32(uint8_t *_payload, uint8_t _index, uint32_t _value)
{
  _payload[_index]     = (uint8_t)((_value >> 24) & 0xFF);
  _payload[_index + 1] = (uint8_t)((_value >> 16) & 0xFF);
  _payload[_index + 2] = (uint8_t)((_value >> 8) & 0xFF);
  _payload[_index + 3] = (uint8_t)(_value & 0xFF);
}
uint32_t WHAT_PayloadGet32(uint8_t *_payload, uint8_t _index)
{
  return ((uint32_t)_payload[_index] << 24) | ((uint32_t)_payload[_index + 1] << 16) | ((uint32_t)_payload[_index + 2] << 8) | (uint32_t)_payload[_index + 3];
}

// PROPOSED (Nov 25, 2026): is _source inside _child's subtree, under the
// proposed trunk reading? Three cases: the child itself; a TRUNK child (path
// 000), whose subtree is its whole trunk; and a fill child, found by walking
// the source's digit-parents up (zero the deepest nonzero path digit, at most
// three steps). The walk stops at a trunk root - a seam parent is not
// derivable from digits (the table is the truth at a seam), so traffic from
// across a seam only credits the seam child itself, never the grandparent.
// That is enough for liveness: every device heartbeats when quiet, so each
// direct child's own transmissions keep its slot fresh on their own.
bool WHAT_SourceInChildSubtree(uint16_t _source, uint16_t _child)
{
  if (_source == _child) return true;
  if ((_child & 0x0FFF) == 0)
  { // _child is a trunk root: every address in its trunk is below it.
    return ((_source & 0xF000) == (_child & 0xF000));
  }
  uint16_t _ancestor = _source;
  for (uint8_t _step = 0; _step < 3; _step++)
  {
    if ((_ancestor & 0x0FFF) == 0) break;  // Reached a trunk root: its parent is a seam, not derivable.
    if (_ancestor & 0x000F)      _ancestor &= 0xFFF0;  // Zero the ones path digit.
    else if (_ancestor & 0x00F0) _ancestor &= 0xFF0F;  // Zero the tens path digit.
    else                         _ancestor &= 0xF0FF;  // Zero the hundreds path digit.
    if (_ancestor == _child) return true;
  }
  return false;
}

// PROPOSED (Nov 25, 2026): the slot of the child the lost indication names -
// worst first: a lost TRUNK child outranks any fill child (a whole subtree is
// stranded behind it), then the longest-lost. 0xFF when no child is lost.
// The replacement offer acts on exactly this slot, and it is recomputed at
// the moment of the offer, so a child heard again in the meantime is never
// replaced by mistake.
uint8_t WHAT_IndicatedLostChild(void)
{
  uint8_t _worst = 0xFF;
  uint32_t _worstHeard = 0xFFFFFFFF;
  for (uint8_t _i = 1; _i < ChildMaxIndex; _i++)
  {
    if (gblNetwork.children[_i].ChildAddress == 0xFFFF) continue;
    if ((uint32_t)(millis() - gblNetwork.childLastHeard[_i]) <= (uint32_t)WHAT_CHILD_LOST_MS) continue;  // Heard recently enough - not lost.
    if ((gblNetwork.children[_i].ChildAddress & 0x0FFF) == 0) return _i;  // A lost trunk child outranks all fill children.
    if (gblNetwork.childLastHeard[_i] < _worstHeard)
    {
      _worstHeard = gblNetwork.childLastHeard[_i];
      _worst = _i;
    }
  }
  return _worst;
}

// (gblRegistry.reusableSlot moved into gblRegistry.reusableSlot, in z_WHAT.h - Nov 2026 rename pass.)
//uint8_t gblRegistry.reusableSlot = 0xFF; //This is registered once, then the callback can be changed
//When connecting to the network, several messages are only valid
//uint8_t gblLink.reusableSlot = 0xFF; //at certian times, so this can be changed to avoid each of them taking
//an entry in the recieved message table.  All of the 1-time, 1-at-a-time use messages
//can be assigned to this same message table entry.

// Group: WHAT_RxFlags (instance gblRxFlags) - "a message arrived" flags.
// Member names are the command names from z_WHAT_Command.h (the f_Rx_ prefix is dropped:
// the group name already says these are receive flags - Gary, Nov 2026).
struct WHAT_RxFlags
{
  bool f_RESET_TO_PREEMIE = false;          // Holding the network button at power on activates this. (was gblRxFlags.f_RESET_TO_PREEMIE)
  bool f_REQUEST_JOIN_NETWORK_03 = false;   // Preemie asking to join network
  bool f_NETWORK_ALERT_PREEMIE_03 = false;  // Notice from another device that a preemie is trying to join
  bool f_CHOOSE_NETWORK_05  = false;        // Notice to other networks that a network has accepted this preemie
  bool f_NETWORK_ALLOW_PREEMIE_04 = false;  // Notice to this preemie that it may accept its parent.
  bool f_PROPOSE_CHILD_NUMBER_05 = false;   // Preemie asking the network for a child number
  bool f_ACCEPT_PARENT_06 = false;
  bool f_NETWORK_JOIN_COMPLETE_03 = false;  // Preeemie accepted a proposed parent 
  bool f_NETWORK_JOIN_ABORT_03 = false;     // User abort of the acceptance process.
  bool f_REQUEST_ACK_03 = false;            // Send out a request for any existing network with my proposed PANID to respond.
  bool f_REPLY_ACKNOWLEDGE_03 = false;      // While a new network is forming, requests existing network to claim its PANID
  bool f_REPLY_ABANDON_CONFIRM_03 = false;  // PROPOSED (Nov 25, 2026): my parent blanked AND saved my child record - my reset to preemie may proceed.
};
WHAT_RxFlags gblRxFlags;

// TOC SETUP
void WHAT_begin(void)
//void WHAT_begin(bool & gblMailbox.networkButton, WHAT_State & gblMailbox.networkState, WHAT_LedColor & gblMailbox.networkStateColor)
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
  //WHAT_Tx_Packet.pan_id = gblNetwork.panID;
  //WHAT_Tx_Packet.destination = 0xFFFF;
  //WHAT_Tx_Packet.sourceaddress = mySourceAddress;

  // setup the network button and lights.
  //pinMode(NETWORK_LIGHT,OUTPUT);
  //digitalWrite(NETWORK_LIGHT,LOW); // Lights are active HIGH
  //pinMode(NETWORK_BUTTON,INPUT_PULLUP);
  //Setup_Button_Light(); // This is IF the button/light is installed.
  //gblMailbox.networkState = gblMailbox.networkState;   // Main application is responsible for monitoring the network button and light (in case of LCD displays, etc...)
  //gblMailbox.networkButton = gblMailbox.networkButton; // Main application is responsible for monitoring the network button and light (in case of LCD displays, etc...)
  //gblMailbox.networkStateColor = gblMailbox.networkStateColor;
  //_Loop_WHAT_NetworkButton_Light = Loop_WHAT_NetworkButton_Light; // Function pointer for lamp updates.
  //pinMode(b_RED_RGB_PIN,OUTPUT);        // Main application is responsible for additional lights, if desired.
  //pinMode(b_GREEN_RGB_PIN,OUTPUT);
  //pinMode(b_BLUE_RGB_PIN,OUTPUT);
  //digitalWrite(b_RED_RGB_PIN,HIGH);  // Lights are active LOW
  //digitalWrite(b_GREEN_RGB_PIN,HIGH);
  //digitalWrite(b_BLUE_RGB_PIN,HIGH);
  WHAT_EEPROM();  // restore any WHAT parameters from EEPROM (Source address/PANID, etc...)

  gblMailbox.networkState = WHAT_STARTUP;
  #ifdef CHILD_MONITOR
  Serial.print(F("Network State set to 'WHAT_STARTUP' "));Serial.println(gblMailbox.networkState,DEC);

  Serial.println(F("PANID\tmySourceAddress\tgblNetwork.parentAddress\tmyLatchkey\tgblMailbox.networkState"));
  Serial.print(gblNetwork.panID,HEX);Serial.print(F(" \t"));Serial.print(mySourceAddress,HEX);Serial.print(F(" \t\t"));Serial.print(gblNetwork.parentAddress,HEX);Serial.print(F(" \t"));Serial.print(myLatchKeyNumber,HEX);Serial.print(F(" \t\t\t"));Serial.println(gblMailbox.networkState,HEX);
  #ifdef __WHAT_FULLLINK_h
    // (Nov 27, 2026: prints the entry table's active entries - the peer list is gone.)
    Serial.print(F("LinkEntries:"));
    for(uint8_t _i=0; _i<WHAT_MAX_LINK_ENTRIES; _i++)
    {
      if(gblLink.entries[_i].source != 0xFFFF){ Serial.print(F(" ")); Serial.print(_i); Serial.print(F(":out")); Serial.print(gblLink.entries[_i].output); Serial.print(F("/0x")); Serial.print(gblLink.entries[_i].source,HEX); }
    }
  #endif
  Serial.println(F(" "));
  #endif
  mrf.init();
  Mrf24j_begin (gblNetwork.panID, mySourceAddress);
  // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// TOC  Register WHAT messages for reception
  #ifdef CHILD_MONITOR
  Serial.println(F("WHAT Auto-addressing enabled"));
  #endif
  RegisterWHATMessage (REQUEST_JOIN_NETWORK_03, &Rx_REQUEST_JOIN_NETWORK_03); // This is an entry that will not change.
  RegisterWHATMessage (NETWORK_ALERT_PREEMIE_03, &Rx_NETWORK_ALERT_PREEMIE_03); // This is an entry that will not change.
  RegisterWHATMessage (PROPOSE_CHILD_NUMBER_05, &Rx_PROPOSE_CHILD_NUMBER_05); // This is an entry that will not change.
  RegisterWHATMessage (ABANDON_CHILD_05, &Rx_ABANDON_CHILD_05); // This is an entry that will not change.
  RegisterWHATMessage (REPLY_ABANDON_CONFIRM_03, &Rx_REPLY_ABANDON_CONFIRM_03); // This is an entry that will not change. (PROPOSED, Nov 25, 2026)
  RegisterWHATMessage (NETWORK_HEARTBEAT_03, &Rx_NETWORK_HEARTBEAT_03); // This is an entry that will not change. (STEP 1, Nov 24, 2026)
 
   // Trying to make these as re-usable messages.
  //RegisterWHATMessage (NETWORK_ALERT_PREEMIE_03, &Rx_NETWORK_ALERT_PREEMIE_03); // This is an entry that will not change.
  RegisterWHATMessage (NETWORK_ALLOW_PREEMIE_04, &Rx_NETWORK_ALLOW_PREEMIE_04); // This is an entry that will not change. (FIX, Nov 27, 2026: registered permanently - a MEMBER must hear another member's allow so it can offer a child too (Gary's design: any member offers when any member's button is pressed; the preemie chooses the lowest address). It used to be reachable only through the reusable slot a waiting preemie parks, so members never heard it and only the pressed unit could offer.)
  gblRegistry.reusableSlot = RegisterWHATMessage (0xFFFF, &holdingFunction); // This entry can be re-used during network adoption.
      #ifdef RX_MONITOR
      Serial.print(F("gblRegistry.reusableSlot :"));Serial.println(gblRegistry.reusableSlot);
      #endif
  gblMailbox.networkButton = LOW;
   gblMailbox.networkState = WHAT_STARTUP;
   gblMailbox.networkStateColor = WHAT_LIGHT_OFF;

  // Check to see if this device has any children.
  gblNetwork.hasChildren = false;
  for(uint8_t _i=0;_i<ChildMaxIndex;_i++)
  {
    if(gblNetwork.children[_i].ChildAddress<0xFFFF)
    {
      gblNetwork.hasChildren = true;
      gblNetwork.childLastHeard[_i] = millis();  // PROPOSED (Nov 25, 2026): a restored child gets a full lost interval from boot to reappear before it can be flagged lost.
      #ifdef RX_MONITOR
      Serial.print(F("Has Children:"));Serial.println(_i);
      #endif
   } 
  }

}
// *********************************************************************
// *********************************************************************
// *********************************************************************
// TOC LOOP
void Loop_WHAT(void)
{
  Mrf24j_Loop();  //Physical Layer 

   WHAT_StateCheck(); // This is only invoked when the device has created or connected to a network.
 
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
  static uint8_t _st_RxbufferIndex = 0;
  if (WHAT_Buffer_RX(gblQueues.rxPacket,_st_RxbufferIndex) )
  { //TOC  WHAT_Buffer_RX returns TRUE if there is a new message in the recieve buffer.
    #ifdef RX_MONITOR
    Serial.print(F("\nRx WHAT"));
    #endif

       #ifdef DX_ONLY
        Serial.print(F("\nGBL_rx_index: "));Serial.print(gblRadio.rxIndex);
        Serial.print(F("\t_RxbufferIndex: "));Serial.print(_st_RxbufferIndex);
        Serial.println(F("\nLength\tCONTROL\tRx pan_id\tdestination\tsource"));
        Serial.print(gblQueues.rxPacket.frame_length,DEC);Serial.print(F("\t  "));
        Serial.print(gblQueues.rxPacket.frame_control,HEX);Serial.print(F("\t  "));
        Serial.print(gblQueues.rxPacket.pan_id,HEX);Serial.print(F("\t\t\t  "));
        Serial.print(gblQueues.rxPacket.destination,HEX);Serial.print(F("\t  "));
        Serial.println(gblQueues.rxPacket.sourceaddress,HEX);
    // read the payload
        Serial.print(F("Adr:"));Serial.print(WHAT_ADR_PAYLOADSTART,HEX);Serial.print(F(":"));
        uint8_t _payloadSize = gblQueues.rxPacket.payload[0];
        Serial.print(F("\t_payloadSize:"));Serial.println(_payloadSize);
        for (uint8_t _i = 0; _i <= _payloadSize; _i++) 
        {
            //gblQueues.rxPacket.payload[_i] = Mrf24j_Read(WHAT_ADR_PAYLOADSTART + _i);
              Serial.print(F("0x"));Serial.print(gblQueues.rxPacket.payload[_i],HEX);Serial.print(F(", "));
        }
        Serial.print(F("FCS: "));Serial.print(gblQueues.rxPacket.payload[_payloadSize+2],HEX);Serial.print(gblQueues.rxPacket.payload[_payloadSize+1],HEX);;
        Serial.print(F("\tlqi: "));Serial.print(gblQueues.rxPacket.lqi,HEX);
        Serial.print(F("\trssi: "));Serial.println(gblQueues.rxPacket.rssi,HEX);
        Serial.print(F("\n"));

      #endif
// TOC Indicate the network is alive 
   if((gblQueues.rxPacket.pan_id == gblNetwork.panID) | (gblQueues.rxPacket.pan_id == GLOBAL_PANID))
    {
        gbl_t_NetworkAlive = millis() + GLB_T_NETWORKALIVE_RESET; // This allows an indication of any other devices on the network.
        f_gbl_t_NetworkAlive = true;      // Flag that other network nodes HAVE been seen (alive timer reset)
        WHAT_RandomStir(((uint32_t)gblQueues.rxPacket.rssi << 8) | gblQueues.rxPacket.lqi);  // STEP 1: radio noise stirs the randomness pool on every packet heard.
    }

// TOC Per-child liveness (PROPOSED, Nov 25, 2026)
// Any traffic on this PANID from inside a child's subtree marks that child
// heard. WHATHouseKeeping compares these stamps against WHAT_CHILD_LOST_MS
// to raise the lost-child indication. A preemie's source (0xFFFF) and this
// device's own address credit nobody.
   if((gblQueues.rxPacket.pan_id == gblNetwork.panID) && (gblQueues.rxPacket.sourceaddress != 0xFFFF) && (gblQueues.rxPacket.sourceaddress != mySourceAddress))
    {
      for (uint8_t _i = 1; _i < ChildMaxIndex; _i++)
      {
        if (gblNetwork.children[_i].ChildAddress == 0xFFFF) continue;
        if (WHAT_SourceInChildSubtree(gblQueues.rxPacket.sourceaddress, gblNetwork.children[_i].ChildAddress))
        {
          gblNetwork.childLastHeard[_i] = millis();
        }
      }
    }

// TOC Identify the message type and act accordingly
    switch (gblQueues.rxPacket.frame_control & 0x07)
    {
// TOC Rx Beacon message
      case MASK_FC_BEACON: //this is a beacon frame.  These are not currently supported.
        #ifdef RX_MONITOR
        Serial.println(F("Beacon"));
        #endif
      break;

// TOC Rx Data message
      case MASK_FC_DATA: //this is a data frame.  Extract and use the data.
        #ifdef RX_MONITOR
        Serial.println(F("Data"));
        #endif
        //Serial.print("MRF_INTSTAT : ");Serial.println(_mrf_intstat);
        //gblQueues.rxPacket.packet_length = Mrf24j_Read(WHAT_ADR_PACKETLENGTH);
        //Serial.print("Address "); Serial.print(WHAT_ADR_RxSTART,HEX); Serial.print(" (length) : ");Serial.println(_length);
        #ifdef MATCH_MONITOR  // PROPOSED (Nov 25, 2026): match confirmations behind their own switch (Gary - cleaner serial log). The raw dump above already shows pan / destination / source.
        Serial.print(F("pan_id : 0x")); Serial.println(gblQueues.rxPacket.pan_id,HEX);
        Serial.print(F("gblNetwork.panID : 0x")); Serial.println(gblNetwork.panID,HEX);
        
        Serial.print(F("destination : 0x")); Serial.println(gblQueues.rxPacket.destination,HEX);
        Serial.print(F("sourceaddress :")); Serial.println(gblQueues.rxPacket.sourceaddress,HEX);
        #endif
        //_length= gblQueues.rxPacket.frame_length - _ZIGBEE_HEADER_LENGTH;
          //Serial.println ("A message Arrived. ");
          if ((gblQueues.rxPacket.pan_id == gblNetwork.panID) | (gblQueues.rxPacket.pan_id == GLOBAL_PANID))
          { // The recieved packet is using the PAN ID of this network or has a global PAN or is configuring a network
            #ifdef MATCH_MONITOR
                Serial.println(F("Data PAN ID Match"));
            #endif
            //Serial.print("Rx Destination: 0x");Serial.println(gblQueues.rxPacket.destination,HEX);
            //Serial.print("MySource: 0x");Serial.println(mySourceAddress,HEX);
            if ((gblQueues.rxPacket.destination == mySourceAddress) | (gblQueues.rxPacket.destination == GLOBAL_DESTINATION))  // FIXED in the RemoteControl copy Nov 24, 2026, carried into the main path at adoption (Nov 26, 2026): the second clause compared pan_id to GLOBAL_DESTINATION, so broadcast DATA packets (destination 0xFFFF on a real PAN) never reached the payload handler. The MAC-command case below always had it right.
            { // The recieved packet is targeting this device, or is general communications
              #ifdef MATCH_MONITOR
                      Serial.println(F("Destination Match"));
              #endif
              WHAT_PayloadHandler();
            } // End If this message is targeting this device

          } // End if this message is on this PAN
        break;
// TOC Rx Acknowledge message
      case MASK_FC_ACKFRAME:  // Default to ACKs, though they are not currently being handled.
        #ifdef RX_MONITOR
        Serial.println(F("Ack"));
        #endif
        break;
// TOC Rx MAC message (network creation/modification)
      case MASK_FC_MAC_CMD: // MAC command... Used in network creation.
        #ifdef RX_MONITOR
        Serial.println(F("MAC Command"));
        #endif
        #ifdef RX_MONITOR
          //Serial.println(F("PANID DESTINATION SOURCE gblNetwork.hasChildren"));
          //Serial.print(gblQueues.rxPacket.pan_id,HEX);Serial.print(" ");Serial.print(gblQueues.rxPacket.destination,HEX);Serial.print(" ");Serial.print(f,HEX);Serial.print(" ");Serial.println(gblNetwork.hasChildren);
        #endif
        if(mySourceAddress == 0xFFFF)
        {
              WHAT_PayloadHandler();
        }
        else
        {
          //Once this device is part of a network, establish more message acceptance critera
          if ((gblQueues.rxPacket.pan_id == gblNetwork.panID) | (gblQueues.rxPacket.pan_id == GLOBAL_PANID))
          { // The recieved packet is using the PAN ID of this network or has a global PAN or is configuring a network
            #ifdef MATCH_MONITOR
            Serial.print(F("match: gblQueues.rxPacket.pan_id:0X"));Serial.println(gblQueues.rxPacket.pan_id,HEX);
            #endif
            //Serial.print("Rx Destination: 0x");Serial.println(gblQueues.rxPacket.destination,HEX);
            //Serial.print("MySource: 0x");Serial.println(mySourceAddress,HEX);
            if ((gblQueues.rxPacket.destination == mySourceAddress) | (gblQueues.rxPacket.destination == GLOBAL_DESTINATION))
            { // The recieved packet is targeting this device, or is general communications
              #ifdef MATCH_MONITOR
              Serial.println(F("Destination Match"));
              #endif
              WHAT_PayloadHandler();
            } // End If this message is targeting this device
          }
          else
          {
            #ifdef MATCH_MONITOR
            Serial.print(F("No Destination Match: "));Serial.println(gblQueues.rxPacket.pan_id);
            #endif
          }
        }
        break;

      default:
        #ifdef RX_MONITOR
        Serial.print(F("Reserved recieved message frame: 0x"));Serial.println(gblQueues.rxPacket.frame_control & 0x07);
        #endif
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
     if ((gblQueues.rxPacket.pan_id == gblNetwork.panID) & (gblQueues.rxPacket.destination != mySourceAddress) & gblNetwork.hasChildren) 
     {  // Only consider re-transmit if the PANID matches, THIS device was not targeted, and this device has children.
        bool _retransmit_message = false;
        if(gblQueues.rxPacket.destination == GLOBAL_DESTINATION) 
        {
           if(gblQueues.rxPacket.sourceaddress == gblNetwork.parentAddress) _retransmit_message = true;
        }
        if(!_retransmit_message)// If already transmitting, no need to evaluate the source and destination...
        {   // source or destination of grand children and great grand children also need be considered....
            // mask to isolate this child's children and grandchildren in the destination  and source address.
            // This is multiplying a comparison with a number.  A false comparison resolves to 0 and when multiplied, removes the term from the equation.
            uint16_t myMask = ( ((mySourceAddress > 0x00) * 0x0F) | ((mySourceAddress > 0x0F) * 0x0FF) | ((mySourceAddress > 0xFF) * 0xFFF) );
            #ifdef RX_MONITOR
            Serial.print(F("Mask:0x"));Serial.println(myMask);
            #endif
             //Apply the mask to the incoming source address to verify that the lineage matches this device before sending this message.
            uint16_t _temporary = gblQueues.rxPacket.sourceaddress & myMask;
            if(_temporary == mySourceAddress) _retransmit_message = true; //This message came FROM one of my (grand)children. 
            _temporary = gblQueues.rxPacket.destination & myMask;
            if(_temporary == mySourceAddress) _retransmit_message = true; //This message is going TO one of my (grand)children. 
        }
        if(!_retransmit_message)// If already transmitting, no need to evaluate the children...
        {// Not re-transmitting.  see if any of the children are targeted.
          for (uint8_t _i=1;_i<ChildMaxIndex;_i++)
          { //see if the arriving souce address or destination address matches any of the children of this device. 
            if(gblQueues.rxPacket.destination == gblNetwork.children[_i].ChildAddress) _retransmit_message = true;
            if(gblQueues.rxPacket.sourceaddress == gblNetwork.children[_i].ChildAddress) _retransmit_message = true;
          }
        }
        if(_retransmit_message)
        { // re transmit this message.
            // My initial thoughts were to use the MSB as an indicator for if the message was 
            // traveling inbound or outbound, but I think the use of the source and destination
            // eliminate the need for that.  That might allow twice as many source addresses to be used.
            WHAT_Packet _WHAT_Tx_Packet;  // Make a temporary transmit packet
            _WHAT_Tx_Packet = gblQueues.rxPacket; // Make it the same as the incoming message.
            #ifdef RX_MONITOR
            Serial.println(F("Retransmitting"));
            #endif
        }
    }
    #ifdef RX_MONITOR
    else
    {
      //Serial.println(F("Not Retransmitted.  PANID DESTINATION SOURCE gblNetwork.hasChildren"));
      //Serial.print(gblQueues.rxPacket.pan_id,HEX);Serial.print(" ");Serial.print(gblQueues.rxPacket.destination,HEX);Serial.print(" ");Serial.print(mySourceAddress,HEX);Serial.print(" ");Serial.println(gblNetwork.hasChildren);
    }
    #endif  // RX_MONITOR
  //  #endif  // RETRANSMIT

    _st_RxbufferIndex++;
    // FIX: Enforce proper zero-indexed circular buffer bounds
    if(_st_RxbufferIndex >= RX_BUFFER_DEPTH ){ 
        _st_RxbufferIndex = 0; 
    }
  }// End if a message has arrived
}// End Function

// TOC Handle the recieve payload
void WHAT_PayloadHandler(void)
{// The PANID, SOURCE and DESTINATION have been verified that this routine
 //   is permitted to sift through the payload and act upon the MIDs
 //   this works on the global variable gblQueues.rxPacket that is buffered seperately.
 //   MIDs are compared to the registered list and when matches are found, callbacks are executed.
 
     uint8_t _length = gblQueues.rxPacket.payload[0];  // Total payload length.
    //Serial.print("Rx payload size:");Serial.println(_length);
    uint8_t _index = 1;
    while (_index < (_length - 1))
    { // Read the first command from the packet.
      uint8_t _NumBytes = gblQueues.rxPacket.payload[_index];  // each command starts with its total data length.
      //Serial.print("_NumBytes:");Serial.println(_NumBytes);
      //uint16_t _thisMessageID = (gblQueues.rxPacket.payload[_index + 1] *0x100) + gblQueues.rxPacket.payload[_index + 2]; // Message IDs uniquely identify message contents
      uint16_t _thisMessageID = gblQueues.rxPacket.payload[_index + 1]; // Message IDs uniquely identify message contents
      if (_thisMessageID > 0xEF) {
        _thisMessageID = _thisMessageID * 0x100 + gblQueues.rxPacket.payload[_index + 2];
      }
      for (uint8_t _i = 0; _i <= gblRegistry.index; _i++)
      { // Search the registered messages and see if they want this messageID.
        if ( gblRegistry.message[_i]. MessageID ==  _thisMessageID)
        {
         #ifdef MATCH_MONITOR
          Serial.print(F("MID Match. Index:"));Serial.print(_i); Serial.print(F("MID:"));Serial.println(_thisMessageID, HEX);
          Serial.print(F("gblMailbox.networkState:")); Serial.println(gblMailbox.networkState, DEC);
          #endif  
         (* gblRegistry.message[_i].callback)(gblQueues.rxPacket, _index);
         //TOC THIS is where the recieved message activates the callback function.

        }
        #ifdef MATCH_MONITOR
        else
        {
          Serial.print(_thisMessageID,HEX);Serial.print(F("<>"));Serial.println(gblRegistry.message[_i]. MessageID,HEX);
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
  // TOC Liveness heartbeat (STEP 1, Nov 24, 2026)
  // If this device has transmitted nothing for a full heartbeat interval, say
  // so - one tiny broadcast. This guarantees every member is heard from at
  // least once per interval, so another device's silence always means it is
  // gone or out of range, never merely quiet. WHAT_TransmitPacket re-arms the
  // deadline, including for this heartbeat itself.
     if( gbl_t_NextHeartbeat <= millis())
     {
        WHAT_Packet _WHAT_Tx_Packet;  // Make a temporary transmit packet
        _WHAT_Tx_Packet.WHAT_MAC_MESSAGE;
        _WHAT_Tx_Packet.pan_id = gblNetwork.panID;
        _WHAT_Tx_Packet.destination = 0xFFFF;   // Send this to all devices.
        _WHAT_Tx_Packet.sourceaddress = mySourceAddress;  // Use the source address of this device.
        uint8_t _index = 0;
        _WHAT_Tx_Packet.payload[_index++] = 4; //Total payload length (including this byte)
        _WHAT_Tx_Packet.payload[_index++] = 3; //MID length (including this byte)
        _WHAT_Tx_Packet.payload[_index++] = highByte(NETWORK_HEARTBEAT_03);
        _WHAT_Tx_Packet.payload[_index++] = lowByte(NETWORK_HEARTBEAT_03);
        WHAT_TransmitPacket(_WHAT_Tx_Packet);
     }
  // TOC Lost-child watch (PROPOSED, Nov 25, 2026)
  // Keep the mailbox's lost indication current: the indicated child (worst
  // first - see WHAT_IndicatedLostChild) and whether any child is lost at
  // all. The WHAT_NET_ALIVE state blinks the light RED while a child is
  // lost - indication only; no button act follows from it (Gary's ruling,
  // Nov 25, 2026). A child heard again clears its own flag here - nothing
  // was ever erased, so a power outage that comes back needs no ceremony
  // at all.
     {
       static uint16_t _st_lastLostAddress = 0xFFFF;  // For a one-time serial note when the indicated child changes.
       uint8_t _lostSlot = WHAT_IndicatedLostChild();
       gblMailbox.childLost = (_lostSlot != 0xFF);
       gblMailbox.childLostAddress = gblMailbox.childLost ? gblNetwork.children[_lostSlot].ChildAddress : 0xFFFF;
       if (gblMailbox.childLostAddress != _st_lastLostAddress)
       {
         _st_lastLostAddress = gblMailbox.childLostAddress;
         #ifdef CHILD_MONITOR
         if (gblMailbox.childLost)
         {
           Serial.print(F("Child lost: 0x")); Serial.println(gblMailbox.childLostAddress,HEX);
         }
         #endif
       }
     }
  }
}

// *************************************************************************
// *************************************************************************
// TOC  Register WHAT messages for reception
uint8_t RegisterWHATMessage (uint16_t _MessageID, void(*callback)(WHAT_Packet , uint8_t))
{
  if(gblRegistry.index>=NUM_MAX_MESSAGE_REGISTRATIONS)
  {Serial.println(F("Message Reception buffer Overflow.  Some messages may not be recieved."));}
  uint8_t ret = gblRegistry.index; // this is the table entry index for THIS message.  This allows entries to be changed.
  gblRegistry.message[gblRegistry.index].MessageID = _MessageID;
  gblRegistry.message[gblRegistry.index].callback = callback;    // Function to be called if this message arrives.
  gblRegistry.index++;
  return ret;
}

// *************************************************************************
// TOC Put new message into buffer
void WHAT_TransmitPacket (WHAT_Packet _WHAT_Tx_Packet)
{
  // STEP 1 (Nov 24, 2026): every transmission re-arms the heartbeat deadline -
  // the heartbeat only fires if a full interval passes with NO transmission.
  // The per-device jitter (from the low nibble of the source address) keeps a
  // fleet of quiet devices from synchronizing their heartbeats into collisions.
  gbl_t_NextHeartbeat = millis() + WHAT_HEARTBEAT_INTERVAL_MS + ((_WHAT_Tx_Packet.sourceaddress & 0x0F) * 173) % 1000;
  // the transmit packet is placed in the buffer to be transmitted.
      gblQueues.tx[gblQueues.txPendIndex] = _WHAT_Tx_Packet;
      gblQueues.txPendIndex++; 
      if(gblQueues.txPendIndex >= TX_BUFFER_DEPTH) 
      {
        gblQueues.txPendIndex=0;
      }
      //Serial.println(gblQueues.txPendIndex);
      //Tx_What_Que(); // go ahead and try to send it now.

  //Mrf24j_Write(MRF24J_TXNCON, MRF24J_TXNTRIG);  // Transmit the packet
  //Serial.println(Mrf24j_Read(MRF24J_TXSTAT),BIN);
}
// *************************************************************************
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
#define EEPROM_myLatchKeyNumberXL_LOCATION  EEPROM_myLatchKeyNumberH_LOCATION + 1
#define EEPROM_myLatchKeyNumberXH_LOCATION  EEPROM_myLatchKeyNumberXL_LOCATION + 1
#define EEPROM_preemie_State_LOCATION EEPROM_myLatchKeyNumberXH_LOCATION + 1
#define _EEPROM_Child_LOCATION (EEPROM_preemie_State_LOCATION + 1)
// STEP 1 fix (Nov 24, 2026): this used to be _EEPROM_Child_LOCATION + 1 - an
// address INSIDE the child records, harmless only because the FullLink code
// that uses it is compiled out. The link table belongs AFTER the child table:
// the store routine writes 14 records (indices 1..14) of 9 bytes each
// (2 address + 4 latchkey + 1 relation + 2 security) = 126 bytes. If the child
// record layout ever changes size, this number must change with it.
#define EEPROM_LINK_LOCATION (_EEPROM_Child_LOCATION + 126)
// PER-OUTPUT LINK TABLE (Nov 27, 2026): the block at EEPROM_LINK_LOCATION
// now holds the link ENTRY table, not the old peer list: a 2-byte magic
// (WHAT_LINK_ENTRY_MAGIC1/2 in z_WHAT_FullLink.h) followed by
// WHAT_MAX_LINK_ENTRIES x 7 bytes (output, source, command, channel,
// flags) = 114 bytes total, spanning 0x90..0x101. NOTE for Step 2: the
// send-counter block once planned at 0xB0 now falls INSIDE this span -
// it must be placed after 0x102 when Step 2 is built.

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
    #ifdef CHILD_MONITOR
    Serial.println(F("Restoring from EEPROM"));
    #endif
    // Values have already been saved and updated in EEPROM.
    // Retrieve the EEPROM values to be used instead of default values
    gblNetwork.panID = EEPROM.read(EEPROM_myPANIDL_LOCATION) + (EEPROM.read(EEPROM_myPANIDH_LOCATION) * 0x100);
    //Serial.print(F("EE read gblNetwork.panID:0x"));Serial.println(gblNetwork.panID,HEX);
    mySourceAddress = EEPROM.read(EEPROM_mySourceAddressL_LOCATION) + (EEPROM.read(EEPROM_mySourceAddressH_LOCATION) * 0x100);
    //Serial.print(F("EE read mySourceAddress:0x"));Serial.println(mySourceAddress,HEX);
    gblNetwork.parentAddress = EEPROM.read(EEPROM_myParentAddressL_LOCATION) + (EEPROM.read(EEPROM_myParentAddressH_LOCATION) * 0x100);
    //Serial.print(F("EE read gblNetwork.parentAddress:0x"));Serial.println(gblNetwork.parentAddress,HEX);
    myLatchKeyNumber = (uint32_t)EEPROM.read(EEPROM_myLatchKeyNumberL_LOCATION) + ((uint32_t)EEPROM.read(EEPROM_myLatchKeyNumberH_LOCATION) * 0x100UL) + ((uint32_t)EEPROM.read(EEPROM_myLatchKeyNumberXL_LOCATION) * 0x10000UL) + ((uint32_t)EEPROM.read(EEPROM_myLatchKeyNumberXH_LOCATION) * 0x1000000UL);  // 32-bit since STEP 1 (Nov 24, 2026)
    //Serial.print(F("EE read myLatchKeyNumber:0x"));Serial.println(myLatchKeyNumber,HEX);
      /* EEPROM.get( eeAddress, f ) for TEENSY!!!! */ 
      /* but this is listed in another example   value = EEPROM.read(address);  */
  
    gblMailbox.networkState = (WHAT_State)EEPROM.read(EEPROM_preemie_State_LOCATION);
    //uint8_t temp = EEPROM.read(EEPROM_preemie_State_LOCATION);
    //gblMailbox.networkState = (en_preemie_states)(temp);

#ifdef __WHAT_FULLLINK_h
    // Retrieve the link ENTRY table (Nov 27, 2026 - was the peer list).
    // The 2-byte magic guards the layout: EEPROM written by the old
    // peer-list build (or never written) has no magic, and the table
    // loads BLANK instead of as garbage entries. The first
    // _EEPROM_store_links() afterwards writes the new layout.
    if ((EEPROM.read(EEPROM_LINK_LOCATION) == WHAT_LINK_ENTRY_MAGIC1) && (EEPROM.read(EEPROM_LINK_LOCATION + 1) == WHAT_LINK_ENTRY_MAGIC2))
    {
      uint8_t _EEPROM_Link_Location = EEPROM_LINK_LOCATION + 2;
      for(uint8_t _i=0;_i<WHAT_MAX_LINK_ENTRIES;_i++)
      {
        gblLink.entries[_i].output = EEPROM.read(_EEPROM_Link_Location++);
        gblLink.entries[_i].source = EEPROM.read(_EEPROM_Link_Location++) + EEPROM.read(_EEPROM_Link_Location++) * 0x100;
        gblLink.entries[_i].command = EEPROM.read(_EEPROM_Link_Location++) + EEPROM.read(_EEPROM_Link_Location++) * 0x100;
        gblLink.entries[_i].channel = EEPROM.read(_EEPROM_Link_Location++);
        gblLink.entries[_i].flags = EEPROM.read(_EEPROM_Link_Location++);
        gblLink.entries[_i].lastValue = 0xFF;  // RAM only: nothing heard yet this boot, so the first packet counts as a change.
      }
    }
    else
    {
      for(uint8_t _i=0;_i<WHAT_MAX_LINK_ENTRIES;_i++)
      {
        gblLink.entries[_i].output = 0xFF;
        gblLink.entries[_i].source = 0xFFFF;
        gblLink.entries[_i].command = 0;
        gblLink.entries[_i].channel = 0;
        gblLink.entries[_i].flags = 0;
        gblLink.entries[_i].lastValue = 0xFF;
      }
    }
#endif
    
    //Serial.print(F("EE read gblMailbox.networkState:0x"));Serial.println(gblMailbox.networkState);
    // reload all child addresses:
    uint8_t _EEPROM_Child_Location = _EEPROM_Child_LOCATION;
    for(uint8_t _i=1;_i<ChildMaxIndex;_i++)
    { // recover index 0 to the end, even though index 0 actually is used for this device.
      gblNetwork.children[_i].ChildAddress = EEPROM.read(_EEPROM_Child_Location++) + (EEPROM.read(_EEPROM_Child_Location++) * 0x100);
      { // 32-bit latchkey since STEP 1 (Nov 24, 2026) - read byte-by-byte in a loop (the running EEPROM index must only advance once per statement; the multi-read expressions used elsewhere in this function are unsequenced for more than two reads).
        uint32_t _v = 0;
        for(uint8_t _b=0;_b<4;_b++){ _v |= ((uint32_t)EEPROM.read(_EEPROM_Child_Location++) << (8 * _b)); }
        gblNetwork.children[_i].LatchKeyValue = _v;
      }
      gblNetwork.children[_i].RelationshipType = EEPROM.read(_EEPROM_Child_Location++);
      gblNetwork.children[_i].Security = EEPROM.read(_EEPROM_Child_Location++) + (EEPROM.read(_EEPROM_Child_Location++) * 0x100);
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
  #ifdef CHILD_MONITOR
  Serial.print(F("Saving to EEPROM. Starting Address: 0x"));Serial.println(EEPROM_myPANIDL_LOCATION,HEX);
  #endif
  //Serial.print("gblNetwork.panID:");Serial.print(gblNetwork.panID,HEX);
  EEPROM.write(EEPROM_myPANIDL_LOCATION, lowByte(gblNetwork.panID)); EEPROM.write(EEPROM_myPANIDH_LOCATION, highByte(gblNetwork.panID));
  //Serial.print("\t EEmyPANID:");Serial.print((EEPROM.read(EEPROM_myPANIDH_LOCATION)*0x100 + EEPROM.read(EEPROM_myPANIDL_LOCATION)),HEX);
  EEPROM.write(EEPROM_mySourceAddressL_LOCATION, lowByte(mySourceAddress)); EEPROM.write(EEPROM_mySourceAddressH_LOCATION, highByte(mySourceAddress));
  //Serial.print("mySourceAddress:");Serial.println(EEPROM.read(EEPROM_mySourceAddressL_LOCATION) + (EEPROM.read(EEPROM_mySourceAddressL_LOCATION) * 0x100),HEX);
  EEPROM.write(EEPROM_myParentAddressL_LOCATION, lowByte(gblNetwork.parentAddress)); EEPROM.write(EEPROM_myParentAddressH_LOCATION, highByte(gblNetwork.parentAddress));
  //Serial.print("gblNetwork.parentAddress:");Serial.println(EEPROM.read(EEPROM_myParentAddressL_LOCATION) + (EEPROM.read(EEPROM_myParentAddressL_LOCATION) * 0x100),HEX);
  EEPROM.write(EEPROM_myLatchKeyNumberL_LOCATION, (uint8_t)(myLatchKeyNumber & 0xFF)); EEPROM.write(EEPROM_myLatchKeyNumberH_LOCATION, (uint8_t)((myLatchKeyNumber >> 8) & 0xFF)); EEPROM.write(EEPROM_myLatchKeyNumberXL_LOCATION, (uint8_t)((myLatchKeyNumber >> 16) & 0xFF)); EEPROM.write(EEPROM_myLatchKeyNumberXH_LOCATION, (uint8_t)((myLatchKeyNumber >> 24) & 0xFF));  // 32-bit since STEP 1 (Nov 24, 2026)
  //Serial.print("myLatchKeyNumber:");Serial.println(EEPROM.read(EEPROM_myLatchKeyNumberL_LOCATION) + (EEPROM.read(EEPROM_myLatchKeyNumberL_LOCATION) * 0x100),HEX);
//  EEPROM.write(EEPROM_myChildIndex_LOCATION, myChildIndex);
  EEPROM.write(EEPROM_preemie_State_LOCATION, gblMailbox.networkState);
  //Serial.print("gblMailbox.networkState:");Serial.println(EEPROM.read(EEPROM_preemie_State_LOCATION),HEX);
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
  for(uint8_t _i=1;_i<ChildMaxIndex;_i++)
  { // Store index 1 to the end (index 0 is this device - its identity is stored in the fields above). STEP 1 fix (Nov 24, 2026): the bound was 16, which stored a 15th record from children[15] - one PAST the end of the 15-entry array (indices 0..14); the retrieve loop never read that record back (it also stops at ChildMaxIndex), so aligning the bound changes nothing that was ever restored - it only stops reading memory past the array.
    EEPROM.write(_EEPROM_Child_Location++, lowByte(gblNetwork.children[_i].ChildAddress)); EEPROM.write(_EEPROM_Child_Location++, highByte(gblNetwork.children[_i].ChildAddress));
    EEPROM.write(_EEPROM_Child_Location++, (uint8_t)(gblNetwork.children[_i].LatchKeyValue & 0xFF)); EEPROM.write(_EEPROM_Child_Location++, (uint8_t)((gblNetwork.children[_i].LatchKeyValue >> 8) & 0xFF)); EEPROM.write(_EEPROM_Child_Location++, (uint8_t)((gblNetwork.children[_i].LatchKeyValue >> 16) & 0xFF)); EEPROM.write(_EEPROM_Child_Location++, (uint8_t)((gblNetwork.children[_i].LatchKeyValue >> 24) & 0xFF));  // 32-bit since STEP 1 (Nov 24, 2026)
    EEPROM.write(_EEPROM_Child_Location++, gblNetwork.children[_i].RelationshipType); 
    EEPROM.write(_EEPROM_Child_Location++, lowByte(gblNetwork.children[_i].Security)); EEPROM.write(_EEPROM_Child_Location++, highByte(gblNetwork.children[_i].Security));
    //Serial.println(_EEPROM_Child_Location,HEX);
   }
   #ifdef CHILD_MONITOR
   Serial.print(F("End _EEPROM_Location. Adr: 0x"));Serial.println(_EEPROM_Child_Location,HEX);
   #endif
}


void _EEPROM_store_links(void)
{ //Having this as a seperate file allows just the link list to be updated.
#ifdef __WHAT_FULLLINK_h  // This routine should only be used if __WHAT_FULLLINK_h is included.
// Store the link ENTRY table (Nov 27, 2026 - was the peer list; the
// function keeps its name so WHAT_StoreEEPROM and the link code are
// unchanged). Layout: magic (2) + WHAT_MAX_LINK_ENTRIES x 7 bytes.
  uint8_t _EEPROM_Link_Location = EEPROM_LINK_LOCATION;
  EEPROM.write(_EEPROM_Link_Location++, WHAT_LINK_ENTRY_MAGIC1); EEPROM.write(_EEPROM_Link_Location++, WHAT_LINK_ENTRY_MAGIC2);
  for(uint8_t _i=0;_i<WHAT_MAX_LINK_ENTRIES;_i++)
  {
  EEPROM.write(_EEPROM_Link_Location++, gblLink.entries[_i].output);
  EEPROM.write(_EEPROM_Link_Location++, lowByte(gblLink.entries[_i].source)); EEPROM.write(_EEPROM_Link_Location++, highByte(gblLink.entries[_i].source));
  EEPROM.write(_EEPROM_Link_Location++, lowByte(gblLink.entries[_i].command)); EEPROM.write(_EEPROM_Link_Location++, highByte(gblLink.entries[_i].command));
  EEPROM.write(_EEPROM_Link_Location++, gblLink.entries[_i].channel);
  EEPROM.write(_EEPROM_Link_Location++, gblLink.entries[_i].flags);
  }
#endif
}

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
  WHAT_set_pan(0xFFFF);
  WHAT_set_SourceAddress(0xFFFF);
  mySourceAddress = 0xFFFF;
  gblNetwork.parentAddress = 0xFFFF;
  myLatchKeyNumber = 0xFFFFFFFF;
  gblNetwork.panID = 0xFFFF;
//  myChildIndex = 0;    // each child will be indexed into the "gblNetwork.children" array.
  mySecurity = 0xFFFF;
#ifdef __WHAT_FULLLINK_h
  // Reset all link entries (Nov 27, 2026 - was the peer address list).
  for(uint8_t _i=0;_i<WHAT_MAX_LINK_ENTRIES;_i++)
  {
    gblLink.entries[_i].output = 0xFF;
    gblLink.entries[_i].source = 0xFFFF;
    gblLink.entries[_i].command = 0;
    gblLink.entries[_i].channel = 0;
    gblLink.entries[_i].flags = 0;
    gblLink.entries[_i].lastValue = 0xFF;
  }
#endif  
  // reset all child addresses:
  for(uint8_t _i=1;_i<15;_i++)
  { 
    gblNetwork.children[_i].ChildAddress = 0xFFFF;
    gblNetwork.children[_i].LatchKeyValue = 0xFFFFFFFF;
    gblNetwork.children[_i].RelationshipType = 0;
    gblNetwork.children[_i].Security = 0xFFFF;
  }
}

// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#ifdef AUTO_ADDRESSING
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// *  Network creation: - This is all built into WHAT.ino, and enabled by #define AUTO_ADDRESSING

// gblRegistry.reusableSlot 
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
bool gblRxFlags.f_REQUEST_JOIN_NETWORK_03 = false;    // Preemie asking to join network
bool gblRxFlags.f_NETWORK_ALERT_PREEMIE_03 = false;   // Notice from another device that a preemie is trying to join
bool gblRxFlags.f_CHOOSE_NETWORK_05  = false;         // Notice to other networks that a network has accepted this preemie
bool gblRxFlags.f_NETWORK_ALLOW_PREEMIE_04 = false;   // Notice to this preemie that it may accept its parent.
bool f_Rx_PROPOSE_CHILD_NUMBER_07 = false;    // Preemie asking the network for a child number
bool gblRxFlags.f_NETWORK_JOIN_COMPLETE_03 = false;   // Preeemie accepted a proposed parent 
bool gblRxFlags.f_NETWORK_JOIN_ABORT_03 = false;      // User abort of the acceptance process.
bool gblRxFlags.f_REPLY_ACKNOWLEDGE_03 = false;       // While a new network is forming, requests existing network to claim its PANID
*/

void _Control_Net_Light(/*LightColor1*/ uint8_t LC1,/*Light Color 1 time*/ double _Time1,/*LightColor2 */ uint8_t LC2,/*Light Color 2 time*/ double _Time2 )
{
  // Consolidates all of the flash timers into one subroutine.
  //static double gbl_t_flash = millis();
  static uint8_t _st_last_LC1 = WHAT_LIGHT_OFF;
  static uint8_t _st_last_LC2 = WHAT_LIGHT_OFF;
  
  if((_st_last_LC1 != LC1) | (_st_last_LC2 != LC2))
  {
    gbl_t_flash = millis();    // If either color command changes, reset the timer.
    _st_last_LC1 = LC1;
    _st_last_LC2 = LC2;
  }

  if( gbl_t_flash <= millis() )
  {
    if(gblMailbox.networkStateColor != LC1 ){
    gblMailbox.networkStateColor = LC1;
    gbl_t_flash = millis() + _Time1;
    }
    else
    {
      gblMailbox.networkStateColor = LC2;
      gbl_t_flash = millis() + _Time2;
    }
  }
}

// ******************** Rx Messages
// non-reusable message.  different functions when a preemie.
// TOC Individual Recieve messages
void Rx_REQUEST_JOIN_NETWORK_03 (WHAT_Packet _WHAT_Rx_Packet, uint8_t _index)
{
   gblRxFlags.f_REQUEST_JOIN_NETWORK_03 = true;
   // STEP 1 (Nov 24, 2026): cache the searching preemie's temp ID as the
   // HEARD temp ID, so this device's alert / propose / allow can bind
   // themselves to THIS join attempt, and so a searching preemie can run
   // the founding tiebreak against it. (My OWN temp ID lives in
   // gblJoining.tempID and is never overwritten by a reception.) Cached only
   // where the founder/member role makes sense: by a network member, or by
   // a preemie still in its own search state (which may be about to found a
   // network with this preemie). A preemie already waiting on an allow for
   // its own attempt must not have the heard ID overwritten by a third
   // device's search.
   if ((mySourceAddress < 0xFFFF) || (gblMailbox.networkState == WHAT_PREEMIE_NETWORK_SEARCH))
   {
     gblJoining.heardTempID = ((_WHAT_Rx_Packet.payload[_index + 3] * 0x100) + _WHAT_Rx_Packet.payload[_index + 4]);
   }
   // PROPOSED (Nov 25, 2026): a member also latches the search's replacement
   // flag (the trailing flags byte, present when the block length is 6 or
   // more; older searches have no such byte and read as ordinary). Only the
   // member holding a lost record acts on it, and only with its own button.
   if (mySourceAddress < 0xFFFF)
   {
     gblJoining.heardReplaceIntent = ((_WHAT_Rx_Packet.payload[_index] >= 6) && (_WHAT_Rx_Packet.payload[_index + 5] != 0));
   }
      #ifdef DX_ONLY
        Serial.println(F("gblRxFlags.f_REQUEST_JOIN_NETWORK_03"));
        Serial.print(F("gblMailbox.networkState:")); Serial.println(gblMailbox.networkState, DEC);
      #endif

}

// TOC Individual Recieve messages
void Rx_NETWORK_HEARTBEAT_03 (WHAT_Packet _WHAT_Rx_Packet, uint8_t _index)
{
  // STEP 1 (Nov 24, 2026): nothing to do. A heartbeat's whole job is to be
  // heard - RX_WHAT has already reset the network-alive timer on this
  // same-PANID reception before dispatch. This callback exists so the
  // message is registered and handled gracefully. (Later steps hook
  // supervision of security-class devices onto this same reception.)
}

// TOC Individual Recieve messages 
void Tx_REQUEST_ACK_03(void)
{        
    //gblNetwork.panID = (uint16_t) millis();  // Make up a new PANID based on the timer.  This will be a pretty random value.
    WHAT_Packet _WHAT_Tx_Packet;
    _WHAT_Tx_Packet.WHAT_MAC_MESSAGE;
    // Now send the ACK request
    //WHAT_set_pan(gblNetwork.panID);
    _WHAT_Tx_Packet.pan_id = gblNetwork.panID;      // Use the desired PANID so only that PANID will reply (if it is already used)
    _WHAT_Tx_Packet.destination = 0xFFFF;  // Send to ANY device with this PANID.
    _WHAT_Tx_Packet.sourceaddress = 0xFFFF;  // no source address claimed yet, in case another network has this address.
    uint8_t _index = 0;
    _WHAT_Tx_Packet.payload[_index++] = 4; //Total payload length (including this byte) - FIX (Nov 25, 2026): was 3, which cut the 2-byte MID in half on the air (the transmitter sizes the frame from this byte), so no holder could ever match the request and reply. The same numbers were in the original code.
    _WHAT_Tx_Packet.payload[_index++] = 3; //MID length (including this byte) - FIX (Nov 25, 2026): was 2.
    _WHAT_Tx_Packet.payload[_index++] = highByte(REQUEST_ACK_03);
    _WHAT_Tx_Packet.payload[_index++] = lowByte(REQUEST_ACK_03);
    //Serial.print(F("Tx_REQUEST_ACK_03: ")); Serial.print(_WHAT_Tx_Packet.payload[0],HEX);Serial.print(_WHAT_Tx_Packet.payload[1],HEX);Serial.print(_WHAT_Tx_Packet.payload[2],HEX);Serial.print(_WHAT_Tx_Packet.payload[3],HEX);
      WHAT_TransmitPacket(_WHAT_Tx_Packet);
        #ifdef CHILD_MONITOR
        Serial.print(F("Asking for ACK on :0x")); Serial.println(gblNetwork.panID,HEX);
        //Serial.print(F("Undefined Preemie state : ")); Serial.println(gblMailbox.networkState,HEX);
      #endif
}

// TOC Individual Transmit messages
// PROPOSED (Nov 25, 2026): the trunk claim check. The same REQUEST_ACK_03
// message founding uses, but addressed to ONE candidate trunk address on THIS
// network: the MAC receive filter passes directed traffic only to its
// destination, so only a device actually holding that address can hear the
// request, and its REPLY_ACKNOWLEDGE_03 then means "claimed". Silence means
// the trunk address is free.
void Tx_TRUNK_CLAIM_CHECK(uint16_t _candidate)
{
    WHAT_Packet _WHAT_Tx_Packet;
    _WHAT_Tx_Packet.WHAT_MAC_MESSAGE;
    _WHAT_Tx_Packet.pan_id = gblNetwork.panID;      // This network.
    _WHAT_Tx_Packet.destination = _candidate;       // Ask the candidate address itself.
    _WHAT_Tx_Packet.sourceaddress = mySourceAddress;
    uint8_t _index = 0;
    _WHAT_Tx_Packet.payload[_index++] = 4; //Total payload length (including this byte) - FIX (Nov 25, 2026): was 3 (copied from Tx_REQUEST_ACK_03), which cut the 2-byte MID in half on the air; the bench claim check's silence proved nothing until this was fixed.
    _WHAT_Tx_Packet.payload[_index++] = 3; //MID length (including this byte) - FIX (Nov 25, 2026): was 2.
    _WHAT_Tx_Packet.payload[_index++] = highByte(REQUEST_ACK_03);
    _WHAT_Tx_Packet.payload[_index++] = lowByte(REQUEST_ACK_03);
    WHAT_TransmitPacket(_WHAT_Tx_Packet);
    #ifdef CHILD_MONITOR
    Serial.print(F("Tx Trunk Claim Check to: 0x")); Serial.println(_candidate,HEX);
    #endif
}

// TOC Individual Recieve messages 
// gblRegistry.reusableSlot 
void Rx_REPLY_ACKNOWLEDGE_03(WHAT_Packet _WHAT_Rx_Packet, uint8_t _index)
{
// Rx Message flags
  gblRxFlags.f_REPLY_ACKNOWLEDGE_03 = true;       // While a new network is forming, requests existing network to claim its PANID
  gblJoining.ackSourceAddress = _WHAT_Rx_Packet.sourceaddress;  // PROPOSED (Nov 25, 2026): who answered. The trunk claim check matches this against its candidate; founding ignores the field.
    #ifdef CHILD_MONITOR
    Serial.print(F("Recieve ACK from :0x")); Serial.println(_WHAT_Rx_Packet.pan_id);
    //Serial.print(F("Undefined Preemie state : ")); Serial.println(gblMailbox.networkState,HEX);
    #endif
}

// *****************
// TOC Individual Transmit messages
// PROPOSED (Nov 25, 2026): the leaver's side of the abandon handshake, built
// once and called from the reset ritual (and its retries). The proof is
// STEP 1's: WHAT_Mix32(my latchkey, my address, parent address) - the
// latchkey itself never crosses the air.
void Tx_ABANDON_TO_PARENT(void)
{
  WHAT_Packet _WHAT_Tx_Packet;  // Make a temporary transmit packet
  _WHAT_Tx_Packet.WHAT_MAC_MESSAGE;
  _WHAT_Tx_Packet.pan_id = gblNetwork.panID;
  _WHAT_Tx_Packet.destination = gblNetwork.parentAddress;
  _WHAT_Tx_Packet.sourceaddress = mySourceAddress;
  uint8_t _index = 0;
  _WHAT_Tx_Packet.payload[_index++] = 8; //Total payload length (including this byte)
  _WHAT_Tx_Packet.payload[_index++] = 7; //MID length (including this byte)
  _WHAT_Tx_Packet.payload[_index++] = highByte(ABANDON_CHILD_05);
  _WHAT_Tx_Packet.payload[_index++] = lowByte(ABANDON_CHILD_05);
  WHAT_PayloadPut32(_WHAT_Tx_Packet.payload, _index, WHAT_Mix32(myLatchKeyNumber, mySourceAddress, gblNetwork.parentAddress)); _index += 4;
  WHAT_TransmitPacket(_WHAT_Tx_Packet);
}

// *****************
// TOC Individual Recieve messages
// PROPOSED (Nov 25, 2026): my parent has blanked AND saved my child record.
// The reset ritual in WHAT_StateCheck is waiting on this flag.
void Rx_REPLY_ABANDON_CONFIRM_03(WHAT_Packet _WHAT_Rx_Packet, uint8_t _index)
{
  gblRxFlags.f_REPLY_ABANDON_CONFIRM_03 = true;
}

// PROPOSED (Nov 25, 2026): the tail of the reset-to-preemie ritual, shared by
// both exits of the abandon handshake (confirmed, and retries exhausted) so
// the wipe itself can never drift between them.
void WHAT_CompletePreemieReset(void)
{
  WHAT_Default_EEPROM_Values();
  WHAT_StoreEEPROM();

  #ifdef CHILD_MONITOR
  Serial.println(F("Preemie state restored."));
  #endif
  gblMailbox.networkState = WHAT_PREEMIE_RESTORED;  //
  #ifdef CHILD_MONITOR
   Serial.println(F("gblMailbox.networkState: WHAT_PREEMIE_RESTORED ")); Serial.println(WHAT_PREEMIE_RESTORED);
  #endif
  gblScratch.timer2 = millis()+ 1000; // Give happy indication for a second once restored.
  _Control_Net_Light(/*LightColor1*/ WHAT_LIGHT_GREEN,/*Light Color 1 time*/ 50,/*LightColor2 */ WHAT_LIGHT_GREEN,/*Light Color 2 time*/ 50 );
}

// &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
// &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
// &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
// &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

// TOC Adult operations 
void WHAT_StateCheck(void)
{
  #define _RQST_WAIT_TIME 60000 //Wait 30 seconds to allow user to push network button.
  #define _RQST_ACK_DELAY 5000  // wait 5 second after requesting an ACK before using the PANID

  #define _WHAT_NEW_CHILD_TIME_DELAY 60000 // Only stay in new child mode for 30 seconds if not accepted earlier.

  bool default_case = true;    // Each routine will mark this as false so if NONE are executed, default will be true.
  static uint8_t _st_AbandonTries = 0;  // PROPOSED (Nov 25, 2026): how many times the leaver has sent its ABANDON_CHILD_05 in the reset ritual's confirmation wait.
 //Serial.println("memberloop");
  //static double gbl_t_flash = millis();
  //static WHAT_State _last_WHAT_network_State = WHAT_STARTUP;

  //TOC Evaluate WHAT network state
//  switch (gblMailbox.networkState)
//  {
    /**********************************************************************************/
    /**********************************************************************************/
    /**********************************************************************************/
    // Check if this is a preemie or adult
    // Startup is the power on default
      if(gblMailbox.networkState == WHAT_STARTUP )//      case WHAT_STARTUP:
      {
          default_case = false;  // Used to verify that one of the cases executed.
          if( (mySourceAddress < 0xFFFF) | (gblNetwork.parentAddress< 0xFFFF))
          {
            // This device has already been connected to a network and assigned an address.
            // see if the operator is trying to reset it at power up.
            gblMailbox.networkState = CHECK_PREEMIE_RESET;
          //Serial.print(F("gblMailbox.networkState -> CHECK_PREEMIE_RESET")); Serial.println(gblMailbox.networkState);
          }
          else
          {
            // This device has never been connected to a network.
            // Handle it as a preemie.
            // PROPOSED (Nov 25, 2026): unless its network button is being held
            // at power on - the same boot check a member gets (which for a
            // member offers the reset ritual), read on a preemie as "I am
            // available to replace a missing unit." Until now the held button
            // did nothing at all for a preemie.
            if (gblMailbox.networkButton == HIGH) // Button is active high
            {
              gblScratch.timer1 = millis() + 100;  // The button must still be held when this expires (the member check's hold time).
              gblMailbox.networkState = WHAT_REPLACE_INTENT_CHECK;
            }
            else
            {
              gblMailbox.networkState = WHAT_PRE_PREEMIE_NETWORK_SEARCH;
            }
          //Serial.print(F("gblMailbox.networkState -> WHAT_PRE_PREEMIE_NETWORK_SEARCH")); Serial.println(gblMailbox.networkState);
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
    if(gblMailbox.networkState == CHECK_PREEMIE_RESET )//    case CHECK_PREEMIE_RESET:
    {
          default_case = false;  // Used to verify that one of the cases executed.
        #ifdef CHILD_MONITOR
         Serial.println(F("Checking for PreemieReset"));
        #endif
        if (gblMailbox.networkButton == HIGH) // Button is active high
        {
          gblScratch.timer1 = millis() + 100;  // NETWORK button must be held for at least this long.
          gblScratch.timer2 = millis()+ 10000; // NETWORK button must be released and re-pressed before this time expires.
          gblMailbox.networkState = WHAT_RESET_PREMIE_INITIATED;  // Have the network light reflect the potential PREEMIE reset status.
          #ifdef CHILD_MONITOR
            Serial.print(F("gblMailbox.networkState: WHAT_RESET_PREMIE_INITIATED ")); Serial.println(gblMailbox.networkState);
          #endif

        }
        else // Button is not pressed.
        {
          #ifdef CHILD_MONITOR
          Serial.println(F("PreemieReset NOT initiated"));
          #endif

          if( (mySourceAddress < 0xFFFF) | (gblNetwork.parentAddress< 0xFFFF))
          { // Device is NOT a preemie
          gblMailbox.networkState = WHAT_PRE_NET_ALIVE;  //
          //Serial.print(F("gblMailbox.networkStateColor:")); Serial.println(gblMailbox.networkStateColor,DEC);
          //Serial.print(F("gblMailbox.networkState:")); Serial.println(gblMailbox.networkState,DEC);
          }
          else
          { // device is a preemie.  try to find a network.
            gblMailbox.networkState = WHAT_PRE_PREEMIE_NETWORK_SEARCH;  //
          }

        }
      // TOC Handle Light color based on network status
      // No lights during this check.  if the button is pressed at power on, validate user request to re-preemie.
    }//break;

   // ************************************************************************
    // PROPOSED (Nov 25, 2026): a preemie validating a held button at power on.
    // Released early, it was an ordinary start; still held at the deadline, it
    // declares replacement intent and searches with a steady WHITE light
    // instead of the usual cycling.
    if(gblMailbox.networkState == WHAT_REPLACE_INTENT_CHECK )
    {
          default_case = false;  // Used to verify that one of the cases executed.
      if (gblMailbox.networkButton == LOW) // Button released before the hold time - an ordinary preemie start.
      {
        gblMailbox.networkState = WHAT_PRE_PREEMIE_NETWORK_SEARCH;
      }
      else if (gblScratch.timer1 <= millis())
      { // Button held for at least the minimum hold time - replacement intent declared.
        gblJoining.replaceIntent = true;
        #ifdef CHILD_MONITOR
          Serial.println(F("Button held at power on: replacement intent"));
        #endif
        gblMailbox.networkState = WHAT_PRE_PREEMIE_NETWORK_SEARCH;
      }
    // TOC Handle Light color based on network status
    // Steady WHITE while the hold is being read.
      _Control_Net_Light(/*LightColor1*/ WHAT_LIGHT_WHITE,/*Light Color 1 time*/ 50,/*LightColor2 */ WHAT_LIGHT_WHITE,/*Light Color 2 time*/ 50 );
    }//  break;

    if(gblMailbox.networkState == WHAT_RESET_PREMIE_INITIATED )//    case WHAT_RESET_PREMIE_INITIATED:   // Button was held at
    {
          default_case = false;  // Used to verify that one of the cases executed.
      if (gblMailbox.networkButton == HIGH) // Button is active high
      {
        if(gblScratch.timer1 <= millis() )
        { // Button was held for at least the minimum hold time.
          // Transition for waiting for it to be released.
          //gblScratch.timer2 <= millis() + 2000;
         #ifdef CHILD_MONITOR
         Serial.print(F("gblMailbox.networkState: WHAT_RESET_PREMIE_PENDING ")); Serial.println(WHAT_RESET_PREMIE_PENDING);
        #endif
          gblMailbox.networkState = WHAT_RESET_PREMIE_PENDING;

        }
      }
      else
      {
           gblMailbox.networkState = WHAT_PRE_NET_ALIVE;  //
      }
    // TOC Handle Light color based on network status
    // Network button held at power on - cycle while waiting for the button to be held long enough.
    // No light during this initial press time.
      //gblMailbox.networkStateColor = WHAT_LIGHT_OFF;
      _Control_Net_Light(/*LightColor1*/ WHAT_LIGHT_OFF,/*Light Color 1 time*/ 50,/*LightColor2 */ WHAT_LIGHT_OFF,/*Light Color 2 time*/ 50 );

      }//  break;
    
    if(gblMailbox.networkState == WHAT_RESET_PREMIE_PENDING )//     case WHAT_RESET_PREMIE_PENDING:
    {
          default_case = false;  // Used to verify that one of the cases executed.
      // Minimum press time established... the button must now be RELEASED before continuing.
      if (gblMailbox.networkButton == HIGH) // Button is active high
      { // ensure max timer has not been exceeded while the button is being held.
       if(gblScratch.timer2 <= millis() )
        { // Button was never released.
          // The network button is not functioning properly.
          #ifdef CHILD_MONITOR
            Serial.println(F("Network Button was never released. "));
            #endif
           gblMailbox.networkState = WHAT_PRE_NETWORK_ERROR;  //
        }
      }
      else
      { // the button was release before the timer expired.  Now it must be pressed again to restore the preemie.
           gblMailbox.networkState = WHAT_RESET_PREMIE_AWAITING_ACTIVATION;
        #ifdef CHILD_MONITOR
         Serial.println(F("gblMailbox.networkState: WHAT_RESET_PREMIE_AWAITING_ACTIVATION ")); Serial.println(WHAT_RESET_PREMIE_AWAITING_ACTIVATION);
        #endif
      }
    // TOC Handle Light color based on network status
    // Network button held at power on - cycle colors while waiting for the button to be released.
      if( gbl_t_flash <= millis() ){
        switch( gblMailbox.networkStateColor)
        {
          case WHAT_LIGHT_BLUE:
            gblMailbox.networkStateColor = WHAT_LIGHT_GREEN;
          break; 
          case WHAT_LIGHT_GREEN:
            gblMailbox.networkStateColor = WHAT_LIGHT_RED;
          break; 
          case WHAT_LIGHT_RED:
            gblMailbox.networkStateColor = WHAT_LIGHT_OFF;
          break; 
          default:
            gblMailbox.networkStateColor = WHAT_LIGHT_BLUE;
          break; 
        }
        gbl_t_flash = millis() + 200;
      }
    }//  break;
    
    if(gblMailbox.networkState == WHAT_RESET_PREMIE_AWAITING_ACTIVATION )//    case WHAT_RESET_PREMIE_AWAITING_ACTIVATION:
    {
          default_case = false;  // Used to verify that one of the cases executed.
      // The button is working - it was pressed and released.  When pressed again, reset to preemie.
      if (gblMailbox.networkButton == HIGH) // Button is active high
      { //The button was pressed again.  Reset to PREEMIE mode.
        #ifdef CHILD_MONITOR
        Serial.println(F("Reset Preemie criteria complete"));
        #endif
        // preemie creation criteria met.   restore to preemie state, and continue with program initialization
        // PROPOSED (Nov 25, 2026): the abandon is now a handshake, not a
        // fire-and-forget. Send ABANDON_CHILD_05 (proof of the latchkey, never
        // the latchkey itself), then WAIT in WHAT_RESET_PREMIE_AWAITING_CONFIRM
        // for the parent's REPLY_ABANDON_CONFIRM_03 - which the parent sends
        // only after blanking the slot AND saving it to EEPROM - before
        // wiping this device. Bounded retries, then the wipe happens anyway:
        // leaving is a local act and is never hostage to the parent.
        Tx_ABANDON_TO_PARENT();
        gblRxFlags.f_REPLY_ABANDON_CONFIRM_03 = false;   // Clear the message flag so only a fresh confirmation counts.
        _st_AbandonTries = 1;
        gblScratch.timer1 = millis() + 2000; // Time to wait for the confirmation before re-sending.
        gblMailbox.networkState = WHAT_RESET_PREMIE_AWAITING_CONFIRM;  //
        #ifdef CHILD_MONITOR
         Serial.println(F("gblMailbox.networkState: WHAT_RESET_PREMIE_AWAITING_CONFIRM ")); Serial.println(WHAT_RESET_PREMIE_AWAITING_CONFIRM);
        #endif

      }// End if the button was pressed.
      else
      { // If the button is never pressed again, have the restore time out.
      if(gblScratch.timer2 <= millis() )
        { // Button was never released.
          // The network button is not functioning properly.
          #ifdef CHILD_MONITOR
          Serial.println(F("Network Button -not- re-pressed"));
          #endif
           gblMailbox.networkState = WHAT_PRE_NETWORK_ERROR;  //
        }
      }
    // TOC Handle Light color based on network status
    // Preemie reset started. Pressing again activates.  fast flash green until activated.
      _Control_Net_Light(/*LightColor1*/ WHAT_LIGHT_GREEN,/*Light Color 1 time*/ 50,/*LightColor2 */ WHAT_LIGHT_OFF,/*Light Color 2 time*/ 50 );
    }//  break;

   // ************************************************************************
    // PROPOSED (Nov 25, 2026): the leaver waits for its abandon to be
    // confirmed. The parent confirms only after the slot is blanked AND saved,
    // so a confirmed leaver knows its old address is genuinely free - for a
    // replacement, or for a trunk re-offer - before it wipes itself.
    if(gblMailbox.networkState == WHAT_RESET_PREMIE_AWAITING_CONFIRM )
    {
          default_case = false;  // Used to verify that one of the cases executed.
      if (gblRxFlags.f_REPLY_ABANDON_CONFIRM_03)
      { // The parent has released the slot, durably. Wipe and restore preemie state.
        gblRxFlags.f_REPLY_ABANDON_CONFIRM_03 = false; // clear the message flag when it is used.
        #ifdef CHILD_MONITOR
          Serial.println(F("Abandon confirmed by parent"));
        #endif
        WHAT_CompletePreemieReset();
      }
      else if (gblScratch.timer1 <= millis())
      { // No confirmation yet. Re-send a bounded number of times, then reset anyway.
        if (_st_AbandonTries < 3)
        {
          Tx_ABANDON_TO_PARENT();
          _st_AbandonTries++;
          gblScratch.timer1 = millis() + 2000;
          #ifdef CHILD_MONITOR
            Serial.println(F("No abandon confirmation yet - re-sent"));
          #endif
        }
        else
        {
          #ifdef CHILD_MONITOR
            Serial.println(F("No abandon confirmation - resetting anyway"));
          #endif
          WHAT_CompletePreemieReset();
        }
      }
    // TOC Handle Light color based on network status
    // Same fast green flash as the activation wait.
      _Control_Net_Light(/*LightColor1*/ WHAT_LIGHT_GREEN,/*Light Color 1 time*/ 50,/*LightColor2 */ WHAT_LIGHT_OFF,/*Light Color 2 time*/ 50 );
    }//  break;

    if(gblMailbox.networkState == WHAT_PREEMIE_RESTORED )//     case WHAT_PREEMIE_RESTORED:
    {
          default_case = false;  // Used to verify that one of the cases executed.
      if(gblScratch.timer2 <= millis() )
        { // Preemie restoration is complete, and indication was given to user.
           gblMailbox.networkState = WHAT_PRE_PREEMIE_NETWORK_SEARCH;  //
        }
    // TOC Handle Light color based on network status
    // Preemie state is restored. Hold the light green as an indication.
        //gblMailbox.networkStateColor = WHAT_LIGHT_GREEN; 
        _Control_Net_Light(/*LightColor1*/ WHAT_LIGHT_GREEN,/*Light Color 1 time*/ 50,/*LightColor2 */ WHAT_LIGHT_GREEN,/*Light Color 2 time*/ 50 );
      }//  break;

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
// TOC PREEMIE operations 
      // ++++++++++++   New preemie has not seen a network yet     ++++++++++++
    if(gblMailbox.networkState == WHAT_PRE_PREEMIE_NETWORK_SEARCH )//       case WHAT_PRE_PREEMIE_NETWORK_SEARCH:
    {
          default_case = false;  // Used to verify that one of the cases executed.
          //The preemie trying to join the network. Use the reusable message to watch for another network.
          // gblMailbox.networkStateColor = WHAT_LIGHT_OFF;
          _Control_Net_Light(/*LightColor1*/ WHAT_LIGHT_OFF,/*Light Color 1 time*/ 50,/*LightColor2 */ WHAT_LIGHT_OFF,/*Light Color 2 time*/ 50 );

           gblRxFlags.f_NETWORK_ALERT_PREEMIE_03 = false;   // Clear the message reception flag.
            gblRxFlags.f_REQUEST_JOIN_NETWORK_03 = false;    // Clear the message reception flag.
            gblJoining.tempID = (uint16_t)WHAT_Random32();  // STEP 1 (Nov 24, 2026): a fresh temp ID for this join attempt - it rides in every search broadcast and must be echoed by the allow, the offers, and the accept.
            gbl_t_flash = millis();
            gblMailbox.networkState = WHAT_PREEMIE_NETWORK_SEARCH;  // request joining any network that might be availble.

          #ifdef CHILD_MONITOR
          Serial.print(F("gblMailbox.networkState: WHAT_PREEMIE_NETWORK_SEARCH ")); Serial.println(WHAT_PREEMIE_NETWORK_SEARCH);
          #endif
    }//    break;      

    if(gblMailbox.networkState == WHAT_PREEMIE_NETWORK_SEARCH )//       case WHAT_PREEMIE_NETWORK_SEARCH:
    {
          default_case = false;  // Used to verify that one of the cases executed.
        // TOC PREEMIE operations 
                // New device.  request joining any network that might be availble.
                // This is a "preemie" - this device is hoping to join a network, and repeatedly sends a request to join.
          if( gbl_t_flash <= millis() ) // If flash timer expired...
          {
    // TOC    Request Joining Network 
              // Tx.PANID and Tx.SOURCEADDRESS are defaulted to 0xFFFF
              WHAT_Packet _WHAT_Tx_Packet;  // Make a temporary transmit packet
              _WHAT_Tx_Packet.WHAT_MAC_MESSAGE; 
              //WHAT_set_pan(gblNetwork.panID);
              //WHAT_set_SourceAddress(mySourceAddress);
              _WHAT_Tx_Packet.pan_id = 0xFFFF;
              _WHAT_Tx_Packet.destination = 0xFFFF;
              _WHAT_Tx_Packet.sourceaddress = 0xFFFF;
              uint8_t _index = 0;
              _WHAT_Tx_Packet.payload[_index++] = 7; //Total payload length (including this byte) - PROPOSED (Nov 25, 2026): was 6 before the flags byte was appended.
              _WHAT_Tx_Packet.payload[_index++] = 6; //MID length (including this byte) - PROPOSED (Nov 25, 2026): was 5.
              _WHAT_Tx_Packet.payload[_index++] = highByte(REQUEST_JOIN_NETWORK_03);
              _WHAT_Tx_Packet.payload[_index++] = lowByte(REQUEST_JOIN_NETWORK_03);
              _WHAT_Tx_Packet.payload[_index++] = highByte(gblJoining.tempID);  // STEP 1 (Nov 24, 2026): this attempt's temp ID.
              _WHAT_Tx_Packet.payload[_index++] = lowByte(gblJoining.tempID);
              _WHAT_Tx_Packet.payload[_index++] = gblJoining.replaceIntent ? 1 : 0;  // PROPOSED (Nov 25, 2026): flags byte - 0x01 = this preemie offers itself as a replacement for a lost child (button held at power on). Appended at the end, so older builds reading the temp ID at its fixed offset are unaffected.
              WHAT_TransmitPacket(_WHAT_Tx_Packet);
              #ifdef CHILD_MONITOR
              Serial.println(F("Tx WHAT_PREEMIE_NETWORK_SEARCH ")); Serial.println(millis());
              #endif
 
               // TOC Handle Light color based on network status
            // Preemie state Looking for available network.  Toggle blue on/off while waiting.
            // Toggle occurs at the same time as a new message is transmitted (once per second)
            // PROPOSED (Nov 25, 2026): a replacement-intent preemie shows steady
            // WHITE instead of the toggle - "I am available to replace a
            // missing unit" - so the operator can tell the two searches apart
            // from across the room.
              if (gblJoining.replaceIntent)
              {
                gblMailbox.networkStateColor = WHAT_LIGHT_WHITE;
              }
              else if(gblMailbox.networkStateColor != WHAT_LIGHT_BLUE )
              {
                gblMailbox.networkStateColor = WHAT_LIGHT_BLUE;
              }
              else
              {
                gblMailbox.networkStateColor = WHAT_LIGHT_OFF;
              }
              gbl_t_flash = millis() + 2000;
           } // End If Flash timer expired

        // ooo One way out is to form a network ooo
        // TOC    Creating a new network 
            if(gblRxFlags.f_REQUEST_JOIN_NETWORK_03)
            { // Another preemie is looking for a network, and this device has not yet joined a network.
              gblRxFlags.f_REQUEST_JOIN_NETWORK_03 = false; // clear the message flag when it is used.
              // STEP 1 TIEBREAK (Nov 24, 2026): if both preemies are still searching when they
              // hear each other, each is 'first to receive' from its own chair, and both would
              // found - seen on the bench Nov 24, 2026, when two fresh uploads founded PANIDs
              // 1F81 and 006B in the same second. The temp ID breaks the symmetry: the LOWER
              // temp ID founds, the other keeps searching and joins the winner's network the
              // normal way. Equal temp IDs (identical devices booted alike can draw the same
              // first random value) re-roll and sit the round out; the next exchange compares
              // fresh values, with the random pools by then stirred apart by radio noise.
              bool _ThisDeviceFounds = true;
              if (gblJoining.heardTempID == gblJoining.tempID)
              {
                gblJoining.tempID = (uint16_t)WHAT_Random32();  // Re-roll my temp ID.
                _ThisDeviceFounds = false;
                #ifdef CHILD_MONITOR
                  Serial.println(F("Temp ID matches other preemie - re-rolled mine, still searching"));
                #endif
              }
              else if (gblJoining.heardTempID < gblJoining.tempID)
              {
                _ThisDeviceFounds = false;  // The other preemie holds the lower temp ID - it founds.
                #ifdef CHILD_MONITOR
                  Serial.println(F("Other preemie has lower temp ID - it founds, still searching"));
                #endif
              }
              if (_ThisDeviceFounds)
              {
              // Form a new network.
              gblNetwork.panID = (uint16_t) millis();  // Make up a new PANID based on the timer.  This will be a pretty random value.
              WHAT_set_pan(gblNetwork.panID);
          
              //prepare to recieve an ACK request from the chosen PANID. If other devices respond, then this address is already used.
              //re-configure the reusable message to receive an ACK from an existing network.
              gblRegistry.message[gblRegistry.reusableSlot].MessageID = REPLY_ACKNOWLEDGE_03;
              gblRegistry.message[gblRegistry.reusableSlot].callback = &Rx_REPLY_ACKNOWLEDGE_03; // Message callback set here to collect the responses
             gblMailbox.networkStateColor = WHAT_LIGHT_OFF;
              gblRxFlags.f_REPLY_ACKNOWLEDGE_03 = false;    // Clear the message flag so only new messages will be used.
              // TOC    Avoiding network conflicts 
                // Now send the ACK request
                // If anything replies, then this network already exists, and a new PANID will be chosen.
                Tx_REQUEST_ACK_03();    // Request an ACK to see if any other device is using this PANID.
                      
                  // Start a delay timer.  If it expires with no ACK reply, this PANID is acceptable.
                gblScratch.timer1 = millis() + _RQST_ACK_DELAY;
                #ifdef CHILD_MONITOR
                Serial.print(F("Creating Network @ ")); Serial.println(gblNetwork.panID, HEX);
                #endif
                gblMailbox.networkState = WHAT_NETWORK_CREATING;  //Creating network, and delaying to see if this PANID is already used.
              } // End if this device won the founding tiebreak.
            }
                // ooo another way out is to be allowed into an existing network ooo
        // TOC    Network identified, waiting to be allowed 
                 if(gblRxFlags.f_NETWORK_ALERT_PREEMIE_03)
                {
                  gblRxFlags.f_NETWORK_ALERT_PREEMIE_03 = false; // clear the message flag when it is used.
                  gblMailbox.networkState = WHAT_NETWORK_SEEN;
                  #ifdef CHILD_MONITOR
                    Serial.print(F("A Network Seen: ")); //Serial.println(millis(), HEX);
                  #endif
               }
      }//  break;

     // ++++++++++++   Preemie is forming a network      ++++++++++++
     if(gblMailbox.networkState == WHAT_NETWORK_CREATING )//     case WHAT_NETWORK_CREATING:
     {
          default_case = false;  // Used to verify that one of the cases executed.
        // TOC    Creating a new network 
      // This device created a network, and sent an ACK request to verify its PANID is not used.
      // wait here for a period of time, and if no reply arrives, then make the network.
      // A reusable message is monitored for this reception. It was registered in "WHAT_PREEMIE_NETWORK_SEARCH".
      //gblRegistry.message[gblRegistry.reusableSlot].MessageID = REPLY_ACKNOWLEDGE_03;
      //gblRegistry.message[gblRegistry.reusableSlot].callback = &Rx_REPLY_ACKNOWLEDGE_03; // Message callback set here to collect the responses
      if( gblRxFlags.f_REPLY_ACKNOWLEDGE_03)
        {// Ack seen.  try a different address.
          // As a preemie converting to a network creator, this device will send a
          // request for ACK to see if the PANID it chose is already used.
          // So, this message should NOT be recieved, (because to get to this point,
          //  2 preemies are within range of each other, and they have not been accepted into another
          //  network, and the new PANID is randomly chosen, so it will probably not match.
          // But if it DOES happen, just try a different PANID.
          gblRxFlags.f_REPLY_ACKNOWLEDGE_03 = false; // clear the message flag when it is used.
          gblNetwork.panID = (uint16_t) millis();  // Make up a -new- PANID based on the timer.  This will be a pretty random value.
          WHAT_set_pan(gblNetwork.panID);
          Tx_REQUEST_ACK_03();    // Request an ACK to see if any other device is using this PANID.
      
          // Start a delay timer.  If it expires with no ACK reply, this PANID is acceptable.
          gblScratch.timer1 = millis() + _RQST_ACK_DELAY;
        #ifdef CHILD_MONITOR
        Serial.print(F("Trying new PANID : ")); Serial.println(millis());
        #endif
        }
        if (gblScratch.timer1 <= millis())
        {
          //Use the current PANID and leave PREEMIE mode.
          //Serial.print(gblScratch.timer1); Serial.print(" <- Timer     millis-> "); Serial.println(millis());
          #ifdef CHILD_MONITOR
          Serial.print(F("Network Created. PANID: 0x")); Serial.println(gblNetwork.panID, HEX);
          #endif
          //WHAT_Default_EEPROM_Values();
          mySourceAddress = 0;  // When forming a new network, start with the address 0x0000
          gblNetwork.parentAddress = 0;  // This is the only device that is its own parent.
          myLatchKeyNumber = WHAT_Random32();  // STEP 1 (Nov 24, 2026): was (uint16_t)millis() - a 16-bit value anyone could guess from uptime. This device will make its own latchkey number (which really can not be used, because no one will know it.
          //myChildIndex = 0;             // When forming a new network, this device will start with no children.
          mySecurity = myLatchKeyNumber; // use this device's latchkey number as the base security number.
          WHAT_set_SourceAddress(mySourceAddress);
          gblJoining.replaceIntent = false;  // PROPOSED (Nov 25, 2026): a founder is nobody's replacement - the intent ends here.

          //Serial.print("Pre-Store PANID:0x"); Serial.println(gblNetwork.panID);
          WHAT_StoreEEPROM();
          #ifdef CHILD_MONITOR
          Serial.println(F("EEPROM Stored"));
          #endif

          //gblMailbox.networkState = WHAT_PRE_NET_ALIVE; // Control will transition to the program loop from here.
          gblScratch.timer1 = millis()+ 1000; // This will hold the indication of successful creation.
          gblMailbox.networkState = WHAT_NETWORK_CREATED; //This device created a network.
        }

        // TOC Handle Light color based on network status
        // This node is creating a network and becoming node 0000.
        // Fast blue blink is the indication.
       _Control_Net_Light(/*LightColor1*/ WHAT_LIGHT_BLUE,/*Light Color 1 time*/ 100,/*LightColor2 */ WHAT_LIGHT_OFF,/*Light Color 2 time*/ 100 );

       }//  break;

      // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  
      // ++++++++++++  Network was created by this device  ++++++++++++
     if(gblMailbox.networkState == WHAT_NETWORK_CREATED )//      case  WHAT_NETWORK_CREATED:
     {
           default_case = false;  // Used to verify that one of the cases executed.
          //gblMailbox.networkStateColor = WHAT_LIGHT_GREEN;
          _Control_Net_Light(/*LightColor1*/ WHAT_LIGHT_GREEN,/*Light Color 1 time*/ 1000,/*LightColor2 */ WHAT_LIGHT_GREEN,/*Light Color 2 time*/ 1000 );

          if( gblScratch.timer1 <= millis() )  //THis is set in "WHAT_NETWORK_CREATING"
          {

            _Control_Net_Light(/*LightColor1*/ WHAT_LIGHT_OFF,/*Light Color 1 time*/ 10,/*LightColor2 */ WHAT_LIGHT_OFF,/*Light Color 2 time*/ 10 );
           gblMailbox.networkState = WHAT_PRE_NET_ALIVE;  // After creation has been indicated, go to live operations.
          }
      }//  break;

      // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  
      // ++++++++++++   Preemie is joining a network      ++++++++++++
      if(gblMailbox.networkState == WHAT_NETWORK_SEEN )//      case WHAT_NETWORK_SEEN:
      {
          default_case = false;  // Used to verify that one of the cases executed.
        // TOC    Network identified, waiting to be allowed 
              gblScratch.timer1 = millis() + 60000; // During this time, collects offered child addresses and waits for network button press.
              #ifdef CHILD_MONITOR
              Serial.println(F("PREEMIE sees network : ")); //Serial.println(millis());
              #endif
              // This is the initial run through for this state.
              // Keep an eye out for offered child slots.
              gblJoining.panID = 0xFFFF; // These are collected in  Rx_PROPOSE_CHILD_NUMBER_07 to
              gblJoining.sourceAddress = 0xFFFF; // identify the lowest offered address seen.
              gblJoining.latchkey = 0xFFFFFFFF;
              gblJoining.replaceOfferReceived = false; // PROPOSED (Nov 25, 2026): a fresh collection round - no replacement offer taken yet.
              gblRegistry.message[gblRegistry.reusableSlot].MessageID = NETWORK_ALLOW_PREEMIE_04;  // these are prospective parents offering free addresses.
              gblRegistry.message[gblRegistry.reusableSlot].callback = &Rx_NETWORK_ALLOW_PREEMIE_04; // Message callback set here to collect the responses
             gblMailbox.networkStateColor = gblJoining.replaceIntent ? WHAT_LIGHT_WHITE : WHAT_LIGHT_OFF;  // PROPOSED FIX (Nov 26, 2026): keep the replacement WHITE unbroken through this entry pass too.
             #ifdef CHILD_MONITOR
                Serial.print(F("gblRegistry.reusableSlot: ")); Serial.println(gblRegistry.reusableSlot);
                //Serial.print(F("Response Timeout : ")); Serial.println(millis());
              #endif
               gblRxFlags.f_NETWORK_ALLOW_PREEMIE_04 = false;   // Clear the message reception flag.

              gblMailbox.networkState = WHAT_NETWORK_SEEN_WAIT;
   
      }//   break;

       if(gblMailbox.networkState == WHAT_NETWORK_SEEN_WAIT )//     case WHAT_NETWORK_SEEN_WAIT:
       {
          default_case = false;  // Used to verify that one of the cases executed.
            if (gblScratch.timer1 <= millis())
            {
              // When the LONG timeout occurs, revert to the request state.
              gblMailbox.networkState = WHAT_PRE_PREEMIE_NETWORK_SEARCH;
              #ifdef CHILD_MONITOR
              Serial.print(F("Network Button not pressed : ")); Serial.println(millis());
              #endif
            }
            if(gblRxFlags.f_NETWORK_ALLOW_PREEMIE_04)
            {// This preemie has been accepted to a network.
              // Get a parent address from the new network.
              //Serial.println(F("Preemie gblRxFlags.f_NETWORK_ALLOW_PREEMIE_04"));
              gblRxFlags.f_NETWORK_ALLOW_PREEMIE_04 = false; // clear the message flag when it is used.
              #ifdef CHILD_MONITOR
                Serial.print(F("Offered PANID:0x")); Serial.println(gblJoining.panID,HEX);
                //Serial.print(F("Response Timeout : ")); Serial.println(millis());
              #endif
               if (gblJoining.panID < 0xFFFF) // Only do this if a PANID was offered.
                {
                  // Other network members on this network seeing the NETWORK_ALLOW_PREEMIE_04 will send a proposed child address.
                  // note that the allowing node will send a proposed child number in the same messages as the allow message.
                  //  because of that, the _offered values must be reset prior to being allowed. (usually done in Network seen)
                  gblMailbox.networkState = WHAT_PRE_NETWORK_ALLOW_PREEMIE_04; // A valid PAN ID was offered.
                 }
            }

                // If an abort message is recieved, return to the beginning.
                if(gblRxFlags.f_NETWORK_JOIN_ABORT_03) 
                {
                  gblRxFlags.f_NETWORK_JOIN_ABORT_03 = false; // clear the message flag when it is used.
                  //gblMailbox.networkState = WHAT_PRE_REQUEST_JOIN_NETWORK_03; // If nothing offered a child slot, return to being a preemie.
                  gblMailbox.networkState = WHAT_PRE_NETWORK_ERROR; //Give the lamp color/text that indicates that the connection was aborted. (FIX, Nov 27, 2026: moved out of the #ifdef CHILD_MONITOR below - a state change must not depend on a monitor switch; same defect species as the FullLink REJECT fix. The print stays gated.)
                #ifdef CHILD_MONITOR
                  //Serial.print(F("Offered PANID:0x")); Serial.println(gblJoining.panID);
                  Serial.println(F("gblRxFlags.f_NETWORK_JOIN_ABORT_03")); 
                #endif
                }
                // TOC Handle Light color based on network status
                // This node is joining an existing network.
                // blue/green alternation is the indication.
                // PROPOSED FIX (Nov 26, 2026): a replacement-intent preemie
                // spends this whole wait here, not in the search state - the
                // green/blue alternation repainted the lamp every pass and
                // the steady WHITE set in the search state never survived
                // long enough to be seen (bench, Nov 26, 2026). Hold WHITE
                // through the wait as well; the join states take the lamp
                // back over once an offer is actually accepted.

       if (gblJoining.replaceIntent)
       {
         // REPLACEMENT PAIRING (Nov 26, 2026, Gary): once a network is in
         // earshot, a replacement-intent preemie alternates WHITE/RED - the
         // same pairing pattern the lost child's parent shows - instead of
         // the steady WHITE of its blind search.
         _Control_Net_Light(/*LightColor1*/ WHAT_LIGHT_WHITE,/*Light Color 1 time*/ 100,/*LightColor2 */ WHAT_LIGHT_RED,/*Light Color 2 time*/ 100 );
       }
       else
       {
         _Control_Net_Light(/*LightColor1*/ WHAT_LIGHT_GREEN,/*Light Color 1 time*/ 100,/*LightColor2 */ WHAT_LIGHT_BLUE,/*Light Color 2 time*/ 100 );
       }

      }// break;
        
      // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  
      // ++++++++++++  Preemie is waiting for a proposed ID    ++++++++++++
       if(gblMailbox.networkState == WHAT_PRE_NETWORK_ALLOW_PREEMIE_04 )//       case WHAT_PRE_NETWORK_ALLOW_PREEMIE_04:
       {
          default_case = false;  // Used to verify that one of the cases executed.
          gblScratch.timer1 = millis() + 5000;   // Wait this amount of time before accepting the lowest offered source address.
          gblMailbox.networkState = WHAT_NETWORK_JOINING;
          #ifdef CHILD_MONITOR
          Serial.print(F("gblMailbox.networkState = WHAT_NETWORK_JOINING")); Serial.println(gblMailbox.networkState,DEC);
          #endif
       }//break;

      // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  
        if(gblMailbox.networkState == WHAT_NETWORK_JOINING )//     case WHAT_NETWORK_JOINING:
        {
           default_case = false;  // Used to verify that one of the cases executed.
        // TOC    Accepted to existing netowrk 
            // A button on the network has permitted this preemie to join.
            // This preemie waits to allow several proposals, then responds with acceptance of the lowest proposed address.
            // the re-usable timer set in "WHAT_PRE_NETWORK_ALLOW_PREEMIE_04" will serve as a delay while the child proposal addresses are collected.
              if (gblScratch.timer1 <= millis()) //Wait a short time after permitted to join to get more proposed addresses.
              {
                //Serial.println(F("Allow Preemie Timer Expired"));
                //Serial.print(F("Offered PANID,PARENT,CHILD:"));Serial.print(gblJoining.panID,HEX);Serial.print(",");Serial.print(gblJoining.parent,HEX);Serial.print(",");Serial.println(gblJoining.sourceAddress,HEX);
               #ifdef CHILD_MONITOR
                Serial.print(F("Joining. Offered PANID:0x")); Serial.println(gblJoining.panID,HEX);
                //Serial.print(F("Response Timeout : ")); Serial.println(millis());
              #endif
               if(gblJoining.sourceAddress < 0xFFFF)
                {  
                  // FIX (Nov 24, 2026): the identity is NOT committed here anymore. The offered
                  // PANID/address/parent stay tentative in gblJoining until the parent confirms
                  // with NETWORK_JOIN_COMPLETE_03 (see WHAT_CONFIRM_NEW_CHILD). If confirmation
                  // never comes, this device is still a preemie (identity FFFF, EEPROM untouched)
                  // and the abort path returns it to preemie search on its own.
                  WHAT_Packet _WHAT_Tx_Packet;  // Make a temporary transmit packet
                  _WHAT_Tx_Packet.WHAT_MAC_MESSAGE;
                  _WHAT_Tx_Packet.pan_id = gblJoining.panID;
                  _WHAT_Tx_Packet.sourceaddress = gblJoining.sourceAddress;
                  myLatchKeyNumber = WHAT_Random32();  // STEP 1 (Nov 24, 2026): was (uint16_t)millis(). Create a random 32-bit latch-key number (tentative until confirmed)
                  gblJoining.confirmNonce = WHAT_Random32();  // STEP 1: the parent must echo a proof over this nonce in NETWORK_JOIN_COMPLETE_03 - proof it stored the latchkey, without re-sending it.
                  //myChildIndex = 0;   // Just in case for some reason this was not already 0 (like if this device was removed from another network).
                  // Now send a message to confirm which parent was chosen.
                  //ACCEPT_PARENT_06
                  _WHAT_Tx_Packet.pan_id = gblJoining.panID;
                  _WHAT_Tx_Packet.sourceaddress = gblJoining.sourceAddress;
                  _WHAT_Tx_Packet.destination = gblJoining.parent;   // Direct this message at the parent that accepted it.
                  // ACCEPT_PARENT_06         0xF005 // 1 byte length, 2 byte command, 2 byte parent, 4 byte latchkey, 1 byte relation type, 4 byte confirm nonce, 2 byte temp ID // preemie accepts offer from parent, and proposes latch key value (STEP 1, Nov 24, 2026: randomly generated, 32-bit - was derived from its timer at 16 bits)
                  uint8_t _index = 0;
                  _WHAT_Tx_Packet.payload[_index++] = 17; //Total payload length (including this byte)
                  _WHAT_Tx_Packet.payload[_index++] = 16; //MID length (including this byte)
                  _WHAT_Tx_Packet.payload[_index++] = highByte(ACCEPT_PARENT_06);   // 1 byte length, 2 byte command, 2 byte child address, 2 byte latchkey // Any device that has open child slots replies to request (REQUEST_JOIN_NETWORK_02).
                  _WHAT_Tx_Packet.payload[_index++] = lowByte(ACCEPT_PARENT_06);  //in order to support multibyte MIDs, put the high byte first.
                  _WHAT_Tx_Packet.payload[_index++] = highByte(gblJoining.parent); // FIX (Nov 24, 2026): the parent field the documented format calls for was never sent - the parent read the latchkey as the parent address and rejected the join.
                  _WHAT_Tx_Packet.payload[_index++] = lowByte(gblJoining.parent);
                  WHAT_PayloadPut32(_WHAT_Tx_Packet.payload, _index, myLatchKeyNumber); _index += 4;  // STEP 1 (Nov 24, 2026): the 32-bit latchkey. This is its ONE transmission - inside this button-gated join.
                  _WHAT_Tx_Packet.payload[_index++] = WHAT_PERMANENT;           // All devices are permanent at this point.
                  WHAT_PayloadPut32(_WHAT_Tx_Packet.payload, _index, gblJoining.confirmNonce); _index += 4;  // STEP 1: confirm nonce - the parent's JOIN_COMPLETE proof is computed over this.
                  _WHAT_Tx_Packet.payload[_index++] = highByte(gblJoining.tempID);  // STEP 1: temp ID echo - the parent verifies it matches the search it heard.
                  _WHAT_Tx_Packet.payload[_index++] = lowByte(gblJoining.tempID);
                      WHAT_TransmitPacket(_WHAT_Tx_Packet);

                  // FIX (Nov 24, 2026): no WHAT_StoreEEPROM() here - the identity is only
                  // committed (globals + EEPROM) when the join is confirmed.
                  #ifdef CHILD_MONITOR
                  Serial.println(F("Tx Parent Chosen"));
                  Serial.println(F("PANID\tmySourceAddress\tmyLatchKeyNumber\tmyParentAddress"));
                  Serial.print(gblJoining.panID,HEX);Serial.print(F("\t"));Serial.print(gblJoining.sourceAddress,HEX);Serial.print(F("\t"));Serial.print(myLatchKeyNumber,HEX);Serial.print(F("\t"));Serial.println(gblJoining.parent,HEX);
                  #endif
                  //gblMailbox.networkButton = LOW;   // indicate that the button is used so it will not roll into the next function.
                  gblMailbox.networkState = WHAT_PRE_CONFIRM_NEW_CHILD;  // Network light to indicate someone else's network was sucessfully joined.

                }
                else
                { // No child address was offered. re-start the preemie process.
                  //Serial.println(F("No Source Address Offered"));
                    gblMailbox.networkState = WHAT_STARTUP;  //blip of red indication network was not joined.
              #ifdef CHILD_MONITOR
                //Serial.print(F("Offered PANID:0x")); Serial.println(gblJoining.panID);
                Serial.print(F("No child address offered: ")); Serial.println(millis());
              #endif
                }
              }
              //else {Serial.println(gblScratch.timer1 - millis());}
         if (gblJoining.replaceIntent)  // REPLACEMENT PAIRING (Nov 26, 2026, Gary): keep the WHITE/RED pairing pattern through the join itself.
         {
           _Control_Net_Light(/*LightColor1*/ WHAT_LIGHT_WHITE,/*Light Color 1 time*/ 100,/*LightColor2 */ WHAT_LIGHT_RED,/*Light Color 2 time*/ 100 );
         }
         else
         {
           _Control_Net_Light(/*LightColor1*/ WHAT_LIGHT_BLUE,/*Light Color 1 time*/ 10,/*LightColor2 */ WHAT_LIGHT_OFF,/*Light Color 2 time*/ 200 );
         }
  
      }// break;

      // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  
      if(gblMailbox.networkState == WHAT_PRE_CONFIRM_NEW_CHILD )//       case WHAT_PRE_CONFIRM_NEW_CHILD:
      {
           default_case = false;  // Used to verify that one of the cases executed.
          gblRegistry.message[gblRegistry.reusableSlot].MessageID = NETWORK_JOIN_COMPLETE_03;
          gblRegistry.message[gblRegistry.reusableSlot].callback = &Rx_NETWORK_JOIN_COMPLETE_03; // Message callback set here to collect the responses
          gblMailbox.networkStateColor = WHAT_LIGHT_OFF;
           gblRxFlags.f_NETWORK_JOIN_COMPLETE_03 = false;    // Clear the message flag so only new messages will be used.
        gblScratch.timer1 = millis() + 1000;  // FIX (Nov 24, 2026): was '<=' (a comparison, result discarded) - the confirm window never got set, so 'Join timed out' fired almost immediately.
        gblMailbox.networkState = WHAT_CONFIRM_NEW_CHILD;  //blip of red indication network was not joined.

      }//   break;
      // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  
      if(gblMailbox.networkState == WHAT_CONFIRM_NEW_CHILD )//      case WHAT_CONFIRM_NEW_CHILD:
      {
           default_case = false;  // Used to verify that one of the cases executed.
          if (gblScratch.timer1 <= millis()) //Wait a short time after permitted to join to get more proposed addresses.
          {
            gblMailbox.networkState = WHAT_PRE_NETWORK_ABORT;  //blip of red indication network was not joined.
            #ifdef CHILD_MONITOR
            //Serial.print(F("Offered PANID:0x")); Serial.println(gblJoining.panID);
            Serial.print(F("Join timed out : ")); Serial.println(millis());
            #endif
          }
        // If an abort message is recieved, return to the beginning.
          if(gblRxFlags.f_NETWORK_JOIN_ABORT_03) 
          {
            gblRxFlags.f_NETWORK_JOIN_ABORT_03 = false; // clear the message flag when it is used.
            //gblMailbox.networkState = WHAT_PRE_REQUEST_JOIN_NETWORK_03; // If nothing offered a child slot, return to being a preemie.
            gblMailbox.networkState = WHAT_PRE_NETWORK_ABORT;  //blip of red indication network was not joined.
            #ifdef CHILD_MONITOR
            //Serial.print(F("Offered PANID:0x")); Serial.println(gblJoining.panID);
            Serial.print(F("Join Abort : ")); Serial.println(millis());
           #endif
          }
          if(gblRxFlags.f_NETWORK_JOIN_COMPLETE_03)
          {
            gblScratch.timer1 = millis() + 1000;     // Just hold the light red for 1 second to indicate the network join aborted.
            #ifdef CHILD_MONITOR
            Serial.print(F("Network join confirmed ")); Serial.println(millis());
            #endif
            // FIX (Nov 24, 2026): commit the tentative identity only now that the parent
            // has confirmed this device was added. Before this moment the device was
            // still a preemie in its globals and in EEPROM.
            gblNetwork.panID = gblJoining.panID;
            WHAT_set_pan(gblNetwork.panID);
            mySourceAddress = gblJoining.sourceAddress;
            WHAT_set_SourceAddress(mySourceAddress);
            gblNetwork.parentAddress = gblJoining.parent;
            WHAT_StoreEEPROM();
            gblJoining.replaceIntent = false;        // PROPOSED (Nov 25, 2026): the intent ends at the join - this device is a member now (an ordinary member, or the replacement for a lost child; either way the offer it took has been honored).
            gblJoining.replaceOfferReceived = false; // PROPOSED (Nov 25, 2026): likewise, the taken-offer marker has done its work.
            // PROPOSED trial (Nov 25, 2026): Gary asked for the assigned address
            // to be printed once the assignment is complete (here, post-commit).
            #ifdef CHILD_MONITOR
            Serial.print(F("Address assigned: 0x")); Serial.print(mySourceAddress,HEX);
            Serial.print(F("  Parent: 0x")); Serial.print(gblNetwork.parentAddress,HEX);
            Serial.print(F("  PAN: 0x")); Serial.println(gblNetwork.panID,HEX);
            #endif
          // gblMailbox.networkStateColor = WHAT_LIGHT_GREEN;
          _Control_Net_Light(/*LightColor1*/ WHAT_LIGHT_GREEN,/*Light Color 1 time*/ 50,/*LightColor2 */ WHAT_LIGHT_GREEN,/*Light Color 2 time*/ 50 );
          gblMailbox.networkState = WHAT_NEW_CHILD_CONFIRMED;  //blip of red indication network was not joined.
           
          }
      }//    break;
      // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  

      if(gblMailbox.networkState == WHAT_NEW_CHILD_CONFIRMED )//      case WHAT_NEW_CHILD_CONFIRMED:
      {
           default_case = false;  // Used to verify that one of the cases executed.
         if (gblScratch.timer1 <= millis()) //Wait a short time after permitted to join to get more proposed addresses.
          {
           //gblMailbox.networkStateColor = WHAT_LIGHT_OFF;
           _Control_Net_Light(/*LightColor1*/ WHAT_LIGHT_OFF,/*Light Color 1 time*/ 50,/*LightColor2 */ WHAT_LIGHT_OFF,/*Light Color 2 time*/ 50 );
    
            gblMailbox.networkState = WHAT_STARTUP;  //blip of red indication network was not joined.
            //#ifdef CHILD_MONITOR
            //Serial.print(F("Offered PANID:0x")); Serial.println(gblJoining.panID);
            //#endif
          }
      }//  break;

      // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  
      if(gblMailbox.networkState == WHAT_PRE_NETWORK_ABORT )//       case WHAT_PRE_NETWORK_ABORT:
      {
           default_case = false;  // Used to verify that one of the cases executed.
           gblScratch.timer1 = millis() + 1000;     // Just hold the light red for 1 second to indicate the network join aborted.
          // gblMailbox.networkStateColor = WHAT_LIGHT_RED;    
            _Control_Net_Light(/*LightColor1*/ WHAT_LIGHT_RED,/*Light Color 1 time*/ 50,/*LightColor2 */ WHAT_LIGHT_RED,/*Light Color 2 time*/ 50 );
           gblMailbox.networkState = WHAT_NETWORK_ABORT;  //blip of red indication network was not joined.

       }// break;

      // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  
      if(gblMailbox.networkState == WHAT_NETWORK_ABORT )//      case WHAT_NETWORK_ABORT:
      {
           default_case = false;  // Used to verify that one of the cases executed.
        if (gblScratch.timer1 <= millis()) //Wait a short time after permitted to join to get more proposed addresses.
          {
            //gblMailbox.networkStateColor = WHAT_LIGHT_OFF;   
            _Control_Net_Light(/*LightColor1*/ WHAT_LIGHT_OFF,/*Light Color 1 time*/ 50,/*LightColor2 */ WHAT_LIGHT_OFF,/*Light Color 2 time*/ 50 );

            gblMailbox.networkState = WHAT_STARTUP;
          }
      }//  break;

/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
// TOC    Normal Adult operations 
      if(gblMailbox.networkState == WHAT_PRE_NET_ALIVE )//       case WHAT_PRE_NET_ALIVE:
      {
           default_case = false;  // Used to verify that one of the cases executed.
              // Use the reusable message to respond to ACK requests.
              gblRegistry.message[gblRegistry.reusableSlot].MessageID = REQUEST_ACK_03;     // destination requested to reply with ACK.
              gblRegistry.message[gblRegistry.reusableSlot].callback = &Rx_REQUEST_ACK_03; // Message callback set here to collect the responses
              gblMailbox.networkStateColor = WHAT_LIGHT_OFF;
               gblRxFlags.f_REQUEST_ACK_03 = false;          // Clear the message flag so only new messages will be used.
              gblRxFlags.f_REQUEST_JOIN_NETWORK_03 = false; // Clear the message flag so only new messages will be used.
              #ifdef __WHAT_FULLLINK_h
                gblLinkRxFlags.f_REQUEST_FULL_LINK_03 = false; // Mark the reception flag as used so it is not duplicated.
              #endif
              gblMailbox.networkState = WHAT_NET_ALIVE;
 
              #ifdef CHILD_MONITOR
               Serial.print(F("gblMailbox.networkStateColor:")); Serial.println(gblMailbox.networkStateColor,DEC);
               Serial.print(F("gblMailbox.networkState: WHAT_NET_ALIVE ")); Serial.println(gblMailbox.networkState);
              #endif
      }//  break;
/**********************************************************************************/

       if(gblMailbox.networkState == WHAT_NET_ALIVE )//      case WHAT_NET_ALIVE:
       {
           default_case = false;  // Used to verify that one of the cases executed.
          // If no network activity is currently underway, monitor network button interactions.
            //#define Network_held gblScratch.timer2  //This will not be active while other CASEs are active.
            if (gblRxFlags.f_REQUEST_JOIN_NETWORK_03)   // A preemie is attempting to join the network.
            {
                gblRxFlags.f_REQUEST_JOIN_NETWORK_03 = false; // Mark the reception flag as used so it is not duplicated.
                gblMailbox.networkState = WHAT_PRE_NEW_CHILD_REQ;
                #ifdef CHILD_MONITOR
                Serial.print(F("gblMailbox.networkState: WHAT_PRE_NEW_CHILD_REQ ")); Serial.println(gblMailbox.networkState);
                #endif
            }
       // PROPOSED (Nov 25, 2026): note what is NOT here - there is no
       // button-erase of a lost child's record (Gary's ruling, Nov 25, 2026,
       // reversing the indication-gated erase sketched earlier the same day).
       // The RED blink is information only. Pressing this device's network
       // button while a child is lost does nothing at all; the lost record
       // leaves the table only by the child's own verified ABANDON, or by a
       // replacement preemie (button held at ITS power on) being accepted
       // with THIS device's button while it asks - the replacement path in
       // WHAT_NEW_CHILD_REQ. A record therefore can never be freed by a
       // button press during a power outage, which is the point.
       #ifdef __WHAT_FULLLINK_h
            if (gblMailbox.networkButton == HIGH)   // Holding the network button will attempt to "link" this device with another device
            {
                gblMailbox.networkState = WHAT_LINKING; // A holding routine for a linking sequence. Dont do anything else while linking.
                  // This is aslo the ONLY way to get into Linking: Must have a source address<0xFFFF, and must be in "Alive" and the Network button pressed.
            }
            if(gblLinkRxFlags.f_REQUEST_FULL_LINK_03)
            {
                gblMailbox.networkState = WHAT_LINKING; // A holding routine for a linking sequence. Dont do anything else while linking.
            }
            if(gblLinkRxFlags.f_REMOVE_FULL_LINK_03)
            {
                gblMailbox.networkState = WHAT_LINKING; // A holding routine for a linking sequence. Dont do anything else while linking.
            }
       #endif
            if (gblRxFlags.f_REQUEST_ACK_03)      // A preemie is trying to form a network using THIS network's PANID
            {
                gblRxFlags.f_REQUEST_ACK_03 = false; // Mark the reception flag as used so it is not duplicated.
                Tx_REPLY_ACKNOWLEDGE_03(); 
                  // If a device is asking for verification that this network is alive, this is a reply to that request.
            }
            // *****************

           if (gblMailbox.childLost && gblJoining.heardReplaceIntent)     // REPLACEMENT PAIRING (Nov 26, 2026, Gary): this parent has a lost child AND a replacement is out there trying to connect - alternate WHITE/RED, the pairing pattern, which outranks the plain lost-child blink below.
           {
           _Control_Net_Light(/*LightColor1*/ WHAT_LIGHT_WHITE,/*Light Color 1 time*/ 100,/*LightColor2 */ WHAT_LIGHT_RED,/*Light Color 2 time*/ 100 );
           }
           else if (gblMailbox.childLost)     // PROPOSED (Nov 25, 2026): a lost child outranks the alive blink - blink RED on the parent that lost one, until the child returns or a replacement takes its place. Indication only: no button act follows from it.
           {
           _Control_Net_Light(/*LightColor1*/ WHAT_LIGHT_RED,/*Light Color 1 time*/ 500,/*LightColor2 */ WHAT_LIGHT_OFF,/*Light Color 2 time*/ 500 );
           }
           else if(f_gbl_t_NetworkAlive )     // Other nodes seen within the alive timeout - blink GREEN
           {
            // While in the alive state, blink the light every few seconds.
           _Control_Net_Light(/*LightColor1*/ WHAT_LIGHT_GREEN,/*Light Color 1 time*/ 100,/*LightColor2 */ WHAT_LIGHT_OFF,/*Light Color 2 time*/ 5000 );
           }
           else
           {
           _Control_Net_Light(/*LightColor1*/ WHAT_LIGHT_BLUE,/*Light Color 1 time*/ 100,/*LightColor2 */ WHAT_LIGHT_OFF,/*Light Color 2 time*/ 5000 );
           }
  
      }//break;
              

// ************************************************************************
// ************************************************************************
// ************************************************************************
// ************************************************************************
// TOC    Child requesting addition to network 
       if(gblMailbox.networkState == WHAT_PRE_NEW_CHILD_REQ )//    case WHAT_PRE_NEW_CHILD_REQ:
       {
           default_case = false;  // Used to verify that one of the cases executed.
          gblRxFlags.f_REQUEST_JOIN_NETWORK_03 = false;
            WHAT_Packet _WHAT_Tx_Packet;    // Create a temporary transmit packet.
            _WHAT_Tx_Packet.WHAT_MAC_MESSAGE;
            _WHAT_Tx_Packet.pan_id = gblNetwork.panID;
            _WHAT_Tx_Packet.destination = 0xFFFF;   // Send this to all devices.
            _WHAT_Tx_Packet.sourceaddress = mySourceAddress;  // Use the source address of this device.
            uint8_t _index = 0;
            _WHAT_Tx_Packet.payload[_index++] = 6; //Total payload length (including this byte)
            _WHAT_Tx_Packet.payload[_index++] = 5; //MID length (including this byte)
            _WHAT_Tx_Packet.payload[_index++] = highByte(NETWORK_ALERT_PREEMIE_03);
            _WHAT_Tx_Packet.payload[_index++] = lowByte(NETWORK_ALERT_PREEMIE_03);
            _WHAT_Tx_Packet.payload[_index++] = highByte(gblJoining.heardTempID);  // STEP 1 (Nov 24, 2026): the temp ID from the preemie's search, so members who missed the search can still bind their offers to this attempt.
            _WHAT_Tx_Packet.payload[_index++] = lowByte(gblJoining.heardTempID);
          WHAT_TransmitPacket(_WHAT_Tx_Packet);
        gblScratch.timer1 = millis() + _WHAT_NEW_CHILD_TIME_DELAY; // allow 30 seconds for a network button to be pressed.
       gblMailbox.networkState = WHAT_NEW_CHILD_REQ;  // In alll cases change to the new state.
        #ifdef CHILD_MONITOR
        Serial.print(F("gblMailbox.networkState: WHAT_NEW_CHILD_REQ ")); Serial.println(gblMailbox.networkState);
        #endif
       }//break;

// ************************************************************************
      if(gblMailbox.networkState == WHAT_NEW_CHILD_REQ )//   case WHAT_NEW_CHILD_REQ:
      {
           default_case = false;  // Used to verify that one of the cases executed.
        // A preemie is trying to join this network.
        // If the network button on this device is pressed, notify the network and child that
        // the operator is allowing it to join.  Also propose a child address (if Available).

        if (gblJoining.heardReplaceIntent && (WHAT_IndicatedLostChild() != 0xFF))  // REPLACEMENT PAIRING (Nov 26, 2026, Gary): a replacement is trying to connect to THIS parent's lost child - WHITE/RED instead of the ordinary blue/green join indication.
        {
        _Control_Net_Light(/*LightColor1*/ WHAT_LIGHT_WHITE,/*Light Color 1 time*/ 100,/*LightColor2 */ WHAT_LIGHT_RED,/*Light Color 2 time*/ 100 );
        }
        else
        {
        _Control_Net_Light(/*LightColor1*/ WHAT_LIGHT_BLUE,/*Light Color 1 time*/ 500,/*LightColor2 */ WHAT_LIGHT_GREEN,/*Light Color 2 time*/ 100 );
        }
        //Serial.print(F("gblMailbox.networkState: WHAT_NEW_CHILD_REQ ")); Serial.println(gblMailbox.networkState);
        //gblMailbox.networkStateColor = WHAT_LIGHT_GREEN;
         if (gblMailbox.networkButton == HIGH) // Button is active high.
         {
             WHAT_Packet _WHAT_Tx_Packet;    // Create a temporary transmit packet.
            _WHAT_Tx_Packet.WHAT_MAC_MESSAGE;
            _WHAT_Tx_Packet.pan_id = gblNetwork.panID;
            _WHAT_Tx_Packet.destination = 0xFFFF;   // Send this to all devices.
            _WHAT_Tx_Packet.sourceaddress = mySourceAddress;  // Use the source address of this device.
            uint8_t _index = 0;
            _WHAT_Tx_Packet.payload[_index++] = 7; //Total payload length (including this byte) - STEP 1 (Nov 24, 2026): was 4, which never counted the confirm byte that was already being written.
            _WHAT_Tx_Packet.payload[_index++] = 6; //MID length (including this byte)
            _WHAT_Tx_Packet.payload[_index++] = highByte(NETWORK_ALLOW_PREEMIE_04);
            _WHAT_Tx_Packet.payload[_index++] = lowByte(NETWORK_ALLOW_PREEMIE_04);
            _WHAT_Tx_Packet.payload[_index++] = 1;  // This is a value for confirming allowance.
            _WHAT_Tx_Packet.payload[_index++] = highByte(gblJoining.heardTempID);  // STEP 1 (Nov 24, 2026): temp ID echo - the preemie verifies this allow is for ITS attempt.
            _WHAT_Tx_Packet.payload[_index++] = lowByte(gblJoining.heardTempID);
          WHAT_TransmitPacket(_WHAT_Tx_Packet);

            #ifdef CHILD_MONITOR
            Serial.print(F("gblMailbox.networkButton Pressed: ")); Serial.println(gblMailbox.networkButton);
            #endif
         }
        
         // PROPOSED (Nov 25, 2026): the replacement path is checked first. A
         // preemie that declared replacement intent may take over THIS
         // device's indicated lost child's record - but only when THIS
         // device's own network button is the one pressed (a received allow
         // from another member can never give away my lost slot), and the
         // button press above has already broadcast the allow the preemie
         // needs in order to accept at all.
         if (gblJoining.heardReplaceIntent && (gblMailbox.networkButton == HIGH) && (WHAT_IndicatedLostChild() != 0xFF))
         {
             gblJoining.replaceOffer = true;
             gblJoining.replaceSlot = WHAT_IndicatedLostChild();
             #ifdef CHILD_MONITOR
               Serial.print(F("Replacement offered for lost child: 0x")); Serial.println(gblNetwork.children[gblJoining.replaceSlot].ChildAddress,HEX);
             #endif
             gblMailbox.networkState = WHAT_PRE_NEW_CHILD_OFFER; //
         }
         else if ((gblRxFlags.f_NETWORK_ALLOW_PREEMIE_04) |  (gblMailbox.networkButton == HIGH))
         {
           // if THIS device or another device allowed a child to join, change to that state.
           gblJoining.replaceOffer = false;  // PROPOSED (Nov 25, 2026): an ordinary offer attempt - make sure no stale replacement arming survives into it.
           uint8_t _WHAT_Child_Used = isChildAvailable(); // Check and see if a Child location if available. (local since Nov 2026 - was a global)
             if(_WHAT_Child_Used < 0xFF)
             {
             gblMailbox.networkState = WHAT_PRE_NEW_CHILD_OFFER; // 
             }
             else
             {
              #ifdef CHILD_MONITOR
              Serial.println(F("Child not available to offer"));
              #endif
              gblMailbox.networkState = WHAT_PRE_NEW_CHILD_REQ; // 
             }
         }
         if(gblRxFlags.f_REQUEST_JOIN_NETWORK_03)
         {
           #ifdef CHILD_MONITOR
           Serial.println(F("Preemie is still requesting join.  reTx allow."));
           #endif
          gblMailbox.networkState = WHAT_PRE_NEW_CHILD_REQ;
         }
    // TOC    Network identified, waiting to be allowed
         if (gblScratch.timer1 <= millis()) //Timer set when a child request is seen.  there is 30seconds to connect.
         { // If the timer expires while in this state, return to normal operations.
           #ifdef CHILD_MONITOR
           Serial.println(F("\n Allow timer expired"));
           #endif
           gblMailbox.networkState = WHAT_PRE_NET_ALIVE; // Reset to the "normal" state when the timer expires.
         }
      }//break;

// ************************************************************************
      if(gblMailbox.networkState == WHAT_PRE_NEW_CHILD_OFFER )//    case WHAT_PRE_NEW_CHILD_OFFER:
      {
            default_case = false;  // Used to verify that one of the cases executed.
         // If this device is already a member of the network,
          //and this or some other device has indicated that it sees the new child, offer an address for the new child.
          gblRegistry.message[gblRegistry.reusableSlot].MessageID = ACCEPT_PARENT_06;   // This message comes once the network button is pressed.
          gblRegistry.message[gblRegistry.reusableSlot].callback = &Rx_ACCEPT_PARENT_06; // Message callback set here to collect the responses
          gblMailbox.networkStateColor = WHAT_LIGHT_OFF;
 
          gblRxFlags.f_ACCEPT_PARENT_06 = false;   // Clear the message reception flag.
          #ifdef CHILD_MONITOR
          Serial.println(F("Offering child"));
          #endif
          // Inform the network that this device has a child spot open while a preemie is trying to join.
          gblRxFlags.f_NETWORK_ALLOW_PREEMIE_04 = false; // clear the message flag when it is used.
          gblScratch.timer1 = millis() + _WHAT_NEW_CHILD_TIME_DELAY;  //This state will only remain active a short time.
          // PROPOSED (Nov 25, 2026): a new offer attempt re-runs any trunk claim check from scratch.
          gblJoining.trunkCandidate = 0xFFFF;
          gblJoining.trunkClaimPending = false;
          gblJoining.trunkClaimPassed = false;
          // Send out a proposal for a child
          Tx_PROPOSE_CHILD_NUMBER_05();
          // There are seperate state machines for preemies and members, each handle the NETWORK_ALLOW_PREEMIE_04 state, but in different state machines.
          gblMailbox.networkState = WHAT_NEW_CHILD_OFFER; // Wait to see if child accepts offer.
      }//break;

 // ************************************************************************ 
      if(gblMailbox.networkState == WHAT_NEW_CHILD_OFFER )//     case WHAT_NEW_CHILD_OFFER:
      {
           default_case = false;  // Used to verify that one of the cases executed.
// TOC    New child allowed to be added to network 
      // The network has approved for the new child to be added and THIS device has an available child slot.
      // A proposed child address was sent.  The child will now decide which parent it wants.
      
      // PROPOSED (Nov 25, 2026): a trunk offer waits on its ACK claim check first.
      if (gblJoining.trunkClaimPending)
      {
        if (gblRxFlags.f_REPLY_ACKNOWLEDGE_03 && (gblJoining.ackSourceAddress == gblJoining.trunkCandidate))
        { // The trunk address itself answered - it is already claimed. Make no offer.
          gblJoining.trunkClaimPending = false;
          gblRxFlags.f_REPLY_ACKNOWLEDGE_03 = false;
          #ifdef CHILD_MONITOR
            Serial.print(F("Trunk address already claimed: 0x")); Serial.println(gblJoining.trunkCandidate,HEX);
          #endif
          gblMailbox.networkState = WHAT_PRE_NET_ALIVE;
        }
        else if (millis() >= gblJoining.trunkClaimDeadline)
        { // Silence for the full window: the trunk address is free. Offer it now.
          gblJoining.trunkClaimPending = false;
          gblJoining.trunkClaimPassed = true;
          Tx_PROPOSE_CHILD_NUMBER_05();
          gblScratch.timer1 = millis() + _WHAT_NEW_CHILD_TIME_DELAY;  // Fresh window for the preemie to accept the trunk offer.
        }
      }

      if (gblJoining.heardReplaceIntent && (WHAT_IndicatedLostChild() != 0xFF))  // REPLACEMENT PAIRING (Nov 26, 2026, Gary): the pairing pattern holds while the replacement's offer is outstanding.
      {
      _Control_Net_Light(/*LightColor1*/ WHAT_LIGHT_WHITE,/*Light Color 1 time*/ 100,/*LightColor2 */ WHAT_LIGHT_RED,/*Light Color 2 time*/ 100 );
      }
      else
      {
      _Control_Net_Light(/*LightColor1*/ WHAT_LIGHT_GREEN,/*Light Color 1 time*/ 500,/*LightColor2 */ WHAT_LIGHT_BLUE,/*Light Color 2 time*/ 100 );
      }

      if (gblScratch.timer1 <= millis()) //Timer set when a network button was pressed.  there is 30seconds to connect.
        {
            #ifdef CHILD_MONITOR
            Serial.println(F("New Child Timeout"));
            #endif
            gblJoining.replaceOffer = false;        // PROPOSED (Nov 25, 2026): the attempt is over - disarm any replacement offer and forget the heard intent.
            gblJoining.heardReplaceIntent = false;  // PROPOSED (Nov 25, 2026): the next attempt latches its own.
            gblMailbox.networkState = WHAT_PRE_NET_ALIVE; // Reset to the "normal" state when the timer expires.
        }
        if(gblRxFlags.f_ACCEPT_PARENT_06)
        {
          gblRxFlags.f_ACCEPT_PARENT_06 = false;    // clear the message flag when it is used.
          // whether the device accepted THIS device as a parent or not, indicate that this state is completed.
          #ifdef CHILD_MONITOR
          Serial.println(F("New Child Accepted"));
          #endif
            //Serial.println(F("Child accepted."));
            //Serial.println(F("PANID\tMyChild\tmyLatchkey\tmyRelationship"));
            //Serial.print(gblNetwork.panID);Serial.print(gblNetwork.children[_myChild_available_Index].ChildAddress);Serial.print(gblNetwork.children[_myChild_available_Index].LatchKeyValue);Serial.println(gblNetwork.children[_myChild_available_Index].RelationshipType);
             WHAT_Packet _WHAT_Tx_Packet;    // Create a temporary transmit packet.
            _WHAT_Tx_Packet.WHAT_MAC_MESSAGE;
            _WHAT_Tx_Packet.pan_id = gblNetwork.panID;
            _WHAT_Tx_Packet.destination = 0xFFFF;   // Send this to all devices.
            _WHAT_Tx_Packet.sourceaddress = mySourceAddress;  // Use the source address of this device.
             uint8_t _index = 0;
             _WHAT_Tx_Packet.payload[_index++] = 10; //Total payload length (including this byte) - FIX (Nov 25, 2026, pre-approved by Gary): was 11, one more than the fields written (length, MID length, command, child address, 4-byte proof = 10), so an undefined buffer byte rode at the end of every JOIN_COMPLETE. Receivers parse at fixed offsets and never read it, which is why joins confirmed anyway.
             _WHAT_Tx_Packet.payload[_index++] = 9; //MID length (including this byte) - FIX (Nov 25, 2026): was 10, same phantom byte.
             _WHAT_Tx_Packet.payload[_index++] = highByte(NETWORK_JOIN_COMPLETE_03);
            _WHAT_Tx_Packet.payload[_index++] = lowByte(NETWORK_JOIN_COMPLETE_03);
            _WHAT_Tx_Packet.payload[_index++] = highByte(gblScratch.u16_1);  // FIX (Nov 24, 2026): child address, handed off by Rx_ACCEPT_PARENT_06 - the broadcast announcement is the copy an unconfirmed child can actually receive (its address is still FFFF, so the directed copy is filtered out).
            _WHAT_Tx_Packet.payload[_index++] = lowByte(gblScratch.u16_1);
            // STEP 1 (Nov 24, 2026): the same proof as the directed copy - WHAT_Mix32(latchkey, confirm nonce, PANID | child address), from the u32 hand-offs - instead of the latchkey itself.
            WHAT_PayloadPut32(_WHAT_Tx_Packet.payload, _index, WHAT_Mix32(gblScratch.u32_1, gblScratch.u32_2, (((uint32_t)gblNetwork.panID) << 16) | gblScratch.u16_1)); _index += 4;
           WHAT_TransmitPacket(_WHAT_Tx_Packet);
           #ifdef CHILD_MONITOR
           Serial.print(F("\nBirth Announcement: "));
           #endif
           gblScratch.timer1 = millis() + 2000;  // FIX (Nov 24, 2026): was '<=' (a comparison, result discarded) - the confirmed-state hold time never got set.
          gblMailbox.networkState = WHAT_NEW_CHILD_CONFIRMED; // return to normal light operation.
        }
     
      }//break;

// ************************************************************************
      if(gblMailbox.networkState == WHAT_PRE_NETWORK_ERROR )//     case WHAT_PRE_NETWORK_ERROR:
      {
            default_case = false;  // Used to verify that one of the cases executed.
       // Something unexpected happened.  Flash the red light for a bit, then return to preemie check.
          #ifdef CHILD_MONITOR
          Serial.println(F("WHAT Network Error: ")); 
          #endif
          gblScratch.timer1 = millis() + 5000; // Keep it green for several seconds.
          gblMailbox.networkState = WHAT_NETWORK_ERROR;  //blip of red indication network was not joined.
          //gblMailbox.networkStateColor = WHAT_LIGHT_RED;    
          _Control_Net_Light(/*LightColor1*/ WHAT_LIGHT_RED,/*Light Color 1 time*/ 50,/*LightColor2 */ WHAT_LIGHT_RED,/*Light Color 2 time*/ 50 );

      }// break;
// ************************************************************************
      if(gblMailbox.networkState == WHAT_NETWORK_ERROR )//     case WHAT_NETWORK_ERROR:
      {
           default_case = false;  // Used to verify that one of the cases executed.
        // Something unexpected happened.  Flash the red light for a bit, then return to preemie check.
              if (gblScratch.timer1 <= millis()) //Timer set when a network button was pressed.  there is 30seconds to connect.
              {
                  #ifdef CHILD_MONITOR
                  Serial.println(F("Returning to Startup. ")); 
                  #endif
                gblMailbox.networkState = WHAT_STARTUP;  //After the red light expires, return to the startup state.
              }
        }//break;
// ************************************************************************
        if(gblMailbox.networkState == WHAT_LINKING )//     case WHAT_LINKING:
        {
           default_case = false;  // Used to verify that one of the cases executed.
          #ifdef __WHAT_FULLLINK_h
            WHAT_LinkCheck();
            if(gblLink.state == WHAT_NO_LINK)
            {
              gblMailbox.networkState = WHAT_NET_ALIVE;
            }
          #endif
          #ifndef __WHAT_FULLLINK_h
            gblMailbox.networkState = WHAT_STARTUP;
          #endif
        }//break;

     if(default_case == true)
     {
       //Serial.print(F("Unhandled PREEMIE state:"));Serial.println(gblMailbox.networkState);
       // If in an invalid state, revert to being a preemie.
         #ifdef CHILD_MONITOR
         //Serial.print(F("Offered PANID:0x")); Serial.println(gblJoining.panID);
         Serial.print(F("Undefined state : ")); Serial.println(gblMailbox.networkState,DEC);
       #endif
       //gblMailbox.networkState = WHAT_PRE_REQUEST_JOIN_NETWORK_03; // If nothing offered a child slot, return to being a preemie.
       gblMailbox.networkState = WHAT_PRE_NETWORK_ERROR;  //blip of red indication network was not joined.
      }// break;

 //  }// End Switch
      
  // //Look for other messages of interest:
  // if(gblRxFlags.f_REQUEST_JOIN_NETWORK_03)
  // {
  //   gblRxFlags.f_REQUEST_JOIN_NETWORK_03 = false; // clear the message flag when it is used.
  //   // Send Network Alert.
  //   WHAT_Packet _WHAT_Tx_Packet;  // Make a temporary transmit packet
  //   _WHAT_Tx_Packet.WHAT_MAC_MESSAGE;
  //   // In all cases, if a child address is being proposed, also inform the network that a preemie wants to join.
  //   // In addition to proposing a child, tell the entire network that a child is trying to join:
  //   _WHAT_Tx_Packet.pan_id = gblNetwork.panID;
  //   _WHAT_Tx_Packet.destination = 0xFFFF;   // Send this to all devices.
  //   _WHAT_Tx_Packet.sourceaddress = mySourceAddress;  // Use the source ddress of this device.
  //   uint8_t _index = 0;
  //   _WHAT_Tx_Packet.payload[_index++] = 5; //Total payload length (including this byte)
  //   _WHAT_Tx_Packet.payload[_index++] = 4; //MID length (including this byte)
  //   _WHAT_Tx_Packet.payload[_index++] = highByte(NETWORK_ALERT_PREEMIE_03);
  //   _WHAT_Tx_Packet.payload[_index++] = lowByte(NETWORK_ALERT_PREEMIE_03);
  //    WHAT_TransmitPacket(_WHAT_Tx_Packet);
     
   
  //  }// end if my proposed address is valid.

  //  if(gblRxFlags.f_NETWORK_ALERT_PREEMIE_03)
  //  {
  //     gblRxFlags.f_NETWORK_ALERT_PREEMIE_03 = false; // clear the message flag when it is used.
  //     // The network has given an alert that a preemie is trying to join the network.
  //     // During that time, if this device address is <0x0100 it will monitor its network button
  //     // and send an acceptance signal if it is pressed.
  //     //Serial.println(F("Rx_NETWORK_ALERT_PREEMIE_03"));
  //     //WHAT_NETWORK_SEEN = true;
  //     gblScratch.timer1 = millis() + _RQST_WAIT_TIME; // Once a network is seen, slow the rate of requests.
  //     //gblMailbox.networkState = WHAT_NEW_CHILD_REQ;
  //     gblMailbox.networkState = WHAT_NETWORK_CREATING;
  //  }
  // if(gblRxFlags.f_CHOOSE_NETWORK_05)
  // {// When a preemie chooses a network, if it is not THIS network, revert to finished state.
  //     //Serial.println(F("Rx_CHOOSE_NETWORK_05"));
  //   gblRxFlags.f_CHOOSE_NETWORK_05 = false; // clear the message flag when it is used.
  //   if (gblJoining.panID != gblNetwork.panID) //Only respond if mature and if the given PANID does not match this one.
  //     {
  //         gblMailbox.networkState = WHAT_PRE_NET_ALIVE; // Return to normal if a different PANID is used.
  //                gblScratch.timer2 = millis()+ NETWORK_HELD_DELAY; // Reset the network held timer
  //     }
  // }
  //  if(gblRxFlags.f_NETWORK_JOIN_COMPLETE_03)
  //  {
  //     //Serial.println(F("Rx_NETWORK_JOIN_COMPLETE_03"));
  //    gblRxFlags.f_NETWORK_JOIN_COMPLETE_03 = false; // clear the message flag when it is used.
  //    gblMailbox.networkState = WHAT_PRE_NET_ALIVE; // Return to normal if the connection is aborted.
  //    gblScratch.timer2 = millis()+ NETWORK_HELD_DELAY; // Reset the network held timer
  //    //gblMailbox.networkState = WHAT_NETWORK_CREATED;
  //    //Setup_WHAT_Show_Connection();
  //  }
  //   // If an abort message is recieved, return to the beginning.
  //   if(gblRxFlags.f_NETWORK_JOIN_ABORT_03) 
  //   {
  //       //Serial.println(F("Rx_NETWORK_JOIN_ABORT_03"));
  //     gblRxFlags.f_NETWORK_JOIN_ABORT_03 = false; // clear the message flag when it is used.
  //     gblMailbox.networkState = WHAT_PRE_NET_ALIVE; // Return to normal if the connection is aborted.
  //    gblScratch.timer2 = millis()+ NETWORK_HELD_DELAY; // Reset the network held timer
  //     gblMailbox.networkState = WHAT_NETWORK_NONE;
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
        //Serial.println(gblNetwork.children[_i].ChildAddress,HEX);
      #endif
      if(gblNetwork.children[_i].ChildAddress == gblJoining.sourceAddress) _WHAT_Child_Empty = _i; // This is already a child of this parent.
      if((_blankspot == 0xFF) & (gblNetwork.children[_i].ChildAddress == 0xFFFF)) _blankspot = _i; // Keep track of the first blank spot
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
  gblRxFlags.f_NETWORK_ALERT_PREEMIE_03 = true;   // Notice from another device that a preemie is trying to join
  // STEP 1 (Nov 24, 2026): a member that missed the preemie's search itself
  // learns the attempt's temp ID from the alert, so its offer can still bind
  // to the attempt. (Members only - a preemie's own temp ID is its own and
  // must never be overwritten by an alert about somebody else.)
  if (mySourceAddress < 0xFFFF)
  {
    gblJoining.heardTempID = ((_WHAT_Rx_Packet.payload[_index + 3] * 0x100) + _WHAT_Rx_Packet.payload[_index + 4]);
  }
}

// TOC Individual Recieve messages 
// gblRegistry.reusableSlot 
void Rx_NETWORK_ALLOW_PREEMIE_04(WHAT_Packet _WHAT_Rx_Packet, uint8_t _index)
{
// Rx Message flags
  // STEP 1 (Nov 24, 2026): the allow must echo the temp ID of the join
  // attempt it belongs to (the confirm byte is at _index + 3, the temp ID
  // at _index + 4 / + 5). An allow for a different attempt - another
  // preemie's join, or a spoof - is ignored.
  // FIX (Nov 27, 2026, Gary's design): the echo is checked against the
  // attempt THIS device takes part in, by role. A preemie checks its OWN
  // temp ID (gblJoining.tempID). A member checks the temp ID of the search
  // it HEARD (gblJoining.heardTempID) - members hear allows now (the
  // message is registered permanently in WHAT_begin), so any member with
  // an open slot offers when any member's button is pressed, and the
  // preemie chooses among the offers. Before this fix, only the pressed
  // unit could ever offer: the message was never registered on a member,
  // and this check compared against the member's own stale temp ID.
  uint16_t _allowTempID = ((_WHAT_Rx_Packet.payload[_index + 4] * 0x100) + _WHAT_Rx_Packet.payload[_index + 5]);
  if (mySourceAddress >= 0xFFFF)
  { // Preemie: the allow must be for MY attempt, and it names my network.
    if (_allowTempID != gblJoining.tempID) { return; }
    gblJoining.panID = _WHAT_Rx_Packet.pan_id;  //Only accepting PANID from the PANID that sent the allowance indication.
  }
  else
  { // Member: the allow must belong to the search I heard. Which network
    // is not in question - it is mine - so gblJoining.panID stays untouched.
    if (_allowTempID != gblJoining.heardTempID) { return; }
  }
  gblRxFlags.f_NETWORK_ALLOW_PREEMIE_04 = true;       // While a new network is forming, requests existing network to claim its PANID
    #ifdef CHILD_MONITOR
    Serial.print(F("Recieve acceptance of child from :0x")); Serial.println(_WHAT_Rx_Packet.sourceaddress);
    //Serial.print(F("Undefined Preemie state : ")); Serial.println(gblMailbox.networkState,HEX);
    #endif
}

// *****************
// TOC Individual Recieve messages
// gblRegistry.reusableSlot
void Rx_PROPOSE_CHILD_NUMBER_05(WHAT_Packet _WHAT_Rx_Packet, uint8_t _index)
{ //This device received a proposed child address.
  // If this device has been accepted to a network, keep the lowest numbered proposal.
  gblRxFlags.f_PROPOSE_CHILD_NUMBER_05 = true;    // Preemie asking the network for a child number
  //Serial.println(F("Rx_PROPOSE_CHILD_NUMBER_05"));
  // STEP 1 (Nov 24, 2026): an offer counts only if it echoes this preemie's
  // temp ID (at _index + 5 / + 6, after the proposed address) - offers meant
  // for a different preemie's attempt must not be collected here.
  uint16_t _offeredAddress = ((_WHAT_Rx_Packet.payload[_index + 3] * 0x100) + _WHAT_Rx_Packet.payload[_index + 4]);
  bool _tempMatches = (((_WHAT_Rx_Packet.payload[_index + 5] * 0x100) + (_WHAT_Rx_Packet.payload[_index + 6])) == gblJoining.tempID);
  // PROPOSED (Nov 25, 2026): the trailing flags byte (present when the block
  // length is 8 or more) marks a REPLACEMENT offer. A preemie that declared
  // replacement intent takes a replacement offer ahead of any ordinary offer,
  // however the addresses compare - it powered on with its button held for
  // exactly this - and once one is taken, ordinary offers no longer displace it.
  bool _isReplaceOffer = ((_WHAT_Rx_Packet.payload[_index] >= 8) && (_WHAT_Rx_Packet.payload[_index + 7] != 0));
  if (gblJoining.replaceIntent && _isReplaceOffer && _tempMatches && !gblJoining.replaceOfferReceived)
    { // Take the replacement offer.
      if( gblJoining.panID == _WHAT_Rx_Packet.pan_id)  //Only accepting PANID from the PANID that sent the allowance indication.
      {
        gblJoining.sourceAddress = _offeredAddress;
        gblJoining.parent = gblQueues.rxPacket.sourceaddress;
        gblJoining.replaceOfferReceived = true;
        #ifdef CHILD_MONITOR
          Serial.print(F("Replacement offer taken: 0x")); Serial.println(_offeredAddress,HEX);
        #endif
      }
    }
  else if (!gblJoining.replaceOfferReceived)
    {
    if (((_offeredAddress) < gblJoining.sourceAddress)
     && (_tempMatches))
    { // Keep the lowest offered child address.
      if( gblJoining.panID == _WHAT_Rx_Packet.pan_id)  //Only accepting PANID from the PANID that sent the allowance indication.
      {
        //gblJoining.panID = _WHAT_Rx_Packet.pan_id;  //Only accepting PANID from the PANID that sent the allowance indication.
        gblJoining.sourceAddress = _offeredAddress;
        //gblJoining.latchkey = ((_WHAT_Rx_Packet.payload[_index + 5] * 0x100) + _WHAT_Rx_Packet.payload[_index + 6]);
        gblJoining.parent = gblQueues.rxPacket.sourceaddress;
        //Serial.print(F("New offered ID: 0x")); Serial.println(gblJoining.sourceAddress);
      }
    }
    }
}

// *****************
// TOC Individual Recieve messages
void Rx_ACCEPT_PARENT_06(WHAT_Packet _WHAT_Rx_Packet, uint8_t _index)// 1 byte length, 2 byte command, 2 byte parent, 4 byte latchkey, 1 byte relation type, 4 byte confirm nonce, 2 byte temp ID // preemie accepts offer from parent, and proposes latch key value (STEP 1, Nov 24, 2026: randomly generated, 32-bit - was derived from its timer at 16 bits)
{
    uint16_t _Rx_ParentAddress = ((_WHAT_Rx_Packet.payload[_index + 3] * 0x100) + _WHAT_Rx_Packet.payload[_index + 4]);
    uint32_t _Rx_Offered_Latchkey = WHAT_PayloadGet32(_WHAT_Rx_Packet.payload, _index + 5);  // STEP 1: 32-bit latchkey at +5..+8.
    uint32_t _Rx_Confirm_Nonce = WHAT_PayloadGet32(_WHAT_Rx_Packet.payload, _index + 10);    // STEP 1: confirm nonce at +10..+13 (relation byte is at +9).
    uint16_t _Rx_TempID = ((_WHAT_Rx_Packet.payload[_index + 14] * 0x100) + _WHAT_Rx_Packet.payload[_index + 15]);  // STEP 1: temp ID at +14/+15.
    if (_Rx_ParentAddress == mySourceAddress) 
    { // Child selected THIS device as a parent.
      // STEP 1 (Nov 24, 2026): the accept must belong to the join attempt this
      // device heard - a mismatched temp ID means a different preemie's
      // attempt (or a spoof), and this accept is not acted on.
      if (_Rx_TempID != gblJoining.heardTempID)
      {
        #ifdef CHILD_MONITOR
          Serial.println(F("Rx Accept Parent: temp ID mismatch - not this join attempt. Child NOT added."));
        #endif
        return;
      }
      gblRxFlags.f_ACCEPT_PARENT_06 = true;   // Indicate that this message was recieved.
        // Verify the child address matches an open child slot:
      //The algorithn is a little confusing.  Offered child addresses use the address of this device with the child index as the next higher nibble.
      // This requires pulling out the address of THIS device (Parent), and rotating the recieved address so only the index remains.
      // This can be done with rotation.
      //    So "If mySourceAddress == 0x0000, then myChildIndex must not move (meaning the proposed address is 0x000C) (where C is the child index)
      //       "If mySourceAddress >0x0000, then myChildIndex must move right 1 nibble (rotate right 4 times)(meaning the proposed address is 0x00Cp) (where C is the child index, and p is the parent address)
      //       "If mySourceAddress >0x000F, then myChildIndex must move left 2 nibbles (rotate right 8 times)(meaning the proposed address is 0x0C0p) (where C is the child index, and p is the parent address)
      //       "If mySourceAddress >0x00FF, then myChildIndex must move left 3 nibbles (rotate right 12 times)(meaning the proposed address is 0xC00p) (where C is the child index, and p is the parent address)
      // At the end of this process, rotate the recieved source address to the least significant nibble location.
      // PROPOSED (Nov 25, 2026): the slot index lives in the PATH digits only
      // (the original extraction rotated the full address, trunk digit and
      // all). A trunk child - path 000 of the next trunk, accepting the very
      // address this device offered - carries no index in its address at all,
      // so it takes the first open slot instead.
      uint8_t _myChild_available_Index = 0xFF;
      // PROPOSED (Nov 25, 2026): a replacement accept commits into the lost
      // child's own slot - the one slot whose occupation is the point. The
      // accepted address must be the very address offered as a replacement,
      // and the slot must still hold that lost record.
      bool _isReplaceCommit = (gblJoining.replaceOffer && (_WHAT_Rx_Packet.sourceaddress == gblJoining.sourceAddress)
        && (gblJoining.replaceSlot >= 1) && (gblJoining.replaceSlot < ChildMaxIndex)
        && (gblNetwork.children[gblJoining.replaceSlot].ChildAddress == _WHAT_Rx_Packet.sourceaddress));
      if (_isReplaceCommit)
      { // Replacement child of THIS device, taking the lost child's slot.
        _myChild_available_Index = gblJoining.replaceSlot;
      }
      else if (((_WHAT_Rx_Packet.sourceaddress & 0x0FFF) == 0) && (_WHAT_Rx_Packet.sourceaddress == ((mySourceAddress & 0xF000) + 0x1000)) && (_WHAT_Rx_Packet.sourceaddress == gblJoining.sourceAddress))
      { // Trunk child of THIS device.
        for (uint8_t _s = 1; _s < ChildMaxIndex; _s++)
        {
          if (gblNetwork.children[_s].ChildAddress == 0xFFFF) { _myChild_available_Index = _s; break; }
        }
      }
      else
      { // Fill child: rotate the filled path digit down to the units place.
        uint16_t _myPath = mySourceAddress & 0x0FFF;
        _myChild_available_Index = (uint8_t)(((_WHAT_Rx_Packet.sourceaddress & 0x0FFF) >> (((_myPath > 0x0) * 4) + ((_myPath > 0x0F) * 4))) & 0x0F);
      }

      //and make sure that index does not already have a child in it, then fill it with this child....
      // (PROPOSED, Nov 25, 2026: the replacement slot is the one exception -
      // it is occupied by the lost record this commit supersedes.)
      if((_myChild_available_Index < ChildMaxIndex) && ((gblNetwork.children[_myChild_available_Index].ChildAddress == 0xFFFF) || _isReplaceCommit))
      {
        gblNetwork.children[_myChild_available_Index].ChildAddress = _WHAT_Rx_Packet.sourceaddress;
        gblNetwork.children[_myChild_available_Index].LatchKeyValue = _Rx_Offered_Latchkey;  // STEP 1 (Nov 24, 2026): the 32-bit latchkey, parsed at +5..+8 above. (FIX history, Nov 24, 2026: the 16-bit value was once read at +4/+5, one field early.)
        gblNetwork.children[_myChild_available_Index].RelationshipType = _WHAT_Rx_Packet.payload[_index+9];  // STEP 1: the relation byte follows the 4-byte latchkey (+9). (Was +7 with the 2-byte latchkey.)
        gblNetwork.childLastHeard[_myChild_available_Index] = millis();  // PROPOSED (Nov 25, 2026): the new (or replacement) child starts its liveness clock at the commit - it has a full lost interval to be heard from.
        if (_isReplaceCommit)
        { // PROPOSED (Nov 25, 2026): the lost record has been superseded - the address lives again under a fresh latchkey, and the replacement arming is spent.
          #ifdef CHILD_MONITOR
            Serial.print(F("Lost child replaced at slot: ")); Serial.println(_myChild_available_Index);
          #endif
          gblJoining.replaceOffer = false;
          gblJoining.heardReplaceIntent = false;
        }
        gblScratch.u16_1 = _WHAT_Rx_Packet.sourceaddress;  // Hand the accepted child's address to the WHAT_NEW_CHILD_OFFER state for the Birth Announcement.
        gblScratch.u32_1 = _Rx_Offered_Latchkey;           // Same hand-off for the latchkey (32-bit now, so a u32 scratch).
        gblScratch.u32_2 = _Rx_Confirm_Nonce;              // STEP 1: and the confirm nonce - the Birth Announcement's proof is computed over it.
        #ifdef CHILD_MONITOR
        Serial.print(F("Child accepted.  Index:"));Serial.println(_myChild_available_Index);
        Serial.println(F("PANID\tMyChild\tmyLatchkey\tmyRelationship"));
        Serial.print(gblNetwork.panID,HEX);Serial.print(F("\t"));Serial.print(gblNetwork.children[_myChild_available_Index].ChildAddress,HEX);Serial.print(F("\t"));Serial.print(gblNetwork.children[_myChild_available_Index].LatchKeyValue,HEX);Serial.print(F("\t"));Serial.println(gblNetwork.children[_myChild_available_Index].RelationshipType,HEX);
        #endif

      // Indicate that the parent accepts the child also!
      // #define NETWORK_JOIN_COMPLETE_03    0xF020   // 1 byte length, 2 byte command  // Sent by new parent (Birth Announcement)
        WHAT_Packet _WHAT_Tx_Packet;  // Make a temporary transmit packet
        _WHAT_Tx_Packet.WHAT_MAC_MESSAGE;
        _WHAT_Tx_Packet.pan_id = gblNetwork.panID;
        _WHAT_Tx_Packet.destination = gblNetwork.children[_myChild_available_Index].ChildAddress;   // Send this to the child.
        _WHAT_Tx_Packet.sourceaddress = mySourceAddress;  // Use the source address of this device.
        uint8_t _index = 0;
        _WHAT_Tx_Packet.payload[_index++] = 10; //Total payload length (including this byte) - FIX (Nov 25, 2026, pre-approved by Gary): was 11 - see the broadcast copy in WHAT_NEW_CHILD_OFFER for the phantom-byte note.
        _WHAT_Tx_Packet.payload[_index++] = 9; //MID size (for the 2nd message) - FIX (Nov 25, 2026): was 10, same phantom byte.
        _WHAT_Tx_Packet.payload[_index++] = highByte(NETWORK_JOIN_COMPLETE_03);// 1 byte length, 2 byte command, 2 byte child address, 4 byte proof // Any device that has open child slots replies to request (REQUEST_JOIN_NETWORK_02).
        _WHAT_Tx_Packet.payload[_index++] = lowByte(NETWORK_JOIN_COMPLETE_03); //in order to support multibyte MIDs, put the high byte first.
        _WHAT_Tx_Packet.payload[_index++] = highByte(_WHAT_Rx_Packet.sourceaddress);  // FIX (Nov 24, 2026): the child address and latchkey the format calls for were never sent, so the child's confirmation check could never pass.
        _WHAT_Tx_Packet.payload[_index++] = lowByte(_WHAT_Rx_Packet.sourceaddress);
        // STEP 1 (Nov 24, 2026): a PROOF replaces the latchkey echo - WHAT_Mix32(latchkey, the child's confirm nonce, PANID | child address). Only a parent that actually received and stored the latchkey can produce it, and the latchkey itself never crosses the air a second time.
        WHAT_PayloadPut32(_WHAT_Tx_Packet.payload, _index, WHAT_Mix32(_Rx_Offered_Latchkey, _Rx_Confirm_Nonce, (((uint32_t)gblNetwork.panID) << 16) | _WHAT_Rx_Packet.sourceaddress)); _index += 4;
         WHAT_TransmitPacket(_WHAT_Tx_Packet);

          WHAT_StoreEEPROM();
      }
      else
      {
        #ifdef CHILD_MONITOR
        Serial.println(F("Child slot was already taken... Child NOT added"));
        #endif

        WHAT_Packet _WHAT_Tx_Packet;  // Make a temporary transmit packet
        _WHAT_Tx_Packet.WHAT_MAC_MESSAGE;
        _WHAT_Tx_Packet.pan_id = gblNetwork.panID;
        _WHAT_Tx_Packet.destination = 0xFFFF;   // Send this to all devices.
        _WHAT_Tx_Packet.sourceaddress = mySourceAddress;  // Use the source ddress of this device.
        uint8_t _index = 0;
        _WHAT_Tx_Packet.payload[_index++] = 8; //Total payload length (including this byte)
        _WHAT_Tx_Packet.payload[_index++] = 7; //MID length (including this byte)
        _WHAT_Tx_Packet.payload[_index++] = highByte(ABANDON_CHILD_05);
        _WHAT_Tx_Packet.payload[_index++] = lowByte(ABANDON_CHILD_05);
        // STEP 1 (Nov 24, 2026): send back a PROOF of the latchkey as message confirmation, never the latchkey itself - same abandon-proof context the child's own abandon uses: WHAT_Mix32(latchkey, child address, parent address). (FIX history, Nov 24, 2026: this once read bytes 8/9 of the received packet, past its end.)
        WHAT_PayloadPut32(_WHAT_Tx_Packet.payload, _index, WHAT_Mix32(_Rx_Offered_Latchkey, _WHAT_Rx_Packet.sourceaddress, mySourceAddress)); _index += 4;
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
  // verify this message was from the proposed parent, is addressed to this
  // device's tentative address, and carries a valid proof.
  // NETWORK_JOIN_COMPLETE_03 payload layout: length, MID length, command, 2 byte child address, 4 byte proof.
  uint16_t _Rx_ChildAddress = ((_WHAT_Rx_Packet.payload[_index + 3] * 0x100) + _WHAT_Rx_Packet.payload[_index + 4]);
  uint32_t _Rx_Proof = WHAT_PayloadGet32(_WHAT_Rx_Packet.payload, _index + 5);  // STEP 1: proof at +5..+8.
  // STEP 1 (Nov 24, 2026): the parent proves it holds the latchkey by mixing
  // it with the confirm nonce this device sent in its ACCEPT - the latchkey
  // itself is never re-transmitted, so this check replaces the old
  // latchkey-equality check. Parent and address are still tentative in
  // gblJoining here - the identity is committed only after this check passes.
  uint32_t _Expected_Proof = WHAT_Mix32(myLatchKeyNumber, gblJoining.confirmNonce, (((uint32_t)gblJoining.panID) << 16) | gblJoining.sourceAddress);

  if((_WHAT_Rx_Packet.sourceaddress == gblJoining.parent ) && (_Rx_ChildAddress == gblJoining.sourceAddress) && (_Rx_Proof == _Expected_Proof))
  {
   gblRxFlags.f_NETWORK_JOIN_COMPLETE_03 = true;   // Preeemie accepted a proposed parent 

  }

}

// *****************
// TOC Individual Transmit messages
void Tx_PROPOSE_CHILD_NUMBER_05 (void)
{
  // PROPOSED (Nov 25, 2026): a replacement offer short-circuits all the
  // address math below. The address is not computed - it is the lost child's
  // own address, read back out of the record it will supersede - and it needs
  // no empty slot, because its slot is the lost record itself. The offer
  // carries the replacement flag so the preemie can take it ahead of any
  // ordinary offer. If the lost record has vanished since the offer was
  // armed (freed by the child's own abandon in the meantime), the arming is dropped and the ordinary
  // path below runs instead.
  if (gblJoining.replaceOffer)
  {
    if ((gblJoining.replaceSlot >= 1) && (gblJoining.replaceSlot < ChildMaxIndex)
        && (gblNetwork.children[gblJoining.replaceSlot].ChildAddress != 0xFFFF))
    {
      gblJoining.sourceAddress = gblNetwork.children[gblJoining.replaceSlot].ChildAddress;
      WHAT_Packet _WHAT_Tx_Packet;  // Make a temporary transmit packet
      _WHAT_Tx_Packet.WHAT_MAC_MESSAGE;
      _WHAT_Tx_Packet.pan_id = gblNetwork.panID;
      _WHAT_Tx_Packet.destination = 0xFFFF;   // Send this to all devices.
      _WHAT_Tx_Packet.sourceaddress = mySourceAddress;  // Use the source address of this device.
      uint8_t _index = 0;
      _WHAT_Tx_Packet.payload[_index++] = 9; //Total payload length (including this byte)
      _WHAT_Tx_Packet.payload[_index++] = 8;   //MID size (for the 2nd message).
      _WHAT_Tx_Packet.payload[_index++] = highByte(PROPOSE_CHILD_NUMBER_05);
      _WHAT_Tx_Packet.payload[_index++] = lowByte(PROPOSE_CHILD_NUMBER_05); //in order to support multibyte MIDs, put the high byte first.
      _WHAT_Tx_Packet.payload[_index++] = highByte(gblJoining.sourceAddress);
      _WHAT_Tx_Packet.payload[_index++] = lowByte(gblJoining.sourceAddress);
      _WHAT_Tx_Packet.payload[_index++] = highByte(gblJoining.heardTempID);  // STEP 1 (Nov 24, 2026): temp ID echo - binds this offer to the preemie whose search was heard.
      _WHAT_Tx_Packet.payload[_index++] = lowByte(gblJoining.heardTempID);
      _WHAT_Tx_Packet.payload[_index++] = 1;  // PROPOSED (Nov 25, 2026): flags byte - 0x01 = replacement offer.
      WHAT_TransmitPacket(_WHAT_Tx_Packet);

      gblRegistry.message[gblRegistry.reusableSlot].MessageID = ACCEPT_PARENT_06;  // these are prospective parents offering free addresses.
      gblRegistry.message[gblRegistry.reusableSlot].callback = &Rx_ACCEPT_PARENT_06; // Message callback set here to collect the responses
      gblMailbox.networkStateColor = WHAT_LIGHT_OFF;
      gblRxFlags.f_ACCEPT_PARENT_06 = false;   // Clear the message reception flag.
      #ifdef CHILD_MONITOR
        Serial.print(F("Tx Propose replacement child address: 0x")); Serial.println(gblJoining.sourceAddress,HEX);
      #endif
      return;
    }
    gblJoining.replaceOffer = false;  // The lost record is gone - fall through to an ordinary offer.
  }
  // further MID will not be sent if this device does not have any available child slots.
  uint8_t _myChild_available_Index = 0;
  uint8_t _i = 1;
  while((_i < 16) & (_myChild_available_Index ==0))
  {
    // Check each index. If the source address is available (0xFFFF), capture that index, and stop the loop.
    if( gblNetwork.children[_i].ChildAddress < 0xFFFF) {_i++;}else{_myChild_available_Index = _i;}
    //Serial.print("Address/_i");Serial.print(gblNetwork.children[_i].ChildAddress);Serial.println(_i);
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
    // PROPOSED (Nov 25, 2026): the fill works on the three PATH digits (the low
    // 12 bits) only - the top digit is the TRUNK number, never a fill position.
    // (This replaces the original computation, which filled all four digits;
    // the validity cap below also changes, from < 0x4FFF to < 0xFFFF.)
    uint16_t _myPath = mySourceAddress & 0x0FFF;
    bool _pathFull = ((_myPath & 0x000F) != 0) && ((_myPath & 0x00F0) != 0) && ((_myPath & 0x0F00) != 0);
    if (_pathFull)
    {
      // No fill position left: the only child this device can offer is the next
      // trunk root - (trunk + 1) with a zeroed path - and only under the trunk
      // rules: a next trunk must exist, this device must be listed permanent
      // (WHAT_PERMANENT in z_WHAT.h), and the trunk address must pass an ACK
      // claim check (REQUEST_ACK_03 to the candidate; a reply means taken).
      uint16_t _trunkCandidate = (mySourceAddress & 0xF000) + 0x1000;
      if ((mySourceAddress & 0xF000) == 0xF000)
      {
        #ifdef CHILD_MONITOR
          Serial.println(F("Path full and no next trunk - no child offer"));
        #endif
        return;
      }
      // PROPOSED (Nov 25, 2026) - Reading B table check (Gary's ruling): under
      // the trunk reading, an address standing in my own child table is SPENT,
      // however the air looks. The step-4 trial re-run exposed the defect this
      // fixes: 0x0111's table still held its 0x1000 child (no ABANDON had
      // released it), the claim check asked the AIR - where the only holder
      // was the searching preemie itself - heard silence, and re-minted
      // 0x1000 into a second slot. My own records are the first and cheapest
      // evidence, so they are consulted before permanence and before the air:
      // table-spent means no offer and no claim check. A verified ABANDON
      // frees the record outright and a replacement supersedes it - those
      // are the only two ways an address is ever released - and only then
      // can the address be offered again.
      for (uint8_t _s = 1; _s < ChildMaxIndex; _s++)
      {
        if (gblNetwork.children[_s].ChildAddress == _trunkCandidate)
        {
          #ifdef CHILD_MONITOR
            Serial.print(F("Trunk address already in my table - not offered: 0x")); Serial.println(_trunkCandidate,HEX);
          #endif
          return;
        }
      }
      #ifndef WHAT_PERMANENT
        #ifdef CHILD_MONITOR
          Serial.println(F("Path full, device not permanent - no trunk offer"));
        #endif
        return;
      #endif
      if (gblJoining.trunkClaimPassed && (gblJoining.trunkCandidate == _trunkCandidate))
      {
        gblJoining.sourceAddress = _trunkCandidate;  // Claim check passed - offer the trunk address (transmit block below).
      }
      else if (gblJoining.trunkClaimPending)
      {
        return;  // Claim check still running - the WHAT_NEW_CHILD_OFFER state resolves it.
      }
      else
      {
        // Start the claim check: ask the candidate address for an ACK and wait
        // out the silence window (the same 5 s founding uses, _RQST_ACK_DELAY).
        gblJoining.trunkCandidate = _trunkCandidate;
        gblJoining.ackSourceAddress = 0xFFFF;
        gblRxFlags.f_REPLY_ACKNOWLEDGE_03 = false;
        gblJoining.trunkClaimDeadline = millis() + 5000;
        gblJoining.trunkClaimPending = true;
        Tx_TRUNK_CLAIM_CHECK(_trunkCandidate);
        gblRegistry.message[gblRegistry.reusableSlot].MessageID = REPLY_ACKNOWLEDGE_03;
        gblRegistry.message[gblRegistry.reusableSlot].callback = &Rx_REPLY_ACKNOWLEDGE_03;
        #ifdef CHILD_MONITOR
          Serial.print(F("Trunk claim check sent to: 0x")); Serial.println(_trunkCandidate,HEX);
        #endif
        return;
      }
    }
    else
    {
      // Fill child: the slot index goes into the lowest zero PATH digit.
      gblJoining.sourceAddress = (_myChild_available_Index) << ( ((_myPath > 0x0) * 4) + ((_myPath > 0x0F) * 4) );
      //Serial.print("Calc child address "); Serial.println(_proposed_child_address,HEX);
      gblJoining.sourceAddress |= mySourceAddress; // This incorporates the parent address (and its trunk digit) into the child's address.
    }
    // *******  NOTE:  This gblJoining.sourceAddress is checked when recieving ACCEPT_PARENT_06 *********** // 
    //Serial.print("mySource"); Serial.println(mySourceAddress,HEX);
    //Serial.print("_proposed_child_address:0x"); Serial.println(_proposed_child_address,HEX);

    if (gblJoining.sourceAddress < 0xFFFF)
    {
  // A valid child address is available. (PROPOSED, Nov 25, 2026: the cap was
  // < 0x4FFF under the old depth rule; any legal proposed address - fill or
  // trunk - is below the broadcast address.)
      WHAT_Packet _WHAT_Tx_Packet;  // Make a temporary transmit packet
      _WHAT_Tx_Packet.WHAT_MAC_MESSAGE;
      _WHAT_Tx_Packet.pan_id = gblNetwork.panID;
      _WHAT_Tx_Packet.destination = 0xFFFF;   // Send this to all devices.
      _WHAT_Tx_Packet.sourceaddress = mySourceAddress;  // Use the source address of this device.
      uint8_t _index = 0;
      _WHAT_Tx_Packet.payload[_index++] = 9; //include the length of the proposed child address. - PROPOSED (Nov 25, 2026): was 8 before the flags byte was appended.
      _WHAT_Tx_Packet.payload[_index++] = 8;   //MID size (for the 2nd message). - PROPOSED (Nov 25, 2026): was 7.
      _WHAT_Tx_Packet.payload[_index++] = highByte(PROPOSE_CHILD_NUMBER_05);// 1 byte length, 2 byte command, 2 byte child address, 2 byte latchkey // Any device that has open child slots replies to request (REQUEST_JOIN_NETWORK_02).
      _WHAT_Tx_Packet.payload[_index++] = lowByte(PROPOSE_CHILD_NUMBER_05); //in order to support multibyte MIDs, put the high byte first.
      _WHAT_Tx_Packet.payload[_index++] = highByte(gblJoining.sourceAddress);//in order to support multibyte MIDs, put the high byte first.
      _WHAT_Tx_Packet.payload[_index++] = lowByte(gblJoining.sourceAddress);
      _WHAT_Tx_Packet.payload[_index++] = highByte(gblJoining.heardTempID);  // STEP 1 (Nov 24, 2026): temp ID echo - binds this offer to the preemie whose search was heard.
      _WHAT_Tx_Packet.payload[_index++] = lowByte(gblJoining.heardTempID);
      _WHAT_Tx_Packet.payload[_index++] = 0;  // PROPOSED (Nov 25, 2026): flags byte - 0x00 = ordinary offer (the replacement build above sends 0x01). Appended at the end, so older builds reading the address and temp ID at their fixed offsets are unaffected.
      WHAT_TransmitPacket(_WHAT_Tx_Packet);

      gblRegistry.message[gblRegistry.reusableSlot].MessageID = ACCEPT_PARENT_06;  // these are prospective parents offering free addresses.
      gblRegistry.message[gblRegistry.reusableSlot].callback = &Rx_ACCEPT_PARENT_06; // Message callback set here to collect the responses
      gblMailbox.networkStateColor = WHAT_LIGHT_OFF;

      gblRxFlags.f_ACCEPT_PARENT_06 = false;   // Clear the message reception flag.

      #ifdef CHILD_MONITOR
        Serial.print(F("Tx Propose child address: 0x")); Serial.println(gblJoining.sourceAddress,HEX);
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
   // FIX (Nov 26, 2026): scan EVERY slot carrying the leaver's address.
   // Duplicate records for one address are real - a device replaced before
   // its old record was released leaves two.  The old code stopped at the
   // FIRST match and tested the proof against that slot only, so a stale
   // record standing in front of the live one made a genuine abandon read
   // as a security breach (seen on the bench Nov 26, 2026: the ghost in
   // slot 1 shadowed the live record in slot 2).  Now the scan releases
   // the slot whose latchkey verifies, and it is a breach only when NO
   // slot with that address verifies.  The loop bound is corrected too:
   // <= ran one slot past the end of the table.
   uint8_t _i=0;
   bool _addressSeen = false;   // At least one slot carries this address.
   bool _released = false;      // One of them verified and was released.
   while (_i<ChildMaxIndex)
   {
      if(_WHAT_Rx_Packet.sourceaddress == gblNetwork.children[_i].ChildAddress)
      { // a slot carries the leaver's address.  Before blanking, make sure it really was this child by checking the proof.
        _addressSeen = true;
        // STEP 1 (Nov 24, 2026): the message carries WHAT_Mix32(latchkey, child address, parent address) at +3..+6 - the latchkey itself is no longer transmitted, so the stored latchkey is never compared against anything that crossed the air.
        if(WHAT_PayloadGet32(_WHAT_Rx_Packet.payload, _index + 3) == WHAT_Mix32(gblNetwork.children[_i].LatchKeyValue, _WHAT_Rx_Packet.sourceaddress, mySourceAddress))
        { // This is the child.  Remove it from the index.
            gblNetwork.children[_i].ChildAddress = 0xFFFF;
            gblNetwork.children[_i].LatchKeyValue = 0xFFFFFFFF;
            gblNetwork.children[_i].RelationshipType = 0;
            gblNetwork.children[_i].Security = 0xFFFF;
            // PROPOSED (Nov 25, 2026): two completions this handler always
            // owed. (1) SAVE the release - the blanking used to be RAM-only,
            // so a power cycle resurrected the released child from EEPROM and
            // its address stayed spent forever. (2) CONFIRM it - the leaver
            // is waiting (bounded retries) to hear that its release is
            // durable before it wipes itself.
            WHAT_StoreEEPROM();
            {
              WHAT_Packet _WHAT_Tx_Packet;  // Make a temporary transmit packet
              _WHAT_Tx_Packet.WHAT_MAC_MESSAGE;
              _WHAT_Tx_Packet.pan_id = gblNetwork.panID;
              _WHAT_Tx_Packet.destination = _WHAT_Rx_Packet.sourceaddress;   // Send this to the leaving child.
              _WHAT_Tx_Packet.sourceaddress = mySourceAddress;  // Use the source address of this device.
              uint8_t _confirmIndex = 0;
              _WHAT_Tx_Packet.payload[_confirmIndex++] = 4; //Total payload length (including this byte)
              _WHAT_Tx_Packet.payload[_confirmIndex++] = 3; //MID length (including this byte)
              _WHAT_Tx_Packet.payload[_confirmIndex++] = highByte(REPLY_ABANDON_CONFIRM_03);
              _WHAT_Tx_Packet.payload[_confirmIndex++] = lowByte(REPLY_ABANDON_CONFIRM_03);
              WHAT_TransmitPacket(_WHAT_Tx_Packet);
            }
            #ifdef CHILD_MONITOR
              Serial.println(F("Child released and confirmed"));
            #endif
            _released = true;
            _i = ChildMaxIndex;   // Released - stop scanning (the increment below carries the index past the loop bound).
        }//End Child removed
        // A failed proof on THIS slot is not a breach yet: a duplicate
        // record further down the table may still verify, so keep scanning.
      }// End if child match found.
    _i++; //Check the next entry.
   }// End while looking for child entry.
   if(_addressSeen && !_released)
   {// Security breach... the proof value matched no record of this address.  Note the security breach (not yet implimented)
          WHAT_Packet _WHAT_Tx_Packet;  // Make a temporary transmit packet
          _WHAT_Tx_Packet.WHAT_MAC_MESSAGE;
          _WHAT_Tx_Packet.pan_id = gblNetwork.panID;
          _WHAT_Tx_Packet.destination = 0xFFFF;   // Send this to all devices.
          _WHAT_Tx_Packet.sourceaddress = mySourceAddress;  // Use the source ddress of this device.
          uint8_t _index = 0;
          _WHAT_Tx_Packet.payload[_index++] = 5; //Total payload length (including this byte)
          _WHAT_Tx_Packet.payload[_index++] = 4; //MID length (including this byte)
          _WHAT_Tx_Packet.payload[_index++] = highByte(Security_Breach_04);
          _WHAT_Tx_Packet.payload[_index++] = lowByte(Security_Breach_04);
          _WHAT_Tx_Packet.payload[_index++] = SB_BUMP;  //(bump)   A parent sees a request to abandon child, but the proof value is incorrect.
          WHAT_TransmitPacket(_WHAT_Tx_Packet);
   }// End security breach
}

// *****************
// TOC Individual Transmit messages
void Rx_REQUEST_ACK_03 (void)
{
    gblRxFlags.f_REQUEST_ACK_03 = true;    // Clear the message flag so only new messages will be used.
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
    _WHAT_Tx_Packet.pan_id = gblNetwork.panID;
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
  //Serial.print("Connected.   PANID:0x");Serial.println(gblNetwork.panID,HEX);
  //Serial.print("           Address:0x");Serial.println(mySourceAddress,HEX);
  //Serial.print("            Parent:0x");Serial.println(gblNetwork.parentAddress,HEX);
  //Serial.print("          LatchKey:0x");Serial.println(myLatchKeyNumber,HEX);
  

}

#endif  // If auto-addressing is defined.
