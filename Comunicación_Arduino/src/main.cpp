#include<Arduino.h>
#include"Comunicacion.h"
#ifdef __xtensa__
  #include<driver/adc.h>
  #define adcB 4095
  #define VREF 3.1
#endif
#ifdef __AVR__
  #define adcB 1023
  #define VREF 5.0
#endif
#define LED1 2
#define LED2 3

//#include<Arduino_FreeRTOS.h>

uint32_t Tiempo = 0;
uint16_t ciclos = 0;
uint16_t serialRegs[5] = { uint16_t(0.625*255/1.25), uint16_t(0.625*255/1.25), uint16_t(0.625/2.5*65535), uint16_t(0.625/2.5*65535), uint16_t(2.5*256/4.096) };
byte serialAddress = 0;
byte serialBuffer[2] = {0,0};

void setup() {
  // Comunicación
  Wire.begin();
  Wire.setClock(400000);
  Serial.begin(115200);
  SPI.begin();
  // Pines
  #ifdef __xtensa__
    adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_12);
    adc1_config_channel_atten(ADC1_CHANNEL_1, ADC_ATTEN_DB_12);
    adc1_config_channel_atten(ADC1_CHANNEL_2, ADC_ATTEN_DB_12);
    pinMode(LED_RED,OUTPUT);
    pinMode(LED_GREEN,OUTPUT);
    pinMode(LED_BLUE,OUTPUT);
  #endif
  pinMode(A0,INPUT);
  pinMode(A1,INPUT);
  pinMode(A2,INPUT);
  pinMode(A3,INPUT);
  digitalWrite(MAX1_CS,HIGH);
  digitalWrite(DAC1_CS,HIGH);
  digitalWrite(MAX2_CS,HIGH);
  digitalWrite(DAC2_CS,HIGH);
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
  pinMode(MAX1_CS,OUTPUT);
  pinMode(DAC1_CS,OUTPUT);
  pinMode(MAX2_CS,OUTPUT);
  pinMode(DAC2_CS,OUTPUT);
  pinMode(LED1,OUTPUT);
  pinMode(LED2,OUTPUT);

  #ifdef __xtensa__
    analogWrite(LED_RED,192);
    analogWrite(LED_GREEN,255);
    analogWrite(LED_BLUE,255);
  #endif
  
  delay(1000);
  
  #ifdef __xtensa__
    analogWrite(LED_RED,255);
    analogWrite(LED_GREEN,192);
    analogWrite(LED_BLUE,255);
  #endif
  
  Tiempo = millis()+1000;
  Serial.println("Inicio");
}

void loop() {
  if(Serial.available()==3)
  {
    #ifdef __xtensa__
      analogWrite(LED_RED,192);
      analogWrite(LED_GREEN,255);
      analogWrite(LED_BLUE,255);
    #endif
    serialAddress = Serial.read();
    Serial.readBytes(&serialBuffer[0],2);
    if(serialAddress<6)
    {
      serialRegs[serialAddress] = uint16_t(serialBuffer[0]*256+uint16_t(serialBuffer[1]));
      Serial.print("Se escribió el valor ");
      Serial.print(serialRegs[serialAddress],HEX);
      Serial.println(" en la memoria.");
    }
    if(serialRegs[0]>255){ serialRegs[0]=255; }
    if(serialRegs[1]>255){ serialRegs[1]=255; }
    if(serialRegs[4]>256){ serialRegs[4]=256; }
    #ifdef __xtensa__
      analogWrite(LED_RED,255);
      analogWrite(LED_GREEN,192);
      analogWrite(LED_BLUE,255);
    #endif
  }

  if(millis()>Tiempo)
  {
    #ifdef __xtensa__
      analogWrite(LED_RED,255);
      analogWrite(LED_GREEN,255);
      analogWrite(LED_BLUE,192);
    #endif
    Tiempo = millis()+5000;
    ciclos = ciclos + 1;
    //Se limpia la ventana de la terminal
    Serial.write(27);
    Serial.print("[2J");
    Serial.write(27);
    Serial.print("[H");
    //Se imprimen los datos
    Serial.print("Ciclo N°");
    Serial.println(ciclos);
    //Se lee la temperatura
    float Tem = MUA::leerTemperatura(12);
    Serial.print("Temperatura = ");
    Serial.print(Tem);
    Serial.println("°C.");
    if(Tem>40){ Tem = 40; }
    if(Tem<20){ Tem = 20; }

    MUA::escribirMAX(serialRegs[0], MAX1_CS);
    Serial.print ("Se escribió el valor 0x");
    Serial.print(serialRegs[0],HEX);
    Serial.print(" al MAX1, que equivale a ");
    float DACMAX1 = 1.25*serialRegs[0]/255;
    Serial.print(DACMAX1, 4);
    Serial.println("V en su DAC.");

    MUA::escribirMAX(serialRegs[1], MAX2_CS);
    Serial.print ("Se escribió el valor 0x");
    Serial.print(serialRegs[1],HEX);
    Serial.print(" al MAX2, que equivale a ");
    float DACMAX2 = 1.25*serialRegs[1]/255;
    Serial.print(DACMAX2, 4);
    Serial.println("V en su DAC.");

    MUA::escribirDAC(serialRegs[2], DAC1_CS);
    Serial.print ("Se escribió el valor 0x");
    Serial.print(serialRegs[2],HEX);
    Serial.print(" al DAC1, que equivale a ");
    float DACDAC1 = serialRegs[2]*2.5/65535;
    Serial.print(DACDAC1, 4);
    Serial.println("V.");

    MUA::escribirDAC(serialRegs[3], DAC2_CS);
    Serial.print ("Se escribió el valor 0x");
    Serial.print(serialRegs[3],HEX);
    Serial.print(" al DAC2, que equivale a ");
    float DACDAC2 = serialRegs[3]*2.5/65535;
    Serial.print(DACDAC2, 4);
    Serial.println("V.");

    Serial.print("El MAX1 debe tener en su salida un valor de ");
    Serial.print( 240/12.7*(1.25-0) + 240/24.9*(1.25-DACMAX1) + 240/24.9*(1.25-DACDAC1) + 240/24.9*(-1.25), 4 );
    Serial.println("V.");

    Serial.print("El MAX2 debe tener en su salida un valor de ");
    Serial.print( 240/12.7*(1.25-0) + 240/24.9*(1.25-DACMAX2) + 240/24.9*(1.25-DACDAC2) + 240/24.9*(-1.25), 4 );
    Serial.println("V.");

    MUA::escribirPotenciometro(serialRegs[4]);
    Serial.print ("Se escribió el valor 0x");
    Serial.print(serialRegs[4],HEX);
    Serial.print(" al Potenciómetro, que equivale a ");
    Serial.print(serialRegs[4]*4.096/256, 4);
    Serial.println("V.");

    #ifdef __xtensa__
      analogWrite(LED_RED,255);
      analogWrite(LED_GREEN,192);
      analogWrite(LED_BLUE,255);
    #endif

    delay(1000);
    Serial.print(".");
    digitalWrite(LED1,HIGH);
    digitalWrite(LED1,LOW);
    digitalWrite(LED2,HIGH);
    digitalWrite(LED2,LOW);
    delay(1000);
    Serial.print(".");
    delay(1000);
    Serial.println(".");
  }
}