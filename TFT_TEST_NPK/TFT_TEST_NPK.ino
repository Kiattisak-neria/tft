#include <LGFX_Config.hpp>
#include <SoftwareSerial.h>
//#include "NPRULOGO.h" 

#define RE_DE 38
const uint32_t TIMEOUT = 500UL;
int NPK_S = 1,open_RS=0,collect_RS =0;
word   a_uint16;
int    a_sint16;  
float  a_float;
uint32_t startTime = 0;
uint8_t  byteCount = 0;
int rede_Sensor=1;

const byte temp[] = {0x01, 0x03, 0x00, 0x13, 0x00, 0x01, 0x75, 0xcf};
const byte mois[] = {0x01, 0x03, 0x00, 0x12, 0x00, 0x01, 0x24, 0x0f};
const byte econ[] = {0x01, 0x03, 0x00, 0x15, 0x00, 0x01, 0x95, 0xce};
const byte ph[] = {0x01, 0x03, 0x00, 0x06, 0x00, 0x01, 0x64, 0x0b};
const byte nitro[] = { 0x01, 0x03, 0x00, 0x1e, 0x00, 0x01, 0xe4, 0x0c };
const byte phos[] = { 0x01, 0x03, 0x00, 0x1f, 0x00, 0x01, 0xb5, 0xcc };
const byte pota[] = { 0x01, 0x03, 0x00, 0x20, 0x00, 0x01, 0x85, 0xc0 };

byte values[11];
SoftwareSerial mod(44, 43); // Rx pin, Tx pin
float soil_EC = 0, soil_N = 0, soil_P = 0, soil_K = 0;
float soil_ph = 0.00, soil_mois = 0.0, soil_temp = 0.0;
float soil_EC_DE=0, soil_N_DE=0, soil_P_DE=0, soil_K_DE=0, soil_ph_DE, soil_mois_DE=0, soil_temp_DE=0;

int i=0;
int Sceenn=0;
bool wasTouched = false;
String Texe_Sensor1[] = {"N   : ", "P   : ", "K   : ", "Do  :", "Vcc :"};
String Texe_Sensor2[] = {"PH  : ", "Temp: ", "Mois: ", "Ec  :", "Icc :"};
String Text_Collect1[] = {"No  : "};
String Text_Collect2[] = {"Time: "};

bool pointInTriangle(int x, int y,int x0, int y0,int x1, int y1,int x2, int y2) {
  int A  = abs((x1-x0)*(y2-y0) - (x2-x0)*(y1-y0));
  int A1 = abs((x0-x)*(y1-y) - (x1-x)*(y0-y));
  int A2 = abs((x1-x)*(y2-y) - (x2-x)*(y1-y));
  int A3 = abs((x2-x)*(y0-y) - (x0-x)*(y2-y));
  return (A == A1 + A2 + A3);
}

void setup() {
  Serial.begin(115200);
  tft.init();
  tft.setBrightness(128);
  tft.fillScreen(TFT_BLACK);
  pinMode(RE_DE, OUTPUT);
  //tft.pushImage(0, 0, 300, 480, unnamed);

}

void loop() {
  /*if(rede_Sensor==1){soil_mois = moisture();}
  if(rede_Sensor==2){soil_temp = temperature();}
  if(rede_Sensor==3){soil_ph = phydrogen();}
  if(rede_Sensor==4){soil_EC = econduc();}
  if(rede_Sensor==5){soil_N = nitrogen();}
  if(rede_Sensor==6){soil_P = phosphorous();}
  if(rede_Sensor==7){soil_K = potassium();}*/
  soil_mois = moisture();
  lgfx::v1::touch_point_t tp;
  if(Sceenn==0){
  tft.fillScreen(TFT_BLACK);
  tft.fillRect(400-100, 10, 200, 50, TFT_WHITE);
  tft.setFont(&fonts::FreeMonoBold12pt7b);
  tft.setTextColor(TFT_BLACK);
  tft.setCursor(400-40, 23);
  tft.println("SENSOR");

  for(int S=0;S<=4;S++){
    int SS=100+(S*10)+(50*S);
    tft.fillRect(70, SS, 200, 50, TFT_WHITE);
    tft.setCursor(70+20, SS+15);
    tft.println(Texe_Sensor1[S]);
    }

  for(int S=0;S<=4;S++){
    int SS=100+(S*10)+(50*S);
    tft.fillRect(530, SS, 200, 50, TFT_WHITE);
    tft.setCursor(530+20, SS+15);
    tft.println(Texe_Sensor2[S]);
    }
  
  tft.fillRect(590, 410, 200, 50, TFT_WHITE);
  tft.setCursor(590+20,410+15);
  tft.println("Collect data");

  //tft.fillTriangle(400, 240-100, 400-50, 240-50, 400+50, 240-50, TFT_WHITE);
  //tft.fillTriangle(400, 240+100, 400-50, 240+50, 400+50, 240+50, TFT_WHITE);
  Sceenn=1;

  }


if(Sceenn==2){
  tft.fillScreen(TFT_BLACK);
  tft.fillRect(400-100, 10, 200, 50, TFT_WHITE);
  tft.setFont(&fonts::FreeMonoBold12pt7b);
  tft.setTextColor(TFT_BLACK);
  tft.setCursor(400-83, 23);
  tft.println("Collect data");

  for(int S=0;S<=0;S++){
    int SS=100+(S*10)+(50*S);
    tft.fillRect(70, SS, 200, 50, TFT_WHITE);
    tft.setCursor(70+20, SS+15);
    tft.println(Text_Collect1[S]);
    }

  for(int S=0;S<=0;S++){
    int SS=100+(S*10)+(50*S);
    tft.fillRect(530, SS, 200, 50, TFT_WHITE);
    tft.setCursor(530+20, SS+15);
    tft.println(Text_Collect2[S]);
    }

  tft.fillRect(590, 410, 200, 50, TFT_WHITE);
  tft.setCursor(590+55,410+15);
  tft.println("SENSOR");
  
  Sceenn=3;
  }

  if (tft.getTouch(&tp)) {
    if (tp.x >= 590 && tp.x <= 740 && tp.y >= 410 && tp.y <= 460){
      if (!wasTouched) {
      if(Sceenn==1){Sceenn=2;}
      if(Sceenn==3){Sceenn=0;}
      Serial.printf("Touched inside triangle! i = %d\n", i);
       wasTouched = true;}}
  }
  else {
    wasTouched = false;
  }

  if(Sceenn==1){
    if(soil_N!=soil_N_DE){
      tft.setTextColor(TFT_WHITE);
       tft.setCursor(165, 100+15);
       tft.println(soil_N_DE);
       soil_N_DE=soil_N;
    }
    tft.setTextColor(TFT_BLACK);
    tft.setCursor(165, 100+15);
    tft.println(soil_N);

    if(soil_P!=soil_P_DE){
      tft.setTextColor(TFT_WHITE);
       tft.setCursor(165, 100+60+15);
       tft.println(soil_P_DE);
       soil_P_DE=soil_P;
    }
    tft.setTextColor(TFT_BLACK);
    tft.setCursor(165, 100+60+15);
    tft.println(soil_P);

    if(soil_K!=soil_K_DE){
      tft.setTextColor(TFT_WHITE);
       tft.setCursor(165, 100+120+15);
       tft.println(soil_K_DE);
       soil_K_DE=soil_K;
    }
    tft.setTextColor(TFT_BLACK);
    tft.setCursor(165, 100+120+15);
    tft.println(soil_K);

    if(soil_ph!=soil_ph_DE){
      tft.setTextColor(TFT_WHITE);
       tft.setCursor(625, 100+15);
       tft.println(soil_ph_DE);
       soil_ph_DE=soil_ph;
    }
    tft.setTextColor(TFT_BLACK);
    tft.setCursor(625, 100+15);
    tft.println(soil_ph);

    if(soil_temp!=soil_temp_DE){
      tft.setTextColor(TFT_WHITE);
       tft.setCursor(625, 100+60+15);
       tft.println(soil_temp_DE);
       soil_temp_DE=soil_temp;
    }
    tft.setTextColor(TFT_BLACK);
    tft.setCursor(625, 100+60+15);
    tft.println(soil_temp);

    if(soil_mois!=soil_mois_DE){
      tft.setTextColor(TFT_WHITE);
       tft.setCursor(625, 100+120+15);
       tft.println(soil_mois_DE);
       soil_mois_DE=soil_mois;
    }
    tft.setTextColor(TFT_BLACK);
    tft.setCursor(625, 100+120+15);
    tft.println(soil_mois);

  }



  
  if (tft.getTouch(&tp)) {
    if (pointInTriangle(tp.x, tp.y,400, 240+100, 400-50, 240+50, 400+50, 240+50)) {
      if (!wasTouched) {
      tft.fillRect(400-100, 240-40, 200, 80, TFT_WHITE);
      i--;
      Serial.printf("Touched inside triangle! i = %d\n", i);
       wasTouched = true;}}
}
else {
    wasTouched = false;
  }
//rede_Sensor=rede_Sensor+1;
//if(rede_Sensor==8){rede_Sensor=1;}
  }







//--- %RH --- 2 Bytes ---
float moisture() {
  word   a_uint16;
  int    a_sint16;  
  float  a_float;
  uint32_t startTime = 0;
  uint8_t  byteCount = 0;

  digitalWrite(RE_DE, HIGH);
  mod.write(mois, sizeof(mois));
  mod.flush();
  digitalWrite(RE_DE, LOW);

  startTime = millis();
  while ( millis() - startTime <= TIMEOUT ) {
    if (mod.available() && byteCount<sizeof(values) ) {
      values[byteCount++] = mod.read();
    }
  }

  a_uint16 = values[3]<<8;
  a_uint16 |= values[4];
  a_sint16 = (int)a_uint16;
  a_float = (float)a_sint16;
  a_float /=  10.0;

  return a_float;
}

//--- Temp --- 2 Bytes ---
float temperature() {
  word   a_uint16;
  int    a_sint16;  
  float  a_float;
  uint32_t startTime = 0;
  uint8_t  byteCount = 0;
  
  digitalWrite(RE_DE, HIGH);
  mod.write(temp, sizeof(temp));
  mod.flush();
  digitalWrite(RE_DE, LOW);

  startTime = millis();
  while ( millis() - startTime <= TIMEOUT ) {
    if (mod.available() && byteCount<sizeof(values) ) {
      values[byteCount++] = mod.read();
    }
  }

  a_uint16 = values[3]<<8;
  a_uint16 |= values[4];
  a_sint16 = (int)a_uint16;
  a_float = (float)a_sint16;
  a_float /=  10.0;

  return a_float;
}

//--- EC ---
byte econduc() {
  uint32_t startTime = 0;
  uint8_t  byteCount = 0;
  
  digitalWrite(RE_DE, HIGH);
  mod.write(econ, sizeof(econ));
  mod.flush();
  digitalWrite(RE_DE, LOW);

  startTime = millis();
  while ( millis() - startTime <= TIMEOUT ) {
    if (mod.available() && byteCount<sizeof(values) ) {
      values[byteCount++] = mod.read();
    }
  }
  return values[4];
}

//--- pH --- 2 Bytes ---
float phydrogen() {
  word   a_uint16;
  int    a_sint16;  
  float  a_float;
  uint32_t startTime = 0;
  uint8_t  byteCount = 0;
  
  digitalWrite(RE_DE, HIGH);
  mod.write(ph, sizeof(ph));
  mod.flush();
  digitalWrite(RE_DE, LOW);

  startTime = millis();
  while ( millis() - startTime <= TIMEOUT ) {
    if (mod.available() && byteCount<sizeof(values) ) {
      values[byteCount++] = mod.read();
    }
  }
  a_uint16 = values[3]<<8;
  a_uint16 |= values[4];
  a_sint16 = (int)a_uint16;
  a_float = (float)a_sint16;
  a_float /=  100.0;

  return a_float;
}  

byte nitrogen() {
  uint32_t startTime = 0;
  uint8_t  byteCount = 0;
  
  digitalWrite(RE_DE, HIGH);
  mod.write(nitro, sizeof(nitro));
  mod.flush();
  digitalWrite(RE_DE, LOW);

  startTime = millis();
  while ( millis() - startTime <= TIMEOUT ) {
    if (mod.available() && byteCount<sizeof(values) ) {
      values[byteCount++] = mod.read();
    }
  }
  return values[4];
}

byte phosphorous() {
  uint32_t startTime = 0;
  uint8_t  byteCount = 0;
  
  digitalWrite(RE_DE, HIGH);
  mod.write(phos, sizeof(phos));
  mod.flush();
  digitalWrite(RE_DE, LOW);

  startTime = millis();
  while ( millis() - startTime <= TIMEOUT ) {
    if (mod.available() && byteCount<sizeof(values) ) {
      values[byteCount++] = mod.read();
    }
  }
  return values[4];
}

byte potassium() {
  uint32_t startTime = 0;
  uint8_t  byteCount = 0;
  
  digitalWrite(RE_DE, HIGH);
  mod.write(pota, sizeof(pota));
  mod.flush();
  digitalWrite(RE_DE, LOW);

  startTime = millis();
  while ( millis() - startTime <= TIMEOUT ) {
    if (mod.available() && byteCount<sizeof(values) ) {
      values[byteCount++] = mod.read();
    }
  }
  return values[4];
}
