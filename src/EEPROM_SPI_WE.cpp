/********************************************************************
* This is a library for SPI EEPROMs like the 25LCxxx, 25Axxx, etc.
*
* In case you like it would be cool if you give it a star.
* 
* If you find bugs, please inform me!
* 
* Written by Wolfgang (Wolle) Ewald
*
* Here you find the detailed documentation:
* https://wolles-elektronikkiste.de/en/eeprom-part-3-external-spi-eeproms   (English)
* https://wolles-elektronikkiste.de/eeprom-teil-3-externe-spi-eeproms       (German)
*
*********************************************************************/

#include "EEPROM_SPI_WE.h"

/************  Constructors ************/

EEPROM_SPI_WE::EEPROM_SPI_WE(SPIClass *s, int cs, int wp){
    _spi = s;
    csPin = cs;
    wpPin = wp;
}

EEPROM_SPI_WE::EEPROM_SPI_WE(int cs, int wp){
    _spi = &SPI;
    csPin = cs;
    wpPin = wp;
}

/************ Basic settings ************/
    
bool EEPROM_SPI_WE::init(){    
    _spi->begin();
    mySPISettings = SPISettings(8000000, MSBFIRST, SPI_MODE0);
    pinMode(csPin, OUTPUT);
    digitalWrite(csPin, HIGH);
    contAddr = 0;
    
    if(wpPin != 999){
        pinMode(wpPin,OUTPUT);
        digitalWrite(wpPin, HIGH);
    }
    
    pageSize = 16;
    writeProtect(PROTECT_NONE);
    eepromWriteStatusReg(EEP_WREN);
    if(eepromReadStatusReg() & 0x02){
        eepromWriteStatusReg(EEP_WRDI);
        return true;
    }
    else return false;  
}

void EEPROM_SPI_WE::setPageSize(eeprom_pageSize eps){
    pageSize = 16 << eps;
}

void EEPROM_SPI_WE::deepPowerDown(){
    eepromWriteStatusReg(EEP_DPD);
}

uint8_t EEPROM_SPI_WE::powerUpAndReadID(){
    uint8_t id = 0;
    _spi->beginTransaction(mySPISettings);
    digitalWrite(csPin, LOW);
    _spi->transfer(EEP_RDID); 
    _spi->transfer(0xFF); // dummy address
    _spi->transfer(0xFF);
    id = _spi->transfer(0x00);
    digitalWrite(csPin, HIGH);
    _spi->endTransaction();
    while(isBusy()){}
    return id;
}

/************ read and write ************/

void EEPROM_SPI_WE::erasePage(uint32_t addr){
    eepromWriteEnable();
    
    _spi->beginTransaction(mySPISettings);
    digitalWrite(csPin, LOW);
    _spi->transfer(EEP_PE);
    _spi->transfer((uint8_t)(addr>>8));
    _spi->transfer((uint8_t)(addr&0xFF));
    digitalWrite(csPin, HIGH);
    _spi->endTransaction();
    
    while(isBusy()){}   
}

void EEPROM_SPI_WE::eraseSector(uint32_t addr){
    eepromWriteEnable();
    
    _spi->beginTransaction(mySPISettings);
    digitalWrite(csPin, LOW);
    _spi->transfer(EEP_SE);
    _spi->transfer((uint8_t)(addr>>8));
    _spi->transfer((uint8_t)(addr&0xFF));
    digitalWrite(csPin, HIGH);
    _spi->endTransaction();
    
    while(isBusy()){}
}

void EEPROM_SPI_WE::eraseCompleteEEPROM(){
    eepromWriteEnable();
    
    _spi->beginTransaction(mySPISettings);
    digitalWrite(csPin, LOW);
    _spi->transfer(EEP_CE);
    digitalWrite(csPin, HIGH);
    _spi->endTransaction();
    
    while(isBusy()){}   
}

void EEPROM_SPI_WE::write(uint32_t addr, uint8_t val){
    if (read(addr) != val) 
        writeEEPROM(addr, &val, 1);
    while(isBusy()){}
}

uint8_t EEPROM_SPI_WE::read(uint32_t addr){
    uint8_t byteVal;
    readEEPROM(addr, &byteVal, 1);
    return byteVal;
}

void EEPROM_SPI_WE::writeProtect(eeprom_writeProtect sectors){
    eepromWriteEnable();
    
    _spi->beginTransaction(mySPISettings);
    digitalWrite(csPin, LOW);
    _spi->transfer(EEP_WRSR);
    _spi->transfer((sectors << 2) | 0x80);
    digitalWrite(csPin, HIGH); 
    _spi->endTransaction();
    while(isBusy()){}
}

void EEPROM_SPI_WE::protectStatusRegister(bool protect){
    uint8_t statusReg = eepromReadStatusReg();
    if(!protect && (wpPin != 999)){
        digitalWrite(wpPin, HIGH);
    }
    else if(protect && (wpPin != 999)){
        statusReg |= 0x80;
        eepromWriteEnable();
        eepromWriteStatusReg(statusReg);
        digitalWrite(wpPin, LOW);
    }
}   

uint8_t EEPROM_SPI_WE::eepromReadStatusReg(){
    uint8_t statusReg = 0;
    
    _spi->beginTransaction(mySPISettings);
    digitalWrite(csPin, LOW);
    _spi->transfer(EEP_RDSR); 
    statusReg = _spi->transfer(0x00); 
    digitalWrite(csPin, HIGH);
    _spi->endTransaction();
    
    return statusReg;
}

bool EEPROM_SPI_WE::isBusy(){
    delayMicroseconds(50);
    uint8_t statusReg = eepromReadStatusReg();
    return (statusReg & 0x01);
}

void EEPROM_SPI_WE::setSPIClockSpeed(unsigned long clock){
    mySPISettings = SPISettings(clock, MSBFIRST, SPI_MODE0);
}

/************************************************ 
     Private Functions
*************************************************/

void EEPROM_SPI_WE::eepromWriteEnable(){
    eepromWriteStatusReg(EEP_WREN); 
}

void EEPROM_SPI_WE::eepromWriteStatusReg(uint8_t cmd){
    _spi->beginTransaction(mySPISettings);
    digitalWrite(csPin, LOW);
    _spi->transfer(cmd); 
    digitalWrite(csPin, HIGH);
    _spi->endTransaction();
    
    while(isBusy()){}
}

void EEPROM_SPI_WE::writeEEPROM(uint32_t addr, const uint8_t *buf, uint16_t sizeOfBuf){
    uint16_t noOfBytesStillToWrite = sizeOfBuf;
    uint16_t arrayIndex = 0;
    
    while((noOfBytesStillToWrite != 0)){
        uint16_t chunk = noOfBytesStillToWrite;
        uint16_t positionInPage = addr % pageSize;
        uint16_t spaceLeftInPage = pageSize - positionInPage;
        
        if(spaceLeftInPage < noOfBytesStillToWrite){
            chunk = spaceLeftInPage;
        }
                
        eepromWriteEnable();
        
        _spi->beginTransaction(mySPISettings);
        digitalWrite(csPin, LOW);
        _spi->transfer(EEP_WRITE);
        _spi->transfer((uint8_t)(addr>>8));
        _spi->transfer((uint8_t)(addr&0xFF));
        
        for(uint16_t i=arrayIndex; i<(arrayIndex + chunk); i++){
            _spi->transfer((uint8_t)buf[i]);
        }
        
        digitalWrite(csPin, HIGH);
        _spi->endTransaction();
        
        addr += chunk;
        arrayIndex += chunk;
        noOfBytesStillToWrite -= chunk;
        while(isBusy()){}       
    }
}

void EEPROM_SPI_WE::continuousPutEnable(uint32_t addr){
    contAddr = addr;
    eepromWriteEnable();
    _spi->beginTransaction(mySPISettings);
    digitalWrite(csPin, LOW);
    _spi->transfer(EEP_WRITE);
    _spi->transfer((uint8_t)(addr>>8));
    _spi->transfer((uint8_t)(addr&0xFF));
}

void EEPROM_SPI_WE::continuousPutDisable(){
    digitalWrite(csPin, HIGH);
    _spi->endTransaction();
    while(isBusy());
}

void EEPROM_SPI_WE::continuousWriteEEPROM(const uint8_t *buf, uint16_t sizeOfBuf){
    uint16_t noOfBytesStillToWrite = sizeOfBuf;
    uint16_t arrayIndex = 0;
    
    while((noOfBytesStillToWrite != 0)){
        uint16_t chunk = noOfBytesStillToWrite;
        uint16_t positionInPage = contAddr % pageSize;
        uint16_t spaceLeftInPage = pageSize - positionInPage;
        bool pageEnd = false;
        
        if(spaceLeftInPage < noOfBytesStillToWrite){
            chunk = spaceLeftInPage;
            pageEnd = true;
        }
               
        for(uint16_t i=arrayIndex; i<(arrayIndex + chunk); i++){
            _spi->transfer((uint8_t)buf[i]);
        }
        
        contAddr += chunk;
        arrayIndex += chunk;
        noOfBytesStillToWrite -= chunk;
        if(pageEnd){
            digitalWrite(csPin, HIGH);
            _spi->endTransaction();
            while(isBusy()){}
            continuousPutEnable(contAddr);
            pageEnd = false;
        }
    }
}

void EEPROM_SPI_WE::readEEPROM(uint32_t addr, uint8_t *buf, uint16_t sizeOfBuf){    
    _spi->beginTransaction(mySPISettings);
    digitalWrite(csPin, LOW);
    _spi->transfer(EEP_READ); 
    _spi->transfer((uint8_t)(addr>>8));
    _spi->transfer((uint8_t)(addr&0xFF));
    for(uint16_t i=0; i<sizeOfBuf; i++){
        buf[i] = _spi->transfer(0x00);
    }
    digitalWrite(csPin, HIGH);
    _spi->endTransaction();
}



