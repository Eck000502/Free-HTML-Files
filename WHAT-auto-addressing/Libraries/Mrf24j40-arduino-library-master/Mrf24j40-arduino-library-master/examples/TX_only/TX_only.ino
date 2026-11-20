/**
 * Example code for using a microchip mrf24j40 module to send simple packets
 *
 * Requirements: 3 pins for spi, 3 pins for reset, chip select and interrupt
 * notifications
 * This example file is considered to be in the public domain
 * Originally written by Karl Palsson, karlp@tweak.net.au, March 2011
 */
#include <SPI.h>
#include <mrf24j_G.h>

const int pin_reset = 6;
const int pin_cs = 7; // default CS pin on ATmega8/168/328
const int pin_interrupt = 2; // default interrupt pin on ATmega8/168/328

//Mrf24j mrf(pin_reset, pin_cs, pin_interrupt);
Mrf24j mrf(pin_cs);

long last_time;
long tx_interval = 1000;

void setup() {
  Serial.begin(9600);
  
  mrf.reset();
  mrf.init();
  
  mrf.set_pan(0xcafe);
  // This is _our_ address
  mrf.address16_write(0x6001); 
  
  // uncomment if you want to enable PA/LNA external control
  //mrf.set_palna(true);

  //attachInterrupt(0, interrupt_routine, CHANGE); // interrupt 0 equivalent to pin 2(INT0) on ATmega8/168/328
  last_time = millis();
  //interrupts();
}

//void interrupt_routine() {
//    mrf.interrupt_handler(); // mrf24 object interrupt routine
//}

void loop() {
    mrf.check_flags(&handle_rx, &handle_tx);
    readRegisters();    // In this routine.  reads the raw registers of the chip to verify communications.
}

void handle_rx() {
    // data to receive, nothing to do
}

void handle_tx() {
    if (mrf.get_txinfo()->tx_ok) {
        Serial.println("TX went ok, got ack");
    } else {
        Serial.print("TX failed after ");Serial.print(mrf.get_txinfo()->retries);Serial.println(" retries\n");
    }
}

void readRegisters(void)    // In this routine.  reads the raw registers of the chip to verify communications.
{
  static long updateTime = 0;

  if(updateTime <= millis())
  {
   updateTime= millis() + 1000;
  // Reads the regsiters of the MRF24J
    Serial.print("PACON2 Register: ");Serial.println(mrf.read_short(MRF_PACON2));
    Serial.print("MRF_TXSTBL Register: ");Serial.println(mrf.read_short(MRF_TXSTBL));
    Serial.print("MRF_RFCON0 Register: ");Serial.println(mrf.read_short(MRF_RFCON0));
    Serial.print("MRF_RFCON1 Register: ");Serial.println(mrf.read_short(MRF_RFCON1));
    Serial.print("MRF_RFCON2 Register: ");Serial.println(mrf.read_short(MRF_RFCON2));
    Serial.print("MRF_RFCON6 Register: ");Serial.println(mrf.read_short(MRF_RFCON6));
    Serial.print("MRF_RFCON7 Register: ");Serial.println(mrf.read_short(MRF_RFCON7));
    Serial.print("MRF_RFCON8 Register: ");Serial.println(mrf.read_short(MRF_RFCON8));
    Serial.print("MRF_SLPCON1 Register: ");Serial.println(mrf.read_short(MRF_SLPCON1));
    //Serial.print("MRF_RFCON0 Register: ");Serial.println(mrf.read_short(MRF_RFCON0));
 
  }
}



