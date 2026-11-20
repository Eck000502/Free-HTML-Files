// ***************************
// TableOfContents:
// TOC Initialize the MRF24J
// TOC Reset the MRF24J
// TOC Get the PAN ID from the MRF24J
// TOC Set the PAN ID to the MRF24J
// TOC Set the source Address for this application
// TOC Get the source Address from this application
// TOC Get the channel for this application
// TOC Recieve MRF24J buffer
// TOC Transmit Message from buffer
// TOC Put new message into buffer
// TOC read from the buffer
// TOC write to the MRF24J
// Function to initialize the MRF24J physical layer.

//#define PHYSICAL_MRF24J
//#include "z_WHAT_mrf24j.ino"	// Physical layer routines.
#include  "z_WHAT_mrf24j.h"
// TOC Initialize the MRF24J
// *************************************************************************
// *************************************************************************
// TOC Initialize the MRF24J
void Mrf24j_begin (uint16_t _myPANID, uint16_t _mySourceAddress)
{
  pinMode(WHAT_CHIP_SELECT, OUTPUT);  // Chip Select pin is defined in WHAT.h
  //pinMode(_pin_reset, OUTPUT);
  //pinMode(_pin_int, INPUT);

  SPI.setBitOrder(MSBFIRST) ;
  SPI.setDataMode(SPI_MODE0);
  SPI.begin();

  Mrf24j_reset();           // Copied from mff24 library to this routine
  WHAT_set_pan(_myPANID);  // Copied from mff24 library to this routine
  // This is _our_ address
  WHAT_set_SourceAddress(_mySourceAddress);  // Copied from mff24 library to this routine
  //Mrf24j_rx_flush();
  // uncomment if you want to receive any packet on this channel
  Mrf24j_set_promiscuous(true);
  // uncomment if you want to enable PA/LNA external control
  //Mrf24j_set_palna(true);
  // uncomment if you want to buffer all PHY Payload
  //Mrf24j_set_bufferPHY(true);

}
void Mrf24j_Loop(void)
{
    RawMessageBuffer(); // see if any new messages have arrived.
    Tx_What_Que();      // Transmit any pending messages
}


#include <avr/pgmspace.h>

// Define a structure to hold the register mapping
struct RegisterConfig {
    uint8_t reg;
    uint8_t val;
};

// Store the configuration array entirely in Flash memory (PROGMEM)
const RegisterConfig initConfigs[] PROGMEM = {
    {MRF24J_PACON2,   0x98},
    {MRF24J_TXPEMISP, 0x95},
    {MRF24J_RFCTRL1,  0x01},
    {MRF24J_RFCTRL2,  0x80},
    {MRF24J_RFCTRL3,  0x90},
    {MRF24J_RFCTRL7,  0x80},
    {MRF24J_RFCTRL8,  0x10},
    {MRF24J_SLPCON1,  0x21},
    {MRF24J_BBREG2,   0xC6},
    // --- BREAK POINT 1: Mrf24j_set_channel(12) runs here dynamically ---
    {MRF24J_RFCTL,    0x04},
    {MRF24J_RFCTL,    0x00},
    // --- BREAK POINT 2: delay(1) runs here dynamically ---
    {MRF24J_RXFLUSH,  0x61},
    {MRF24J_TRISGPIO, 0b11110001},
    {MRF24J_GPIO,     MRF24J_GPIO_Rx},
    {MRF24J_TESTMODE, 0b00001111},
    {MRF24J_TRISGPIO, 0b11111111},
    {MRF24J_TESTMODE, 0b00001000},
    {MRF24J_PACON2,   0b00001000},
    {MRF24J_SADRL,    0xFF},
    {MRF24J_SADRH,    0xFF},
    {MRF24J_PANIDL,   0xFF},
    {MRF24J_PANIDH,   0xFF},
    {MRF24J_EADR0,    0xAA},
    {MRF24J_EADR1,    0xAA},
    {MRF24J_EADR2,    0xAA},
    {MRF24J_EADR3,    0xAA},
    {MRF24J_EADR4,    0xAA},
    {MRF24J_EADR5,    0xAA},
    {MRF24J_EADR6,    0xAA},
    {MRF24J_EADR7,    0xAA},
    {MRF24J_PACON2,   0x98},
    {MRF24J_RFCTRL1,  0x01},
    {MRF24J_RFCTRL2,  0x80},
    {MRF24J_RFCTRL3,  0x00},
    {MRF24J_RFCTRL6,  0x90},
    {MRF24J_RFCTRL7,  0x80},
    {MRF24J_RFCTRL8,  0x10},
    {MRF24J_TXPEMISP, 0x95},
    {MRF24J_BBREG2,   0x80},
    {MRF24J_CCAEDTH,  0x60},
    {MRF24J_BBREG6,   0x40},
    {MRF24J_RXMCR,    0x01},
    {MRF24J_ACKTMOUT, 0xB9},
    {MRF24J_TXPEMISP, 0x95},
    {MRF24J_SLPCON1,  0x01},
    {MRF24J_SLPACK,   0x5F},
    {MRF24J_RFCTL,    0x04},
    {MRF24J_RFCTL,    0x00},
    {MRF24J_ORDER,    0xFF}
    // --- BREAK POINT 3: delay(2) runs here dynamically ---
};

// Helper macro to calculate total items in flash array
#define CONFIG_COUNT (sizeof(initConfigs) / sizeof(RegisterConfig))

void Mrf24j_reset(void) {
    bool allZeros = true;
    bool allFFs = true;

    for (size_t i = 0; i < CONFIG_COUNT; i++) {
        // Intercept loops to execute middle-routine steps exactly where they belong
        if (i == 9) { 
            Mrf24j_set_channel(12); 
        }
        else if (i == 11) { 
            delay(1); 
        }

        // Fetch register address and target data from Flash
        uint8_t targetReg = pgm_read_byte(&(initConfigs[i].reg));
        uint8_t targetVal = pgm_read_byte(&(initConfigs[i].val));

        // Write to the register
        Mrf24j_Write(targetReg, targetVal);

        // Immediate Read-Back Verification
        uint8_t readVal = Mrf24j_Read(targetReg);
        
        // Track if everything read so far is 0x00 or 0xFF
        if (readVal != 0x00) allZeros = false;
        if (readVal != 0xFF) allFFs = false;
    }

    // Final delay from your original routine
    delay(2);

    // Evaluate communication status
    if (allZeros || allFFs) {
        Serial.println(F("No communication with MRF24J - check Chip Select"));
        
        // Flush serial buffer to ensure the message transmits before stopping
        Serial.flush(); 
        
        // Infinite loop to stop all further program execution
        while (1) {
            delay(100); // Small delay to keep the watchdog timer happy if enabled
        }
    } else {
        Serial.println(F("Mrf24J registers initialized"));
    }
}

// *************************************************************************

uint16_t Mrf24j_get_pan(void)
{
  uint16_t _ret =  Mrf24j_Read(MRF24J_PANIDH) * 0x100; // get the high byte and shift it into the upper byte of the return value
  _ret = _ret + Mrf24j_Read(MRF24J_PANIDL); // add on the low byte.
  return _ret;
}
// *************************************************************************

void WHAT_set_pan(uint16_t _panid)
{
  Mrf24j_Write(MRF24J_PANIDH, highByte(_panid));
  Mrf24j_Write(MRF24J_PANIDL, lowByte(_panid));
}
// *************************************************************************
void   WHAT_set_SourceAddress(uint16_t _mySourceAddress)  // Copied from mff24 library to this routine
{
  Mrf24j_Write(MRF24J_SADRH , highByte(_mySourceAddress));
  Mrf24j_Write(MRF24J_SADRL , lowByte(_mySourceAddress));
}
uint16_t WHAT_get_SourceAddress(void)  // Copied from mff24 library to this routine
{
  return Mrf24j_Read(MRF24J_SADRH) * 0x100 + Mrf24j_Read(MRF24J_SADRL);
}

// *************************************************************************
void Mrf24j_set_promiscuous(boolean _enabled)
{
  if (_enabled)
  {
    Mrf24j_Write(MRF24J_RXMCR, 0x01);
  } else
  {
    Mrf24j_Write(MRF24J_RXMCR, 0x00);
  }
}
// *************************************************************************

void Mrf24j_set_channel(uint8_t _channel)
{
  Mrf24j_Write(MRF24J_RFCTRL0, (((_channel - 11) << 4) | 0x03));
}
// *************************************************************************

void Mrf24j_set_palna(bool _enabled)
{
  if (_enabled)
  {
    Mrf24j_Write(MRF24J_TESTMODE, 0x07); // Enable PA/LNA on MRF24J40MB module.
  } else
  {
    Mrf24j_Write(MRF24J_TESTMODE, 0x00); // Disable PA/LNA on MRF24J40MB module.
  }
}
// *************************************************************************
// *************************************************************************
// TOC Recieve MRF24J buffer
bool WHAT_Buffer_RX( WHAT_Packet & _ptr_requested_msg, uint8_t _this_index) 
{
  /*
    This section handles the call of this routine by a consumer of the data.
    The consumer sends a "WHAT_Packet" pointer and _this_index.
    IF _this_index is different than GBL_rx_index, this section fills WHAT_Packet with WHAT_Rx_Message[_this_index] 
    recieved buffer message, and returns TRUE. otherwise it returns false and does not modify WHAT_Packet
    It is up to the consumer to verify which messages they have already consumed.
  */
    if(GBL_rx_index != _this_index) {
        // FIX: Dereference pointer to copy actual structure data back to caller
        _ptr_requested_msg = WHAT_Buff_Message[_this_index]; 
        return true;
    } 
    else
    {
        return false;
    }

}
    // Ensure naming consistency for buffer limits

void RawMessageBuffer(void)
{
    //uint8_t _mrf_intstat;
   //_mrf_intstat = Mrf24j_Read(MRF24J_INTSTAT);
    if (Mrf24j_Read(MRF24J_INTSTAT) & MASK_RXIF ) 
    {
      // #ifdef DX_ONLY
      //     Serial.print("GBL_rx_index: ");Serial.println(GBL_rx_index);
      //     Serial.print("_this_index: ");Serial.println(_this_index);
      // #endif
        // Read header
        WHAT_Buff_Message[GBL_rx_index].frame_length = Mrf24j_Read(MRF24J_RxNFrameLength);
        WHAT_Buff_Message[GBL_rx_index].frame_control = Mrf24j_Read(MRF24J_RxNFrmCntrl) + (Mrf24j_Read(MRF24J_RxNFrmCntrl2) * 0x100);
        WHAT_Buff_Message[GBL_rx_index].pan_id = Mrf24j_Read(MRF24J_RxN_PANIDL) + Mrf24j_Read(MRF24J_RxN_PANIDH) * 0x100;
        WHAT_Buff_Message[GBL_rx_index].destination = Mrf24j_Read(MRF24J_RxN_DESTL) + Mrf24j_Read(MRF24J_RxN_DESTH) * 0x100;
        WHAT_Buff_Message[GBL_rx_index].sourceaddress = Mrf24j_Read(MRF24J_RxN_SOURCEL) + Mrf24j_Read(MRF24J_RxN_SOURCEH) * 0x100;

        // #ifdef DX_ONLY
        // Serial.println(F("Length\tCONTROL\tRx pan_id\tdestination\tsource"));
        // Serial.print(WHAT_Buff_Message[GBL_rx_index].frame_length,DEC);Serial.print("\t  ");
        // Serial.print(WHAT_Buff_Message[GBL_rx_index].frame_control,HEX);Serial.print("\t  ");
        // Serial.print(WHAT_Buff_Message[GBL_rx_index].pan_id,HEX);Serial.print("\t\t  ");
        // Serial.print(WHAT_Buff_Message[GBL_rx_index].destination,HEX);Serial.print("\t  ");
        // Serial.println(WHAT_Buff_Message[GBL_rx_index].sourceaddress,HEX);
        // #endif
    
    // read the payload
      // #ifdef DX_ONLY
      //   Serial.print(F("Adr:"));Serial.print(WHAT_ADR_PAYLOADSTART,HEX);Serial.print(F(":"));
      // #endif
        // Load the payload safely
        uint8_t _payloadSize = Mrf24j_Read(WHAT_ADR_PAYLOADSTART) - 1;
        
        // Target structural size check (Replace MAX_PAYLOAD_SIZE with your actual struct array size)
        if (_payloadSize >= NUM_PAYLOAD_BYTES) {
            _payloadSize = NUM_PAYLOAD_BYTES - 1; 
        }

        for (uint8_t _i = 0; _i <= _payloadSize; _i++) {
            WHAT_Buff_Message[GBL_rx_index].payload[_i] = Mrf24j_Read(WHAT_ADR_PAYLOADSTART + _i);
         }

        WHAT_Buff_Message[GBL_rx_index].lqi = Mrf24j_Read(WHAT_ADR_PAYLOADSTART + _payloadSize + 3);
        WHAT_Buff_Message[GBL_rx_index].rssi = Mrf24j_Read(WHAT_ADR_PAYLOADSTART + _payloadSize + 4);

    // Extra bytes are added to the FIFO.
      // #ifdef DX_ONLY
      //   Serial.print("FCS: ");Serial.print(Mrf24j_Read(WHAT_ADR_PAYLOADSTART + _payloadSize+2),HEX);Serial.print(Mrf24j_Read(WHAT_ADR_PAYLOADSTART + _payloadSize+1),HEX);
      //    Serial.print("\tlqi: ");Serial.print(WHAT_Buff_Message[GBL_rx_index].lqi,HEX);
      //   Serial.print("\trssi: ");Serial.println(WHAT_Buff_Message[GBL_rx_index].rssi,HEX);
      //   Serial.print("\n");
      // #endif
 
       GBL_rx_index++;
        //Serial.print("GBL_rx_index: ");Serial.println(GBL_rx_index);
        // FIX: Enforce proper zero-indexed circular buffer bounds
        if(GBL_rx_index >= RX_BUFFER_DEPTH )
        { 
            GBL_rx_index = 0; 
        }
    }
    else 
    {
      //Serial.print("0");
      //return false;   // If this is enabled, if a NEW message did not arrive, the index will not be checked.
    }
}
 

// *************************************************************************
// *************************************************************************
// TOC Transmit Message from buffer
void Tx_What_Que(void)  // If more that one message is being send, this will send the next message once the previous message is gone.
{ // Checks to see if a message is waiting to be placed in the transmit queue
    //Serial.print(F("Pend/send "));Serial.print(TxQueue_PendIndex);Serial.println(" / ");Serial.print(TxQueue_SentIndex);

  if(TxQueue_PendIndex != TxQueue_SentIndex)
  {
    if(!(Mrf24j_Read(MRF24J_TXNCON)& MRF24J_TXNTRIG))  // If the transmit bit has cleared...
    {
      // Header length is primarily derermined from the Frame Control, which
      // decides if PANID, SOURCE,and DESTINATION are included in the header.
      TxQueue[TxQueue_SentIndex].frame_length = TxQueue[TxQueue_SentIndex].header_length + TxQueue[TxQueue_SentIndex].payload[0];
      Mrf24j_Write(MRF24J_TxNFrameLength, TxQueue[TxQueue_SentIndex].frame_length);
      Mrf24j_Write(MRF24J_TxNHeadLength, TxQueue[TxQueue_SentIndex].header_length);
      Mrf24j_Write(MRF24J_TxNFrmCntrl, lowByte(TxQueue[TxQueue_SentIndex].frame_control));
      Mrf24j_Write(MRF24J_TxNFrmCntrl2, highByte(TxQueue[TxQueue_SentIndex].frame_control));
      Mrf24j_Write(MRF24J_TxNSeq, TxQueue[TxQueue_SentIndex].sequence_number);
      //Mrf24j_Write(MRF24J_TxNFrmCntrl, 0b01100001);
      //Mrf24j_Write(MRF24J_TxNFrmCntrl2, 0b10001000);
      //Mrf24j_Write(MRF24J_TxNSeq, 1);
      Mrf24j_Write(MRF24J_TxN_PANIDL, lowByte(TxQueue[TxQueue_SentIndex].pan_id));
      Mrf24j_Write(MRF24J_TxN_PANIDH, highByte(TxQueue[TxQueue_SentIndex].pan_id));
      Mrf24j_Write(MRF24J_TxN_DESTL, lowByte(TxQueue[TxQueue_SentIndex].destination));
      Mrf24j_Write(MRF24J_TxN_DESTH, highByte(TxQueue[TxQueue_SentIndex].destination));
      Mrf24j_Write(MRF24J_TxN_SOURCEL, lowByte(TxQueue[TxQueue_SentIndex].sourceaddress));
      Mrf24j_Write(MRF24J_TxN_SOURCEH, highByte(TxQueue[TxQueue_SentIndex].sourceaddress));
    
      /*
        Serial.print("Header Length (TX Buf 0): ");Serial.println(Mrf24j_Read(MRF24J_TxNHeadLength),HEX);
        Serial.print("\tPacket Length (TX Buf 1): ");Serial.println(Mrf24j_Read(MRF24J_TxNFrameLength),HEX);
        Serial.print("\tFrame Control Low byte (TX Buf 2): ");Serial.println(Mrf24j_Read(MRF24J_TxNFrmCntrl),HEX);
        Serial.print("\tFrame Control High byte (TX Buf 3): ");Serial.println(Mrf24j_Read(MRF24J_TxNFrmCntrl2),HEX);
        Serial.print("\tSequence number (TX Buf 4): ");Serial.println(Mrf24j_Read(MRF24J_TxNSeq),HEX);
        Serial.print("\tPANID (TX Buf 5,6): ");Serial.print(Mrf24j_Read(MRF24J_TxN_PANIDH),HEX);Serial.println(Mrf24j_Read(MRF24J_TxN_PANIDL),HEX);
        Serial.print("\tDestination (TX Buf 7,8): ");Serial.print(Mrf24j_Read(MRF24J_TxN_DESTH),HEX);Serial.println(Mrf24j_Read(MRF24J_TxN_DESTL),HEX);
        Serial.print("\tSource (TX Buf 9/A): ");Serial.print(Mrf24j_Read(MRF24J_TxN_SOURCEH),HEX);Serial.println(Mrf24j_Read(MRF24J_TxN_SOURCEL),HEX);
        Serial.print("\t\tPayloadSize (TX Buf B): ");Serial.println(Mrf24j_Read(MRF24J_TxN_PAYLOADSTART),HEX);
        Serial.print("\t\tPayloadByte1 (TX Buf C): ");Serial.println(Mrf24j_Read(MRF24J_TxN_PAYLOADSTART+1),HEX);
    
    
        Serial.print("\n");
        Serial.print("Header Length (Packet): ");Serial.println(TxQueue[TxQueue_SentIndex].header_length,HEX);
        Serial.print("\tPacket Length (Packet): ");Serial.println(TxQueue[TxQueue_SentIndex].frame_length,HEX);
        Serial.print("\tFrame Control (Packet): ");Serial.println(TxQueue[TxQueue_SentIndex].frame_control,HEX);
        Serial.print("\tDestination (Packet): ");Serial.println(TxQueue[TxQueue_SentIndex].destination,HEX);
        Serial.print("\tPANID (Packet): ");Serial.println(TxQueue[TxQueue_SentIndex].pan_id,HEX);
        Serial.print("\tSource (Packet): ");Serial.println(TxQueue[TxQueue_SentIndex].sourceaddress,HEX);
      */
      // Now load the payload:
      uint8_t _payloadSize = TxQueue[TxQueue_SentIndex].payload[0];  //User must supply payload length in the first byte of the array.
      Mrf24j_Write(MRF24J_TxN_PAYLOADSTART, _payloadSize);
      for (uint8_t _i = 1; _i <= _payloadSize; _i++)
      {
        Mrf24j_Write(MRF24J_TxN_PAYLOADSTART + _i, TxQueue[TxQueue_SentIndex].payload[_i]);
        // Serial.println(TxQueue[TxQueue_SentIndex].payload[_i]);
      }
    
      Mrf24j_Write(MRF24J_TXNCON, MRF24J_TXNTRIG);  // Transmit the packet
      //Serial.println(Mrf24j_Read(MRF24J_TXSTAT),BIN);
      TxQueue_SentIndex++; if(TxQueue_SentIndex > TX_BUFFER_DEPTH) TxQueue_SentIndex=0;
      #ifdef TX_MONITOR
      Serial.print("Tx ");Serial.println(TxQueue_SentIndex);
      #endif
    }
  }
}

// *************************************************************************
// *************************************************************************
uint8_t Mrf24j_Read(uint16_t _address)
{
  uint8_t _ret;

  digitalWrite(WHAT_CHIP_SELECT, LOW);  // Activate the chip

  // Determine if the address is a "short" or "Long" address:
  // If the high byte is 0, it is a short address
  if (highByte(_address) == 0)
  {
    // This is a SHORT READ.
    // When determining the address,  Bit0 must be 0 to read(1 to WRITE), and for bit 8 0=short addresses(1=Long address).
    SPI.transfer((lowByte(_address) << 1) & 0b01111110);
    _ret = SPI.transfer(0x00);    // read the data byte (by writing a byte, since the Arduino is the SPI master device responsible for clocking)
    digitalWrite(WHAT_CHIP_SELECT, HIGH); // de-select the chip select.
    return _ret; // send back the data that was read from the register.
  }
  else
  {
    // If the high byte is NOT ZERO, it is a long address
    //  Long Addresses use 3 total bytes:
    //  most significant bit is 1, next 10 are the address, 12th is 0=read/1=write, 4 "clock" bits, 1 byte data
    // *** NOTE *** long address requires 4 "clock bits" between address and Data.
    //  This is accomplished by LEFT JUSTIFYING the address.
    //  Using "#define  MRF24J_SLPCON0" (Address 211)as an example....
    //  The register address listed in the spec is "211h"
    //      Which in Binary is           0000 0010 0001 0001
    //  The ADDRESS is only 10 bits, but LONG addresses BEGIN with a 1, so left shift the number 5 times, then SET the high bit.
    //        Register address shifted left 5 times - 0100 0010 0010 0000
    //        Forcing the first bit high becomes    - 1100 0010 0010 0000
    //  If this parameter is to be WRITTEN, 12th bit sent must be a 1. (OR with 0x10)
    //            The new number becomes         -  1100 0010 0011 0000
    //  So the table entry is (converted back to Hex) -  C    2    3    0
    //  If this were a READ parameter, it would be    -  C    2    2    0
    //  A quick visual indication is if the number is EVEN, it is a read, but ODD numbers are WRITES.
    _address = _address << 5;  // the raw address is only 10 bits.  rotating it left 4 spots leaves the MSB (long/short indicator) as 0, and the top 7 bits in the high byte of the address.
    _address |= 0x8000;   // Set the "Long" address bit and leave the "write" bit clear.
    SPI.transfer(highByte(_address));  // set the high bit to indicate "long address" that was previously Zeroed out, and transfer the address
    SPI.transfer(lowByte(_address));  //  use 0x10 to set(for writing) the read/write bit of the address and transfer the rest of the address.
    _ret = SPI.transfer(0x00);    // read the data byte (by writing a byte, since the Arduino is the SPI master device responsible for clocking)
    digitalWrite(WHAT_CHIP_SELECT, HIGH); // de-select the chip select.
    return _ret; // send back the data that was read from the register.
  }
}
// *************************************************************************

void Mrf24j_Write(uint16_t _address, uint8_t _data)
{
  pinMode(WHAT_CHIP_SELECT, OUTPUT);
  digitalWrite(WHAT_CHIP_SELECT, LOW);  // Activate the chip

  // Determine if the address is a "short" or "Long" address:
  // If the high byte is 0, it is a short address
  if (highByte(_address) == 0)
  {
    // This is a SHORT write.
    // When determining the address,  Bit0 must be 0 to read/1 to WRITE, and for bit 8 0=short addresses/1=Long address.
    //Serial.print("Short ");Serial.print(_address,HEX);Serial.print("\t Data: ");Serial.println(_data,HEX);
    //Serial.print("\tRaw Short: ");Serial.println(_address,HEX);
    //_address = (lowByte(_address)<<1);
    //Serial.print("\t<<1 : ");Serial.println(_address,HEX);
    //_address = (_address & 0b01111110);
    //Serial.print("\t & 0x7E : ");Serial.println(_address,HEX);
    //_address = (_address | 0b00000001);
    //Serial.print("\t | 1 : ");Serial.println(_address,HEX);
    _address = (_address << 1 & 0b01111110) | 0b00000001;
    SPI.transfer(_address);
    SPI.transfer(_data);  // Write the data byte.
    //Serial.print("\t Send ");Serial.print(_address,HEX);Serial.print("\t Data: ");Serial.println(_data,HEX);
  }
  else
  {
    // If the high byte is NOT ZERO, it is a long address
    //  Long Addresses use 3 total bytes:
    //  most significant bit is 1, next 10 are the address, 12th is 0=read/1=write, 4 "clock" bits, 1 byte data
    // *** NOTE *** long address requires 4 "clock bits" between address and Data.
    //  This is accomplished by LEFT JUSTIFYING the address.
    //  Using "#define  MRF24J_SLPCON0" (Address 211)as an example....
    //  The register address listed in the spec is "211h"
    //      Which in Binary is           0000 0010 0001 0001
    //  The ADDRESS is only 10 bits, but LONG addresses BEGIN with a 1, so left shift the number 5 times, then SET the high bit.
    //        Register address shifted left 5 times - 0100 0010 0010 0000
    //        Forcing the first bit high becomes    - 1100 0010 0010 0000
    //  If this parameter is to be WRITTEN, 12th bit sent must be a 1. (OR with 0x10)
    //            The new number becomes         -  1100 0010 0011 0000
    //  So the table entry is (converted back to Hex) -  C    2    3    0
    //  If this were a READ parameter, it would be    -  C    2    2    0
    //  A quick visual indication is if the number is EVEN, it is a read, but ODD numbers are WRITES.
    //Serial.print(" long _address : ");Serial.print(_address,HEX);Serial.print("\Data: ");Serial.println(_data,HEX);
    _address = _address << 5;  // the raw address is only 10 bits.  rotating it left 1 spots leaves the MSB (long/short indicator) as 0, and the top 7 bits in the high byte of the address.
    _address |= 0x8010;   // Set the "Long" address bit and SET the "write" bit.
    SPI.transfer(highByte(_address));  // set the high bit to indicate "long address" that was previously Zeroed out, and transfer the address
    SPI.transfer(lowByte(_address));  //  use 0x10 to set(for writing) the read/write bit of the address and transfer the rest of the address.
    SPI.transfer(_data);         // and finally the data that is being sent to that address.
    //Serial.print("_address ");Serial.println(_address,HEX);
  }
  digitalWrite(WHAT_CHIP_SELECT, HIGH); // de-select the chip select.
}


// Function to initialize the MRF24J physical layer.
//void Mrf24j_init() {
    // This function can be used to encapsulate the physical layer initialization
    // currently handled by Mrf24j_begin.
    // It is primarily a placeholder for abstraction.
    // Actual initialization with PANID and Source Address will still happen via Mrf24j_begin.
//}