
#ifndef __WHAT_MRF24J_h
#define __WHAT_MRF24J_h
#define WHAT_MRF24J_VERSION 260724

#include <SPI.h>
#include <mrf24j_G.h> // *****************  trying to remove this from the final project.
#include "z_mrf24j_Registers.h"

#//define NumBufferRxMessages 4 // Make this number larger if "registering" more messages.
uint8_t GBL_rx_index = 0;
WHAT_Packet WHAT_Buff_Message[RX_BUFFER_DEPTH];

//#define PHYSICAL_MRF24J
//#include "z_WHAT_mrf24j.h"	// Physical layer routines.


/*
 * I pulled the useful information from the mrf24jmodule library into this file.
 * Without needing the library, this file initializes the mrf24j module, and will establish "WHAT" communications
 * including:
 *    - Network creation
 *    - Joining to network (with auto-addressing)
 *    - child/parent protocols
 *    - MessageID registration, monitoring, reception and callback.

*/

//#define TX_BUFFER_DEPTH 3 //This will have a dramatic effect on Dynamic memory, so try to keep it small.

// 888888888888888888 Prototypes 88888888888888888888888888
// Public:
//void  mrf.init(void);
void Mrf24j_begin (uint16_t myPANID, uint16_t mySourceAddress);
void Mrf24j_Loop(void);
bool WHAT_Buffer_RX( WHAT_Packet *,uint8_t);
// Private

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

#endif
