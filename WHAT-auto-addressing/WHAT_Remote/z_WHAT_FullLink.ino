#include "z_WHAT_FullLink.h"   // -This routine- as part of the WHAT system.

// ***************************
// TableOfContents:
// TOC Global Variables
// TOC Begin
// TOC Loop
// TOC Link operations 



// *************************************************************************
// *************************************************************************
// TOC Begin
void WHAT_LINK_begin (void)
{
  WHAT_Reusable_Message2 = RegisterWHATMessage (0xFFFF, &holdingFunction); // This entry can be re-used during network adoption.
  RegisterWHATMessage (REMOVE_FULL_LINK_03, &Rx_REMOVE_FULL_LINK_03); // This is an entry that will not change.
  WHAT_HardwareSetup();

  //RegisterWHATMessage (REQUEST_FULL_LINK_03, &Rx_REQUEST_FULL_LINK_03); // This is an entry that will not change.
  //RegisterWHATMessage (ACCEPT_FULL_LINK_03, &Rx_ACCEPT_FULL_LINK_03); // This is an entry that will not change.
  //RegisterWHATMessage (REJECT_FULL_LINK_03, &Rx_REJECT_FULL_LINK_03); // This is an entry that will not change.

}
// TOC Loop
void Loop_WHAT_LINK (void)
{
  WHAT_StateCheck();  // Assess the link state
  WHAT_HardwareCheck();


}

// *****************
// TOC Individual Recieve messages
void  Rx_REQUEST_FULL_LINK_03(WHAT_Packet _WHAT_Rx_Packet, uint8_t _index)
{
  _Offered_SourceAddress = _WHAT_Rx_Packet.sourceaddress;
  f_Rx_REQUEST_FULL_LINK_03 = true;   
   #ifdef LINK_MONITOR
  Serial.println(F("Rx Rqst Link"));
   #endif
}
// *****************
// TOC Individual Transmit messages
void Tx_REQUEST_FULL_LINK(void)
{
   #ifdef LINK_MONITOR
  Serial.println(F("Tx REQUEST_FULL_LINK"));
   #endif
    WHAT_Packet _WHAT_Tx_Packet;  // Make a temporary transmit packet
    _WHAT_Tx_Packet.WHAT_MAC_MESSAGE;
    _WHAT_Tx_Packet.pan_id = myPANID;
    _WHAT_Tx_Packet.destination = 0xFFFF;   // Send this to all devices.
    _WHAT_Tx_Packet.sourceaddress = mySourceAddress;  // Use the source ddress of this device.
    uint8_t _index = 0;
    _WHAT_Tx_Packet.payload[_index++] = 4; //Total payload length (including this byte)
    _WHAT_Tx_Packet.payload[_index++] = 3; //MID length (including this byte)
    _WHAT_Tx_Packet.payload[_index++] = highByte(REQUEST_FULL_LINK_03);
    _WHAT_Tx_Packet.payload[_index++] = lowByte(REQUEST_FULL_LINK_03);
        WHAT_TransmitPacket(_WHAT_Tx_Packet);
}

// *****************
// TOC Individual Recieve messages
void  Rx_ACCEPT_FULL_LINK_03(WHAT_Packet _WHAT_Rx_Packet, uint8_t _index)
{
  _Offered_SourceAddress = _WHAT_Rx_Packet.sourceaddress;
  f_Rx_ACCEPT_FULL_LINK_03 = true;
   #ifdef LINK_MONITOR
  Serial.println(F("Rx Accept Link"));
   #endif
}

void Rx_REJECT_FULL_LINK_03(WHAT_Packet _WHAT_Rx_Packet, uint8_t _index)
{  // This would be recieved if some other device started creating a link, but then timed out.
   _Offered_SourceAddress = _WHAT_Rx_Packet.sourceaddress;
   COMMANDED_OUTPUTS = _WHAT_Rx_Packet.payload[_index + 4];
   f_Rx_REJECT_FULL_LINK_03 = true;
   #ifdef LINK_MONITOR
     Serial.println(F("Rx REJECT Link"));
   #endif

}

// *****************
// TOC Individual Transmit messages
void Tx_REJECT_FULL_LINK_03 (void)
{   
   #ifdef LINK_MONITOR
  Serial.println(F("Tx REJECT_FULL_LINK"));
   #endif
    WHAT_Packet _WHAT_Tx_Packet;  // Make a temporary transmit packet
    _WHAT_Tx_Packet.WHAT_MAC_MESSAGE;
    _WHAT_Tx_Packet.pan_id = myPANID;
    _WHAT_Tx_Packet.destination = 0xFFFF;   // Send this to all devices.
    _WHAT_Tx_Packet.sourceaddress = mySourceAddress;  // Use the source ddress of this device.
    uint8_t _index = 0;
    _WHAT_Tx_Packet.payload[_index++] = 4; //Total payload length (including this byte)
    _WHAT_Tx_Packet.payload[_index++] = 3; //MID length (including this byte)
    _WHAT_Tx_Packet.payload[_index++] = highByte(REJECT_FULL_LINK_03);
    _WHAT_Tx_Packet.payload[_index++] = lowByte(REJECT_FULL_LINK_03);
        WHAT_TransmitPacket(_WHAT_Tx_Packet);
}

// *****************
// TOC Individual Recieve messages
void Rx_REMOVE_FULL_LINK_03 (WHAT_Packet _WHAT_Rx_Packet, uint8_t _index)
{ // An already linked device indicated it wanted to be un-linked.
  // locate that device in the link list and set its address to "Available"
  _Offered_SourceAddress = _WHAT_Rx_Packet.sourceaddress;

  f_Rx_REMOVE_FULL_LINK_03 = true;
   #ifdef LINK_MONITOR
   Serial.println(F("Rx REMOVE_FULL_LINK"));
   #endif
}

void Rx_CMD_DIG01_04 (WHAT_Packet _WHAT_Rx_Packet, uint8_t _index)
{
  _Offered_SourceAddress = _WHAT_Rx_Packet.sourceaddress;
  f_Rx_CMD_DIG01_04 = true;
   #ifdef LINK_MONITOR
   Serial.println(F("Rx Digital Command"));
   #endif
}

// *****************
 // other supporting functions.
uint8_t  isLinkAvailable(uint16_t _Offered_SourceAddress = 0xFFFF)
{    
        uint8_t _blankspot = 0xFF;
        _WHAT_Already_Linked = numWHATLinks;
        // Link is being requested.
        for (uint8_t _i=0;_i<numWHATLinks;_i++)
        {
          #ifdef LINK_MONITOR
            Serial.println(WHATLinks[_i],HEX);
          #endif
          if((WHATLinks[_i] == _Offered_SourceAddress)) _WHAT_Already_Linked = _i; // This device was previously linked.
          if((_blankspot == 0xFF) & (WHATLinks[_i] == 0xFFFF)) _blankspot = _i; // Keep track fo teh first blank spot
        }
        if(_WHAT_Already_Linked == numWHATLinks) _WHAT_Already_Linked = _blankspot; // if not already linked, use the blank spot.
        // Now "_WHAT_Already_Linked" = either the location of previous link, an empty spot, or 0xFF if neither found,
      #ifdef LINK_MONITOR
      Serial.print(F("_WHAT_Already_Linked value: "));Serial.println(_WHAT_Already_Linked);
      //Serial.print(F("WHATLinks[_i] value: "));Serial.println(WHATLinks[_WHAT_Already_Linked]);
      #endif
      return _WHAT_Already_Linked;
}


// TOC Link operations 
void WHAT_LinkCheck(void)
{

  //TOC Evaluate WHAT LINK state
  switch (_WHAT_LINK_State)
  {
    /**********************************************************************************/
    /**********************************************************************************/
    /**********************************************************************************/

    case  WHAT_NO_LINK:
      if( (_WHAT_NetworkButton == HIGH) & (mySourceAddress < 0xFFFF) )
       { // If the button is held and this is NOT a preemie, start the linking process.
          _WHAT_Already_Linked = isLinkAvailable(); // Check and see if a link location if available.
          if(_WHAT_Already_Linked < numWHATLinks)
          { //only move to the next phase if a link location is available.
            WHAT_Reusable_Timer1 = millis() + _WHAT_NEW_LINK_HELD_TIME_DELAY; // Network button must be held for a while before acting on the link.
           _WHAT_LINK_State = WHAT_PRE_LINKING_INITIATED;  // This device is initiating a link
          }
          else
          {
            #ifdef LINK_MONITOR
            Serial.println(F("No Link Spots Available"));
            #endif
          }
       }
      if (f_Rx_REQUEST_FULL_LINK_03)     // If another device wants to link to this device, the link will be initiated.
      {
        _WHAT_Already_Linked = isLinkAvailable(); // Check and see if a link location if available.
        if(_WHAT_Already_Linked < numWHATLinks)
        {
          f_Rx_REQUEST_FULL_LINK_03 = false; // Mark the reception flag as used so it is not duplicated.
          WHAT_Reusable_Timer1 = millis() + _WHAT_NEW_LINK_HELD_TIME_DELAY; // Network button must be held for a while before acting on the link.
          _WHAT_LINK_State = WHAT_PRE_LINK_REQUESTED;
          #ifdef LINK_MONITOR
           Serial.print(F("_WHAT_network_State: WHAT_PRE_LINK_REQUESTED ")); Serial.println(_WHAT_network_State);
          #endif
        }
        else
        {
          #ifdef LINK_MONITOR
           Serial.println(F("No Available Link Spots"));
          #endif
        }
      }
      if(f_Rx_REMOVE_FULL_LINK_03)
      {
        f_Rx_REMOVE_FULL_LINK_03 = false;   // Make sure previously set flag is cleared.  
        _WHAT_LINK_State = WHAT_REMOVE_LINK;
      }

   break;

// ************************************************************************
// ************************************************************************
// ************************************************************************
// ************************************************************************
    case  WHAT_PRE_LINKING_INITIATED:
          if (_WHAT_NetworkButton == HIGH)
          { // Button is still held.
            _Control_Net_Light(/*LightColor1*/ WHAT_LIGHT_GREEN,/*Light Color 1 time*/ 200,/*LightColor2 */ WHAT_LIGHT_OFF,/*Light Color 2 time*/ 200 );

            if(WHAT_Reusable_Timer1 <= millis())
            { // Network button was held long enough.
              WHATRxMessage[WHAT_Reusable_Message1].MessageID = ACCEPT_FULL_LINK_03;    // A preemie is trying to gain access to the network.
              WHATRxMessage[WHAT_Reusable_Message1].callback = &Rx_ACCEPT_FULL_LINK_03; // Message callback set here to collect the responses
              WHATRxMessage[WHAT_Reusable_Message2].MessageID = REJECT_FULL_LINK_03;    // A preemie is trying to gain access to the network.
              WHATRxMessage[WHAT_Reusable_Message2].callback = &Rx_REJECT_FULL_LINK_03; // Message callback set here to collect the responses
              _WHAT_network_StateColor = WHAT_LIGHT_OFF;
              f_Rx_REQUEST_FULL_LINK_03 = false;  // Make sure previously set flag is cleared.  
              f_Rx_ACCEPT_FULL_LINK_03 = false;  // Make sure previously set flag is cleared.     
              f_Rx_REJECT_FULL_LINK_03 = false;  // Make sure previously set flag is cleared.  
              Tx_REQUEST_FULL_LINK();  // Request linking across the network.
              #ifdef LINK_MONITOR
                  Serial.println(F("Tx Rqst Link"));
              #endif
              WHAT_Reusable_Timer1 = millis() + _WHAT_NEW_Link_TIME_DELAY;
              _WHAT_LINK_State = WHAT_LINKING_INITIATED;

            }
          }
           else
          { // If the button is released before the required amount of time, do not initiate the state.
            _WHAT_network_StateColor = WHAT_LIGHT_OFF;
            _WHAT_LINK_State = WHAT_NO_LINK;
          }
        break;

 // ************************************************************************
// TOC    This Device attempting to link to another device 
    case WHAT_LINKING_INITIATED:
        // This device requested a link.  wait for another device to accept the link.
        // Blink GREEN while waiting.
      _Control_Net_Light(/*LightColor1*/ WHAT_LIGHT_GREEN,/*Light Color 1 time*/ 500,/*LightColor2 */ WHAT_LIGHT_BLUE,/*Light Color 2 time*/ 100 );

      if(WHAT_Reusable_Timer1 < millis()) 
        {
          #ifdef LINK_MONITOR
          Serial.println(F("Link to me -timeout-"));
          Serial.println(F("Tx_REJECT_FULL_LINK_03"));
        #endif
          Tx_REJECT_FULL_LINK_03();   // Let other devices know the link was rejected.
           WHAT_Reusable_Timer1 = millis() + _RQST_WAIT_TIME; // Keep it red for several seconds.
           _WHAT_network_StateColor = WHAT_LIGHT_RED;
           _WHAT_LINK_State = WHAT_LINK_COMPLETED; // no response.  indicate that the link was not accepted.  
        }
      if(f_Rx_ACCEPT_FULL_LINK_03)
      { // Link accepted.  
        // Make sure the offered address is not already linked...
        f_Rx_ACCEPT_FULL_LINK_03 = false;     // Make sure previously set flag is cleared.  
        _WHAT_Already_Linked = isLinkAvailable(); // Check and see if a link location if available.
        if(_WHAT_Already_Linked < numWHATLinks)
        { // A Link spot is available
          if(WHATLinks[_WHAT_Already_Linked] == _Offered_SourceAddress)
          { // If the same address tries to link again, it should be removed from the link:
            WHATLinks[_WHAT_Already_Linked] = 0xFFFF;
            #ifdef LINK_MONITOR
              Serial.println(F("Link to me -Removed-"));
              Serial.print(F("List Location: "));Serial.println(_WHAT_Already_Linked);
              //Serial.print(F("WHATLinks[_i] value: 0x"));Serial.println(WHATLinks[_WHAT_Already_Linked],HEX);
            #endif
            _WHAT_network_StateColor = WHAT_LIGHT_RED;
            WHAT_Reusable_Timer1 = millis() + _RQST_WAIT_TIME; // Keep it RED for several seconds.
            _WHAT_LINK_State = WHAT_LINK_COMPLETED; // no response.  act like it never happened.  
          }
          else
          {
            WHATLinks[_WHAT_Already_Linked] = _Offered_SourceAddress;
            #ifdef LINK_MONITOR
              Serial.println(F("Link to me -Accepted-"));
              Serial.print(F("List Location: "));Serial.println(_WHAT_Already_Linked);
              //Serial.print(F("WHATLinks[_i] value: 0x"));Serial.println(WHATLinks[_WHAT_Already_Linked],HEX);
            #endif
            _WHAT_network_StateColor = WHAT_LIGHT_GREEN;
            WHAT_Reusable_Timer1 = millis() + _RQST_WAIT_TIME; // Keep it green for several seconds.
            _WHAT_LINK_State = WHAT_LINK_COMPLETED; // no response.  act like it never happened.  
          }
        }
        else
        { // for some reason, there are no longer availble links on this device.
            _WHAT_network_StateColor = WHAT_LIGHT_RED;
            #ifdef LINK_MONITOR
              Serial.println(F("No Links available suddenly"));
              Serial.print(F("_WHAT_Already_Linked: "));Serial.println(_WHAT_Already_Linked);
              //Serial.print(F("WHATLinks[_i] value: 0x"));Serial.println(WHATLinks[_WHAT_Already_Linked],HEX);
            #endif
            WHAT_Reusable_Timer1 = millis() + _RQST_WAIT_TIME; // Keep it green for several seconds.
            _WHAT_LINK_State = WHAT_LINK_COMPLETED; // no response.  act like it never happened.  

        }
      }
    break;


// ************************************************************************
    case WHAT_PRE_LINK_REQUESTED:
        //another device is requesting a link.  If the button on this is pressed, this will join.
        // Before giving the chance, make sure this device is not already connected.
        // ALso, if this device is already linked to this device, pressing the button will remove the link.
        
        _WHAT_Already_Linked = isLinkAvailable(_Offered_SourceAddress); // Check and see if a link location if available.
        if(WHATLinks[_WHAT_Already_Linked] == _Offered_SourceAddress )
        {
          WHAT_Reusable_Timer1 = millis() + _RQST_WAIT_TIME;
          _WHAT_LINK_State = WHAT_REMOVE_LINK; //Prepare to remove link
        }
        else
        {
          _WHAT_LINK_State = WHAT_LINK_REQUESTED; // Prepare to accept link

        }
 
      break;

// ************************************************************************
    case WHAT_LINK_REQUESTED:
// TOC    Another device is attempting to link to this (or another) device
        // If the button on this device is pressed, make a link.

      if( (_WHAT_NetworkButton == HIGH) & (mySourceAddress <0xFFFF) )
      {
        // WHAT_PRE_LINK_REQUESTED already verified that this device has a free spot and not already linked to this source address. 
        _WHAT_Already_Linked = isLinkAvailable(_Offered_SourceAddress); // find the free spot where this link will be marked.
        WHATLinks[_WHAT_Already_Linked] = _Offered_SourceAddress;
        _EEPROM_store_links();  // Update the link list table with the new address.
        // Tx link Confirmation
        WHAT_Packet _WHAT_Tx_Packet;  // Make a temporary transmit packet
        _WHAT_Tx_Packet.WHAT_MAC_MESSAGE;
        // In all cases, if a child address is being proposed, also inform the network that a preemie wants to join.
        // In addition to proposing a child, tell the entire network that a child is trying to join:
        _WHAT_Tx_Packet.pan_id = myPANID;
        _WHAT_Tx_Packet.destination = _Offered_SourceAddress;   // Send this to all devices.
        _WHAT_Tx_Packet.sourceaddress = mySourceAddress;  // Use the source ddress of this device.
        uint8_t _index = 0;
        _WHAT_Tx_Packet.payload[_index++] = 4; //Total payload length (including this byte)
        _WHAT_Tx_Packet.payload[_index++] = 3; //MID length (including this byte)
        _WHAT_Tx_Packet.payload[_index++] = highByte(ACCEPT_FULL_LINK_03);
        _WHAT_Tx_Packet.payload[_index++] = lowByte(ACCEPT_FULL_LINK_03);
        WHAT_TransmitPacket(_WHAT_Tx_Packet);
      #ifdef LINK_MONITOR
           Serial.print(F("Link Accepted to : 0x"));Serial.println(_Offered_SourceAddress,HEX);
      #endif
        _WHAT_network_StateColor = WHAT_LIGHT_GREEN;
        WHAT_Reusable_Timer1 = millis() + _RQST_WAIT_TIME; // Keep it green for several seconds.
        _WHAT_LINK_State = WHAT_LINK_COMPLETED; // no response.  act like it never happened.  
 
      }
      if(f_Rx_REJECT_FULL_LINK_03)
      {   
        f_Rx_REJECT_FULL_LINK_03 = false; // Make sure previously set flag is cleared.  
      #ifdef LINK_MONITOR
        Serial.println(F("Link to me -Rx reject-"));
        _WHAT_LINK_State =  WHAT_NO_LINK; // Reset to the "normal" state when the timer expires.
      #endif
      }

      if(WHAT_Reusable_Timer1 <= millis())
      {
      #ifdef LINK_MONITOR
       Serial.println(F("Link to me -timeout-"));
        _WHAT_LINK_State =  WHAT_NO_LINK; // Reset to the "normal" state when the timer expires.
      #endif
      }

     break;

    case WHAT_REMOVE_LINK:
      if( (_WHAT_NetworkButton == HIGH) & (mySourceAddress <0xFFFF) )
      {
         _WHAT_Already_Linked = isLinkAvailable(_Offered_SourceAddress); // Find where this source address is stored. 
        WHATLinks[_WHAT_Already_Linked] = 0xFFFF; //remove the link to this address.
        _WHAT_network_StateColor = WHAT_LIGHT_RED;
        WHAT_Reusable_Timer1 = millis() + _RQST_WAIT_TIME; // Keep it green for several seconds.
        _WHAT_LINK_State = WHAT_LINK_COMPLETED; // no response.  act like it never happened.  
        #ifdef LINK_MONITOR
        Serial.print(F("Unlink :0x"));Serial.println(WHATLinks[_WHAT_Already_Linked]);
        #endif
        _EEPROM_store_links();  // Update the link list table with the new address.
        WHAT_Packet _WHAT_Tx_Packet;  // Make a temporary transmit packet
        _WHAT_Tx_Packet.WHAT_MAC_MESSAGE;
        // In all cases, if a link is being removed, also inform the other device that it should be removed from the link table.
        _WHAT_Tx_Packet.pan_id = myPANID;
        _WHAT_Tx_Packet.destination = _Offered_SourceAddress;   // Send this to the previously linked device.
        _WHAT_Tx_Packet.sourceaddress = mySourceAddress;  // Use the source address of this device.
        uint8_t _index = 0;
        _WHAT_Tx_Packet.payload[_index++] = 4; //Total payload length (including this byte)
        _WHAT_Tx_Packet.payload[_index++] = 3; //MID length (including this byte)
        _WHAT_Tx_Packet.payload[_index++] = highByte(REMOVE_FULL_LINK_03);
        _WHAT_Tx_Packet.payload[_index++] = lowByte(REMOVE_FULL_LINK_03);
          WHAT_TransmitPacket(_WHAT_Tx_Packet);
      }
      if(WHAT_Reusable_Timer1 <= millis())
      {
        Serial.println(F("REMOVE link -timeout-"));
        _WHAT_LINK_State =  WHAT_NO_LINK; // Reset to the "normal" state when the timer expires.
      }
    break;
 
    case  WHAT_LINK_COMPLETED:
        if (WHAT_Reusable_Timer2 <= millis())
        {  // The button was held long enough.
          _WHAT_network_StateColor = WHAT_LIGHT_OFF;
        #ifdef LINK_MONITOR
            Serial.println(F("WHAT_LINKING_FAIL"));
          #endif
        _WHAT_LINK_State = WHAT_NO_LINK;
        }
    break;


    default:
        // If in an invalid state, revert to no link.
        #ifdef LINK_MONITOR
        //Serial.print(F("Offered PANID:0x")); Serial.println(_Offered_PANID);
        Serial.print(F("Undefined LINK state : ")); Serial.println(_WHAT_LINK_State,DEC);
        #endif
        //_WHAT_network_State = WHAT_PRE_REQUEST_JOIN_NETWORK_03; // If nothing offered a child slot, return to being a preemie.
        _WHAT_LINK_State = WHAT_NO_LINK;  //
      break;

  }
}

void  WHAT_HardwareSetup(void)
{
  for (uint8_t _i = 0; _i <= NUM_INPUTS; _i++)
  {
      pinMode(_InputPinForLink[_i],INPUT);
  }
  for (uint8_t _i = 0; _i <= NUM_OUTPUTS; _i++)
  {
      pinMode(_OutputPinForLink[_i],OUTPUT);
  }
}

void WHAT_HardwareCheck(void)
{
  uint8_t _this_InputValueForLink = 0;   // This is a combined value for the inputs.
  uint8_t _this_OutputValueForLink = 0;   // This is a combined value for the inputs.

  static uint8_t _last_InputValueForLink;   // This is a combined value for the inputs.
  static uint8_t _last_OutputValueForLink;   // This is a combined value for the inputs.
  static double  _transmission_delay = millis();       // Limit I/O updates using this timer.

  // *********
 // -IF- an input value changed, transmit the new value.
  for (uint8_t _i = 0; _i <= NUM_INPUTS; _i++)
  {
      _this_InputValueForLink = (((_i*2)*digitalRead(_InputPinForLink[_i])) | _this_InputValueForLink);
          // This takes each bit, and shifts it (right to left) to fill the input value bit.
          // After all itterations, it is a single byte representing "NUM_INPUTS" bits.
  } 
  if((_this_InputValueForLink != _last_InputValueForLink) & (_transmission_delay <= millis()))
  { // the input status has changed.  send the new value.
       WHAT_Packet _WHAT_Tx_Packet;    // Create a temporary transmit packet.
      _WHAT_Tx_Packet.pan_id = myPANID;
      _WHAT_Tx_Packet.destination = 0xFFFF;   // Send this to all devices.
      _WHAT_Tx_Packet.sourceaddress = mySourceAddress;  // Use the source address of this device.
      uint8_t _index = 0;
      _WHAT_Tx_Packet.payload[_index++] = 5; //Total payload length (including this byte)
      _WHAT_Tx_Packet.payload[_index++] = 4; //MID length (including this byte)
      _WHAT_Tx_Packet.payload[_index++] = highByte(CMD_DIG01_04);
      _WHAT_Tx_Packet.payload[_index++] = lowByte(CMD_DIG01_04);
      _WHAT_Tx_Packet.payload[_index++] = _this_InputValueForLink;  // This is a value for confirming allowance.
    WHAT_TransmitPacket(_WHAT_Tx_Packet);
    _last_InputValueForLink = _this_InputValueForLink;
    _transmission_delay = millis() + _WHAT_LINK_INPUT_UPDATE_RATE;

     #ifdef LINK_MONITOR
     Serial.println(F("Linked Input Update Sent : ")); 
     #endif
 
  }


  // *********
  // -IF- a I/O message arrived, verify it is from a LINKED address, then act on it.
  if( f_Rx_CMD_DIG01_04)
  {
    f_Rx_CMD_DIG01_04 = false;  // Make sure previously set flag is cleared.  
    _WHAT_Already_Linked = isLinkAvailable(_Offered_SourceAddress); // Find -IF- this source address is stored. 
    if(_WHAT_Already_Linked < numWHATLinks)
    {
      for (uint8_t _i = 0; _i <= NUM_OUTPUTS; _i*2)
      {
          digitalWrite(_OutputValueForLink[_i],(COMMANDED_OUTPUTS & _i));
            //i moves binary one bit at a time through the commanded outputs to isolate the specific commanded output value.
      }

    }
  }

}
