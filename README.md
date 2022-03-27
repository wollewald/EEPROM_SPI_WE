# EEPROM_SPI_WE
Arduino library for SPI based EEPROMs

With this library you can write data to and read data from SPI based EEPROMs of the 25 series. Although it uses page writing, you don't have to worry about the page limits. You can directly write bytes, integers, floats, arrays and strings (strings only work for AVR based boards). I have taken the method of handling different variable types for put() and get() from Sparkfun's great library SparkFun_External_EEPROM_Arduino_Library for I2C EEPROMs: https://github.com/sparkfun/SparkFun_External_EEPROM_Arduino_Library

I have implemented the protection options for the storage and and the status register. 

The functions for erasing pages, sectors or the entire EEPROM only work with the larger EEPROMs. Check the data sheet if you are not sure. 

I have tested several EEPROMs (25LC512, 25A512, 25LC640) using various MCU boards, such as the ESP32 development board, WEMOS D1 mini (ESP8266) and the Arduino Nano. 

You will find a documentation on my home page soon:

https://wolles-elektronikkiste.de/en/
