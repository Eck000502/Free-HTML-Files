#ifndef __WHAT_FULLLINK_h
#define __WHAT_FULLLINK_h
#define WHAT_FULLLINK_VERSION 26112704

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

PER-OUTPUT LINK TABLE (Nov 27, 2026, Gary's design): the button pairing
above is now a SHORTCUT that fills a general table. Only the OUTPUTS
need link entries: each entry names one output, the source address it
listens to, the command it watches, and which of the source's channels
it follows. A full link writes one entry per output, all naming the
same peer, channels straight across. Entries can also be written one
at a time over the air (SET_LINK_ENTRY_03) - so a larger device (a
Teensy app) can be the named source for a single output, apply its own
scaling, and leave the other outputs to other sources. The table is a
flat list: several entries may name the same output (x-way control),
and an output is driven by whichever of its sources CHANGED last -
each entry remembers the last value its source sent, and a re-sent
unchanged value has no vote. A value entry ADOPTS its new value on
change; an entry flagged TOGGLE flips its output on change instead
(the hallway switch case). Transmitters need no table at all: a
device's ordinary value broadcasts are all a source ever sends.

*/

#define LINK_MONITOR  1   // ON in this folder (Nov 27, 2026): first bench of the entry-table build - the link narrative stays visible. Turn off for a production build.

// These were originally only going to be DIGITAL values. PWM REPLACEMENT
// (Nov 27, 2026, Gary's call): the link carries each input as a 0-100
// PWM value (CMD_PWM01_04 blocks) instead of packed digital bits - a
// switch is simply 0 or 100, and a pot gets its full range. Digital
// commands (CMD_DIG01_04) are also watched: an entry naming one follows
// its channel bit (mask-gated) as 0 / 100.
#define NUM_OUTPUTS 3   // (Limit to 8) Each OUPUT can hold a link to being controlled by an INPUT.
#define NUM_INPUTS  3   // (Limit to 8) The inputs really just broadcast their value.  It is up to the recieving output to monitor this input.
#define WHAT_MAX_LINK_ENTRIES 16  // PER-OUTPUT LINK TABLE (Nov 27, 2026): entries in gblLink.entries[]. (Was numWHATLinks, the count of peer addresses in the old symmetric link list - the list itself is gone; see WHAT_LinkEntry.)

// WHAT_LINK_APP_SENDS_VALUES (Nov 27, 2026): defined in THIS folder
// because the WHAT_RemoteControl app already broadcasts its inputs as
// CMD_PWM01_04 blocks on its own schedule - the link layer must not
// ALSO originate value broadcasts from WHAT_HardwareCheck (the two
// would send the same channels on different scales and fight). A host
// app that does NOT broadcast its own inputs (WHAT_Remote) leaves this
// undefined, and WHAT_HardwareCheck originates the broadcasts there.
#define WHAT_LINK_APP_SENDS_VALUES

#define _WHAT_NEW_LINK_HELD_TIME_DELAY  5000  // Time the network button must be held while in NetAlive before initiating a link
#define _WHAT_NEW_Link_TIME_DELAY 60000 // Only stay in new child mode for 30 seconds if not accepted earlier.
#define _WHAT_LINK_INPUT_UPDATE_RATE 50   // minimum milliseconds between message

// PER-OUTPUT LINK TABLE (Nov 27, 2026): one entry = one output's subscription.
// EEPROM stores output/source/command/channel/flags (7 bytes per entry,
// 16 entries = 112 bytes, behind a 2-byte magic at EEPROM_LINK_LOCATION).
// lastValue is RAM only: the last value this entry's source sent, for
// change detection (0xFF = nothing heard yet, so the first packet counts).
#define LINK_ENTRY_FLAG_TOGGLE 0x01   // On a change from this source, flip the output (off <-> last adopted level) instead of adopting the value.
struct WHAT_LinkEntry
{
  uint8_t output = 0xFF;      // Which of THIS device's outputs (0-based). 0xFF = blank entry.
  uint16_t source = 0xFFFF;   // The address this output listens to. 0xFFFF = blank entry.
  uint16_t command = 0;       // The message ID watched (e.g. CMD_PWM01_04, CMD_DIG01_04).
  uint8_t channel = 0;        // Which of the source's channels this output follows (1-based).
  uint8_t flags = 0;          // LINK_ENTRY_FLAG_* above.
  uint8_t lastValue = 0xFF;   // RAM only: last value heard from this source+channel (0-100), 0xFF = none yet.
};
#define WHAT_LINK_ENTRY_MAGIC1 0x4C   // 'L' - guards the EEPROM entry block: EEPROM written by the old peer-list layout (or never written) has no magic, and the table loads blank instead of as garbage entries.
#define WHAT_LINK_ENTRY_MAGIC2 0x32   // '2' - entry table layout version 2 (the peer list was 1, implicitly).

// Group: WHAT_LinkIO (instance gblLinkIO) - FullLink's input/output pin and value tables.
struct WHAT_LinkIO
{
  uint8_t inputPin[NUM_INPUTS];     // Pin number
  uint8_t inputValue[NUM_INPUTS];   // Value read for the given pin.
  uint8_t outputPin[NUM_OUTPUTS];   // Pin number
  volatile uint8_t outputValue[NUM_OUTPUTS]; // Value for the given pin. (volatile: read by the app's PWM timer ISR in the FullLink build.)
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
  // (Nov 27, 2026: the old peer list - links[numWHATLinks] and the
  // alreadyLinked slot index - is GONE, replaced by the entry table
  // below. Pairing now asks: does any entry name this peer (linked),
  // is there a blank entry (room), fill entries from a peer, clear a
  // peer's entries. See the helpers in z_WHAT_FullLink.ino.)
  WHAT_LinkEntry entries[WHAT_MAX_LINK_ENTRIES];   // PER-OUTPUT LINK TABLE (Nov 27, 2026) - see WHAT_LinkEntry.
  uint8_t lastAdoptedLevel[NUM_OUTPUTS] = {0xFF, 0xFF, 0xFF};   // RAM only: the last level a VALUE entry adopted per output (0xFF = never) - a TOGGLE entry flips back to this.
  uint8_t commandedOutputs = 0;  // Staging: the STATES byte of a received CMD_DIG01_04 (its MASK byte is commandedDigMask). (Was the draft's packed digital byte in the original design; the name is Gary's.)
  uint8_t commandedDigMask = 0;  // Staging: the MASK byte of a received CMD_DIG01_04 - a channel whose mask bit is clear is not being commanded and produces no change.
  uint8_t commandedPWM[NUM_OUTPUTS];  // Staging: a received packet's per-channel PWM values (0-100), by channel-1. WHAT_LinkDispatch matches them against the entry table.
  uint16_t commandedSource = 0xFFFF;   // Staging: the source address of the staged value packet, captured WITH the values - gblJoining.sourceAddress is shared scratch that a later packet in the same receive batch could overwrite before dispatch runs.
  uint16_t commandedDestination = 0xFFFF;   // Staging: the staged packet's destination, captured with it. It decides how dispatch treats the packet (Gary's rule, Nov 27, 2026): DIRECTED at this device (== mySourceAddress) applies directly; BROADCAST (0xFFFF) applies only through the entry table.
  // GET/REPLY dump state (Nov 27, 2026): a received GET arms this; WHAT_LinkDumpCheck() paces the REPLY packets out (one per entry, then an end-of-list marker) so a broadcast GET does not make every unit answer in the same instant.
  bool     dumpActive = false;
  uint16_t dumpRequester = 0xFFFF;
  uint8_t  dumpSelector = 0xFF;   // Entry# asked for, or 0xFF = all entries.
  uint8_t  dumpNextEntry = 0;
  uint32_t dumpNextAt = 0;
  bool     dumpEndPending = false;
  // The last REPLY heard, staged for the asking app (replyEntry 0xFF = that unit's end-of-list marker).
  uint16_t replyFrom = 0xFFFF;
  uint8_t  replyEntry = 0xFF;
  uint8_t  replyOutput = 0xFF;
  uint16_t replySource = 0xFFFF;
  uint16_t replyCommand = 0xFFFF;
  uint8_t  replyChannel = 0xFF;
  uint8_t  replyFlags = 0xFF;
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
  bool f_CMD_DIG01_04 = false;   // A digital command arrived and is staged (commandedDigMask / commandedOutputs).
  bool f_CMD_PWM01_04 = false;   // A PWM command arrived and is staged (commandedPWM).
  bool f_LINK_OUTPUTS_APPLIED = false;   // WHAT_LinkDispatch changed at least one output from the entry table - the app renders gblLinkIO.outputValue[].
  bool f_LINK_LEGACY_PWM = false;   // A PWM packet arrived DIRECTED at this device - the app applies it directly (all channels), bypassing the table.
  bool f_LINK_LEGACY_DIG = false;   // A digital packet arrived DIRECTED at this device - the app applies it directly, bypassing the table.
  bool f_REPLY_LINK_ENTRY_03 = false;   // A REPLY to a GET arrived and is staged in gblLink (replyFrom / replyEntry / ...).
};
WHAT_LinkRxFlags gblLinkRxFlags;



#endif
