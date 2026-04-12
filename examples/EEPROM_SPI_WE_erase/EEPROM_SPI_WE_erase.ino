/******************************************************************************
* This is an example sketch for the EEPROM_SPI_WE library.
*
* Here you learn the different how to erase pages, sectors or the
* entire EEPROM.
* 
* THESE FUNCTIONS ARE ONLY AVAILABLE ON BIGGER EEPROMS. Check the
* data sheet!
* 
* Written by Wolfgang (Wolle) Ewald.
*
* Here you find the detailed documentation:
* https://wolles-elektronikkiste.de/en/eeprom-part-3-external-spi-eeproms   (English)
* https://wolles-elektronikkiste.de/eeprom-teil-3-externe-spi-eeproms       (German)
*
*******************************************************************************/

#include <SPI.h>
#include <EEPROM_SPI_WE.h>
const int csPin = 10; // Chip select pin
const int wpPin = 9;  // Write protect pin (optional)

/* There are different options to create your EEPROM_SPI_WE object:
 * EEPROM_SPI_WE myEEP = EEPROM_SPI_WE(csPin, wpPin, spiClockSpeed); // e.g. uint32t spiClockSpeed = 4000000
 * EEPROM_SPI_WE myEEP = EEPROM_SPI_WE(csPin, wpPin);
 * EEPROM_SPI_WE myEEP = EEPROM_SPI_WE(csPin); 
 * EEPROM_SPI_WE myEEP = EEPROM_SPI_WE(&SPI, csPin, wpPin, spiClockSpeed); 
 * EEPROM_SPI_WE myEEP = EEPROM_SPI_WE(&SPI, csPin, wpPin);  
 * EEPROM_SPI_WE myEEP = EEPROM_SPI_WE(&SPI, csPin);  
 * If you don't define the wpPin and you connect it to GND or VCC,  
 * then protectStatusRegister() is the only function that won't work.
 * Passing the SPI object allows you, for example, to use both SPI 
 * interfaces on the ESP32.
 */
  EEPROM_SPI_WE myEEP = EEPROM_SPI_WE(csPin, wpPin);

void setup(){
  Serial.begin(9600);
  if(myEEP.init()){
    Serial.println("EEPROM connected");
  }
  else{
    Serial.println("EEPROM does not respond");
    while(1);
  }
   /* You can change the SPI clock speed also after you have created your 
   * object. The default is 8 MHz. Check the data sheet which clock speed is 
   * allowed. The frequency limit migh also depend on the voltage. 
   */
  //myEEP.setSPIClockSpeed(4000000); // use AFTER init()!
  
  /* Select the page size of your EEPROM.
   * Choose EEPROM_PAGE_SIZE_xxx,  
   * with xxx = 16, 32, 64, 128 or 256
   */
  myEEP.setPageSize(EEPROM_PAGE_SIZE_128);

  /* Select the memory size of your EEPROM. This is important in case you use 
   * EEPROMs > 512 kBit or in case you want to erase a complete small EEPROM 
   * (typ. < 8 kBits). You can choose from:
   * EEPROM_KBITS_2 = 256 bytes,
   * EEPROM_KBITS_4 = 512 bytes,
   * EEPROM_KBITS_8 = 1024 bytes,
   * EEPROM_KBITS_16 = 2028 bytes,
   * EEPROM_KBITS_32 = 4096 bytes,
   * EEPROM_KBITS_64 = 8192 bytes,
   * EEPROM_KBITS_128 = 16384 bytes,
   * EEPROM_KBITS_256 = 32768 bytes,
   * EEPROM_KBITS_512 = 65536 bytes,
   * EEPROM_KBITS_1024 = 131072 bytes,
   * EEPROM_KBITS_2048 = 262144 bytes,
   * EEPROM_KBITS_4096 = 524288 bytes,
   */
  //myEEP.setMemorySize(EEPROM_KBITS_1024);

  int testInt = 42;
  myEEP.put(10, testInt);
  myEEP.get(10, testInt);
  Serial.print("testInt: ");
  Serial.println(testInt);
  Serial.println("Erasing page....");
  myEEP.erasePage(3);  // choose any address within the page you want to erase
  myEEP.get(10, testInt);
  Serial.print("testInt: ");
  Serial.println(testInt);
  Serial.println("");

  testInt = 42;
  myEEP.put(10, testInt);
  myEEP.get(10, testInt);
  Serial.print("testInt: ");
  Serial.println(testInt);
  Serial.println("Erasing sector....");
  myEEP.eraseSector(7); // choose any address within the sector you want to erase
  myEEP.get(10, testInt);
  Serial.print("testInt: ");
  Serial.println(testInt);
  Serial.println("");
  
  testInt = 42;
  myEEP.put(10, testInt);
  myEEP.get(10, testInt);
  Serial.print("testInt: ");
  Serial.println(testInt);
  Serial.println("Erasing complete EEPROM....");
  myEEP.eraseCompleteEEPROM();
  myEEP.get(10, testInt);
  Serial.print("testInt: ");
  Serial.println(testInt);

  Serial.print("testInt, MSB: ");
  Serial.println(myEEP.read(10), BIN);
  Serial.print("testInt, LSB: ");
  Serial.println(myEEP.read(11), BIN);
  Serial.println("Erasing means writing 0xFF to all addresses"); 
}
  
void loop(){}
