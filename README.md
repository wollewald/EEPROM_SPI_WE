# EEPROM_SPI_WE
Arduino library for SPI based EEPROMs

With this library you can write data to and read data from SPI based EEPROMs of the 25 series. Although it uses page writing, you don't have to worry about the page limits. You can directly write bytes, integers, floats, arrays and strings (strings only work for AVR based boards). I have adopted the method of handling different variable types for put() and get() from Sparkfun's great library <a href="https://github.com/sparkfun/SparkFun_External_EEPROM_Arduino_Library">SparkFun_External_EEPROM_Arduino_Library</a> for I2C EEPROMs.

I have implemented the protection options for the storage and and the status register. 

The functions for erasing pages, sectors or the entire EEPROM only work with the larger EEPROMs. Check the data sheet if you are not sure. 

I have tested several EEPROMs (25LC512, 25A512, 25LC640) using various MCU boards, such as the ESP32 development board, WEMOS D1 mini (ESP8266) and the Arduino Nano. 

Here you find the detailed documentation:

https://wolles-elektronikkiste.de/en/eeprom-part-3-external-spi-eeproms   (English)

https://wolles-elektronikkiste.de/eeprom-teil-3-externe-spi-eeproms       (German)
