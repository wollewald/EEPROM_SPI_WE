/*******************************************************************************
* This is an example sketch for the EEPROM_SPI_WE library
*
* It shows how to write data to and read data from the EEPROM. 
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
  myEEP.setPageSize(EEPROM_PAGE_SIZE_32);

  byte byteToWrite = 42; 
  myEEP.write(10, byteToWrite);  // write a byte to EEPROM address 10
  byte byteToRead = myEEP.read(10);
  Serial.print("Byte read: ");
  Serial.println(byteToRead);

  int intToWrite = -4242; 
  int intToRead = 0;
  myEEP.put(10, intToWrite); // write an integer to EEPROM address 10
  myEEP.get(10, intToRead);
  Serial.print("Integer read: ");
  Serial.println(intToRead);

  float floatToWrite = 42.42; 
  float floatToRead = 0.0; 
  myEEP.put(10, floatToWrite);
  myEEP.get(10, floatToRead);
  Serial.print("Float read: ");
  Serial.println(floatToRead);

  char charArrayToWrite[] = "This is no poetry, I am just a simple char array"; 
  myEEP.put(110, charArrayToWrite);  // write stringToWrite to address 110
  char charArrayToRead[60] = "";  // reserve sufficient space 
  myEEP.get(110, charArrayToRead);
  Serial.print("Char array read: ");
  Serial.println(charArrayToRead);

  String stringToWrite = "Hello, I am a test string";
  unsigned int nextAddr = myEEP.putString(200, stringToWrite);   // String objects need a different put function
  
  String stringToRead = "";
  myEEP.getString(200, stringToRead);
  Serial.print("String read: ");
  Serial.println(stringToRead);
  Serial.print("Next free address: ");
  Serial.println(nextAddr);

  int intArrayToWrite[20];
  int intArrayToRead[20];
  for(unsigned int i=0; i<(sizeof(intArrayToWrite)/sizeof(int)); i++){
    intArrayToWrite[i] = 10*i;
  } 
  myEEP.put(250, intArrayToWrite);
  myEEP.get(250, intArrayToRead);
  for(unsigned int i=0; i<(sizeof(intArrayToRead)/sizeof(int)); i++){
    Serial.print("intArrayToRead[");
    Serial.print(i);
    Serial.print("]: ");
    Serial.println(intArrayToRead[i]);
  }  
}
  
void loop(){}
