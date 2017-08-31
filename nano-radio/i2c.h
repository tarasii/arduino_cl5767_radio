#include "Arduino.h"
#include <Wire.h>

uint8_t setDevAddr(uint8_t devAddr, uint8_t Addr);
uint8_t writeDev(uint8_t devAddr, uint8_t* buf, uint8_t len);
void readDev(uint8_t devAddr, uint8_t* buf, uint8_t len);
uint8_t readDevAddr(uint8_t devAddr, uint8_t Addr, uint8_t* buf, uint8_t len);
void scan_i2c(void);
