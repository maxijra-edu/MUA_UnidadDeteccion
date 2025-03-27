#include<Wire.h>
#include<SPI.h>
#include<Arduino.h>
#define MAX1_CS 10
#define DAC1_CS 9
#define MAX2_CS 8
#define DAC2_CS 7
#define TMP100 0b01001000   //0x48
#define MCP4561 0b00101110  //0x2E


namespace MUA{
float leerTemperatura();

float leerTemperatura(int bits);

int configurarResolucionTMP(int bits);

int escribirPotenciometro(uint16_t valor);

int escribirMAX(uint8_t val, uint8_t nCS);

int escribirDAC(uint16_t val, uint8_t nCS);
}
