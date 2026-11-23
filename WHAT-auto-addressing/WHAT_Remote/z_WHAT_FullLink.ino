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
  gblLink.reusableSlot = RegisterWHATMessage (0xFFFF, &holdingFunction); // This entry can be re-used during network adoption.
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
  gblJoining.sourceAddress = _WHAT_Rx_Packet.sourceaddress;
  gblLinkRxFlags.f_REQUEST_FULL_LINK_03 = true;   
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
    _WHAT_Tx_Packet.pan_id = gblNetwork.panID;
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
  gblJoining.sourceAddress = _WHAT_Rx_Packet.sourceaddress;
  gblLinkRxFlags.f_ACCEPT_FULL_LINK_03 = true;
   #ifdef LINK_MONITOR
  Serial.println(F("Rx Accept Link"));
   #endif
}

void Rx_REJECT_FULL_LINK_03(WHAT_Packet _WHAT_Rx_Packet, uint8_t _index)
{  // This would be recieved if some other device started creating a link, but then timed out.
   gblJoining.sourceAddress = _WHAT_Rx_Packet.sourceaddress;
   gblLink.commandedOutputs = _WHAT_Rx_Packet.payload[_index + 4];
   gblLinkRxFlags.f_REJECT_FULL_LINK_03 = true;
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
    _WHAT_Tx_Packet.pan_id = gblNetwork.panID;
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
  gblJoining.sourceAddress = _WHAT_Rx_Packet.sourceaddress;

  gblLinkRxFlags.f_REMOVE_FULL_LINK_03 = true;
   #ifdef LINK_MONITOR
   Serial.println(F("Rx REMOVE_FULL_LINK"));
   #endif
}

void Rx_CMD_DIG01_04 (WHAT_Packet _WHAT_Rx_Packet, uint8_t _index)
{
  gblJoining.sourceAddress = _WHAT_Rx_Packet.sourceaddress;
  gblLinkRxFlags.f_CMD_DIG01_04 = true;
   #ifdef LINK_MONITOR
   Serial.println(F("Rx Digital Command"));
   #endif
}

// *****************
 // other supporting functions.
uint8_t  isLinkAvailable(uint16_t _offeredSourceAddress = 0xFFFF)
{    
        uint8_t _blankspot = 0xFF;
        gblLink.alreadyLinked = numWHATLinks;
        // Link is being requested.
        for (uint8_t _i=0;_i<numWHATLinks;_i++)
        {
          #ifdef LINK_MONITOR
            Serial.println(gblLink.links[_i],HEX);
          #endif
          if((gblLink.links[_i] == _offeredSourceAddress)) gblLink.alreadyLinked = _i; // This device was previously linked.
          if((_blankspot == 0xFF) & (gblLink.links[_i] == 0xFFFF)) _blankspot = _i; // Keep track fo teh first blank spot
        }
        if(gblLink.alreadyLinked == numWHATLinks) gblLink.alreadyLinked = _blankspot; // if not already linked, use the blank spot.
        // Now "gblLink.alreadyLinked" = either the location of previous link, an empty spot, or 0xFF if neither found,
      #ifdef LINK_MONITOR
      Serial.print(F("gblLink.alreadyLinked value: "));Serial.println(gblLink.alreadyLinked);
      //Serial.print(F("gblLink.links[_i] value: "));Serial.println(gblLink.links[gblLink.alreadyLinked]);
      #endif
      return gblLink.alreadyLinked;
}


// TOC Link operations 
void WHAT_LinkCheck(void)
{

  //TOC Evaluate WHAT LINK state
  switch (gblLink.state)
  {
    /**********************************************************************************/
    /**********************************************************************************/
    /**********************************************************************************/

    case  WHAT_NO_LINK:
      if( (gblMailbox.networkButton == HIGH) & (mySourceAddress < 0xFFFF) )
       { // If the button is held and this is NOT a preemie, start the linking process.
          gblLink.alreadyLinked = isLinkAvailable(); // Check and see if a link location if available.
          if(gblLink.alreadyLinked < numWHATLinks)
          { //only move to the next phase if a link location is available.
            gblScratch.timer1 = millis() + _WHAT_NEW_LINK_HELD_TIME_DELAY; // Network button must be held for a while before acting on the link.
           gblLink.state = WHAT_PRE_LINKING_INITIATED;  // This device is initiating a link
          }
          else
          {
            #ifdef LINK_MONITOR
            Serial.println(F("No Link Spots Available"));
            #endif
          }
       }
      if (gblLinkRxFlags.f_REQUEST_FULL_LINK_03)     // If another device wants to link to this device, the link will be initiated.
      {
        gblLink.alreadyLinked = isLinkAvailable(); // Check and see if a link location if available.
        if(gblLink.alreadyLinked < numWHATLinks)
        {
          gblLinkRxFlags.f_REQUEST_FULL_LINK_03 = false; // Mark the reception flag as used so it is not duplicated.
          gblScratch.timer1 = millis() + _WHAT_NEW_LINK_HELD_TIME_DELAY; // Network button must be held for a while before acting on the link.
          gblLink.state = WHAT_PRE_LINK_REQUESTED;
          #ifdef LINK_MONITOR
           Serial.print(F("gblMailbox.networkState: WHAT_PRE_LINK_REQUESTED ")); Serial.println(gblMailbox.networkState);
          #endif
        }
        else
        {
          #ifdef LINK_MONITOR
           Serial.println(F("No Available Link Spots"));
          #endif
        }
      }
      if(gblLinkRxFlags.f_REMOVE_FULL_LINK_03)
      {
        gblLinkRxFlags.f_REMOVE_FULL_LINK_03 = false;   // Make sure previously set flag is cleared.  
        gblLink.state = WHAT_REMOVE_LINK;
      }

   break;

// ************************************************************************
// ************************************************************************
// ************************************************************************
// ************************************************************************
    case  WHAT_PRE_LINKING_INITIATED:
          if (gblMailbox.networkButton == HIGH)
          { // Button is still held.
            _Control_Net_Light(/*LightColor1*/ WHAT_LIGHT_GREEN,/*Light Color 1 time*/ 200,/*LightColor2 */ WHAT_LIGHT_OFF,/*Light Color 2 time*/ 200 );

            if(gblScratch.timer1 <= millis())
            { // Network button was held long enough.
              gblRegistry.message[gblRegistry.reusableSlot].MessageID = ACCEPT_FULL_LINK_03;    // A preemie is trying to gain access to the network.
              gblRegistry.message[gblRegistry.reusableSlot].callback = &Rx_ACCEPT_FULL_LINK_03; // Message callback set here to collect the responses
              gblRegistry.message[gblLink.reusableSlot].MessageID = REJECT_FULL_LINK_03;    // A preemie is trying to gain access to the network.
              gblRegistry.message[gblLink.reusableSlot].callback = &Rx_REJECT_FULL_LINK_03; // Message callback set here to collect the responses
              gblMailbox.networkStateColor = WHAT_LIGHT_OFF;
              gblLinkRxFlags.f_REQUEST_FULL_LINK_03 = false;  // Make sure previously set flag is cleared.  
              gblLinkRxFlags.f_ACCEPT_FULL_LINK_03 = false;  // Make sure previously set flag is cleared.     
              gblLinkRxFlags.f_REJECT_FULL_LINK_03 = false;  // Make sure previously set flag is cleared.  
              Tx_REQUEST_FULL_LINK();  // Request linking across the network.
              #ifdef LINK_MONITOR
                  Serial.println(F("Tx Rqst Link"));
              #endif
              gblScratch.timer1 = millis() + _WHAT_NEW_Link_TIME_DELAY;
              gblLink.state = WHAT_LINKING_INITIATED;

            }
          }
           else
          { // If the button is released before the required amount of time, do not initiate the state.
            gblMailbox.networkStateColor = WHAT_LIGHT_OFF;
            gblLink.state = WHAT_NO_LINK;
          }
        break;

 // ************************************************************************
// TOC    This Device attempting to link to another device 
    case WHAT_LINKING_INITIATED:
        // This device requested a link.  wait for another device to accept the link.
        // Blink GREEN while waiting.
      _Control_Net_Light(/*LightColor1*/ WHAT_LIGHT_GREEN,/*Light Color 1 time*/ 500,/*LightColor2 */ WHAT_LIGHT_BLUE,/*Light Color 2 time*/ 100 );

      if(gblScratch.timer1 < millis()) 
        {
          #ifdef LINK_MONITOR
          Serial.println(F("Link to me -timeout-"));
          Serial.println(F("Tx_REJECT_FULL_LINK_03"));
        #endif
          Tx_REJECT_FULL_LINK_03();   // Let other devices know the link was rejected.
           gblScratch.timer1 = millis() + _RQST_WAIT_TIME; // Keep it red for several seconds.
           gblMailbox.networkStateColor = WHAT_LIGHT_RED;
           gblLink.state = WHAT_LINK_COMPLETED; // no response.  indicate that the link was not accepted.  
        }
      if(gblLinkRxFlags.f_ACCEPT_FULL_LINK_03)
      { // Link accepted.  
        // Make sure the offered address is not already linked...
        gblLinkRxFlags.f_ACCEPT_FULL_LINK_03 = false;     // Make sure previously set flag is cleared.  
        gblLink.alreadyLinked = isLinkAvailable(); // Check and see if a link location if available.
        if(gblLink.alreadyLinked < numWHATLinks)
        { // A Link spot is available
          if(gblLink.links[gblLink.alreadyLinked] == gblJoining.sourceAddress)
          { // If the same address tries to link again, it should be removed from the link:
            gblLink.links[gblLink.alreadyLinked] = 0xFFFF;
            #ifdef LINK_MONITOR
              Serial.println(F("Link to me -Removed-"));
              Serial.print(F("List Location: "));Serial.println(gblLink.alreadyLinked);
              //Serial.print(F("gblLink.links[_i] value: 0x"));Serial.println(gblLink.links[gblLink.alreadyLinked],HEX);
            #endif
            gblMailbox.networkStateColor = WHAT_LIGHT_RED;
            gblScratch.timer1 = millis() + _RQST_WAIT_TIME; // Keep it RED for several seconds.
            gblLink.state = WHAT_LINK_COMPLETED; // no response.  act like it never happened.  
          }
          else
          {
            gblLink.links[gblLink.alreadyLinked] = gblJoining.sourceAddress;
            #ifdef LINK_MONITOR
              Serial.println(F("Link to me -Accepted-"));
              Serial.print(F("List Location: "));Serial.println(gblLink.alreadyLinked);
              //Serial.print(F("gblLink.links[_i] value: 0x"));Serial.println(gblLink.links[gblLink.alreadyLinked],HEX);
            #endif
            gblMailbox.networkStateColor = WHAT_LIGHT_GREEN;
            gblScratch.timer1 = millis() + _RQST_WAIT_TIME; // Keep it green for several seconds.
            gblLink.state = WHAT_LINK_COMPLETED; // no response.  act like it never happened.  
          }
        }
        else
        { // for some reason, there are no longer availble links on this device.
            gblMailbox.networkStateColor = WHAT_LIGHT_RED;
            #ifdef LINK_MONITOR
              Serial.println(F("No Links available suddenly"));
              Serial.print(F("gblLink.alreadyLinked: "));Serial.println(gblLink.alreadyLinked);
              //Serial.print(F("gblLink.links[_i] value: 0x"));Serial.println(gblLink.links[gblLink.alreadyLinked],HEX);
            #endif
            gblScratch.timer1 = millis() + _RQST_WAIT_TIME; // Keep it green for several seconds.
            gblLink.state = WHAT_LINK_COMPLETED; // no response.  act like it never happened.  

        }
      }
    break;


// ************************************************************************
    case WHAT_PRE_LINK_REQUESTED:
        //another device is requesting a link.  If the button on this is pressed, this will join.
        // Before giving the chance, make sure this device is not already connected.
        // ALso, if this device is already linked to this device, pressing the button will remove the link.
        
        gblLink.alreadyLinked = isLinkAvailable(gblJoining.sourceAddress); // Check and see if a link location if available.
        if(gblLink.links[gblLink.alreadyLinked] == gblJoining.sourceAddress )
        {
          gblScratch.timer1 = millis() + _RQST_WAIT_TIME;
          gblLink.state = WHAT_REMOVE_LINK; //Prepare to remove link
        }
        else
        {
          gblLink.state = WHAT_LINK_REQUESTED; // Prepare to accept link

        }
 
      break;

// ************************************************************************
    case WHAT_LINK_REQUESTED:
// TOC    Another device is attempting to link to this (or another) device
        // If the button on this device is pressed, make a link.

      if( (gblMailbox.networkButton == HIGH) & (mySourceAddress <0xFFFF) )
      {
        // WHAT_PRE_LINK_REQUESTED already verified that this device has a free spot and not already linked to this source address. 
        gblLink.alreadyLinked = isLinkAvailable(gblJoining.sourceAddress); // find the free spot where this link will be marked.
        gblLink.links[gblLink.alreadyLinked] = gblJoining.sourceAddress;
        _EEPROM_store_links();  // Update the link list table with the new address.
        // Tx link Confirmation
        WHAT_Packet _WHAT_Tx_Packet;  // Make a temporary transmit packet
        _WHAT_Tx_Packet.WHAT_MAC_MESSAGE;
        // In all cases, if a child address is being proposed, also inform the network that a preemie wants to join.
        // In addition to proposing a child, tell the entire network that a child is trying to join:
        _WHAT_Tx_Packet.pan_id = gblNetwork.panID;
        _WHAT_Tx_Packet.destination = gblJoining.sourceAddress;   // Send this to all devices.
        _WHAT_Tx_Packet.sourceaddress = mySourceAddress;  // Use the source ddress of this device.
        uint8_t _index = 0;
        _WHAT_Tx_Packet.payload[_index++] = 4; //Total payload length (including this byte)
        _WHAT_Tx_Packet.payload[_index++] = 3; //MID length (including this byte)
        _WHAT_Tx_Packet.payload[_index++] = highByte(ACCEPT_FULL_LINK_03);
        _WHAT_Tx_Packet.payload[_index++] = lowByte(ACCEPT_FULL_LINK_03);
        WHAT_TransmitPacket(_WHAT_Tx_Packet);
      #ifdef LINK_MONITOR
           Serial.print(F("Link Accepted to : 0x"));Serial.println(gblJoining.sourceAddress,HEX);
      #endif
        gblMailbox.networkStateColor = WHAT_LIGHT_GREEN;
        gblScratch.timer1 = millis() + _RQST_WAIT_TIME; // Keep it green for several seconds.
        gblLink.state = WHAT_LINK_COMPLETED; // no response.  act like it never happened.  
 
      }
      if(gblLinkRxFlags.f_REJECT_FULL_LINK_03)
      {   
        gblLinkRxFlags.f_REJECT_FULL_LINK_03 = false; // Make sure previously set flag is cleared.  
      #ifdef LINK_MONITOR
        Serial.println(F("Link to me -Rx reject-"));
        gblLink.state =  WHAT_NO_LINK; // Reset to the "normal" state when the timer expires.
      #endif
      }

      if(gblScratch.timer1 <= millis())
      {
      #ifdef LINK_MONITOR
       Serial.println(F("Link to me -timeout-"));
        gblLink.state =  WHAT_NO_LINK; // Reset to the "normal" state when the timer expires.
      #endif
      }

     break;

    case WHAT_REMOVE_LINK:
      if( (gblMailbox.networkButton == HIGH) & (mySourceAddress <0xFFFF) )
      {
         gblLink.alreadyLinked = isLinkAvailable(gblJoining.sourceAddress); // Find where this source address is stored. 
        gblLink.links[gblLink.alreadyLinked] = 0xFFFF; //remove the link to this address.
        gblMailbox.networkStateColor = WHAT_LIGHT_RED;
        gblScratch.timer1 = millis() + _RQST_WAIT_TIME; // Keep it green for several seconds.
        gblLink.state = WHAT_LINK_COMPLETED; // no response.  act like it never happened.  
        #ifdef LINK_MONITOR
        Serial.print(F("Unlink :0x"));Serial.println(gblLink.links[gblLink.alreadyLinked]);
        #endif
        _EEPROM_store_links();  // Update the link list table with the new address.
        WHAT_Packet _WHAT_Tx_Packet;  // Make a temporary transmit packet
        _WHAT_Tx_Packet.WHAT_MAC_MESSAGE;
        // In all cases, if a link is being removed, also inform the other device that it should be removed from the link table.
        _WHAT_Tx_Packet.pan_id = gblNetwork.panID;
        _WHAT_Tx_Packet.destination = gblJoining.sourceAddress;   // Send this to the previously linked device.
        _WHAT_Tx_Packet.sourceaddress = mySourceAddress;  // Use the source address of this device.
        uint8_t _index = 0;
        _WHAT_Tx_Packet.payload[_index++] = 4; //Total payload length (including this byte)
        _WHAT_Tx_Packet.payload[_index++] = 3; //MID length (including this byte)
        _WHAT_Tx_Packet.payload[_index++] = highByte(REMOVE_FULL_LINK_03);
        _WHAT_Tx_Packet.payload[_index++] = lowByte(REMOVE_FULL_LINK_03);
          WHAT_TransmitPacket(_WHAT_Tx_Packet);
      }
      if(gblScratch.timer1 <= millis())
      {
        Serial.println(F("REMOVE link -timeout-"));
        gblLink.state =  WHAT_NO_LINK; // Reset to the "normal" state when the timer expires.
      }
    break;
 
    case  WHAT_LINK_COMPLETED:
        if (gblScratch.timer2 <= millis())
        {  // The button was held long enough.
          gblMailbox.networkStateColor = WHAT_LIGHT_OFF;
        #ifdef LINK_MONITOR
            Serial.println(F("WHAT_LINKING_FAIL"));
          #endif
        gblLink.state = WHAT_NO_LINK;
        }
    break;


    default:
        // If in an invalid state, revert to no link.
        #ifdef LINK_MONITOR
        //Serial.print(F("Offered PANID:0x")); Serial.println(gblJoining.panID);
        Serial.print(F("Undefined LINK state : ")); Serial.println(gblLink.state,DEC);
        #endif
        //gblMailbox.networkState = WHAT_PRE_REQUEST_JOIN_NETWORK_03; // If nothing offered a child slot, return to being a preemie.
        gblLink.state = WHAT_NO_LINK;  //
      break;

  }
}

void  WHAT_HardwareSetup(void)
{
  for (uint8_t _i = 0; _i <= NUM_INPUTS; _i++)
  {
      pinMode(gblLinkIO.inputPin[_i],INPUT);
  }
  for (uint8_t _i = 0; _i <= NUM_OUTPUTS; _i++)
  {
      pinMode(gblLinkIO.outputPin[_i],OUTPUT);
  }
}

void WHAT_HardwareCheck(void)
{
  uint8_t _this_InputValueForLink = 0;   // This is a combined value for the inputs.
  uint8_t _this_OutputValueForLink = 0;   // This is a combined value for the inputs.

  static uint8_t _st_last_InputValueForLink;   // This is a combined value for the inputs.
  static uint8_t _st_last_OutputValueForLink;   // This is a combined value for the inputs.
  static double  _st_transmission_delay = millis();       // Limit I/O updates using this timer.

  // *********
 // -IF- an input value changed, transmit the new value.
  for (uint8_t _i = 0; _i <= NUM_INPUTS; _i++)
  {
      _this_InputValueForLink = (((_i*2)*digitalRead(gblLinkIO.inputPin[_i])) | _this_InputValueForLink);
          // This takes each bit, and shifts it (right to left) to fill the input value bit.
          // After all itterations, it is a single byte representing "NUM_INPUTS" bits.
  } 
  if((_this_InputValueForLink != _st_last_InputValueForLink) & (_st_transmission_delay <= millis()))
  { // the input status has changed.  send the new value.
       WHAT_Packet _WHAT_Tx_Packet;    // Create a temporary transmit packet.
      _WHAT_Tx_Packet.pan_id = gblNetwork.panID;
      _WHAT_Tx_Packet.destination = 0xFFFF;   // Send this to all devices.
      _WHAT_Tx_Packet.sourceaddress = mySourceAddress;  // Use the source address of this device.
      uint8_t _index = 0;
      _WHAT_Tx_Packet.payload[_index++] = 5; //Total payload length (including this byte)
      _WHAT_Tx_Packet.payload[_index++] = 4; //MID length (including this byte)
      _WHAT_Tx_Packet.payload[_index++] = highByte(CMD_DIG01_04);
      _WHAT_Tx_Packet.payload[_index++] = lowByte(CMD_DIG01_04);
      _WHAT_Tx_Packet.payload[_index++] = _this_InputValueForLink;  // This is a value for confirming allowance.
    WHAT_TransmitPacket(_WHAT_Tx_Packet);
    _st_last_InputValueForLink = _this_InputValueForLink;
    _st_transmission_delay = millis() + _WHAT_LINK_INPUT_UPDATE_RATE;

     #ifdef LINK_MONITOR
     Serial.println(F("Linked Input Update Sent : ")); 
     #endif
 
  }


  // *********
  // -IF- a I/O message arrived, verify it is from a LINKED address, then act on it.
  if( gblLinkRxFlags.f_CMD_DIG01_04)
  {
    gblLinkRxFlags.f_CMD_DIG01_04 = false;  // Make sure previously set flag is cleared.  
    gblLink.alreadyLinked = isLinkAvailable(gblJoining.sourceAddress); // Find -IF- this source address is stored. 
    if(gblLink.alreadyLinked < numWHATLinks)
    {
      for (uint8_t _i = 0; _i <= NUM_OUTPUTS; _i*2)
      {
          digitalWrite(gblLinkIO.outputValue[_i],(gblLink.commandedOutputs & _i));
            //i moves binary one bit at a time through the commanded outputs to isolate the specific commanded output value.
      }

    }
  }

}
