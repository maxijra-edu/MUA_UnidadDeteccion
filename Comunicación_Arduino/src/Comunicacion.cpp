#include"Comunicacion.h"
#include<Arduino.h>
#define byte uint8_t
byte data[2] = {0x00, 0x00};

float MUA::leerTemperatura()
{
  data[0] = 0x00;
  data[1] = 0x00;
  Wire.beginTransmission(int(TMP100)); //Dirección del TMP100
  Wire.write(0x00);
  Wire.endTransmission(true);
  Wire.requestFrom(int(TMP100), 2);
  for (int i = 0; i < 2; ++i) {
    if (Wire.available()) {
      data[i] = Wire.read();
    }
  }
  int16_t temp = int(data[0]);
  temp = temp * 256;
  temp = temp + (unsigned int)(data[1]);
  temp = temp / 16;
  float T = 0;
  T = 256.0 * (temp + 2048.0) / 4095.0 - 128;
  return T;
}

float MUA::leerTemperatura(int bits)
{
  //Se encienden las mediciones
  Wire.beginTransmission(TMP100); //Dirección del TMP100
  Wire.write(0x01);
  if(bits==9)      { Wire.write(0b00000000); Wire.endTransmission(true);}
  else if(bits==10){ Wire.write(0b00100000); Wire.endTransmission(true);}
  else if(bits==11){ Wire.write(0b01000000); Wire.endTransmission(true);}
  else if(bits==12){ Wire.write(0b01100000); Wire.endTransmission(true);}
  else { Wire.endTransmission(true); return -1; }
  delay(800);
  //Se inicia la lectura del valor
  Wire.beginTransmission(TMP100); //Dirección del TMP100
  Wire.write(0x00);
  Wire.endTransmission(true);
  data[0] = 0x00;
  data[1] = 0x00;
  Wire.requestFrom(TMP100, 2);
  delay(10);
  data[0] = 0x00;
  data[1] = 0x00;
  Wire.requestFrom(TMP100, 2);

  //Entra en estado de reposo.
  Wire.beginTransmission(int(TMP100)); //Dirección del TMP100
  Wire.write(0x01);
  Wire.write(0b00000001);
  Wire.endTransmission(true);
    
  for (int i = 0; i < 2; ++i) {
    if (Wire.available()) {
      data[i] = Wire.read();
    }
  }
  int16_t temp = int(data[0]);
  temp = temp * 256;
  temp = temp + (unsigned int)(data[1]);
  temp = temp / 16;
  float T = 0;
  T = 256.0 * (temp + 2048.0) / 4095.0 - 128;
  return T;
}

int MUA::configurarResolucionTMP(int bits)
{
  Wire.beginTransmission(int(TMP100)); //Dirección del TMP100
  Wire.write(0x01);
  if(bits==9)      { Wire.write(0b00000001); return Wire.endTransmission(true); }
  else if(bits==10){ Wire.write(0b00100001); return Wire.endTransmission(true); }
  else if(bits==11){ Wire.write(0b01000001); return Wire.endTransmission(true); }
  else if(bits==12){ Wire.write(0b01100001); return Wire.endTransmission(true); }
  else { Wire.endTransmission(true); return -1; }
}

int MUA::escribirPotenciometro(uint16_t valor)
{
  if(valor>256) {valor=256;}
  Wire.beginTransmission(MCP4561); //Dirección del MCP4561
  byte DH = valor/256;
  byte D1 = 0b00000000 or DH;
  byte D2 = byte(valor);
  //Serial.println(D1,BIN);
  //Serial.println(D2,BIN);
  Wire.write(D1);
  Wire.write(D2);
  return Wire.endTransmission(true);
}

int MUA::escribirMAX(uint8_t val, uint8_t nCS)
{
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
  digitalWrite(nCS,LOW);
  SPI.transfer(val);
  digitalWrite(nCS,HIGH);
  SPI.endTransaction();
  return 0;
}

int MUA::escribirDAC(uint16_t val, uint8_t nCS)
{
  byte byte1 = 0x00;
  byte byte2 = byte(val/256);
  byte byte3 = byte(val);
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE1));
  digitalWrite(nCS,LOW);
  SPI.transfer(byte1);
  SPI.transfer(byte2);
  SPI.transfer(byte3);
  digitalWrite(nCS,HIGH);
  SPI.endTransaction();
  return 0;
}
