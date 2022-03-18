/********************************************************************
* This is a library for SPI EEPROMs like the 25LCxxx, 25Axxx, etc.
*
* In case you like it would be cool if you give it a star.
* 
* If you find bugs, please inform me!
* 
* Written by Wolfgang (Wolle) Ewald
*
* You will find a tutorial on my website soon:
* https://wolles-elektronikkiste.de/    (German)
* https://wolles-elektronikkiste.de/en/ (English)
*
*********************************************************************/

#ifndef EEPROM_SPI_WE_H_
#define EEPROM_SPI_WE_H_

#if ARDUINO < 100
#include <WProgram.h>
#else
#include <Arduino.h>
#endif

#include <SPI.h>
#define EEP_READ    0x03  // write
#define EEP_WRITE   0x02  // read
#define EEP_WREN    0x06  // write enable
#define EEP_WRDI    0x04  // write disable
#define EEP_RDSR    0x05  // read status register
#define EEP_WRSR    0x01  // write status register
#define EEP_PE      0x42  // page erase
#define EEP_SE      0xD8  // sector erase
#define EEP_CE      0xC7  // chip erase
#define EEP_RDID    0xAB  // release from deep power down and read electronic signature
#define EEP_DPD     0xB9  // deep power down mode


typedef enum EEPROM_PAGE_SIZE {
    EEPROM_PAGE_SIZE_16, EEPROM_PAGE_SIZE_32, EEPROM_PAGE_SIZE_64, EEPROM_PAGE_SIZE_128, EEPROM_PAGE_SIZE_256
} eeprom_pageSize;

typedef enum EEPROM_WRITE_PROTECT {
    PROTECT_NONE, PROTECT_UPPER_QUARTER, PROTECT_UPPER_HALF, PROTECT_ALL
} eeprom_writeProtect;

class EEPROM_SPI_WE
{
public: 
    
    EEPROM_SPI_WE(SPIClass *s, int cs, int wp = 999);
    EEPROM_SPI_WE(int cs, int wp = 999);
        
    bool init();
    void setPageSize(eeprom_pageSize eps);
    void deepPowerDown();
    uint8_t powerUpAndReadID();
    void erasePage(uint32_t addr);
    void eraseSector(uint32_t addr);
    void eraseCompleteEEPROM();
    void write(uint32_t addr, uint8_t val);
    uint8_t read(uint32_t addr);
    void writeProtect(eeprom_writeProtect sectors);
    void protectStatusRegister(bool protect = true);
    
    template <typename T>
    T &get(uint32_t addr, T &t){
        uint8_t *data = (uint8_t *)&t;
        readEEPROM(addr, data, sizeof(T)); 
        return t;
    }

    template <typename T>
    const T &put(uint32_t addr, const T &t){ 
        const uint8_t *data = (const uint8_t *)&t;
        writeEEPROM(addr, data, sizeof(T)); 
        return t;
    }
    
    bool isBusy();
    uint8_t eepromReadStatusReg(); 
    void setSPIClockSpeed(unsigned long clock); 
        
private:
    SPIClass *_spi;
    SPISettings mySPISettings;
    int16_t csPin;
    int16_t wpPin;
    uint16_t pageSize;
        
    void eepromWriteEnable();
    void eepromWriteStatusReg(uint8_t cmd);
    void writeEEPROM(uint32_t addr, const uint8_t *buf, uint16_t sizeOfBuf);
    void readEEPROM(uint32_t addr, uint8_t *buf, uint16_t sizeOfBuf);   
};

#endif


