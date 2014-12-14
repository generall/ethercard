# EtherCard (porting from Arduino to AT91SAM7S)

**EtherCard** is a driver for the ENC28J60 chip, compatible with arm-gcc-no-eabi.  
Adapted and extended from code written by Guido Socher and Pascal Stang.

License: GPL2

The documentation for this library is at http://jeelabs.net/pub/docs/ethercard/.

## SPI interface

Used software SPI because of some wired bug with hardware one. 

## Physical Installation

### PIN Connections (Using SAM7-P256):

    VCC -   3.3V
    GND -    GND
    SCK -   PA14
    SO  -   PA12
    SI  -   PA13
    CS  -   PA10 # Selectable with the ether.begin() function

