
#ifndef __mrf24j_registers__
#define __mrf24j_registers__

// MRF24j Register Names and Addresses
// This is a definition of CONSTANTS that will be used by the ZIGBEE.Inc file so names can be used in stead of addresses.
// These need to be defined at the beginning of the file.  By defining them here and using this include, the main routine can be cleaner.

//#include "z_mrf24j_Registers.h"  ; The addresses for all the mrf24j40 registers are defined in this seperate file to keep things clean.

// *****************;*****************;*****************;*****************;*****************;*****************
// *****************;*****************;*****************;*****************;*****************;*****************
// *****************;*****************;*****************;*****************;*****************;*****************
                    
//#define  MRF24J_FrameControlDefault 0b00100000    //  FrameType(3bits), Security, Frame Pending, Ack Request,PANID COmpression,Reserved
//                   //  Found in the 802.15.4-2011 spec by searching Frame Control
//#define  MRF24J_FrameControlDefault2 0b00000010 //  Reserved (2bits),DestinationMode, Frame Version, SourceAddressMode
//                   //  Found in the 802.15.4-2011 spec by searching Frame Control
#define  MRF24J_FrameControlDefault 0b00100000   //  FrameType(3bits), Security, Frame Pending, Ack Request,PANID COmpression,Reserved
                    //  Found in the 802.15.4-2011 spec by searching Frame Control
#define  MRF24J_FrameControlDefault2 0b00010001  //  Reserved (2bits),DestinationMode, Frame Version, SourceAddressMode
                    //  Found in the 802.15.4-2011 spec by searching Frame Control
#define  MRF24J_GPIO_Rx   0b00000000 //  MRF24J40 GPIO pins - Bit4-7 = N/C, Bit3=Rx,Bit2=Tx, Bit1=2.2vPower, Bit0=N/C 

  // *** NOTE, execute z_GPIO_2v_On 150uS prior to sending z_GPIO_Tx 
#define  MRF24J_GPIO_2v_On  0b00000010 //  Enables the "2.2v" circuitry ONLY    Bit4-7 = N/C, Bit3=Rx,Bit2=Tx, Bit1=2.2vPower, Bit0=N/C 
#define  MRF24J_GPIO_Tx   0b00000110 //  Enables the "PA" and 2.2v circuitry  Bit4-7 = N/C, Bit3=Rx,Bit2=Tx, Bit1=2.2vPower, Bit0=N/C 

#define  MRF24J_GPIO_AllOff 0b00000000 //  Bit4-7 = N/C, Bit3=Rx,Bit2=Tx, Bit1=2.2vPower, Bit0=N/C 

//Bits
//INTSTAT: INTERRUPT STATUS REGISTER
//RC-0 RC-0 RC-0 RC-0 RC-0 RC-0 RC-0 RC-0
//SLPIF WAKEIF HSYMTMRIF SECIF RXIF GTS2TXIF GTS1TXIF TXIF
//ZNTXIF = 0    //  1=The data has transmitted.
//ZGTS1TXIF = 1 //  1=The data has transmitted.
//ZGTS2TXIF = 2 //  1=The data has transmitted.
//ZRXIF = 3   //  1=Data has been Received.
//ZSECIF = 4    //  1 = Security key request interrupt occurred
//ZHSYMTMRIF = 5  //  1 = Half symbol timer interrupt occurred
//ZWAKEIF = 6   //  1 = Wake-up interrupt occurred.
//ZSPLIF = 7    //  1 = Sleep alert interrupt occurred



// When determining the value,  Bit0 must be 0 to read/1 to WRITE, and for bit 8 0=short addresses/1=Long address.  
// Short Address control registers
#define  MRF24J_RXMCR  0x00 //   /TxCRCEn,Reserved,/AckEn,Reserved,PANCOORD,COORD,/RxCRCEn,/DiscardRxErr
#define  MRF24J_PANIDL 0x01 //  (address1)READ
#define  MRF24J_PANIDH 0x02 //  (address2)READ
#define  MRF24J_SADRL  0x03 //  (address3)READ
#define  MRF24J_SADRH  0x04 //  (address4)READ
#define  MRF24J_EADR0  0x05  //  (address5)READ
#define  MRF24J_EADR1  0x06  //  (address6)READ
#define  MRF24J_EADR2  0x07  //  (address7)READ
#define  MRF24J_EADR3  0x08  //  (address8)READ
#define  MRF24J_EADR4  0x09 //  (address9)READ
#define  MRF24J_EADR5  0x0A //  (addressA)READ
#define  MRF24J_EADR6  0x0B //  (addressB)READ
#define  MRF24J_EADR7  0x0C //  (addressC)READ
#define  MRF24J_RXFLUSH  0x0D //  (addressD)READ
#define  MRF24J_Res0E  0x0E //  (addressE)READ
#define  MRF24J_Res0F  0x0F //  (addressF)READ
#define  MRF24J_ORDER  0x10 //  (address10h)READ
#define  MRF24J_TXMCR  0x11 //  (address11h)READ
#define  MRF24J_ACKTMOUT  0x12  //  (address12h)READ
#define  MRF24J_ESLOTG1  0x13 //  (address13h)READ
#define  MRF24J_SYMTICKL  0x14  //  (address14h)READ
#define  MRF24J_SYMTICKH  0x15  //  (address15h)READ
#define  MRF24J_PACON0  0x16 //  (address16h)READ
#define  MRF24J_PACON1  0x17 //  (address17h)READ
#define  MRF24J_PACON2  0x18 //  (address18h)READ
#define  MRF24J_Res19  0x19 //  (address19h)READ
#define  MRF24J_TXBCON0  0x1A //  (address1Ah)READ
// MRF24J_TXNCON: TRANSMIT NORMAL FIFO CONTROL REGISTER (ADDRESS: 0x1B)
#define  MRF24J_TXNCON  0x1B //  Read (address1Bh) Initiates Data transmit
  #define MRF24J_TXNTRIG     0b00000001 // Transmit now bit
  #define MRF24J_TXNSECEN    1
  #define MRF24J_TXNACKREQ   2
  #define MRF24J_INDIRECT    3
  #define MRF24J_FPSTAT      4

#define  MRF24J_TXG1CON  0x1C //  Read (address1Ch) Initiates Data transmit
#define  MRF24J_TXG2CON  0x1D //  Read (address1Dh) Initiates Data transmit
#define  MRF24J_ESLOTG23  0x1E  //  (address1Eh)READ
#define  MRF24J_ESLOTG45  0x1F  //  (address1Fh)READ

#define  MRF24J_ESLOTG67  0x20  //  (address20h)READ
#define  MRF24J_TXPEND  0x21 //  (address21h)READ
#define  MRF24J_WAKECON  0x22 //  (address22h)READ
#define  MRF24J_FrmOffset  0x23 //  (address23h)READ
#define  MRF24J_TXSR  0x24 //  (address24h)READ 
#define  MRF24J_TXSTAT  0x24//  (address24h)READ  ** Different name for the same register
#define  MRF24J_TXBCON1  0x25 //  (address25h)READ
#define  MRF24J_GateClk  0x26 //  (address26h)READ
#define  MRF24J_TxTime  0x27 //  (address27h)READ
#define  MRF24J_HSYMTMRL  0x28 //  (address28h)READ
#define  MRF24J_HSYMTMRH  0x29 //  (address29h)READ
#define  MRF24J_SOFTRST  0x2A //  From Microchip stack(address2Ah)READ
//#define  MRF24J_RESERVED  0x2B //  (address2Bh)READ
//#define  MRF24J_RESERVED  0x2C //  (address2Ch)READ
#define  MRF24J_SECCON1  0x2D  //  (address2Dh)READ
#define  MRF24J_TXPEMISP  0x2E  //  Read (address2Eh)READ
//#define  MRF24J_RESERVED  0x2F //  (address2Ch)READ

#define  MRF24J_RXSR  0x30 //  (address2Ch)READ
#define  MRF24J_INTSTAT  0x31 //  (address31h)READ
#define  MRF24J_INTMSK  0x32 //  (address32h)READ
#define  MRF24J_GPIO  0x33 //  (address33h)READ
#define  MRF24J_TRISGPIO  0x34 //  (address34h)READ
#define  MRF24J_SLPACK  0x35   //  (Address 35h)Read
#define  MRF24J_RFCTL  0x36 //  (address36h)READ
#define  MRF24J_SECCR2  0x37 //  (address37h)READ

#define  MRF24J_BBREG0  0x38 //  (address38h)READ
#define  MRF24J_BBREG1  0x39 //  (address39h)READ
#define  MRF24J_BBREG2  0x3A //   CCAMode(7-6), CCAThresh (1110 recommended), Unimplimented(1-0)
#define  MRF24J_BBREG3  0x3B //  (address3Bh)READ
#define  MRF24J_BBREG4  0x3C //  (address3Ch)READ

//#define  MRF24J_RESERVED  0x3D //  (address3Dh)READ
#define MRF24J_BBREG6  0x3E //  RSSIReqMode1,RSSIReqMode2,Reserved(5-1), RSSIReady
#define MRF24J_CCAEDTH 0x3F //

/*
#define  MRF24J_CCAEDTH 0x3F //  (address3Fh)READ
*/

// **********************

// When determining the value,  Bit0 must be 0 to read/1 to WRITE, and for bit 8 0=short addresses/1=Long address.  
//  Long Address control registers.
//  First bit is 1, next 10 are the address, 12th is 0=read/1=write
// *** NOTE *** long address requires 4 clock bits between address and Data.
//  This is accomplished by LEFT JUSTIFYING these constants.
//  Using "#define  MRF24J_CLKINTCR_w" as an example....
  //  The register address listed in the spec is "211h"
  //      Which in Binary is           0010 0001 0001
  //  The ADDRESS is only 10 bits, but LONG addresses BEGIN with a 1, so left shift the number once, then SET the high bit.
  //        Register address shifted left once - 0100 0010 0010
  //        Forcing the first bit high becomes - 1100 0010 0010 
  //  Since this parameter is to be written, 12th bit sent must be a 1.
  //            The new number becomes         - 1100 0010 0011 
  //  The lowest nibble of the lowest byte is the 4 Zeros required between address and data
  //            The new number becomes         - 1100 0010 0011 0000
  //  So the table entry is (converted back to Hex) -  C    2    3    0 
    //  If this were a READ parameter, it would be    -  C    2    2    0 
  //  A quick visual indication is if the number is EVEN, it is a read, but ODD numbers are WRITES.

#define  MRF24J_RFCTRL0   0x200 //  (Address 200h) READ
#define  MRF24J_RFCTRL1   0x201 //  (Address 201h) READ
#define  MRF24J_RFCTRL2   0x202 // ;RF-PLL, Reserved, Reserved, Reserved, Reserved, Unimplimented, Unimplimented, Unimplimented,
#define  MRF24J_RFCTRL3   0x203 // ;TXPower(7-3)(-1.25perBit),Unimplemented(Bits2-0)
//#define  MRF24J_RESERVED   0x204 //  (Address 204h) READ
#define  MRF24J_RFCTRL5   0x205 //  (Address 205h) READ
#define  MRF24J_RFCTRL6   0x206 //; TxFilter,unimplemented,Reserved,Reserved,BatteryMonitorEn,unimplemented,unimplemented,unimplemented
#define  MRF24J_RFCTRL7   0x207 //  (Address 207h) READ
#define  MRF24J_RFCTRL8   0x208 //   ; unimplemented,unimplemented,unimplemented,RF_VCO,unimplemented,unimplemented,unimplemented,SleepClkOut
#define  MRF24J_SLPCAL0   0x209 //  (Address 209h) READ
#define  MRF24J_SLPCAL1   0x20A //  (Address 20Ah) READ
#define  MRF24J_SLPCAL2   0x20B //  (Address 20Bh) READ
//#define  MRF24J_RESERVED   0x20C //  (Address 20Ch) READ
//#define  MRF24J_RESERVED   0x20D //  (Address 20Dh) READ
//#define  MRF24J_RESERVED   0x20E //  (Address 20Eh) READ
#define  MRF24J_RFSTATE   0x20F //  (Address 20Fh) READ


#define  MRF24J_CLKINTCR  0x200  //  ; Unimplemented(bits7-2), IntEdge,/SleepClkEn
#define  MRF24J_RSSI      0x210  //  (Address 210 READ)  
#define  MRF24J_SLPCON0   0x211  //  (Address 211 READ)  
#define  MRF24J_SLPCON1   0x220  //  (Address 220 READ)
//#define  MRF24J_RESERVED   0x221 //  (Address 221h) READ
#define  MRF24J_WAKETIMEL   0x222  //  (Address 222 READ)
#define  MRF24J_WAKETIMEH   0x223  //  (Address 223 READ)
#define  MRF24J_REMCNTL   0x224  //  (Address 224 READ)
#define  MRF24J_REMCNTH   0x225  //  (Address 225 READ)
#define  MRF24J_MAINCNT0   0x226  //  (Address 226 READ)
#define  MRF24J_MAINCNT1   0x227  //  (Address 227 READ)
#define  MRF24J_MAINCNT2   0x228  //  (Address 228 READ)
#define  MRF24J_MAINCNT3   0x229  //  (Address 229 READ)
//#define  MRF24J_RESERVED   0x22A // 
//#define  MRF24J_RESERVED   0x22B // 
//#define  MRF24J_RESERVED   0x22C //  
//#define  MRF24J_RESERVED   0x22D //  
//#define  MRF24J_RESERVED   0x22E // 
#define  MRF24J_TESTMODE   0x22F  //  

#define  MRF24J_ASSOEADR0   0x230  //  
#define  MRF24J_ASSOEADR1   0x231  //  
#define  MRF24J_ASSOEADR2   0x232  //  
#define  MRF24J_ASSOEADR3   0x233  //  
#define  MRF24J_ASSOEADR4   0x234  //  
#define  MRF24J_ASSOEADR5   0x235  //  
#define  MRF24J_ASSOEADR6   0x236  //  
#define  MRF24J_ASSOEADR7   0x237  //  
#define  MRF24J_ASSOSADR0   0x238  //  
#define  MRF24J_ASSOSADR1   0x239  //  
//#define  MRF24J_RESERVED   0x23A // 
//#define  MRF24J_RESERVED   0x23B // 
//#define  MRF24J_UNIMPLEMENT   0x23C // 
//#define  MRF24J_UNIMPLEMENT   0x23D // 
//#define  MRF24J_UNIMPLEMENT   0x23E // 
//#define  MRF24J_UNIMPLEMENT   0x23F // 

#define  MRF24J_UPNONCE0   0x240 // 
#define  MRF24J_UPNONCE1   0x241 // 
#define  MRF24J_UPNONCE2   0x242 // 
#define  MRF24J_UPNONCE3   0x243 // 
#define  MRF24J_UPNONCE4   0x244 // 
#define  MRF24J_UPNONCE5   0x245 // 
#define  MRF24J_UPNONCE6   0x246 // 
#define  MRF24J_UPNONCE7   0x247 // 
#define  MRF24J_UPNONCE8   0x248 // 
#define  MRF24J_UPNONCE9   0x249 // 
#define  MRF24J_UPNONCE10   0x24A // 
#define  MRF24J_UPNONCE11   0x24B // 
#define  MRF24J_UPNONCE12   0x24C // 

// Long Address 000 - Transmit buffers
// Note that my routines need the upper byte to be non-zero in order 
// to treat an address as "LONG", so the addresses 000-0FF will be listed here with 
// an upper byte of 0x80 to make the value appear to be a long address, but the 
// upper byte automatically rotates out the MSB when the address is being written.

//  Normal message buffer
#define  MRF24J_TxNHeadLength 0x8000   //the length, in octets (bytes), of the MAC Header (MHR). In Unsecure mode, this field **is ignored**.
#define  MRF24J_TxNFrameLength 0x8001  //Frame length – Contains the length, in octets (bytes), of the MAC Header (MHR) AND data payload.  
// ****  Header – Contains the MAC Header (MHR).  ******
#define  MRF24J_TxNFrmCntrl 0x8002   //  Normal transmit buffer starting address (Frame Control).
#define  MRF24J_TxNFrmCntrl2 0x8003  //  Normal transmit buffer starting address (Frame Control).
//Bits: 0–2   3         4      5        6     7–9     10–11      12–13        1 4–15
//    Frame Security  Frame   Ack.    Intra Reserved  Dest.     Reserved      Source
//    type  enabled   pending request  PAN          addressing              addressing
#define MRF24J_TxNSeq       0x8004   //  Sequence number is after the 2 byte Frame Control.
#define MRF24J_TxN_PANIDL   0x8005
#define MRF24J_TxN_PANIDH   0x8006
#define MRF24J_TxN_DESTL    0x8007
#define MRF24J_TxN_DESTH    0x8008
#define MRF24J_TxN_SOURCEL  0x8009
#define MRF24J_TxN_SOURCEH  0x800A
#define MRF24J_TxN_PAYLOADSTART 0x800B

// ****
//  Beacon message buffer
#define  MRF24J_TxBLength_r 0x9000   //  BEACON transmit buffer starting address (length).
#define  MRF24J_TxBFrmCntrl_r 0x9020   //  BEACON transmit buffer starting address (Frame Control).

// ****
//  GTS1 message buffer
#define  MRF24J_TxG1Length_r  0xA000   //  GTS1 transmit buffer starting address (length).
#define  MRF24J_TxG1FrmCntrl_r 0xA020    //  GTS1 transmit buffer starting address (Frame Control).

// ****
//  GTS2 message buffer
#define  MRF24J_TxG2Length_r  0xB000   //  GTS2 transmit buffer starting address (length).
#define  MRF24J_TxG2FrmCntrl_r 0xB020    //  GTS2 transmit buffer starting address (Frame Control).

// ****
// When determining the value,  Bit0 must be 0 to read/1 to WRITE, and for bit 8 0=short addresses/1=Long address.  
//  Long Address control registers.
//  First bit is 1, next 10 are the address, 12th is 0=read/1=write
// *** NOTE *** long address requires 4 clock bits between address and Data.
//  This is accomplished by LEFT JUSTIFYING these constants.
//  Using "#define  MRF24J_RxNHeadLength" as an example....
  //  The register address listed in the spec is "300h"
  //      Which in Binary is           0011 0000 0000
  //  The ADDRESS is only 10 bits, but LONG addresses BEGIN with a 1, so left shift the number once, then SET the high bit.
  //        Register address shifted left once - 0110 0000 0000
  //        Forcing the first bit high becomes - 1110 0000 0000 
  //  Since this parameter is to be READ, 12th bit sent must be a 0.
  //            The new number becomes         - 1110 0000 0000 
  //  The lowest nibble of the lowest byte is the 4 Zeros required between address and data
  //            The new number becomes         - 1110 0000 0000 0000
  //  So the table entry is (converted back to Hex) -  E    0    0    0 
    //  If this were a WRITE parameter, it would be    -  E    0    1    0 
  //  A quick visual indication is if the number is EVEN, it is a read, but ODD numbers are WRITES.

//  Address 300h - receive Buffer   0011 0000 0000 -> 1110 0000 0000 ->E000
//#define  MRF24J_RxNHeadLength 0x300   //the length, in octets (bytes), of the MAC Header (MHR). In Unsecure mode, this field **is ignored**.
#define  MRF24J_RxNFrameLength 0x300  //Frame length – Contains the length, in octets (bytes), of the MAC Header (MHR) AND data payload.  
// ****  Header – Contains the MAC Header (MHR).  ******
#define  MRF24J_RxNFrmCntrl 0x301   //  Normal transmit buffer starting address (Frame Control).
#define  MRF24J_RxNFrmCntrl2 0x302  //  Normal transmit buffer starting address (Frame Control).
//Bits: 0–2   3         4      5        6     7–9     10–11      12–13        1 4–15
//    Frame Security  Frame   Ack.    Intra Reserved  Dest.     Reserved      Source
//    type  enabled   pending request  PAN          addressing              addressing
#define MRF24J_RxNSeq       0x303   //  Sequence number is after the 2 byte Frame Control.
#define MRF24J_RxN_PANIDL   0x304
#define MRF24J_RxN_PANIDH   0x305
#define MRF24J_RxN_DESTL    0x306
#define MRF24J_RxN_DESTH    0x307
#define MRF24J_RxN_SOURCEL  0x308
#define MRF24J_RxN_SOURCEH  0x309
#define MRF24J_RxN_PAYLOADSTART 0x30A

#endif
