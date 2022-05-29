/******************************************************************************
* This is an example sketch for the EEPROM_SPI_WE library.
*
* It shows how to apply the deep power down mode and different 
* methods for write protection and protection of the status register. 
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
const int wpPin = 9;  // Write protect pin (needed for this sketch)

/* There are different options to create your EEPROM_SPI_WE object:
 * EEPROM_SPI_WE myEEP = EEPROM_SPI_WE(csPin, wpPin);
 * EEPROM_SPI_WE myEEP = EEPROM_SPI_WE(csPin); 
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
  /* You can change the SPI clock speed. The default of is 8 MHz */
  //myEEP.setSPIClockSpeed(4000000); // use AFTER init()!
  
  /* Select the page size of your EEPROM.
   * Choose EEPROM_PAGE_SIZE_xxx,  
   * with xxx = 16, 32, 64, 128 or 256
   */
  myEEP.setPageSize(EEPROM_PAGE_SIZE_128);

  int testInt = 42;
  myEEP.put(50, testInt);
  Serial.print("testInt: ");
  Serial.println(testInt);
  Serial.println("Going in deep power down mode...");
  myEEP.deepPowerDown();
  Serial.println("Trying to read testInt... ");
  myEEP.get(50, testInt);
  Serial.print("testInt: ");
  Serial.println(testInt);
  Serial.println("Releasing from deep power down  mode...");
  byte id = myEEP.powerUpAndReadID();
  Serial.print("testInt: ");
  myEEP.get(50, testInt);
  Serial.println(testInt);
  Serial.print("Device ID: 0x");
  Serial.println(id, HEX);
  Serial.println("");

/* You can write protect the device. The following options are available:
 * PROTECT_ALL              Complete write protection;
 * PROTECT_UPPER_QUARTER    Protect the upper quarter; 
 * PROTECT_UPPER_HALF       Protect the upper half;
 * PROTECT_NONE             No write protection
 */
  myEEP.writeProtect(PROTECT_ALL);
  Serial.println("Protecting Device, trying to overwrite testInt...");
  myEEP.put(50, 4321);
  myEEP.writeProtect(PROTECT_NONE);
  Serial.print("testInt: ");
  myEEP.get(50, testInt);
  Serial.println(testInt);
  Serial.println("Protection removed, trying to overwrite testInt again...");
  myEEP.put(50, 4321);
  myEEP.get(50, testInt);
  Serial.print("testInt: ");
  Serial.println(testInt);
  Serial.println("");

  Serial.println("Protecting status register...");
  myEEP.protectStatusRegister(true);  //protects the non-volatile bits of status register
  Serial.println("Trying to protect device...");
  myEEP.writeProtect(PROTECT_ALL);
  Serial.println("Now overwriting testInt...");
  myEEP.put(50, 1234);
  myEEP.get(50, testInt);
  Serial.print("testInt: ");
  Serial.println(testInt);
  Serial.println("Status register protection prevented changing the device protection!");
  myEEP.protectStatusRegister(false);
}
  
void loop(){}
