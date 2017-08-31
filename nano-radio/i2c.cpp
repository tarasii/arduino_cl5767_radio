#include "i2c.h"

uint8_t setDevAddr(uint8_t devAddr, uint8_t Addr){
  Wire.beginTransmission(devAddr);
  Wire.write(Addr);                  
  if (Wire.endTransmission() != 0)
    return(0);

  return(1);
}

uint8_t writeDev(uint8_t devAddr, uint8_t* buf, uint8_t len){
  Wire.beginTransmission(devAddr);
  for (uint8_t i=0; i<len; i++) 
  {
    Wire.write(buf[i]);         
  }
  if (Wire.endTransmission() != 0)
    return(0);

  return(1);
}

void readDev(uint8_t devAddr, uint8_t* buf, uint8_t len){
  memset(buf,0,len);
  Wire.requestFrom(devAddr, len);
  while (!Wire.available()); 
  for (uint8_t i=0; i<len; i++)
  {
    buf[i] = Wire.read();
  }
}

uint8_t readDevAddr(uint8_t devAddr, uint8_t Addr, uint8_t* buf, uint8_t len){
  uint8_t res;
  res = setDevAddr(devAddr, Addr);
  if (res == 1)
    readDev(devAddr, buf, len);
  else
    return(0);
  return(1);
}

void scan_i2c(void){
  byte res;
  int i;

  for(i=0;i<127;i++){
    Wire.beginTransmission(i);
    res = Wire.endTransmission();  
    if (res==0){
      Serial.print(i,HEX);
      Serial.println("  !");
    }else if (res==4){
      Serial.print(i,HEX);
      Serial.println("  .");
    }  
  }
}
