#ifndef __WHATCommands_h_
#define __WHATCommands_h_
#define WHAT_COMMAND_VERSION 191013 

//#include "z_WHAT_Command.h"

/* W.H.A.T.
 *  - Holds the common command list.
 *  - Not all commands are supported in every hardware set.
 *  - for compatability, commands should be chosen from this list.
*/

// WHAT message definitions

// General Purpose information: //
#define GOODMORNING_02          0x00 // 1 byte length, 1 byte command
#define GOODNIGHT_02            0x01 // 1 byte length, 1 byte command
#define I_ARRIVEDHOME_02        0x02 // 1 byte length, 1 byte command
#define I_LEAVINGHOME_02        0x03 // 1 byte length, 1 byte command
#define EVERYONELEAVING_02      0x04 // 1 byte length, 1 byte command
#define ALL_AWAY_X_HOURS_03     0x05 // 1 byte length, 1 byte command, 1 byte Number of hours
#define ALL_AWAY_X_DAYS_03      0x06 // 1 byte length, 1 byte command, 1 byte Number of days
#define LEAVE_FOR_VACATION_02   0x07 // 1 byte length, 1 byte command
#define ARRIVE_FROM_VACATION_02 0x08 // 1 byte length, 1 byte command
#define TYPICAL_SCHEDULE_03     0x09 // 1 byte length, 1 byte command, 1 byte TypicalScheduleNumber 
#define ALTERNATE_SCHEDULE_03   0x0A // 1 byte length, 1 byte command, 1 byte AlternateScheduleNumber

// 0x3? reserved for more general purpose commands.
#define LOCAL_TIME01_05         0x30    // 1 byte length, 1 byte command,Local time for this location H,M,S
#define LOCAL_TIME02_05         0x31    // 1 byte length, 1 byte command,Secondary Local time for this location H,M,S
#define LOCAL_TIME_NTPv4_06     0x35       //1 byte length, 1 byte command,4 byte NTPV4 format TIMESTAMP (excludes fractional seconds)  //Seconds since Jan 1,1900
#define LOCAL_TIME01_NTPv4_ERA_08   0x36 // 1 byte length, 1 byte command,8 byte NTPV4 format TIMESTAMP (Upper 32 bits is ERA)(excludes fractional seconds) //Seconds since Jan 1,1900

#define UNIV_TIME_NTPv4_06      0x38        //1 byte length, 1 byte command,4 byte NTPV4 format TIMESTAMP (excludes fractional seconds)  //Seconds since Jan 1,1900
#define UNIV_TIME01_NTPv4_ERA_08   0x39  // 1 byte length, 1 byte command,8 byte NTPV4 format TIMESTAMP (Upper 32 bits is ERA)(excludes fractional seconds) //Seconds since Jan 1,1900

#define LOCAL_DATE_NTPv4         0x3D
#define LOCAL_DATE_NTPv4_ERA_10  0x3E  // 1 byte length, 1 byte command,8 bytes This is an invalid 64 bit format. Upper 32bit is Era, lower is seconds since January 1,1900

#define WAKE_TIME_00_05         0x40    // 1 byte length, 1 byte command,NEXT alarm, H,M,S     // 1 byte command, 3 byte of data (thus the 04 total bytes)
#define WAKE_TIME_01_05         0x41    // 1 byte length, 1 byte command,Second alarm, H,M,S   // 1 byte command, 3 byte of data (thus the 04 total bytes)
#define WAKE_WEEK_01_06         0x45    // 1 byte length, 1 byte command,BULK Alarm times for monday-sunday H,M,S,(SMUWRFA)  (mask for Sunday,Monday...)     // 1 byte command, 4 byte of data (thus the 05 total bytes)
#define WAKE_WEEK_02_06         0x46    // 1 byte length, 1 byte command,BULK Alarm times for monday-sunday H,M,S,(SMUWRFA)  (mask for Sunday,Monday...)     // 1 byte command, 4 byte of data (thus the 05 total bytes)
#define WAKE_WEEK_03_06         0x47    // 1 byte length, 1 byte command,BULK Alarm times for monday-sunday H,M,S,(SMUWRFA)  (mask for Sunday,Monday...)     // 1 byte command, 4 byte of data (thus the 05 total bytes)
#define WAKE_DATE_13            0x4A    // 1 byte length, 1 byte command,Alarm time H,M,S,LOCAL_DATE_NTPv4_ERA_08 (see date specification above)    // 1 byte command, 4 byte of data (thus the 05 total bytes)

#define TEMPERATURE01_DEG_C_03  0x61       // 1 byte length, 1 byte command, 1 byte of data (thus the 03 total bytes)(Signed Integer)
#define TEMPERATURE01_DEG_F_03  0x67       // 1 byte length, 1 byte command, 1 byte of data (thus the 03 total bytes)(Signed Integer)

// Destination specific controls. 
#define CMD_ANALOG01_06 0x71       // 1 byte length, 1 byte command, 4 byte of data (floating point value of VOLTS)(thus the 06 total bytes)
#define CMD_ANALOG02_06 0x72       // 1 byte length, 1 byte command, 4 byte of data (floating point value of VOLTS)(thus the 06 total bytes)
#define CMD_ANALOG03_06 0x73       // 1 byte length, 1 byte command, 4 byte of data (floating point value of VOLTS)(thus the 06 total bytes)
#define REPLY_ANALOG01_06 0x75     // 1 byte length, 1 byte command, 4 byte of data (floating point value of VOLTS)(thus the 06 total bytes)
#define REPLY_ANALOG02_06 0x76     // 1 byte length, 1 byte command, 4 byte of data (floating point value of VOLTS)(thus the 06 total bytes)
#define REPLY_ANALOG03_06 0x77     // 1 byte length, 1 byte command, 4 byte of data (floating point value of VOLTS)(thus the 06 total bytes)

#define CMD_DIG01_04  0x78     // 1 byte length, 1 byte command, 1 byte (8 bits) of MASK, 1 byte(8 bits) of HIGH/LOW (thus the 04 total bytes)
#define CMD_DIG02_04  0x79     // 1 byte length, 1 byte command, 1 byte (8 bits) of MASK, 1 byte(8 bits) of HIGH/LOW (thus the 04 total bytes)
#define CMD_DIG03_04  0x7A     // 1 byte length, 1 byte command, 1 byte (8 bits) of MASK, 1 byte(8 bits) of HIGH/LOW (thus the 04 total bytes)
#define REPLY_DIG01_04  0x7B   // 1 byte length, 1 byte command, 1 byte (8 bits) of MASK, 1 byte(8 bits) of HIGH/LOW (thus the 04 total bytes)
#define REPLY_DIG02_04  0x7C   // 1 byte length, 1 byte command, 1 byte (8 bits) of MASK, 1 byte(8 bits) of HIGH/LOW (thus the 04 total bytes)
#define REPLY_DIG03_04  0x7D   // 1 byte length, 1 byte command, 1 byte (8 bits) of MASK, 1 byte(8 bits) of HIGH/LOW (thus the 04 total bytes)

#define CMD_LIGHTS01_04   0x81     // 1 byte length, 1 byte command, 1 byte (8 bits) of MASK, 1 byte(8 bits) of ON/OFF (thus the 04 total bytes)(lights 0-7)
#define CMD_LIGHTS02_04   0x82     // 1 byte length, 1 byte command, 1 byte (8 bits) of MASK, 1 byte(8 bits) of ON/OFF (thus the 04 total bytes)(lights 8-15)
#define CMD_LIGHTS03_04   0x83     // 1 byte length, 1 byte command, 1 byte (8 bits) of MASK, 1 byte(8 bits) of ON/OFF (thus the 04 total bytes)(lights 16-23)
#define REPLY_LIGHTS01_04   0x84   // 1 byte length, 1 byte command, 1 byte (8 bits) of MASK, 1 byte(8 bits) of ON/OFF (thus the 04 total bytes)(lights 0-7)
#define REPLY_LIGHTS02_04   0x85   // 1 byte length, 1 byte command, 1 byte (8 bits) of MASK, 1 byte(8 bits) of ON/OFF (thus the 04 total bytes)(lights 8-15)
#define REPLY_LIGHTS03_04   0x86   // 1 byte length, 1 byte command, 1 byte (8 bits) of MASK, 1 byte(8 bits) of ON/OFF (thus the 04 total bytes)(lights 16-23)

#define CMD_COLOR00_05    	0x89   // 1 byte length, 1 byte command, 1 byte (8 bits) light MASK, 2 byte (16-Bit) color (thus the 05 total bytes)(lights 0-7)
#define CMD_COLOR01_05    	0x8A   // 1 byte length, 1 byte command, 1 byte (8 bits) light MASK, 2 byte (16-Bit) color (thus the 05 total bytes)(lights 8-15)
#define CMD_COLOR02_05    	0x8B   // 1 byte length, 1 byte command, 1 byte (8 bits) light MASK, 2 byte (16-Bit) color (thus the 05 total bytes)(lights 16-23)
#define REPLY_COLOR01_05    0x8C   // 1 byte length, 1 byte command, 1 byte (8 bits) light MASK, 2 byte (16-Bit) color (thus the 05 total bytes)(lights 0-7)
#define REPLY_COLOR02_05	0x8D   // 
#define CMD_COLOR0n_08    	0x8E   // 1 byte length, 1 byte length, 1 byte command, 1 byte size, 1 byte light number, 2 byte color, 1 byte light number, 2  
#define REPLY_COLOR01_0n    0x8F   // 1 byte length, 1 byte length, 1 byte command, 1 byte size, 1 byte light number, 2 byte color, 1 byte light number, 2 byte color....

#define CMD_FET01_04    0x90      // 1 byte length, 1 byte command, 1 byte (8 bits) of MASK, 1 byte(8 bits) of ON/OFF (thus the 04 total bytes)
#define CMD_FET02_04    0x91      // 1 byte length, 1 byte command, 1 byte (8 bits) of MASK, 1 byte(8 bits) of ON/OFF (thus the 04 total bytes)
#define CMD_FET03_04    0x92      // 1 byte length, 1 byte command, 1 byte (8 bits) of MASK, 1 byte(8 bits) of ON/OFF (thus the 04 total bytes)
#define REPLY_FET01_04  0x94      // 1 byte length, 1 byte command, 1 byte (8 bits) of MASK, 1 byte(8 bits) of ON/OFF (thus the 04 total bytes)
#define REPLY_FET02_04  0x95      // 1 byte length, 1 byte command, 1 byte (8 bits) of MASK, 1 byte(8 bits) of ON/OFF (thus the 04 total bytes)
#define REPLY_FET03_04  0x96      // 1 byte length, 1 byte command, 1 byte (8 bits) of MASK, 1 byte(8 bits) of ON/OFF (thus the 04 total bytes)

#define CMD_TRIAC01_04    0x98     // 1 byte length, 1 byte command, 1 byte MASK, 1 byte(8 bits) of data (thus the 04 total bytes)
#define CMD_TRIAC02_04    0x99     // 1 byte length, 1 byte command, 1 byte MASK, 1 byte(8 bits) of data (thus the 04 total bytes)
#define CMD_TRIAC03_04    0x9A     // 1 byte length, 1 byte command, 1 byte MASK, 1 byte(8 bits) of data (thus the 04 total bytes)
#define REPLY_TRIAC01_04  0x9B     // 1 byte length, 1 byte command, 1 byte MASK, 1 byte(8 bits) of data (thus the 04 total bytes)
#define REPLY_TRIAC02_04  0x9C     // 1 byte length, 1 byte command, 1 byte MASK, 1 byte(8 bits) of data (thus the 04 total bytes)
#define REPLY_TRIAC03_04  0x9D     // 1 byte length, 1 byte command, 1 byte MASK, 1 byte(8 bits) of data (thus the 04 total bytes)

#define CMD_PWM01_03    0xA0     	// 1 byte length, 1 byte command, PWM percentage (0 to 100%)(positive integer)
#define CMD_PWM02_04    0xA1     	// 1 byte length, 1 byte command, 2 bytes value (0.00 to 100.00%(16 bits)) 0.01% scalar
#define CMD_PWM03_04    0xA2     	// 1 byte length, 1 byte command, 2 bytes value (0.00 to 100.00%(16 bits)) 0.01% scalar
#define REPLY_PWM01_03    0xA7      // 1 byte length, 1 byte command, PWM percentage (0 to 100%)(positive integer)
#define REPLY_PWM02_04    0xA8     	// 1 byte length, 1 byte command, 2 bytes value (0.00 to 100.00%(16 bits)) 0.01% scalar
#define REPLY_PWM03_04    0xA9     	// 1 byte length, 1 byte command, 2 bytes value (0.00 to 100.00%(16 bits)) 0.01% scalar
			
#define NETWORK_CREATIONPAGE 0xF000		// Network creation commands.	
#define COMMAND_PAGEF1 0xF100   		// Security commands - Page 1.  Page 1 command is next byte, thus #bytes,0xF1,Command,data
#define COMMAND_PAGEF2 0xF200   		// Additional commands - Page 2.  Page 2 command is next byte, thus #bytes,0xF1,Command,data
#define COMMAND_PAGEF3 0xF300  		// Additional commands - Page 3.  Page 3 command is next byte, thus #bytes,0xF1,Command,data
#define DEFINITION_PAGEFA 0xFA00  	// Support for new commands can be defined using this command page.
#define DEFINED_COMMAND_PAGE 0xFB00	// Commands defined in FA are used in this page.
#define UNDEFINED_PAGEFE 0xFE00  // Any users can send any proprietary commands they like on this page.  other devices will not necessarily accept these commands.

// *************************************page F0 commands ***************************************
// ************************************ Network Creation ***************************************
/*
*	Network creation:	- This is all built into WHAT.ino, and enabled by #define AUTO_ADDRESSING
*	All new devices are programmed with PANID FFFF and source address FFFF (considered a preemie)
*   When a device recognizes it is a preemie (PANID and SA are FFFF), it will periodically send a "REQUEST_JOIN_NETWORK_02" message.
*
*	Addressing is done in the following manner: (Hierarchy expands to more significant digits)
*			Device that forms network is assigned address "0x0000"
*			First child becomes address "0x0001", 2nd child "0x0002" etc...
*			A child of the first child device becomes "0x0011", 2nd child of the first device is "0x0021" etc...
*			A (grand)child of the SECOND child is address "0x0012", 2nd child OF THE 2nd child is "0x0022"
*				A great-grand child - for example, a child of the 3rd (grand)child of the 2nd child would be address "0x0132"
*			Device 0x0000 is the only device that does not have a parent.
*			Device 0x3FFF is the highest allowed address on this network. (16383 max devices)
*			Highest two bits are used for mesh direction and reserved purposes.
*			Address used as source address will be the address responded to as a destination address.
*
*	Preemie device sends a REQUEST_JOIN_NETWORK_03 message Every 5 seconds 
*			If network NOT seen, blink network light and send request at 1Hz
*			If PROPOSE_CHILD_NUMBER_06 seen, blink at 10Hz and request rate slows to every 10 seconds.
*			If NETWORK_ALLOW_PREEMIE message is seen (after at least 1 second wait) make network light solid on.
*       - the preemie selects the lowest address offered on this PANID
*       - If the PANID does not match the PANID from the NETWORK_ALLOW_PREEMIE message, revert the state to REQUEST_JOIN_NETWORK_03
*     When ACCEPT_PARENT_08 is seen, the preemie turns off its network light, and becomes a mature device.
*     the new parent sends NETWORK_JOIN_COMPLETE_03
*     New child now uses its new address and latchkey to ask parent for the security key.

*	If a mature device ((non-preemie) one that has already joined a network or created a network) receives 
* a "REQUEST_JOIN_NETWORK_03" (from a preemie):
*			- Sends a NETWORK_ALERT_PREEMIE message so other devices know a preemie is joining (so they can blink their network light)
*						- This has an advantage of identifying addresses that can see the preemie
*			- Begins blinking its network light
*			- Determines if it has a child address available (ChildMask <0x0F) (each device can have up to 16 children)
*			- Sends a PROPOSE_CHILD_NUMBER_06 message which includes:
*					Proposed child address
*					Proposed Latchkey value
*					Proposed relationship value
*	Devices with addresses less than 0x1100 (so no grand-children or great-grand children)when NETWORK_ALERT_PREEMIE message is seen:
*			- Blink their network light
*			- Monitor their network button for the next 30 seconds
*			- If their button is PRESSD AND RELEASEd, send a NETWORK_ALLOW_PREEMIE message.
*			- If their button is HELD (longer than 1 second) - send a NETWORK_JOIN_ABORT message.
*			- Security measures are monitored to verify no hi-jack,join-on,add-on, or lost child conditions have been breeched.
*				- Breeched security sends a NETWORK_JOIN_ABORT message.
*				- All devices will revert to non-network joining mode upon receipt of that message.
* 	Upon receiving a NETWORK_ALLOW_PREEMIE_04 message:
*			- The preemie device waits for 1 full second, while recording PROPOSE_CHILD_NUMBER_06 messages from the network.
*			- Received PROPOSE_CHILD_NUMBER_06 will be recorded.
*			- Additional received PROPOSE_CHILD_NUMBER_06 will over-write stored value **IF** the source address of the sender is LOWER than previous messages.
*	After 1 second, the preemie sends ACCEPT_PARENT_08 message:
*			- uses the PANID of the network
*			- Uses the chosen source address from those proposed.
*			- Sends  CONFIRM_NEW_CHILD_03 message.	
*				- network devices other than the specified parent have the same reaction as NETWORK_JOIN_ABORT
*
*  PREEMIES that do NOT get a response to REQUEST_JOIN_NETWORK_02, or see NETWORK_ALERT_PREEMIE, 
*			Repeat message every 10 seconds.
*
*  PREEMIES that RECIEVE a REQUEST_JOIN_NETWORK_02,
*			Stop broadcasting the message
*			Create its own network, becoming address 0x0000
*			send a ACCEPT_PARENT_08 message.
*				if not accepted within 30 seconds, reverts to preemie, and sends NETWORK_JOIN_ABORT_03.
*
*  NEW_PARENT will send NETWORK_JOIN_COMPLETE_03.
*     - All devices will leave NETWORK_ALERT_PREEMIE and/or NETWORK_ALLOW_PREEMIE_04 mode, and return to normal operation.
*
*/

//New device joining network (preemie) (corresponding responses from network in the 0xF01? section)
#define REQUEST_JOIN_NETWORK_03  0xF001 // 1 byte length, 2 byte command   // preemie requests joining network listed in PANID (invalid for PAN 0xFFFF)
#define CHOOSE_NETWORK_05        0xF003 // 1 byte length, 2 byte command, 2 byte PANID - PANID of 0xFFFF must be used in transmission.  Other networks should send NETWORK_JOIN_ABORT_03 to stop trying to join this device.
               // Preemie assigns itself the PANID of this network for further connection activities.
#define ACCEPT_PARENT_06         0xF005 // 1 byte length, 2 byte command, 2 byte latchkey, 1 byte relation type // preemie accepts offer from parent, and proposes latch key value (derived from its timer upon sending)
#define REQUEST_SECURITY_KEY_03  0xF007 // 1 byte length, 2 byte command   // Upon reciept of this message, the parent accepts this child.

//New device is joining network (mature response)  (see requests from child in the 0x0? section)
#define NETWORK_ALERT_PREEMIE_03	  0xF011	  // 1 byte length, 2 byte command  // A preemie is trying to gain access to the network.
#define NETWORK_ALLOW_PREEMIE_04	  0xF013	  // 1 byte length, 2 byte command, 1 byte confirm/deny, Any network button can be pressed to allow the addition of a preemie.  this message provides that indication.
#define PROPOSE_CHILD_NUMBER_05     0xF015    // 1 byte length, 2 byte command, 2 byte proposed child address // Any device that has open child slots replies to request (REQUEST_JOIN_NETWORK_02).

#define NETWORK_JOIN_COMPLETE_03    0xF020   // 1 byte length, 2 byte command  // Sent by new parent (Birth Announcement)
#define NETWORK_JOIN_ABORT_03		    0xF022	 // 1 byte length, 2 byte command  //the button was HELD on some device on the network being joined.
#define ABANDON_CHILD_05            0xF026   // 1 byte length, 2 byte command, 2 byte latchkey. (destination is parent address, source is child)

#define PARENT_GIVE_SECURITY_KEY_17 0xF030  //1 byte length, 2 byte command, 14 bytes with latchkey followed by security key.

// Network/communications information
#define REQUEST_ACK_03              0xF040    // 1 byte length, 2 byte command // destination requested to reply with ACK.
#define REQUEST_DATA01_0x           0xF041    // 1 byte length, 2 byte command, 1dataCmd, data, 1dataCmd, data, ...
#define REQUEST_DATA02_0x           0xF042    // 1 byte length, 2 byte command, 1dataCmd, data, 1dataCmd, data, ...
#define REQUEST_DATA03_0x           0xF043    // 1 byte length, 2 byte command, 1dataCmd, data, 1dataCmd, data, ...
#define REQUEST_DATA04_0x           0xF044    // 1 byte length, 2 byte command, 1dataCmd, data, 1dataCmd, data, ...

// ** info is text of : function, #inputs, #outputs, description, options, Display screen data
#define REQUEST_YOUR_INFO_SIZE_02   0xF048  // 1 byte length, 2 byte command // asks destination how many bytes of information destination is able to send.
#define REQUEST_YOUR_INFO_02        0xF049  // 1 byte length, 2 byte command // Databyte is number of bytes of information for the destination to send. 
#define REQUEST_SUPPORT_THIS_02     0xF04A  // 1 byte length, 2 byte command, 1 databyte containing command that support is being requested for. 
          // For instance...  REQUEST_SUPPORT_THIS_02 o_TRIAC01_02 .....   "does destination support triac output control?"
#define REQUEST_SUPPORTED_COMMANDS_02 0xF04C  // 1 byte length, 2 byte command //reciever replys with REPLY_SUPPORTED_COMMANDS01_xx...
#define SWITCH_TO_STANDARD_COMM_02  0xF04E     // 1 byte length, 2 byte command //reciever replys with REPLY_STANDARD_COMM_02
#define SWITCH_TO_TURBO_COMM_02     0xF04F     // 1 byte length, 2 byte command //reciever replys with REPLY_SWITCH_TO_TURBO_COMM_02

#define REPLY_ACKNOWLEDGE_03        0xF050    // 1 byte length, 2 byte command// Sent in response to REQUEST_ACK.  No data bytes.
#define REPLY_TO_REQUEST_DATA01_xx  0xF051    // 1 Databyte is the command number of the request (for single-byte commands)
#define REPLY_TO_REQUEST_DATA02_xx  0xF052    // 2 Databytes is the command number of the request (for multi-byte commands)
#define REPLY_TO_REQUEST_DATA03_xx  0xF053    // 3 Databytes is the command number of the request (for multi-byte commands)
#define REPLY_TO_REQUEST_DATA04_xx  0xF054    // 4 Databytes is the command number of the request (for multi-byte commands)

// ** info is text of : function, #inputs, #outputs, description, options, Display screen data
#define REPLY_YOUR_INFO_SIZE_03     0xF058       // 1 byte length, 2 byte command, destination informs how many bytes of information destination is able to send.
#define REPLY_YOUR_INFO_0x          0xF059       // 1 byte length, 2 byte command, number of bytes of information the destination as requested to send. 
#define REPLY_SUPPORT_THIS_03       0xF05A       // 1 byte length, 2 byte command, 00 = NOT supported, 01 = Supported 
#define REPLY_SUPPORTED_COMMANDS01_xx 0xF05B  // A list of supported 1-byte commands.1st byte is number of commands supported. next 29 bytes are list of commands
#define REPLY_SUPPORTED_COMMANDS02_xx 0xF05C  // NEXT list of supported 1-byte commands. (only valid if more than 29 commands supported.)
#define REPLY_SUPPORTED_COMMANDS03_xx 0xF05D  // LAST list of supported 1-byte commands. 
#define REPLY_STANDARD_COMM_03        0xF05E  // 1 byte length, 2 byte command
#define REPLY_SWITCH_TO_TURBO_COMM_03 0xF05F  // 1 byte length, 2 byte command

#define REQUEST_FULL_LINK_03        0xF060    // 1 byte length, 2 byte command. Connect this device's inputs to the responding devices outputs and vice versa.
#define ACCEPT_FULL_LINK_03         0xF067    // 1 byte length, 2 byte command. Connected device's accepting response.
#define REMOVE_FULL_LINK_03         0xF06E    // 1 byte length, 2 byte command. Formerly linked device was unlinked.
#define REJECT_FULL_LINK_03         0xF06F    // 1 byte length, 2 byte command. Connected device's refusing response.

// *************************************page F1 commands ***************************************
// Page 1 commands are used entirely for security.
// All devices must comply with the following security procedures: (These are built into the WHAT.ino file.)
//		1) if a SecurityInformation command (0xF100) is seen, all devices replace their security key with the new security key.
//		2) 00 - (hijack) if a message that this device did not send has the same PANID and SourceADDRESS, this device must send a level 1 security breach message.
//		3) 10 - (join-on) A parent sees a source address for a child that it has not adopted.
//		4) 20 - (add-on) A parent sees a source address higher than its highest adopted child's address.
//		5) 50 - (lostChild)A parent tries to communicate with a child that should be present but does not get a response

// Temporary or foster devices must comply with the following security procedures: (These are **NOT** built into the WHAT.ino file.)
//		1) 30 - (kidnap) A temporary or foster device returns without following the proper return protocol.
// 		2) 40 - (mugging)A temporary or foster device returns to the network but has the wrong latchkey.

//  Main forms of integrity breaches are:
//    - Accident (neighbor is installing a system that conflicts with this system)
//    - Vandalism (Someone trying to initiate nuisance actions - blare alarm, start vehicle, open/close garage...)
//    - Security disabling (Attempt to disable security by initiating lock-down)
//    - Security defeating (attempting to make false alarms so user will turn it off or not trust it.)
// 
//  Several integrity breach indications are possible:
//    - (hijack) A device sees a message it did not initiate using its own source address.
//    - (join-on) A parent sees a source address for a child that it has not adopted.
//    - (add-on) A source address higher than the highest adopted child is seen.
//    - (kidnap) A temporary or foster device returns without following the proper return protocol.
//    - (mugging)A temporary or foster device returns to the network but has the wrong latchkey.
//    - (lostKid)A parent tries to communicate with a child that should be present but does not get a response
// The device that identifies the breach should send the "INTEGRITYBREACH" message (a Page 1 message).
// The designated security *controller*(s) will begin a security procedure once it is connected to the network.
//	A security *controller* is a device capable of sounding an alarm.  Example are:
//		- Siren controller
//		- Text/e-mail/call initiation devices.
//	The lowest address security *controller* will begin sending a rotating key message once every 5 seconds.
//		: #bytes, SecurityInformation command (0xF100), 14 bytes of keys
//		Each security key is 2 bytes.  Key rotation uses the same 7 (initially random) keys for message iterations.
//		The current key will be placed in a random location in the list, immediately followed by the new key. 
//				** if the current key is the last byte, the new key would be the first byte.


#define SECURITY_INFO_xx	0xF100		// 1 byte length, 1 byte command,
#define Security_Breach_04 	0xF101		// 1 byte length, 2 byte command, 1 byte Breach type:
#define   SB_HIJACK 00    //(hijack) A device sees a message it did not initiate using its own source address.
#define   SB_JOIN_ON 00   //(join-on) A parent sees a source address for a child that it has not adopted.
#define   SB_ADD_ON 00    //(add-on) A parent sees a source address higher than its highest adopted child's address.
#define   SB_KIDNAP 00    //(kidnap) A temporary or foster device returns without following the proper return protocol.
#define   SB_MUGGING 00   //(mugging)A temporary or foster device returns to the network but has the wrong latchkey.
#define   SB_BUMP 00      //(bump)   A parent sees a request to abandon child, but the latchkey value is incorrect.
#define   SB_LOSTCHILD 00    //(lostChild)A parent tries to communicate with a child that should be present but does not get a response

#define SECURITY_IN01_03    0xF1E1     // 1 byte length, 1 byte command, 1 byte(8 bits) of data (thus the 03 total bytes)
#define SECURITY_IN02_03    0xF1E2     // 1 byte length, 1 byte command, 1 byte(8 bits) of data (thus the 03 total bytes)
#define SECURITY_IN03_03    0xF1E3     // 1 byte length, 1 byte command, 1 byte(8 bits) of data (thus the 03 total bytes)
#define SECURITY_OUT01_03   0xF1E6     // 1 byte length, 1 byte command, 1 byte(8 bits) of data (thus the 03 total bytes)
#define SECURITY_OUT02_03   0xF1E7     // 1 byte length, 1 byte command, 1 byte(8 bits) of data (thus the 03 total bytes)
#define SECURITY_OUT03_03   0xF1E8     // 1 byte length, 1 byte command, 1 byte(8 bits) of data (thus the 03 total bytes)


// ************************************************************************
// ************************************************************************
// ************************************************************************
// Relationship types of children. (given to parent in ACCEPT_PARENT_08 or child in)  
//    (Upper nibble is relationship, lower nibble is confirm/deny)
#define WHAT_PERMANENT   0x10  // The intent is for this device to be a permanent member of this network.
#define WHAT_TEMPORARY   0x20  // This device will only be part of the network for x days,and must re-new its membership.
#define WHAT_PART_TIME   0x30     // This device will come and go, but stays a permanent member of the network 
#define WHAT_LEASE       0x40       // This device will come and go, but must re-new its lease periodically.


#endif
