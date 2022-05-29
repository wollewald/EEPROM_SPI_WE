/*******************************************************************************
* This is an example sketch for the EEPROM_SPI_WE library
*
* It shows how to write data continuously and how to use the hold pin. 
*  
* Written by Wolfgang (Wolle) Ewald.
*
* Here you find the detailed documentation:
* https://wolles-elektronikkiste.de/en/eeprom-part-3-external-spi-eeproms   (English)
* https://wolles-elektronikkiste.de/eeprom-teil-3-externe-spi-eeproms       (German)
*
********************************************************************************/

#include <SPI.h>
#include <EEPROM_SPI_WE.h>
const int csPin = 10; // Chip select pin
const int wpPin = 9;  // Write protect pin (optional)
const int holdPin = 5; // Hold pin, pauses SPI transaction if LOW

EEPROM_SPI_WE myEEP = EEPROM_SPI_WE(csPin, wpPin);

void setup(){
  Serial.begin(9600);
  pinMode(holdPin,OUTPUT);
  digitalWrite(holdPin, HIGH);
  if(myEEP.init()){
    Serial.println("EEPROM connected");
  }
  else{
    Serial.println("EEPROM does not respond");
    while(1);
  }
  /* You can change the SPI clock speed. The default of is 8 MHz */
  //myEEP.setSPIClockSpeed(4000000); // use AFTER init()!
  
  /* Select the page size of your EEPROM.
   * Choose EEPROM_PAGE_SIZE_xxx,  
   * with xxx = 16, 32, 64, 128 or 256
   */
  myEEP.setPageSize(EEPROM_PAGE_SIZE_128);

  unsigned int address = 0;
  unsigned long writeDuration = 0;
  unsigned long startTime = millis();

  /* Conventional way of writing: 
   * For each call of put() you have to wait the write cycle 
   * time (~4 ms).
   */
  myEEP.put(125,0);
  myEEP.put(127, 1111);
  myEEP.put(129, 2222);
  myEEP.put(131, 3333);
  myEEP.put(133, 4444);
  myEEP.put(135, 5555);
  myEEP.put(137, 6666);
  myEEP.put(139, 7777);
  myEEP.put(141, 8888);
  myEEP.put(143, 9999);
  writeDuration = millis() - startTime;
  Serial.print("Time needed for discontinuous writing [ms]: ");
  Serial.println(writeDuration); 
  
  /* Continuous writing:  
   * You only need to wait the write cycle time when a page is 
   * completed and when calling continuousPutDisable().
   * In this example the page is completed after writing to address 127.
   */
  address = 125;
  startTime = millis();
  myEEP.continuousPutEnable(address); // start address
  myEEP.continuousPut(9999);
  myEEP.continuousPut(8888);
  myEEP.continuousPut(7777);
  myEEP.continuousPut(6666);
  myEEP.continuousPut(5555);
  myEEP.continuousPut(4444);
  myEEP.continuousPut(3333);
  myEEP.continuousPut(2222);
  myEEP.continuousPut(1111);
  myEEP.continuousPut(0);
  myEEP.continuousPutDisable();
  writeDuration = millis() - startTime;
  
  Serial.print("Time needed for continuous writing incl. page change [ms]: ");
  Serial.println(writeDuration);
  /* Just to check that writing worked fine: */
  int intToRead;
  address = 125;
  for(unsigned int i=0; i<10; i++){
    myEEP.get(address, intToRead);
    address += sizeof(int);
    Serial.println(intToRead);
  }
 

  /* Another continuous writing:  
   * Here writing starts at address 0. If your page size is >= 32 you will write
   * everything into one page. So, only one write cycle waiting time is needed at 
   * the end. 
   */
  address = 0;
  startTime = millis();
  myEEP.continuousPutEnable(address);
  myEEP.continuousPut(0);
  myEEP.continuousPut(1111);
  myEEP.continuousPut(2222);
  myEEP.continuousPut(3333);
  myEEP.continuousPut(4444);
  myEEP.continuousPut(5555);
  myEEP.continuousPut(6666);
  myEEP.continuousPut(7777);
  myEEP.continuousPut(8888);
  myEEP.continuousPut(9999);
  myEEP.continuousPutDisable();
  writeDuration = millis() - startTime;
  Serial.print("Time needed for continuous writing without page change [ms]: ");
  Serial.println(writeDuration); 

  /* The next example shows how to use the hold pin */ 
  address = 0;
  myEEP.continuousPutEnable(address);
  myEEP.continuousPut(999);
  myEEP.continuousPut(888);
  myEEP.continuousPut(777);
  myEEP.continuousPut(666);
  digitalWrite(holdPin,LOW);
  /* With the hold pin low you can now use the SPI lines to control other 
     devices. The current SPI transaction with the EEPOM is just paused 
     (CS pin is still LOW). After the hold Pin is back to HIGH you can continue 
     the former transaction.
   */
  digitalWrite(holdPin,HIGH);
  myEEP.continuousPut(555);
  myEEP.continuousPut(444);
  myEEP.continuousPutDisable();
  /* Just to show that writing worked fine: */
  address = 0;
  for(unsigned int i=0; i<10; i++){
    myEEP.get(address, intToRead);
    address += sizeof(int);
    Serial.println(intToRead);
  }  
}
  
void loop(){}
