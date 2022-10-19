#include <SoftwareSerial.h>
#include <Wire.h>
#include <OneWire.h>
SoftwareSerial nodemcu(2,3);
OneWire  ds(10);
float temp;
char readdata;
String datas;
float limit;
void setup()
{
  Serial.begin(115200);
  nodemcu.begin(115200);
}
void loop()
{
  byte i;
  byte present = 0;
  byte type_s;
  byte data[12];
  byte addr[8];
  float celsius;
  
  if ( !ds.search(addr)) {
    ds.reset_search();
    delay(250);
    return;
  }
  if (OneWire::crc8(addr, 7) != addr[7]) {
      return;
  }
  switch (addr[0]) {
    case 0x10:
      type_s = 1;
      break;
    case 0x28:
      type_s = 0;
      break;
    case 0x22:
      type_s = 0;
      break;
    default:
      return;
  } 

  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1);        // start conversion, with parasite power on at the end 
  delay(500);      
  present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE);         // Read Scratchpad
  for ( i = 0; i < 9; i++) {           
    data[i] = ds.read();
  }
 
  int16_t raw = (data[1] << 8) | data[0];
  if (type_s) {
    raw = raw << 3; // 9 bit resolution default
    if (data[7] == 0x10) {
      raw = (raw & 0xFFF0) + 12 - data[6];
    }
  } else {
    byte cfg = (data[4] & 0x60);
    if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
    else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
    else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
  }
  celsius = (float)raw / 16.0;
 
  temp=celsius;

  if(nodemcu.available()==0)
  {
    Serial.println(temp);
    nodemcu.println(temp);
  }
  if(nodemcu.available()>0){
    limit = nodemcu.parseFloat();
    delay(100); 
    //Serial.println(limit); 
   }
  
}
