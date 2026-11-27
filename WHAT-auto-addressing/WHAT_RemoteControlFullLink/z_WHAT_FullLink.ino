#define WHAT_FULLLINK_INO_VERSION 26112706   // Revision: YYMMDD## - bump ## on any change to this file.

#include "z_WHAT_FullLink.h"   // -This routine- as part of the WHAT system.

// ***************************
// TableOfContents:
// TOC Global Variables
// TOC Begin
// TOC Loop
// TOC Link operations
//
// PER-OUTPUT LINK TABLE (Nov 27, 2026, Gary's design): this file was
// reworked around gblLink.entries[] - see z_WHAT_FullLink.h for the
// model. The button pairing state machine (WHAT_LinkCheck) is carried
// over from the Nov 26, 2026 implementation with its table writes
// converted: where it stored one peer address in links[], it now
// FILLS one entry per output naming the peer (WHAT_LinkFillFromPeer),
// and where it blanked a peer's slot it now clears ALL of the peer's
// entries (WHAT_LinkClearPeer). Received values no longer consult a
// peer list at all: the staging handlers capture them, and
// WHAT_LinkDispatch matches them against the entries.



// *************************************************************************
// *************************************************************************
// TOC Begin
void WHAT_LINK_begin (void)
{
  gblLink.reusableSlot = RegisterWHATMessage (0xFFFF, &holdingFunction); // This entry can be re-used during network adoption.
  // FULL LINK IMPLEMENTATION (Nov 26, 2026): the link messages are registered
  // PERMANENTLY here. The draft instead re-pointed registry slots at request
  // time, and one of those writes used the NETWORK machine's reusable slot
  // (gblRegistry.reusableSlot), which the join machinery also parks its
  // awaited message in - the two machines could overwrite each other's
  // registration. Permanent entries (the registry holds 32) remove the
  // whole class of problem; the Rx flags in gblLinkRxFlags carry the
  // events to the link state machine.
  RegisterWHATMessage (REQUEST_FULL_LINK_03, &Rx_REQUEST_FULL_LINK_03); // This is an entry that will not change.
  RegisterWHATMessage (ACCEPT_FULL_LINK_03, &Rx_ACCEPT_FULL_LINK_03); // This is an entry that will not change.
  RegisterWHATMessage (REJECT_FULL_LINK_03, &Rx_REJECT_FULL_LINK_03); // This is an entry that will not change.
  RegisterWHATMessage (REMOVE_FULL_LINK_03, &Rx_REMOVE_FULL_LINK_03); // This is an entry that will not change.
  RegisterWHATMessage (CMD_PWM01_04, &Rx_CMD_PWM01_04); // Value staging: a source's PWM values. (In this folder these two registrations REPLACE the app's own - the app receives value packets through the link dispatch and its legacy path; see WHAT_RemoteControlFullLink.ino.)
  RegisterWHATMessage (CMD_DIG01_04, &Rx_CMD_DIG01_04); // Value staging: a source's digital mask/states.
  RegisterWHATMessage (SET_LINK_ENTRY_03, &Rx_SET_LINK_ENTRY_03); // PER-OUTPUT LINK TABLE (Nov 27, 2026): a directed message writes one entry of this device's table.
  RegisterWHATMessage (GET_LINK_ENTRY_03, &Rx_GET_LINK_ENTRY_03); // PER-OUTPUT LINK TABLE (Nov 27, 2026): interrogate entries - directed asks one unit, broadcast asks every unit with entries.
  RegisterWHATMessage (REPLY_LINK_ENTRY_03, &Rx_REPLY_LINK_ENTRY_03); // PER-OUTPUT LINK TABLE (Nov 27, 2026): the directed answer to a GET.
  WHAT_HardwareSetup();

}
// TOC Loop
void Loop_WHAT_LINK (void)
{
  // FULL LINK IMPLEMENTATION (Nov 26, 2026): this used to call
  // WHAT_StateCheck() - the NETWORK state machine, which Loop_WHAT()
  // already runs every pass; running it twice per loop could double-fire
  // one-shot transitions. The link machine is WHAT_LinkCheck(), and it
  // must run every pass regardless of the network state, because the
  // button-hold that INITIATES a link is watched for in its NO_LINK case
  // while the network sits in NET_ALIVE. (The network machine's
  // WHAT_LINKING case also calls WHAT_LinkCheck() while a link sequence
  // holds the network - the transitions are timer/button guarded, so the
  // extra evaluation is harmless.)
  WHAT_LinkCheck();  // Assess the link state
  WHAT_LinkDispatch();  // Match staged value packets against the entry table.
  WHAT_LinkDumpCheck();  // Pace out REPLY packets for a pending GET.
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
    _WHAT_Tx_Packet.sourceaddress = mySourceAddress;  // Use the source address of this device.
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
  // (Nov 27, 2026: the state machine clears ALL of the sender's table
  // entries - a peer can hold several now, not one slot.)
  gblJoining.sourceAddress = _WHAT_Rx_Packet.sourceaddress;

  gblLinkRxFlags.f_REMOVE_FULL_LINK_03 = true;
   #ifdef LINK_MONITOR
   Serial.println(F("Rx REMOVE_FULL_LINK"));
   #endif
}

void Rx_CMD_DIG01_04 (WHAT_Packet _WHAT_Rx_Packet, uint8_t _index)
{
  // Value staging (Nov 27, 2026). CMD_DIG01_04 format (z_WHAT_Command.h):
  // [length][MID][MASK][STATES] - the MASK says which channels this
  // message commands; the STATES byte carries their on/off bits. Stage
  // both bytes (and the source, captured with them); WHAT_LinkDispatch
  // extracts per-entry channel values, mask-gated. (The original draft
  // handler read a packed byte at _index + 4 from a format this command
  // never used, and was never registered; this one is.)
  gblJoining.sourceAddress = _WHAT_Rx_Packet.sourceaddress;
  gblLink.commandedSource = _WHAT_Rx_Packet.sourceaddress;
  gblLink.commandedDestination = _WHAT_Rx_Packet.destination;
  gblLink.commandedDigMask = _WHAT_Rx_Packet.payload[_index + 2];
  gblLink.commandedOutputs = _WHAT_Rx_Packet.payload[_index + 3];
  gblLinkRxFlags.f_CMD_DIG01_04 = true;
   #ifdef LINK_MONITOR
   Serial.println(F("Rx Digital Command"));
   #endif
}

void Rx_CMD_PWM01_04 (WHAT_Packet _WHAT_Rx_Packet, uint8_t _index)
{
  // Value staging (Nov 27, 2026): a source's analog values arrive as
  // CMD_PWM01_04 blocks (the same blocks WHAT_RemoteControl sends) -
  // channel at _index + 2, value (0-100) at _index + 3. The payload
  // handler calls this once per block in the packet. Values stage into
  // gblLink.commandedPWM[] with the source captured alongside;
  // WHAT_LinkDispatch matches them against the entry table.
  gblJoining.sourceAddress = _WHAT_Rx_Packet.sourceaddress;
  gblLink.commandedSource = _WHAT_Rx_Packet.sourceaddress;
  gblLink.commandedDestination = _WHAT_Rx_Packet.destination;
  uint8_t _channel = _WHAT_Rx_Packet.payload[_index + 2];
  if ((_channel >= 1) && (_channel <= NUM_OUTPUTS))
  {
    gblLink.commandedPWM[_channel - 1] = _WHAT_Rx_Packet.payload[_index + 3];
  }
  gblLinkRxFlags.f_CMD_PWM01_04 = true;
   #ifdef LINK_MONITOR
   Serial.println(F("Rx PWM Command"));
   #endif
}

// *****************
// TOC Link entry messages (Nov 27, 2026): SET_LINK_ENTRY_03 writes ONE
// entry of the receiving device's link table. Directed only - a
// broadcast set would reconfigure every device that hears it, so the
// handler refuses anything not addressed to this device. A set naming
// source 0xFFFF CLEARS the entry. The write persists to EEPROM in the
// handler (the abandon lesson: a state change is not done until it is
// stored). There is deliberately no reply message yet - the sender
// can be read back with GET_LINK_ENTRY_03 / REPLY_LINK_ENTRY_03
// (0xF062 / 0xF063, below) - the interrogation pair Gary asked for.
void Rx_SET_LINK_ENTRY_03 (WHAT_Packet _WHAT_Rx_Packet, uint8_t _index)
{
  if (_WHAT_Rx_Packet.destination != mySourceAddress) return;  // Directed only.
  uint8_t _entry = _WHAT_Rx_Packet.payload[_index + 3];
  if (_entry >= WHAT_MAX_LINK_ENTRIES) return;
  uint8_t _output = _WHAT_Rx_Packet.payload[_index + 4];
  uint16_t _source = _WHAT_Rx_Packet.payload[_index + 5] + _WHAT_Rx_Packet.payload[_index + 6] * 0x100;
  uint16_t _command = _WHAT_Rx_Packet.payload[_index + 7] + _WHAT_Rx_Packet.payload[_index + 8] * 0x100;
  uint8_t _channel = _WHAT_Rx_Packet.payload[_index + 9];
  uint8_t _flags = _WHAT_Rx_Packet.payload[_index + 10];
  WHAT_LinkEntry *_e = &gblLink.entries[_entry];
  if (_source == 0xFFFF)
  { // A set naming the blank source clears the entry.
    _e->output = 0xFF; _e->source = 0xFFFF; _e->command = 0; _e->channel = 0; _e->flags = 0; _e->lastValue = 0xFF;
  }
  else
  {
    if (_output >= NUM_OUTPUTS) return;  // This device has no such output - refuse rather than corrupt the table.
    _e->output = _output; _e->source = _source; _e->command = _command; _e->channel = _channel; _e->flags = _flags; _e->lastValue = 0xFF;  // lastValue reset: the next packet from the (new) source counts as a change.
  }
  _EEPROM_store_links();  // Persist the table (the function keeps its Nov 24 name; it stores the entry table now).
   #ifdef LINK_MONITOR
   Serial.print(F("Link entry ")); Serial.print(_entry); Serial.print(F(" <- out ")); Serial.print(_e->output);
   Serial.print(F(" src 0x")); Serial.print(_e->source,HEX); Serial.print(F(" cmd 0x")); Serial.print(_e->command,HEX);
   Serial.print(F(" ch ")); Serial.print(_e->channel); Serial.print(F(" flags 0x")); Serial.println(_e->flags,HEX);
   #endif
}

void Tx_SET_LINK_ENTRY_03 (uint16_t _destination, uint8_t _entry, uint8_t _output, uint16_t _source, uint16_t _command, uint8_t _channel, uint8_t _flags)
{
  // The sending half of SET_LINK_ENTRY_03, for configurator apps (a
  // Teensy app arranging a smaller device's outputs, or a device
  // arranging its own table through the same path). Byte-counted
  // twice, per the claim-check lesson: 8 data bytes after a 2-byte
  // MID; block size 11 counts from the size byte; total payload 12
  // counts the total-length byte itself.
  WHAT_Packet _WHAT_Tx_Packet;  // Make a temporary transmit packet
  _WHAT_Tx_Packet.WHAT_DATA_MESSAGE;
  _WHAT_Tx_Packet.pan_id = gblNetwork.panID;
  _WHAT_Tx_Packet.destination = _destination;   // Directed: exactly one device applies a set.
  _WHAT_Tx_Packet.sourceaddress = mySourceAddress;
  uint8_t _index = 0;
  _WHAT_Tx_Packet.payload[_index++] = 12; //Total payload length (including this byte)
  _WHAT_Tx_Packet.payload[_index++] = 11; //Block length (including this byte): 1 + 2 MID + 8 data
  _WHAT_Tx_Packet.payload[_index++] = highByte(SET_LINK_ENTRY_03);
  _WHAT_Tx_Packet.payload[_index++] = lowByte(SET_LINK_ENTRY_03);
  _WHAT_Tx_Packet.payload[_index++] = _entry;
  _WHAT_Tx_Packet.payload[_index++] = _output;
  _WHAT_Tx_Packet.payload[_index++] = lowByte(_source);
  _WHAT_Tx_Packet.payload[_index++] = highByte(_source);
  _WHAT_Tx_Packet.payload[_index++] = lowByte(_command);
  _WHAT_Tx_Packet.payload[_index++] = highByte(_command);
  _WHAT_Tx_Packet.payload[_index++] = _channel;
  _WHAT_Tx_Packet.payload[_index++] = _flags;
  WHAT_TransmitPacket(_WHAT_Tx_Packet);
}

// *****************
// TOC Link interrogation (Nov 27, 2026, Gary's semantics): a GET
// directed at a unit gets THAT unit's entries; a GET broadcast to
// 0xFFFF prompts EVERY unit that has any link entries to reply (a
// unit with an empty table stays silent, and a preemie never answers).
// Answers are REPLY_LINK_ENTRY_03 packets, directed back to the
// asker, one per entry, in the same data layout as SET - plus a final
// entry#-0xFF packet marking the end of that unit's list. Because a
// broadcast GET could make many units answer at once, the replies are
// paced by WHAT_LinkDumpCheck(): each unit waits (its address % 16) x
// LINK_DUMP_JITTER_MS before starting, then sends one entry every
// LINK_REPLY_SPACING_MS - the heartbeat's transmit-when-quiet idiom.
#define LINK_REPLY_SPACING_MS 40
#define LINK_DUMP_JITTER_MS 150

void Rx_GET_LINK_ENTRY_03 (WHAT_Packet _WHAT_Rx_Packet, uint8_t _index)
{
  if (mySourceAddress >= 0xFFFF) return;  // A preemie has no list to give, and no name to give it under.
  uint8_t _selector = _WHAT_Rx_Packet.payload[_index + 3];
  bool _broadcast = (_WHAT_Rx_Packet.destination >= 0xFFFF);
  if (_broadcast)
  { // Broadcast asks only units WITH entries (Gary's rule). A single
    // entry ask stays silent if that entry is blank; an "all" ask
    // stays silent if the whole table is blank.
    if (_selector < WHAT_MAX_LINK_ENTRIES)
    {
      if (gblLink.entries[_selector].source == 0xFFFF) return;
    }
    else
    {
      bool _any = false;
      for (uint8_t _i = 0; _i < WHAT_MAX_LINK_ENTRIES; _i++)
      {
        if (gblLink.entries[_i].source != 0xFFFF) { _any = true; break; }
      }
      if (!_any) return;
    }
  }
  gblLink.dumpRequester = _WHAT_Rx_Packet.sourceaddress;
  gblLink.dumpSelector = _selector;
  gblLink.dumpNextEntry = (_selector < WHAT_MAX_LINK_ENTRIES) ? _selector : 0;
  gblLink.dumpEndPending = false;
  gblLink.dumpNextAt = millis() + (_broadcast ? (uint32_t)(mySourceAddress % 16) * LINK_DUMP_JITTER_MS : 0);
  gblLink.dumpActive = true;
   #ifdef LINK_MONITOR
   Serial.print(F("Rx GET link entries, asker 0x")); Serial.print(_WHAT_Rx_Packet.sourceaddress,HEX);
   Serial.print(F(" selector ")); Serial.println(_selector);
   #endif
}

void WHAT_LinkDumpCheck(void)
{
  if (!gblLink.dumpActive) return;
  if ((int32_t)(millis() - gblLink.dumpNextAt) < 0) return;

  if (gblLink.dumpEndPending)
  {
    Tx_REPLY_LINK_ENTRY_03(gblLink.dumpRequester, 0xFF);  // End of this unit's list.
    gblLink.dumpActive = false;
    return;
  }

  if (gblLink.dumpSelector < WHAT_MAX_LINK_ENTRIES)
  { // A single entry was asked for (a directed ask gets it even if blank - the blank IS the answer).
    Tx_REPLY_LINK_ENTRY_03(gblLink.dumpRequester, gblLink.dumpSelector);
    gblLink.dumpEndPending = true;
    gblLink.dumpNextAt = millis() + LINK_REPLY_SPACING_MS;
    return;
  }

  while ((gblLink.dumpNextEntry < WHAT_MAX_LINK_ENTRIES) && (gblLink.entries[gblLink.dumpNextEntry].source == 0xFFFF))
    gblLink.dumpNextEntry++;
  if (gblLink.dumpNextEntry < WHAT_MAX_LINK_ENTRIES)
  {
    Tx_REPLY_LINK_ENTRY_03(gblLink.dumpRequester, gblLink.dumpNextEntry);
    gblLink.dumpNextEntry++;
  }
  else
  {
    gblLink.dumpEndPending = true;
  }
  gblLink.dumpNextAt = millis() + LINK_REPLY_SPACING_MS;
}

void Tx_REPLY_LINK_ENTRY_03 (uint16_t _destination, uint8_t _entry)
{
  // One entry, reported in SET's exact data layout so a configurator
  // parses both the same way. Entry 0xFF = end-of-list marker (all
  // fields blank). Byte count: 8 data bytes after a 2-byte MID; block
  // 11; total 12 - the same arithmetic as SET_LINK_ENTRY_03.
  uint8_t _output = 0xFF;
  uint16_t _source = 0xFFFF;
  uint16_t _command = 0xFFFF;
  uint8_t _channel = 0xFF;
  uint8_t _flags = 0xFF;
  if (_entry < WHAT_MAX_LINK_ENTRIES)
  {
    _output = gblLink.entries[_entry].output;
    _source = gblLink.entries[_entry].source;
    _command = gblLink.entries[_entry].command;
    _channel = gblLink.entries[_entry].channel;
    _flags = gblLink.entries[_entry].flags;
  }
  WHAT_Packet _WHAT_Tx_Packet;  // Make a temporary transmit packet
  _WHAT_Tx_Packet.WHAT_DATA_MESSAGE;
  _WHAT_Tx_Packet.pan_id = gblNetwork.panID;
  _WHAT_Tx_Packet.destination = _destination;   // Directed back to the asker.
  _WHAT_Tx_Packet.sourceaddress = mySourceAddress;
  uint8_t _index = 0;
  _WHAT_Tx_Packet.payload[_index++] = 12; //Total payload length (including this byte)
  _WHAT_Tx_Packet.payload[_index++] = 11; //Block length (including this byte): 1 + 2 MID + 8 data
  _WHAT_Tx_Packet.payload[_index++] = highByte(REPLY_LINK_ENTRY_03);
  _WHAT_Tx_Packet.payload[_index++] = lowByte(REPLY_LINK_ENTRY_03);
  _WHAT_Tx_Packet.payload[_index++] = _entry;
  _WHAT_Tx_Packet.payload[_index++] = _output;
  _WHAT_Tx_Packet.payload[_index++] = lowByte(_source);
  _WHAT_Tx_Packet.payload[_index++] = highByte(_source);
  _WHAT_Tx_Packet.payload[_index++] = lowByte(_command);
  _WHAT_Tx_Packet.payload[_index++] = highByte(_command);
  _WHAT_Tx_Packet.payload[_index++] = _channel;
  _WHAT_Tx_Packet.payload[_index++] = _flags;
  WHAT_TransmitPacket(_WHAT_Tx_Packet);
}

void Tx_GET_LINK_ENTRY_03 (uint16_t _destination, uint8_t _selector)
{
  // The asking half, for configurator apps: _destination is one unit,
  // or 0xFFFF to census every unit with entries; _selector is an
  // entry#, or 0xFF for all. Byte count: 1 data byte after a 2-byte
  // MID; block 4; total 5.
  WHAT_Packet _WHAT_Tx_Packet;  // Make a temporary transmit packet
  _WHAT_Tx_Packet.WHAT_DATA_MESSAGE;
  _WHAT_Tx_Packet.pan_id = gblNetwork.panID;
  _WHAT_Tx_Packet.destination = _destination;
  _WHAT_Tx_Packet.sourceaddress = mySourceAddress;
  uint8_t _index = 0;
  _WHAT_Tx_Packet.payload[_index++] = 5; //Total payload length (including this byte)
  _WHAT_Tx_Packet.payload[_index++] = 4; //Block length (including this byte): 1 + 2 MID + 1 data
  _WHAT_Tx_Packet.payload[_index++] = highByte(GET_LINK_ENTRY_03);
  _WHAT_Tx_Packet.payload[_index++] = lowByte(GET_LINK_ENTRY_03);
  _WHAT_Tx_Packet.payload[_index++] = _selector;
  WHAT_TransmitPacket(_WHAT_Tx_Packet);
}

void Rx_REPLY_LINK_ENTRY_03 (WHAT_Packet _WHAT_Rx_Packet, uint8_t _index)
{
  // Stage the reply for the asking app; LINK_MONITOR prints it, which
  // is also how a bench census reads out on the asker's serial log.
  gblLink.replyFrom = _WHAT_Rx_Packet.sourceaddress;
  gblLink.replyEntry = _WHAT_Rx_Packet.payload[_index + 3];
  gblLink.replyOutput = _WHAT_Rx_Packet.payload[_index + 4];
  gblLink.replySource = _WHAT_Rx_Packet.payload[_index + 5] + _WHAT_Rx_Packet.payload[_index + 6] * 0x100;
  gblLink.replyCommand = _WHAT_Rx_Packet.payload[_index + 7] + _WHAT_Rx_Packet.payload[_index + 8] * 0x100;
  gblLink.replyChannel = _WHAT_Rx_Packet.payload[_index + 9];
  gblLink.replyFlags = _WHAT_Rx_Packet.payload[_index + 10];
  gblLinkRxFlags.f_REPLY_LINK_ENTRY_03 = true;
   #ifdef LINK_MONITOR
   if (gblLink.replyEntry >= 0xFF)
   {
     Serial.print(F("Link list end from 0x")); Serial.println(gblLink.replyFrom,HEX);
   }
   else
   {
     Serial.print(F("Link entry from 0x")); Serial.print(gblLink.replyFrom,HEX);
     Serial.print(F(" #")); Serial.print(gblLink.replyEntry);
     Serial.print(F(" out ")); Serial.print(gblLink.replyOutput);
     Serial.print(F(" src 0x")); Serial.print(gblLink.replySource,HEX);
     Serial.print(F(" cmd 0x")); Serial.print(gblLink.replyCommand,HEX);
     Serial.print(F(" ch ")); Serial.print(gblLink.replyChannel);
     Serial.print(F(" flags 0x")); Serial.println(gblLink.replyFlags,HEX);
   }
   #endif
}

// *****************
// TOC Link entry helpers (Nov 27, 2026). These replace isLinkAvailable()
// and the links[] array: the pairing machine's questions are now "is
// there a blank entry", "does any entry name this peer", "fill entries
// from this peer", "clear this peer's entries".
uint8_t WHAT_LinkFirstBlankEntry(void)
{
  for (uint8_t _i = 0; _i < WHAT_MAX_LINK_ENTRIES; _i++)
  {
    if (gblLink.entries[_i].source == 0xFFFF) return _i;
  }
  return 0xFF;
}

bool WHAT_LinkPeerLinked(uint16_t _peer)
{
  if (_peer == 0xFFFF) return false;
  for (uint8_t _i = 0; _i < WHAT_MAX_LINK_ENTRIES; _i++)
  {
    if (gblLink.entries[_i].source == _peer) return true;
  }
  return false;
}

bool WHAT_LinkFillFromPeer(uint16_t _peer)
{
  // A full link: one entry per output, all naming the peer, watching
  // CMD_PWM01_04, channels straight across, adopt behavior. An entry
  // that already says exactly that is refreshed in place; otherwise
  // the first blank entry is used. Returns true if at least one output
  // got its entry (a nearly-full table fills what it can).
  uint8_t _written = 0;
  for (uint8_t _o = 0; _o < NUM_OUTPUTS; _o++)
  {
    uint8_t _slot = 0xFF;
    for (uint8_t _i = 0; _i < WHAT_MAX_LINK_ENTRIES; _i++)
    {
      if ((gblLink.entries[_i].output == _o) && (gblLink.entries[_i].source == _peer) && (gblLink.entries[_i].command == CMD_PWM01_04) && (gblLink.entries[_i].channel == _o + 1)) { _slot = _i; break; }
    }
    if (_slot == 0xFF) _slot = WHAT_LinkFirstBlankEntry();
    if (_slot == 0xFF) break;  // Table full - the remaining outputs stay unlinked.
    gblLink.entries[_slot].output = _o;
    gblLink.entries[_slot].source = _peer;
    gblLink.entries[_slot].command = CMD_PWM01_04;
    gblLink.entries[_slot].channel = _o + 1;
    gblLink.entries[_slot].flags = 0;
    gblLink.entries[_slot].lastValue = 0xFF;
    _written++;
  }
  if (_written > 0) _EEPROM_store_links();
  return (_written > 0);
}

bool WHAT_LinkClearPeer(uint16_t _peer)
{
  // Blank every entry naming this peer (a peer can hold several -
  // one per output, plus any hand-set entries). Returns true if the
  // table changed; persists when it did.
  bool _changed = false;
  for (uint8_t _i = 0; _i < WHAT_MAX_LINK_ENTRIES; _i++)
  {
    if (gblLink.entries[_i].source == _peer)
    {
      gblLink.entries[_i].output = 0xFF;
      gblLink.entries[_i].source = 0xFFFF;
      gblLink.entries[_i].command = 0;
      gblLink.entries[_i].channel = 0;
      gblLink.entries[_i].flags = 0;
      gblLink.entries[_i].lastValue = 0xFF;
      _changed = true;
    }
  }
  if (_changed) _EEPROM_store_links();
  return _changed;
}

// *****************
// TOC Link dispatch (Nov 27, 2026): match a staged value packet against
// the entry table. Gary's rule: an output is driven by whichever of its
// sources CHANGED last - each entry remembers its source's last value,
// and an unchanged re-send has no vote (a board re-sends all its values
// when any one moves, and again at boot; neither may steal an output).
// A value entry ADOPTS its new value; a TOGGLE entry flips its output
// between off and the last adopted level.
// WHICH packets may drive outputs at all is also Gary's rule (clarified
// Nov 27, 2026): (1) a packet DIRECTED at this device (destination ==
// mySourceAddress) is a command - it applies directly, all channels,
// bypassing the table; the legacy flags hand it to the app, whose
// handlers are the pre-link direct control. (2) a BROADCAST packet
// (destination 0xFFFF) applies ONLY through the table, per output - a
// broadcast from a source no entry names does nothing at all. (Packets
// directed at OTHER devices never reach the handlers - the base's
// DATA-frame destination check sees to that.)
void WHAT_LinkDispatch(void)
{
  if (gblLinkRxFlags.f_CMD_PWM01_04)
  {
    gblLinkRxFlags.f_CMD_PWM01_04 = false;
    if (gblLink.commandedDestination == mySourceAddress)
    {
      gblLinkRxFlags.f_LINK_LEGACY_PWM = true;  // Directed command: the app applies it directly.
    }
    else if (gblLink.commandedDestination == 0xFFFF)
    {
      bool _applied = false;
      for (uint8_t _i = 0; _i < WHAT_MAX_LINK_ENTRIES; _i++)
      {
        WHAT_LinkEntry *_e = &gblLink.entries[_i];
        if ((_e->source == 0xFFFF) || (_e->source != gblLink.commandedSource) || (_e->command != CMD_PWM01_04)) continue;
        if ((_e->channel < 1) || (_e->channel > NUM_OUTPUTS)) continue;
        uint8_t _v = gblLink.commandedPWM[_e->channel - 1];
        if (_v != _e->lastValue)
        {
          _e->lastValue = _v;
          if (_e->flags & LINK_ENTRY_FLAG_TOGGLE)
          {
            gblLinkIO.outputValue[_e->output] = (gblLinkIO.outputValue[_e->output] == 0) ? ((gblLink.lastAdoptedLevel[_e->output] <= 100) ? gblLink.lastAdoptedLevel[_e->output] : 100) : 0;
          }
          else
          {
            gblLinkIO.outputValue[_e->output] = _v;
            gblLink.lastAdoptedLevel[_e->output] = _v;
          }
          _applied = true;
        }
      }
      if (_applied) gblLinkRxFlags.f_LINK_OUTPUTS_APPLIED = true;
      // A broadcast that matched nothing is simply not for this device.
    }
  }
  if (gblLinkRxFlags.f_CMD_DIG01_04)
  {
    gblLinkRxFlags.f_CMD_DIG01_04 = false;
    if (gblLink.commandedDestination == mySourceAddress)
    {
      gblLinkRxFlags.f_LINK_LEGACY_DIG = true;  // Directed command: the app applies it directly.
    }
    else if (gblLink.commandedDestination == 0xFFFF)
    {
      bool _applied = false;
      for (uint8_t _i = 0; _i < WHAT_MAX_LINK_ENTRIES; _i++)
      {
        WHAT_LinkEntry *_e = &gblLink.entries[_i];
        if ((_e->source == 0xFFFF) || (_e->source != gblLink.commandedSource) || (_e->command != CMD_DIG01_04)) continue;
        if ((_e->channel < 1) || (_e->channel > 8)) continue;
        if (!(gblLink.commandedDigMask & (1 << (_e->channel - 1)))) continue;  // This message does not command this channel - no change, no vote.
        uint8_t _v = (gblLink.commandedOutputs & (1 << (_e->channel - 1))) ? 100 : 0;
        if (_v != _e->lastValue)
        {
          _e->lastValue = _v;
          if (_e->flags & LINK_ENTRY_FLAG_TOGGLE)
          {
            gblLinkIO.outputValue[_e->output] = (gblLinkIO.outputValue[_e->output] == 0) ? ((gblLink.lastAdoptedLevel[_e->output] <= 100) ? gblLink.lastAdoptedLevel[_e->output] : 100) : 0;
          }
          else
          {
            gblLinkIO.outputValue[_e->output] = _v;
            gblLink.lastAdoptedLevel[_e->output] = _v;
          }
          _applied = true;
        }
      }
      if (_applied) gblLinkRxFlags.f_LINK_OUTPUTS_APPLIED = true;
      // A broadcast that matched nothing is simply not for this device.
    }
  }
}

// TOC Link operations
// FIX (Nov 27, 2026): every deadline in this state machine runs on
// gblLink.linkTimer, the link layer's own timer - never on
// gblScratch.timer1. The bench failure this fixes had this machine's
// idle button-watch overwrite the network machine's child-offer
// deadline (see the linkTimer field in z_WHAT_FullLink.h), which
// dropped a joining child's ACCEPT on the floor.
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
          // (Nov 27, 2026: the gate is now "a blank ENTRY exists" - the
          // table, not a peer slot, is what a new link consumes.)
          if(WHAT_LinkFirstBlankEntry() != 0xFF)
          { //only move to the next phase if a link entry is available.
            gblLink.linkTimer = millis() + _WHAT_NEW_LINK_HELD_TIME_DELAY; // Network button must be held for a while before acting on the link.
           gblLink.state = WHAT_PRE_LINKING_INITIATED;  // This device is initiating a link
          }
          else
          {
            #ifdef LINK_MONITOR
            Serial.println(F("No Link Entries Available"));
            #endif
          }
       }
      if (gblLinkRxFlags.f_REQUEST_FULL_LINK_03)     // If another device wants to link to this device, the link will be initiated.
      {
        if(WHAT_LinkFirstBlankEntry() != 0xFF)
        {
          gblLinkRxFlags.f_REQUEST_FULL_LINK_03 = false; // Mark the reception flag as used so it is not duplicated.
          gblLink.linkTimer = millis() + _WHAT_NEW_LINK_HELD_TIME_DELAY; // Network button must be held for a while before acting on the link.
          gblLink.state = WHAT_PRE_LINK_REQUESTED;
          #ifdef LINK_MONITOR
           Serial.print(F("gblMailbox.networkState: WHAT_PRE_LINK_REQUESTED ")); Serial.println(gblMailbox.networkState);
          #endif
        }
        else
        {
          #ifdef LINK_MONITOR
           Serial.println(F("No Available Link Entries"));
          #endif
        }
      }
      if(gblLinkRxFlags.f_REMOVE_FULL_LINK_03)
      {
        gblLinkRxFlags.f_REMOVE_FULL_LINK_03 = false;   // Make sure previously set flag is cleared.
        // FULL LINK IMPLEMENTATION (Nov 26, 2026): a REMOVE from a linked
        // device is complete on receipt - the button press that authorizes
        // it already happened on the REMOVING device. The draft parked
        // this in WHAT_REMOVE_LINK, which waits for a second press here
        // and would have sent a REMOVE back. (WHAT_REMOVE_LINK still
        // serves the locally armed case - a linked device asking again,
        // where THIS device's press is the act.) Nov 27, 2026: clearing
        // now blanks ALL of the sender's entries (WHAT_LinkClearPeer
        // persists when it changed anything).
        if(WHAT_LinkClearPeer(gblJoining.sourceAddress))
        {
          #ifdef LINK_MONITOR
            Serial.print(F("Link removed by peer: 0x")); Serial.println(gblJoining.sourceAddress,HEX);
          #endif
        }
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

            if(gblLink.linkTimer <= millis())
            { // Network button was held long enough.
              // FULL LINK IMPLEMENTATION (Nov 26, 2026): the ACCEPT / REJECT
              // handlers are registered permanently in WHAT_LINK_begin() -
              // the dynamic re-pointing that used to happen here is gone.
              gblMailbox.networkStateColor = WHAT_LIGHT_OFF;
              gblLinkRxFlags.f_REQUEST_FULL_LINK_03 = false;  // Make sure previously set flag is cleared.
              gblLinkRxFlags.f_ACCEPT_FULL_LINK_03 = false;  // Make sure previously set flag is cleared.
              gblLinkRxFlags.f_REJECT_FULL_LINK_03 = false;  // Make sure previously set flag is cleared.
              Tx_REQUEST_FULL_LINK();  // Request linking across the network.
              #ifdef LINK_MONITOR
                  Serial.println(F("Tx Rqst Link"));
              #endif
              gblLink.linkTimer = millis() + _WHAT_NEW_Link_TIME_DELAY;
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

      if(gblLinkRxFlags.f_REMOVE_FULL_LINK_03)
      { // The peer answered a RE-ASK by removing the link: its press in
        // WHAT_REMOVE_LINK sends REMOVE instead of ACCEPT. Complete the
        // removal HERE (Nov 27, 2026) - the flag was previously consumed
        // only in WHAT_NO_LINK, so the initiator cleared its entries
        // only after the 60 s accept timeout had run out, with a
        // spurious REJECT broadcast on the way.
        gblLinkRxFlags.f_REMOVE_FULL_LINK_03 = false;
        WHAT_LinkClearPeer(gblJoining.sourceAddress);  // (Persists inside, if anything was cleared.)
        #ifdef LINK_MONITOR
          Serial.println(F("Link removed by peer while waiting"));
        #endif
        gblMailbox.networkStateColor = WHAT_LIGHT_RED;
        gblLink.linkTimer = millis() + _RQST_WAIT_TIME; // Keep it RED for several seconds.
        gblLink.state = WHAT_LINK_COMPLETED;
      }

      if(gblLink.linkTimer < millis())
        {
          #ifdef LINK_MONITOR
          Serial.println(F("Link to me -timeout-"));
          Serial.println(F("Tx_REJECT_FULL_LINK_03"));
        #endif
          Tx_REJECT_FULL_LINK_03();   // Let other devices know the link was rejected.
           gblLink.linkTimer = millis() + _RQST_WAIT_TIME; // Keep it red for several seconds.
           gblMailbox.networkStateColor = WHAT_LIGHT_RED;
           gblLink.state = WHAT_LINK_COMPLETED; // no response.  indicate that the link was not accepted.
        }
      if(gblLinkRxFlags.f_ACCEPT_FULL_LINK_03)
      { // Link accepted.
        gblLinkRxFlags.f_ACCEPT_FULL_LINK_03 = false;     // Make sure previously set flag is cleared.
        // (Nov 27, 2026: the slot logic became entry logic - if any entry
        // already names the acceptor, this re-ask REMOVES the link, as
        // before; otherwise the fill writes one entry per output.)
        if(WHAT_LinkPeerLinked(gblJoining.sourceAddress))
        { // If the same address tries to link again, it should be removed from the link:
          WHAT_LinkClearPeer(gblJoining.sourceAddress);  // (Persists inside.)
          WHAT_Packet _WHAT_Tx_Packet;  // Make a temporary transmit packet
          _WHAT_Tx_Packet.WHAT_MAC_MESSAGE;
          _WHAT_Tx_Packet.pan_id = gblNetwork.panID;
          _WHAT_Tx_Packet.destination = gblJoining.sourceAddress;   // Send this to the previously linked device.
          _WHAT_Tx_Packet.sourceaddress = mySourceAddress;  // Use the source address of this device.
          uint8_t _index = 0;
          _WHAT_Tx_Packet.payload[_index++] = 4; //Total payload length (including this byte)
          _WHAT_Tx_Packet.payload[_index++] = 3; //MID length (including this byte)
          _WHAT_Tx_Packet.payload[_index++] = highByte(REMOVE_FULL_LINK_03);
          _WHAT_Tx_Packet.payload[_index++] = lowByte(REMOVE_FULL_LINK_03);
          WHAT_TransmitPacket(_WHAT_Tx_Packet);
          #ifdef LINK_MONITOR
            Serial.println(F("Link to me -Removed-"));
          #endif
          gblMailbox.networkStateColor = WHAT_LIGHT_RED;
          gblLink.linkTimer = millis() + _RQST_WAIT_TIME; // Keep it RED for several seconds.
          gblLink.state = WHAT_LINK_COMPLETED; // no response.  act like it never happened.
        }
        else if(WHAT_LinkFillFromPeer(gblJoining.sourceAddress))
        { // Entries written (and persisted inside the fill).
          #ifdef LINK_MONITOR
            Serial.println(F("Link to me -Accepted-"));
          #endif
          gblMailbox.networkStateColor = WHAT_LIGHT_GREEN;
          gblLink.linkTimer = millis() + _RQST_WAIT_TIME; // Keep it green for several seconds.
          gblLink.state = WHAT_LINK_COMPLETED; // no response.  act like it never happened.
        }
        else
        { // for some reason, there are no longer availble link entries on this device.
            gblMailbox.networkStateColor = WHAT_LIGHT_RED;
            #ifdef LINK_MONITOR
              Serial.println(F("No Link entries available suddenly"));
            #endif
            gblLink.linkTimer = millis() + _RQST_WAIT_TIME; // Keep it green for several seconds.
            gblLink.state = WHAT_LINK_COMPLETED; // no response.  act like it never happened.

        }
      }
    break;


// ************************************************************************
    case WHAT_PRE_LINK_REQUESTED:
        //another device is requesting a link.  If the button on this is pressed, this will join.
        // Before giving the chance, make sure this device is not already connected.
        // ALso, if this device is already linked to this device, pressing the button will remove the link.

        // (Nov 27, 2026: "already linked to this requester" = any entry names it.)
        if(WHAT_LinkPeerLinked(gblJoining.sourceAddress))
        {
          gblLink.linkTimer = millis() + _RQST_WAIT_TIME;
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
        // WHAT_PRE_LINK_REQUESTED already verified that this device is not already linked to this source address.
        WHAT_LinkFillFromPeer(gblJoining.sourceAddress); // Write one entry per output naming the requester (persists inside the fill).
        // Tx link Confirmation
        WHAT_Packet _WHAT_Tx_Packet;  // Make a temporary transmit packet
        _WHAT_Tx_Packet.WHAT_MAC_MESSAGE;
        _WHAT_Tx_Packet.pan_id = gblNetwork.panID;
        _WHAT_Tx_Packet.destination = gblJoining.sourceAddress;   // Send this to the requesting device.
        _WHAT_Tx_Packet.sourceaddress = mySourceAddress;  // Use the source address of this device.
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
        gblLink.linkTimer = millis() + _RQST_WAIT_TIME; // Keep it green for several seconds.
        gblLink.state = WHAT_LINK_COMPLETED; // no response.  act like it never happened.

      }
      if(gblLinkRxFlags.f_REJECT_FULL_LINK_03)
      {
        gblLinkRxFlags.f_REJECT_FULL_LINK_03 = false; // Make sure previously set flag is cleared.
      #ifdef LINK_MONITOR
        Serial.println(F("Link to me -Rx reject-"));
      #endif
        gblLink.state =  WHAT_NO_LINK; // Reset to the "normal" state. (Nov 27, 2026: this line sat inside the LINK_MONITOR ifdef since the draft - with the monitor off, a REJECT never reset the state and only the timer below saved it.)
      }

      if(gblLink.linkTimer <= millis())
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
        WHAT_LinkClearPeer(gblJoining.sourceAddress); // Blank every entry naming this peer (persists inside).
        gblMailbox.networkStateColor = WHAT_LIGHT_RED;
        gblLink.linkTimer = millis() + _RQST_WAIT_TIME; // Keep it green for several seconds.
        gblLink.state = WHAT_LINK_COMPLETED; // no response.  act like it never happened.
        #ifdef LINK_MONITOR
        Serial.print(F("Unlink :0x"));Serial.println(gblJoining.sourceAddress,HEX);
        #endif
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
      if(gblLink.linkTimer <= millis())
      {
        #ifdef LINK_MONITOR
        Serial.println(F("REMOVE link -timeout-"));
        #endif  // LINK_MONITOR
        gblLink.state =  WHAT_NO_LINK; // Reset to the "normal" state when the timer expires.
      }
    break;

    case  WHAT_LINK_COMPLETED:
        if (gblLink.linkTimer <= millis())  // FULL LINK IMPLEMENTATION (Nov 26, 2026): was timer2 - but every path INTO this state sets timer1, so the hold ran on a stale timer2 and could end instantly or hang. timer1 is the one that was set.
        {  // The result color has been shown long enough.
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
        Serial.print(F("Undefined LINK state : ")); Serial.println(gblLink.state,DEC);
        #endif
        gblLink.state = WHAT_NO_LINK;  //
      break;

  }
}

void  WHAT_HardwareSetup(void)
{
  // FULL LINK IMPLEMENTATION (Nov 26, 2026): both loops ran one past the
  // end (<= instead of <), pinMode-ing a garbage pin number read from
  // beyond the arrays. NUM_INPUTS / NUM_OUTPUTS are counts, not last indexes.
  for (uint8_t _i = 0; _i < NUM_INPUTS; _i++)
  {
      pinMode(gblLinkIO.inputPin[_i],INPUT);
  }
  for (uint8_t _i = 0; _i < NUM_OUTPUTS; _i++)
  {
      pinMode(gblLinkIO.outputPin[_i],OUTPUT);
  }
}

void WHAT_HardwareCheck(void)
{
#ifdef WHAT_LINK_APP_SENDS_VALUES
  // WHAT_LINK_APP_SENDS_VALUES (Nov 27, 2026): this folder's app
  // (WHAT_RemoteControl) already broadcasts its inputs as CMD_PWM01_04
  // blocks from its own Loop_SendData, on its own scale (percent of
  // measured supply). The link layer originates nothing here, and
  // received values are matched in WHAT_LinkDispatch() - so in this
  // folder there is nothing for the hardware check to do.
#else
  // Host apps that do NOT broadcast their own inputs (WHAT_Remote):
  // read the inputs as percents of full scale and broadcast them as
  // CMD_PWM01_04 blocks when any of them moves beyond the deadband.
  // (The Remote Board's inputs arrive through divider networks into
  // analog pins, and A6 has no digital input at all on the Nano -
  // analogRead serves all three.) Received values are matched in
  // WHAT_LinkDispatch(); this function only transmits.
  uint8_t _this_InputPercent[NUM_INPUTS];   // This pass's input values, 0-100.
  static uint8_t _st_lastSentPercent[NUM_INPUTS] = {0xFF, 0xFF, 0xFF};   // Last TRANSMITTED values - 0xFF forces a first send so a freshly linked peer syncs.
  static double  _st_transmission_delay = millis();       // Limit I/O updates using this timer.

  for (uint8_t _i = 0; _i < NUM_INPUTS; _i++)
  {
      _this_InputPercent[_i] = (uint8_t)(((uint32_t)analogRead(gblLinkIO.inputPin[_i]) * 100 + 511) / 1023);
  }

  // -IF- any input moved beyond the deadband, transmit the new values.
  // The deadband (2 points) keeps ADC jitter from flooding the network
  // with near-identical packets; the rate cap is the link's own
  // _WHAT_LINK_INPUT_UPDATE_RATE.
  bool _changed = false;
  for (uint8_t _i = 0; _i < NUM_INPUTS; _i++)
  {
      uint8_t _diff = (_this_InputPercent[_i] > _st_lastSentPercent[_i]) ? (_this_InputPercent[_i] - _st_lastSentPercent[_i]) : (_st_lastSentPercent[_i] - _this_InputPercent[_i]);
      if (_diff >= 2) _changed = true;
  }
  if(_changed & (_st_transmission_delay <= millis()))
  { // an input status has changed.  send the new values.
       WHAT_Packet _WHAT_Tx_Packet;    // Create a temporary transmit packet.
      _WHAT_Tx_Packet.pan_id = gblNetwork.panID;
      _WHAT_Tx_Packet.destination = 0xFFFF;   // Send this to all devices.
      _WHAT_Tx_Packet.sourceaddress = mySourceAddress;  // Use the source address of this device.
      uint8_t _index = 0;
      _WHAT_Tx_Packet.payload[_index++] = 1 + (4 * NUM_INPUTS); //Total payload length (including this byte): the length byte plus one 4-byte block per input.
      for (uint8_t _i = 0; _i < NUM_INPUTS; _i++)
      {
        _WHAT_Tx_Packet.payload[_index++] = 4; // Length of this channel's block (including this byte)
        _WHAT_Tx_Packet.payload[_index++] = CMD_PWM01_04;  //MID (single byte)
        _WHAT_Tx_Packet.payload[_index++] = _i + 1;   // Channel number (1-based, as WHAT_RemoteControl sends)
        _WHAT_Tx_Packet.payload[_index++] = _this_InputPercent[_i];  // Data value, 0-100
        _st_lastSentPercent[_i] = _this_InputPercent[_i];
      }
    WHAT_TransmitPacket(_WHAT_Tx_Packet);
    _st_transmission_delay = millis() + _WHAT_LINK_INPUT_UPDATE_RATE;

     #ifdef LINK_MONITOR
     Serial.println(F("Linked Input Update Sent : "));
     #endif

  }
#endif
}
