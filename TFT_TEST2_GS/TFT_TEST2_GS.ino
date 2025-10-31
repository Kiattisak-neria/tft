#include <LGFX_Config.hpp>//ทำการเรียกใช้เพื่อตั้งค่าหน้าจอควรมีไฟล์ในโฟเดอร์ของดค้ด
#include <SoftwareSerial.h>//ใช้ในการอ่านค่าเซ็นเซอร์ NPK ในรูปแบบ RS485
#include<WiFi.h>//ใช้ในการเชื่อต่อ WiFi
#include <HTTPClient.h>//ใช้ในการส่งข้อมูลไปยัง Google 

int Sceenn=0;//ใช้ในการเปลี่ยนหน้าจอ
bool wasTouched = false;//ตัวแปรในการช่วยในการสัมผัสหน้าจอ(สัมผัสครั้งเดียวแม้จะกดค้างก็ตาม)

#define WiFi_SSID "ชื่อ WiFi" 
#define WiFi_PASSWORD "รหัส WiFi"
unsigned long TimecheckWiFi,timeCe;//ตัวแปรเวลาในการทำการเชื่อมต่อ WiFi ในทุกๆ 1 วิ,ตัวแปรในการส่งข้อมูลขึ้น Google Sheet ทุกๆ 3 วิ,
int sWiFi=1,pSWiFi,Success_Sheet;//ตัวแปรที่ใช้ในการเข้าเงือไขเมื่อจะทำการส่งข้อมูล Google sheet เมื่อเชื่อมต่อ WiFi ไม่ได้จะไม่สามารถส่งข้อมูลไปยัง Google Sheet ได้,ตัวแปรในการแสดงข้อความเมื่อกำลังข้ปมูลขึ้น Google Sheet,ตัวแปรในการเชคเมื่อส่งข้อมูลขึ้น Google sheet สำเร็จ
int co=1;//ตัวแปรในการนับรอบการส่งข้อมูลขึ้น Google Sheet


#define RE 38 //ขา pin ที่ใช้ในการเปิดปิดเพื่อใช้ในการรับส่งข้อมูลแบบ Rs485

//id และ การตั้งค่าต่างๆในการอ่านค่าเซ็นเซอร์ NPK การตั้งค่ามีอะไรบ้างสามารถเข้าไปดูในโค้ด 485
const byte temp[] = {0x01, 0x03, 0x00, 0x13, 0x00, 0x01, 0x75, 0xcf};
const byte mois[] = {0x01, 0x03, 0x00, 0x12, 0x00, 0x01, 0x24, 0x0f};
const byte econ[] = {0x01, 0x03, 0x00, 0x15, 0x00, 0x01, 0x95, 0xce};
const byte ph[] = {0x01, 0x03, 0x00, 0x06, 0x00, 0x01, 0x64, 0x0b};

const byte nitro[] = { 0x01, 0x03, 0x00, 0x1e, 0x00, 0x01, 0xe4, 0x0c };
const byte phos[] = { 0x01, 0x03, 0x00, 0x1f, 0x00, 0x01, 0xb5, 0xcc };
const byte pota[] = { 0x01, 0x03, 0x00, 0x20, 0x00, 0x01, 0x85, 0xc0 };

byte values[11];
SoftwareSerial mod(44, 43);// pin Rx Tx

//ตัวแปรในการเก็บค่าต่างๆของเซ็นเซอร์
float soil_mois,soil_mois_DE;
float soil_temp,soil_temp_DE;
float soil_ph,soil_ph_DE;
float soil_EC,soil_EC_DE;
float soil_N,soil_N_DE;
float soil_P,soil_P_DE;
float soil_K,soil_K_DE;

int NO_coco,NO_coco_DE;//ตัวแปรในการเก็บค่าแปลงของมะพร้าว
int co_coco,co_coco_DE;//ตัวแปรในการเก็บค่าจำนวนการส่งข้อมูลของ Google Sheet
int no_kk;//ตัวแปรใการเข้าเงื่อนไขในการเลือกแป้นพิมพ์ระหว่าง แปรงมะพร้าว กับ จำนวนการส่งข้อมูลข้อ Google Sheet
int set=100;//พิกัดแกน x ของแป้นพิมพ์สามารถเปลี่ยนเพื่อเคลื่อนแป้นพิมได้ ค่าน้อยลงเคลื่อนไปทางซ้าย มากขึ้นไปทางขวา
int num_P,num_P_DE,sum_NUM1[5],countTu1=0;//2ตัวแปรแรกเป็นตัวแปลในการเก็บเลขของแปลงมะพร้าวแล้วแสดงขึ้นบนแป้นพิมพ์,ตัวแปรในการเก็บตัวเลขที่ได้ทำการพิมพ์(แปลงมะพร้าว),ตัวแปรในการเก็บเลขหลักต่าก
int num_C,num_C_DE,sum_NUM2[5],countTu2=0;//2ตัวแปรแรกเป็นตัวแปลในการเก็บเลขของส่งข้อมูลของ Google Sheetแล้วแสดงขึ้นบนแป้นพิมพ์,ตัวแปรในการเก็บตัวเลขที่ได้ทำการพิมพ์(ส่งข้อมูลของ Google Sheet),ตัวแปรในการเก็บเลขหลักต่าก

const uint32_t TIMEOUT = 500UL;//เวลาในการอ่านเซ็นเซอร์แต่ละครั้ง

//ตัวแปรในการส่งค่าของเซ็นเซอร์กลับไปเก็บไปในตัวแปรต่างๆ
word   a_uint16;
int    a_sint16;  
float  a_float;
uint32_t startTime = 0;
uint8_t  byteCount = 0;

//ตัวแปรในการเก็บชื่อเพื่อใช้ในการแสดงขึ้นจอ
String Texe_Sensor1[] = {"N   : ", "P   : ", "K   : ", "Do  :", "Vcc :"};
String Texe_Sensor2[] = {"PH  : ", "Temp: ", "Mois: ", "Ec  :", "Icc :"};
String Text_Collect1[] = {"No  : "};
String Text_Collect2[] = {"count: "};


void setup() {
  Serial.begin(9600);
  pinMode(RE, OUTPUT);
  mod.begin(9600);//ตั้งค่าบอร์ดเรดของ เซ็นเซอร์
  tft.init();//เริ่มใช้งานจอ TFT 
  tft.setBrightness(128);//ตั้งค่าความสว่างของจอ TFT
  tft.fillScreen(TFT_BLACK);//ตั้งค่าพื้นหลังเป็นสีดำ
  WiFi.begin(WiFi_SSID,WiFi_PASSWORD);//ทำการเชื่อมต่า WiFi

}

void loop() {
  //อ่านค่าเซ็นเซอร์ NPK
  soil_mois = moisture();  
  soil_temp = temperature(); 
  soil_ph = phydrogen(); 
  soil_EC = econduc(); 
  soil_N = nitrogen();  
  soil_P = phosphorous(); 
  soil_K = potassium();
  Serial.println("NPK");
  Sceen1_2();//ฟังชั่นของหน้าจอทั้งหมด
  }


//ฟังชั่นของหน้าจอทั้งหมด
void Sceen1_2(){
  lgfx::v1::touch_point_t tp;//สร้างตัวแปรในการเขียนฟังชั่นต่างๆของหน้าจอ

  //เงือนไขของของการแสดงตัวข้อของหน้าจอเพียงครั้งเดียวในเงือนไขนี่เป็นหัวข้อเซ็นเซอร์
  if(Sceenn==0){
  tft.fillScreen(TFT_BLACK);//สร้างพื้นหลังสีดำ
  tft.fillRect(400-100, 10, 200, 50, TFT_WHITE);//สร้างพื้นที่สี่เหลี่ยมสีขาวตามพิกัดและขนาด(ด้านบนกลางๆ)
  tft.setFont(&fonts::FreeMonoBold12pt7b);//ตั้งค่าฟอนต์และขนาด
  tft.setTextColor(TFT_BLACK);//ตั้งค่าตัวอักษรเป็นสีดำ
  tft.setCursor(400-40, 23);//ตั้งค่าพิกัดเริ่มของตัวอักษร
  tft.println("SENSOR");//ข้อความที่ต้องการแสดง

  //สร้างพื้นที่สี่เหลี่ยมและข้อความต่างๆของเซ็นเซอร์
  for(int S=0;S<=4;S++){
    int SS=100+(S*10)+(50*S);
    tft.fillRect(70, SS, 200, 50, TFT_WHITE);
    tft.setCursor(70+20, SS+15);
    tft.println(Texe_Sensor1[S]);
    }

  //สร้างพื้นที่สี่เหลี่ยมและข้อความต่างๆของเซ็นเซอร์
  for(int S=0;S<=4;S++){
    int SS=100+(S*10)+(50*S);
    tft.fillRect(530, SS, 200, 50, TFT_WHITE);
    tft.setCursor(530+20, SS+15);
    tft.println(Texe_Sensor2[S]);
    }
  //สร้างพื้นที่สี่เหลี่ยมและข้อความเพื่อใช้ในการสัมผัส(เก็บข้อมูล)
  tft.fillRect(590, 410, 200, 50, TFT_WHITE);
  tft.setCursor(590+20,410+15);
  tft.println("Collect data");
  //สร้างพื้นที่สี่เหลี่ยมและข้อความเพื่อใช้ในการสัมผัส(เก็บเปลี่ยนหน้า)
  tft.fillRect(290, 410, 210, 50, TFT_WHITE);
  tft.setCursor(300+5,410+15);
  tft.println("Data to Sheet");

  Sceenn=1;//ทำการเปลี่ยนค่าตัวแปรเพื่อไม่ให้กลับมาทำซ้ำ

  }

  //เงือนไขของของการแสดงตัวข้อของหน้าจอเพียงครั้งเดียวในเงือนไขนี่เป็นตั้งค่าเก็บข้อมูล
  if(Sceenn==2){
  tft.fillScreen(TFT_BLACK);
  tft.fillRect(400-100, 10, 200, 50, TFT_WHITE);
  tft.setFont(&fonts::FreeMonoBold12pt7b);
  tft.setTextColor(TFT_BLACK);
  tft.setCursor(400-83, 23);
  tft.println("Collect data");

  //สร้างพื้นที่สี่เหลี่ยมและข้อความต่างๆของการเลือกแปรง
  for(int S=0;S<=0;S++){
    int SS=100+(S*10)+(50*S);
    tft.fillRect(70, SS, 200, 50, TFT_WHITE);
    tft.setCursor(70+20, SS+15);
    tft.println(Text_Collect1[S]);
    }
  //สร้างพื้นที่สี่เหลี่ยมและข้อความต่างๆของเลือกจำนวนครั้งของการเก็บข้อมูล
  for(int S=0;S<=0;S++){
    int SS=100+(S*10)+(50*S);
    tft.fillRect(530, SS, 200, 50, TFT_WHITE);
    tft.setCursor(530+20, SS+15);
    tft.println(Text_Collect2[S]);
    }

  tft.fillRect(590, 410, 200, 50, TFT_WHITE);
  tft.setCursor(590+55,410+15);
  tft.println("SENSOR");
  
  Sceenn=3;//ทำการเปลี่ยนค่าตัวแปรเพื่อไม่ให้กลับมาทำซ้ำ
  }

  //เงื่อนไขในการสัมพัสหน้าจอเพื่อเปลี่ยนหน้าจอ
  if (tft.getTouch(&tp)) {//ตรวจสอบการสัมผัสหน้าจอ
    if (tp.x >= 590 && tp.x <= 790 && tp.y >= 410 && tp.y <= 460){//ตรวจสอบว่าการการสัมผัสหน้าจอตรงกับพิกัดดังกล่าวหรือไม่
      if (!wasTouched) {//เงื่อนไขที่ใช้ในการสำผัสหน้าจอเพียงครั้งเดียวแม้จะกดข้างก็ตาม
      if(Sceenn==1){Sceenn=2;}//เงื่ิอนไขเปลี่ยนหน้าจอ
      if(Sceenn==3){Sceenn=0;}//เงื่ิอนไขเปลี่ยนหน้าจอ
       wasTouched = true;}}//เปลี่ยนค่าเพื่อสัมผัสเพยงตรั้งเดียว
  }
  else {//เงือ่นไขเมื่อเลิกสัมผัส
    wasTouched = false;
  }


  //เงื่อนไขในการการแสดงค่าต่างๆของเซนเซอร์ขึ้นจอ
  if(Sceenn==1){
    if(soil_N!=soil_N_DE){//เงือ่ไขในการลบค่าเดิมเมื่อค่าเปลี่ยนเพื่อไม่ให้เลขทับซ้อนกันและค่าอื่นก็เป็นลักษณะเดียวกัน
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

    if(soil_EC!=soil_EC_DE){
      tft.setTextColor(TFT_WHITE);
       tft.setCursor(625, 100+180+15);
       tft.println(soil_EC_DE);
       soil_EC_DE=soil_EC;
    }

    tft.setTextColor(TFT_BLACK);
    tft.setCursor(625, 100+180+15);
    tft.println(soil_EC);

    //เงื่อนไขในการแสดงข้อความเมื่อทำการส่งข้อมูลขึ้น Google Sheet
    if (tft.getTouch(&tp)) {
    if (tp.x >= 290 && tp.x <= 500 && tp.y >= 410 && tp.y <= 460){
      if (!wasTouched) {
        tft.setTextColor(TFT_BLACK);
        tft.setCursor(300+50,410-100);
        tft.println("Success");
        if(pSWiFi==2){
        tft.setTextColor(TFT_WHITE);
        tft.setCursor(300+15,410-100);
        tft.println("Please wait..");
        Success_Sheet=1;
        timeCe=millis();}

        wasTouched = true;}}
  }
  else {
    wasTouched = false;
  }
 //เงื่อนไขในการส่งข้อมูลขึ้น Google Sheet
  if(Success_Sheet==1){
    if(co<=co_coco_DE){
      if(millis()-timeCe>=3000){
        Google_Sheet();
        timeCe=millis();
        co=co+1;
      }
    }
    
    else{
      Success_Sheet=0;
      co=1;
      tft.setTextColor(TFT_BLACK);
      tft.setCursor(300+15,410-100);
      tft.println("Please wait..");

      tft.setTextColor(TFT_WHITE);
      tft.setCursor(300+50,410-100);
      tft.println("Success");
    }
    
  }

  }
  //เงื่อนไขในการสร้างแป้นพิมและสัมผัส
  if(Sceenn==3){
    if(NO_coco!=NO_coco_DE){//เงือ่ไขในการลบค่าเดิมเมื่อค่าเปลี่ยนเพื่อไม่ให้เลขทับซ้อนกันและค่าอื่นก็เป็นลักษณะเดียวกัน
      tft.setTextColor(TFT_WHITE);
       tft.setCursor(165, 100+15);
       tft.println(NO_coco_DE);
       NO_coco_DE=NO_coco;
    }
    tft.setTextColor(TFT_BLACK);
    tft.setCursor(165, 100+15);
    tft.println(NO_coco);

    if(co_coco!=co_coco_DE){
      tft.setTextColor(TFT_WHITE);
       tft.setCursor(625, 100+15);
       tft.println(co_coco_DE);
       co_coco_DE=co_coco;
    }
    tft.setTextColor(TFT_BLACK);
    tft.setCursor(635, 100+15);
    tft.println(co_coco);




    //พิกัดการสำผัสเพื่อแสดงแป้นพิม(เลขแปรง)
    if (tft.getTouch(&tp)) {
    if (tp.x >= 70 && tp.x <= 270 && tp.y >= 100 && tp.y <= 150){
      if (!wasTouched) {
      no_time(set);
      no_kk=1;
       wasTouched = true;}}
  }
  else {
    wasTouched = false;
  }

  if(no_kk==0){
  tft.fillRect(set, 200, 400, 300, TFT_BLACK);
  num_P_DE=0;
  num_C_DE=0;}

  if(no_kk==1){//เงื่อนไขในการแสดงข้อความบนแป้นพิมพ์
    if(num_P!=num_P_DE){
    tft.setFont(&fonts::FreeMonoBold18pt7b);
    tft.setTextColor(TFT_WHITE);
    tft.setCursor(set+40,220);
    tft.println(num_P);
    tft.setTextColor(TFT_BLACK);
    tft.setCursor(set+40,220);
    tft.println(num_P_DE);
    num_P=num_P_DE;
    tft.setFont(&fonts::FreeMonoBold12pt7b);}

    //เงื่อนไขในการสัมผัสเลขต่างๆเพื่อนำไปใช้
    if (tft.getTouch(&tp)) {
      if (tp.x >= set+5 && tp.x <= set+95 && tp.y >= 270 && tp.y <= 330){
        if (!wasTouched) {
            countTu1=countTu1+1;
            sum_NUM1[countTu1]=1;
            wasTouched = true;}}}
      else {wasTouched = false;}

    if (tft.getTouch(&tp)) {
      if (tp.x >= set+105 && tp.x <= set+195 && tp.y >= 270 && tp.y <= 330){
        if (!wasTouched) {
            countTu1=countTu1+1;
            sum_NUM1[countTu1]=2;
            wasTouched = true;}}}
      else {wasTouched = false;}
    
    if (tft.getTouch(&tp)) {
      if (tp.x >= set+205 && tp.x <= set+295 && tp.y >= 270 && tp.y <= 330){
        if (!wasTouched) {
            countTu1=countTu1+1;
            sum_NUM1[countTu1]=3;
            wasTouched = true;}}}
      else {wasTouched = false;}

    
    if (tft.getTouch(&tp)) {
      if (tp.x >= set+5 && tp.x <= set+95 && tp.y >= 340 && tp.y <= 400){
        if (!wasTouched) {
            countTu1=countTu1+1;
            sum_NUM1[countTu1]=4;
            wasTouched = true;}}}
      else {wasTouched = false;}

    if (tft.getTouch(&tp)) {
      if (tp.x >= set+105 && tp.x <= set+195 && tp.y >= 340 && tp.y <= 400){
        if (!wasTouched) {
            countTu1=countTu1+1;
            sum_NUM1[countTu1]=5;
            wasTouched = true;}}}
      else {wasTouched = false;}
    
    if (tft.getTouch(&tp)) {
      if (tp.x >= set+205 && tp.x <= set+295 && tp.y >= 340 && tp.y <= 400){
        if (!wasTouched) {
            countTu1=countTu1+1;
            sum_NUM1[countTu1]=6;
            wasTouched = true;}}}
      else {wasTouched = false;}

    if (tft.getTouch(&tp)) {
      if (tp.x >= set+5 && tp.x <= set+95 && tp.y >= 410 && tp.y <= 470){
        if (!wasTouched) {
            countTu1=countTu1+1;
            sum_NUM1[countTu1]=7;
            wasTouched = true;}}}
      else {wasTouched = false;}

    if (tft.getTouch(&tp)) {
      if (tp.x >= set+105 && tp.x <= set+195 && tp.y >= 410 && tp.y <= 470){
        if (!wasTouched) {
            countTu1=countTu1+1;
            sum_NUM1[countTu1]=8;
            wasTouched = true;}}}
      else {wasTouched = false;}
    
    if (tft.getTouch(&tp)) {
      if (tp.x >= set+205 && tp.x <= set+295 && tp.y >= 410 && tp.y <= 470){
        if (!wasTouched) {;
            countTu1=countTu1+1;
            sum_NUM1[countTu1]=9;
            wasTouched = true;}}}
      else {wasTouched = false;}

    if (tft.getTouch(&tp)) {
      if (tp.x >= set+305 && tp.x <= set+395 && tp.y >= 410 && tp.y <= 470){
        if (!wasTouched) {
            countTu1=countTu1+1;
            if(countTu1==1){countTu1=0;}
            sum_NUM1[countTu1]=0;
            wasTouched = true;}}}
      else {wasTouched = false;}

    //เงื่อไขในการลบตัวเลข
    if (tft.getTouch(&tp)) {
      if (tp.x >= set+305 && tp.x <= set+355 && tp.y >= 210 && tp.y <= 260){
        if (!wasTouched) {
            countTu1=countTu1-1;
            if(countTu1<0){countTu1=0;}
            wasTouched = true;}}}
      else {wasTouched = false;}
    //เงื่อนไขในการนำเลขที่พิมพ์ไปใช้
    if (tft.getTouch(&tp)) {
      if (tp.x >= set+305 && tp.x <= set+395 && tp.y >= 270 && tp.y <= 500){
        if (!wasTouched) {
            no_kk=0;
            NO_coco=num_P_DE;
            wasTouched = true;}}}
      else {wasTouched = false;}
    //เงื่อนไขในการทำให้ตัวเลขมีแค่สองหลัก
    if(countTu1==0){num_P_DE=0;}
    if(countTu1==1){num_P_DE=sum_NUM1[1];}
    if(countTu1==2){num_P_DE=(sum_NUM1[1]*10)+sum_NUM1[2];}
    if(countTu1>=3){countTu1=2;}
    

  }


  //พิกัดการสำผัสเพื่อแสดงแป้นพิม(เลขจำนวนครั้งการเก็บข้อมูล)
  if (tft.getTouch(&tp)) {
    if (tp.x >= 530 && tp.x <= 730 && tp.y >= 100 && tp.y <= 150){
      if (!wasTouched) {
      no_time(set);
      no_kk=2;
       wasTouched = true;}}
  }
  else {
    wasTouched = false;
  }

  //เงื่อนไขในการแสดงข้อความบนแป้นพิมพ์
  if(no_kk==2){
    if(num_C!=num_C_DE){
    tft.setFont(&fonts::FreeMonoBold18pt7b);
    tft.setTextColor(TFT_WHITE);
    tft.setCursor(set+40,220);
    tft.println(num_C);
    tft.setTextColor(TFT_BLACK);
    tft.setCursor(set+40,220);
    tft.println(num_C_DE);
    num_C=num_C_DE;
    tft.setFont(&fonts::FreeMonoBold12pt7b);}

    //เงื่อนไขในการสัมผัสเลขต่างๆเพื่อนำไปใช้
    if (tft.getTouch(&tp)) {
      if (tp.x >= set+5 && tp.x <= set+95 && tp.y >= 270 && tp.y <= 330){
        if (!wasTouched) {
            countTu2=countTu2+1;
            sum_NUM2[countTu2]=1;
            wasTouched = true;}}}
      else {wasTouched = false;}

    if (tft.getTouch(&tp)) {
      if (tp.x >= set+105 && tp.x <= set+195 && tp.y >= 270 && tp.y <= 330){
        if (!wasTouched) {
            countTu2=countTu2+1;
            sum_NUM2[countTu2]=2;
            wasTouched = true;}}}
      else {wasTouched = false;}
    
    if (tft.getTouch(&tp)) {
      if (tp.x >= set+205 && tp.x <= set+295 && tp.y >= 270 && tp.y <= 330){
        if (!wasTouched) {
            countTu2=countTu2+1;
            sum_NUM2[countTu2]=3;
            wasTouched = true;}}}
      else {wasTouched = false;}

    
    if (tft.getTouch(&tp)) {
      if (tp.x >= set+5 && tp.x <= set+95 && tp.y >= 340 && tp.y <= 400){
        if (!wasTouched) {
            countTu2=countTu2+1;
            sum_NUM2[countTu2]=4;
            wasTouched = true;}}}
      else {wasTouched = false;}

    if (tft.getTouch(&tp)) {
      if (tp.x >= set+105 && tp.x <= set+195 && tp.y >= 340 && tp.y <= 400){
        if (!wasTouched) {
            countTu2=countTu2+1;
            sum_NUM2[countTu2]=5;
            wasTouched = true;}}}
      else {wasTouched = false;}
    
    if (tft.getTouch(&tp)) {
      if (tp.x >= set+205 && tp.x <= set+295 && tp.y >= 340 && tp.y <= 400){
        if (!wasTouched) {
            countTu2=countTu2+1;
            sum_NUM2[countTu2]=6;
            wasTouched = true;}}}
      else {wasTouched = false;}

    
    if (tft.getTouch(&tp)) {
      if (tp.x >= set+5 && tp.x <= set+95 && tp.y >= 410 && tp.y <= 470){
        if (!wasTouched) {
            countTu2=countTu2+1;
            sum_NUM2[countTu2]=7;
            wasTouched = true;}}}
      else {wasTouched = false;}

    if (tft.getTouch(&tp)) {
      if (tp.x >= set+105 && tp.x <= set+195 && tp.y >= 410 && tp.y <= 470){
        if (!wasTouched) {
            countTu2=countTu2+1;
            sum_NUM2[countTu2]=8;
            wasTouched = true;}}}
      else {wasTouched = false;}
    
    if (tft.getTouch(&tp)) {
      if (tp.x >= set+205 && tp.x <= set+295 && tp.y >= 410 && tp.y <= 470){
        if (!wasTouched) {;
            countTu2=countTu2+1;
            sum_NUM2[countTu2]=9;
            wasTouched = true;}}}
      else {wasTouched = false;}

    if (tft.getTouch(&tp)) {
      if (tp.x >= set+305 && tp.x <= set+395 && tp.y >= 410 && tp.y <= 470){
        if (!wasTouched) {
            countTu2=countTu2+1;
            if(countTu2==1){countTu2=0;}
            sum_NUM2[countTu2]=0;
            wasTouched = true;}}}
      else {wasTouched = false;}
    //เงื่อไขในการลบตัวเลข
    if (tft.getTouch(&tp)) {
      if (tp.x >= set+305 && tp.x <= set+355 && tp.y >= 210 && tp.y <= 260){
        if (!wasTouched) {
            countTu2=countTu2-1;
            if(countTu2<0){countTu2=0;}
            wasTouched = true;}}}
      else {wasTouched = false;}
    //เงื่อนไขในการนำเลขที่พิมพ์ไปใช้
    if (tft.getTouch(&tp)) {
      if (tp.x >= set+305 && tp.x <= set+395 && tp.y >= 270 && tp.y <= 500){
        if (!wasTouched) {
            no_kk=0;
            co_coco=num_C_DE;
            tft.fillRect(530, 100, 200, 50, TFT_WHITE);
            tft.setCursor(530+20, 100+15);
            tft.println(Text_Collect2[0]);
            wasTouched = true;}}}
      else {wasTouched = false;}
    //เงื่อนไขในการทำให้ตัวเลขมีแค่สามหลัก
    if(countTu2==0){num_C_DE=0;}
    if(countTu2==1){num_C_DE=sum_NUM2[1];}
    if(countTu2==2){num_C_DE=(sum_NUM2[1]*10)+sum_NUM2[2];}
    if(countTu2==3){num_C_DE=(sum_NUM2[1]*100)+(sum_NUM2[2]*10)+sum_NUM2[3];}
    if(countTu2>=4){countTu2=3;}
    
  }

  }

  checkWiFi();

}

//เงื่อนไขในการสร้างแป้นพิมตัวเลข
void no_time(int set){
  tft.fillRect(set, 200, 400, 300, TFT_LIGHTGREY);
  tft.fillRect(set+5, 210, 290, 50, TFT_WHITE);
  tft.fillRect(set+305, 210, 90, 50, TFT_RED);
  tft.fillRect(set+305, 270, 90, 130, TFT_GREEN);
  tft.fillRect(set+5, 270, 90, 60, TFT_WHITE);
  tft.fillRect(set+105, 270, 90, 60, TFT_WHITE);
  tft.fillRect(set+205, 270, 90, 60, TFT_WHITE);

  tft.fillRect(set+5, 340, 90, 60, TFT_WHITE);
  tft.fillRect(set+105, 340, 90, 60, TFT_WHITE);
  tft.fillRect(set+205, 340, 90, 60, TFT_WHITE);

  tft.fillRect(set+5, 410, 90, 60, TFT_WHITE);
  tft.fillRect(set+105, 410, 90, 60, TFT_WHITE);
  tft.fillRect(set+205, 410, 90, 60, TFT_WHITE);
  tft.fillRect(set+305, 410, 90, 60, TFT_WHITE);

  tft.setFont(&fonts::FreeMonoBold18pt7b);
  tft.setTextColor(TFT_BLACK);
  tft.setCursor(set+40,285);
  tft.println("1");
  tft.setCursor(set+140,285);
  tft.println("2");
  tft.setCursor(set+240,285);
  tft.println("3");
  tft.setCursor(set+40,355);
  tft.println("4");
  tft.setCursor(set+140,355);
  tft.println("5");
  tft.setCursor(set+240,355);
  tft.println("6");
  tft.setCursor(set+40,425);
  tft.println("7");
  tft.setCursor(set+140,425);
  tft.println("8");
  tft.setCursor(set+240,425);
  tft.println("9");
  tft.setCursor(set+340,425);
  tft.println("0");
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(set+315,325);
  tft.println("SET");
  tft.setTextColor(TFT_BLACK);
  //เงื่อนไขในการสร้างลูกศร
  int x = set+375; // จุดปลายลูกศร
  int y = 235;  // จุดศูนย์กลางลูกศร
  int size = 50;
  tft.drawLine(x, y, x - size, y, TFT_WHITE);
  tft.drawLine(x - size, y, x - size + 6, y - 6, TFT_WHITE);
  tft.drawLine(x - size, y, x - size + 6, y + 6, TFT_WHITE);

  tft.setFont(&fonts::FreeMonoBold12pt7b);
}

//เงื่อนไขในการตรวจสอบ WiFi
void checkWiFi(){
if(WiFi.status() != WL_CONNECTED)//ทำการเข้าเงื่อนไขเมื่อเชื่อมต่อ WiFi ไม่ได้
  { if(millis()-TimecheckWiFi<=1000){
    tft.setFont(&fonts::FreeMonoBold9pt7b);
    tft.setTextColor(TFT_BLACK);
    tft.setCursor(10, 23);
    if(sWiFi==1){tft.fillRect(10, 23, 250, 25, TFT_BLACK);
    sWiFi=2;}
    tft.println("                   .");
    tft.setTextColor(TFT_WHITE);
    tft.setCursor(10, 23);
    tft.println("connecting to WiFi.");
    }

    if(millis()-TimecheckWiFi<=2000 && millis()-TimecheckWiFi>1000){
    tft.setFont(&fonts::FreeMonoBold9pt7b);
    tft.setTextColor(TFT_WHITE);
    tft.setCursor(10, 23);
    tft.println("                   .");
    }

    if(millis()-TimecheckWiFi>2000){TimecheckWiFi=millis();}
    
    Serial.print(".");//ทำการแสดง . เมื่อเข้าเงื่อนไขเชื่อมต่อ WiFi ไม่ได้
    pSWiFi=1;
  }

if(WiFi.status() == WL_CONNECTED){
  Serial.println("WiFi connected");//เมื่อสามารถเชื่อมต่อ WiFi ได้จะทำการแสดงข้อความขึ้น Serial Monitor
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  tft.setFont(&fonts::FreeMonoBold9pt7b);
  tft.setTextColor(TFT_BLACK);
  tft.setCursor(10, 23);
  if(sWiFi==2){tft.fillRect(10, 23, 250, 25, TFT_BLACK);
  sWiFi=1;}
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(10, 23);
  tft.println("WiFi IP : "+String(WiFi.localIP()));
  pSWiFi=2;
  }

  tft.setFont(&fonts::FreeMonoBold12pt7b);
}

//เงื่อนไขในการส่งข้อมูลขึ้น Google Sheet
void Google_Sheet(){
  HTTPClient http;//สร้างตัวแปรในการส่งข้อมูลไปยัง Google Sheet
        //ทำการสร้างตัวแปรในการเก็บ url ของ App script
        String url = "https://script.google.com/macros/s/AKfycbyFy4OfW6sdaX1NrY2mDMty5GZFe5Ij968zE8cZ18HXzsiqjjHnBTUuXx7dny5j4bfPjw/exec?mois="+String(soil_mois)+"&temp="+String(soil_temp)+"&ph="+String(soil_ph)+"&n="+String(soil_N)+"&p="+String(soil_P)+"&k="+String(soil_K)+"&no="+String(NO_coco_DE)+"&ceee"+String(0);
        Serial.println("Making a request");//ทำการแสดงข้อความเมื่อจะทำการส่งข้อมูลไปยัง google sheet
        http.begin(url.c_str()); //ทำการส่งข้อมูลไปยัง google sheet  
        http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);//ทำการรับสเตต้สของข้อมูลที่ทำการติดตาม
        int httpCode = http.GET();//สร้างตัวแปรเก็บข้อความที่ถูกส่งกลับมา
        String payload;//สร้างตัวแปรเก็บข้อความที่ถูกส่งกลับมา
        if (httpCode > 0) { 
          payload = http.getString();//รับข้อมูลที่ได้ทำการติดตาม
          Serial.println(httpCode);//แสดงเสตัสบน Serial moniter
          Serial.println(payload);//แสดงข้อมูลที่ได้ทำการติดตามบน Serial moniter
        }
        else {//เงื่อนไขเมื่อไม่สามารถส่งข้อมูลไปยัง Google Sheet ได้
          Serial.println("Error on HTTP request");//แสดงข้อความบน Serial moniter เมื่อไม่สามารถส่วข้อมูลไปยัง Google Sheet ได้
        }
        http.end();//สิ้นสุดการใช้งานการส่งข้อมูลขึ้น Google Sheet (ควรจะสิ้นสุดการใช้งานทุกครั้งเมื่อการส่งเสร็จสิ้นเพื่อที่จะสามารถส่งข้อมูลไปยัง Google Sheet ได้ในครั้งถัดไป)

}




//การอ่านเซนเซอร์ NPK 

float moisture() {
  a_uint16;
  a_sint16;  
  a_float;
  startTime = 0;
  byteCount = 0;
  
  digitalWrite(RE, HIGH);
  mod.write(mois, sizeof(mois));
  mod.flush();
  digitalWrite(RE, LOW);

  startTime = millis();
  while ( millis() - startTime <= TIMEOUT ) {
    if (mod.available() && byteCount<sizeof(values) ) {
      values[byteCount++] = mod.read();
    }
    Sceen1_2();
  }

  a_uint16 = values[3]<<8;
  a_uint16 |= values[4];
  a_sint16 = (int)a_uint16;
  a_float = (float)a_sint16;
  a_float /=  10.0;

  return a_float;
}


float temperature() {
  word   a_uint16;
  int    a_sint16;  
  float  a_float;
  uint32_t startTime = 0;
  uint8_t  byteCount = 0;
  
  digitalWrite(RE, HIGH);
  mod.write(temp, sizeof(temp));
  mod.flush();
  digitalWrite(RE, LOW);

  startTime = millis();
  while ( millis() - startTime <= TIMEOUT ) {
    if (mod.available() && byteCount<sizeof(values) ) {
      values[byteCount++] = mod.read();
    }
    Sceen1_2();
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
  
  digitalWrite(RE, HIGH);
  mod.write(econ, sizeof(econ));
  mod.flush();
  digitalWrite(RE, LOW);

  startTime = millis();
  while ( millis() - startTime <= TIMEOUT ) {
    if (mod.available() && byteCount<sizeof(values) ) {
      values[byteCount++] = mod.read();
    }
    Sceen1_2();
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
  
  digitalWrite(RE, HIGH);
  mod.write(ph, sizeof(ph));
  mod.flush();
  digitalWrite(RE, LOW);

  startTime = millis();
  while ( millis() - startTime <= TIMEOUT ) {
    if (mod.available() && byteCount<sizeof(values) ) {
      values[byteCount++] = mod.read();
    }
    Sceen1_2();
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
  
  digitalWrite(RE, HIGH);
  mod.write(nitro, sizeof(nitro));
  mod.flush();
  digitalWrite(RE, LOW);

  startTime = millis();
  while ( millis() - startTime <= TIMEOUT ) {
    if (mod.available() && byteCount<sizeof(values) ) {
      values[byteCount++] = mod.read();
    }
    Sceen1_2();
  }
  return values[4];
}

byte phosphorous() {
  uint32_t startTime = 0;
  uint8_t  byteCount = 0;
  
  digitalWrite(RE, HIGH);
  mod.write(phos, sizeof(phos));
  mod.flush();
  digitalWrite(RE, LOW);

  startTime = millis();
  while ( millis() - startTime <= TIMEOUT ) {
    if (mod.available() && byteCount<sizeof(values) ) {
      values[byteCount++] = mod.read();
    }
    Sceen1_2();
  }
  return values[4];
}

byte potassium() {
  uint32_t startTime = 0;
  uint8_t  byteCount = 0;
  
  digitalWrite(RE, HIGH);
  mod.write(pota, sizeof(pota));
  mod.flush();
  digitalWrite(RE, LOW);

  startTime = millis();
  while ( millis() - startTime <= TIMEOUT ) {
    if (mod.available() && byteCount<sizeof(values) ) {
      values[byteCount++] = mod.read();
    }
    Sceen1_2();
  }
  return values[4];
}


