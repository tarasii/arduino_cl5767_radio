
#include "cl5767.h" 
#include <MD_DS3231.h>
#include <Wire.h>
#include "i2c.h"

#include <PCD8544.h>

#define PCD8544_CLK 6
#define PCD8544_DIN 5
#define PCD8544_DC 4
#define PCD8544_CE 3
#define PCD8544_RST 2
#define PCD8544_LIGHT 7

#define ENC_A 9
#define ENC_B 8
#define ENC_C 10

#define LED 13

#define BTN_CNTR 678
#define BTN_LEFT 670
#define BTN_RGHT 665
#define BTN_UP   651
#define BTN_DOWN 638
#define BTN_ESC  625

#define CMD_CHAN_UP 1
#define CMD_CHAN_DOWN 2
#define CMD_VOL_UP 3
#define CMD_VOL_DOWN 4
#define CMD_SEEK_UP 5
#define CMD_SEEK_DOWN 6
#define CMD_BASS 7
#define CMD_MUTE 8
#define CMD_SMUTE 9
#define CMD_INFO 10 

//unsigned char buf[30];
  
bool blnk;
bool first = true;

PCD8544 display = PCD8544(PCD8544_CLK, PCD8544_DIN, PCD8544_DC, PCD8544_RST, PCD8544_CE);
CL5767 cl5767;

//uint32_t sttns[9]={95600,100000,100500,101100,101500,102500,103100,107400,107900};
uint32_t sttns[8]={95600,100000,100500,101500,102500,103100,107400,107900};
int8_t cur_sttn = 0;
uint16_t tmp_btn;
int8_t cur_vol = MAX_VOL;
uint8_t cur_cmd = 0;

uint32_t cur_mil, tmp_mil;

int8_t cnt_enc = 0;
uint8_t tmp_enc;

//void printBuf(uint8_t *buf, uint8_t len){
//  uint8_t i, m, n;
//  for (i=0;i<len;i++){ 
//    m = i % 2;
//    n = i / 2;
//    if (!m){
//      Serial.print(n);
//      Serial.print(":");
//    }   
//    if (*buf< 0x10) Serial.print("0");
//    Serial.print(*buf, HEX);     
//    Serial.print(" ");
//    if (m) {
//      Serial.println("");
//    }
//    buf++;
//  }
//  Serial.println(" "); 
//}

void setup() {
  
  Serial.begin(9600);
  Serial.println("Start"); 

  pinMode(LED, OUTPUT);
  pinMode(PCD8544_LIGHT, OUTPUT);

  pinMode (ENC_A, INPUT);
  pinMode (ENC_B, INPUT);
  pinMode (ENC_C, INPUT);
  
  digitalWrite(PCD8544_LIGHT, HIGH);
      
  display.begin(84, 48);              // Инициализация дисплея
  display.setContrast(60);      // Устанавливаем контраст
  display.clear(); 

  Wire.begin();
    
  display.setCursor(0, 0);
  display.print("hello");
  delay(500);
  
  tmp_mil = millis();

  cl5767.init();
  cl5767.print_state();
  
  cl5767.turn_on();
  cl5767.set_vol(cur_vol);
  cl5767.tune(sttns[cur_sttn]);

  cl5767.read_buf();  
  cl5767.print_state_hex();
  cl5767.print_state(); 
  
  delay(500);
     
//  scan_i2c();
  
  //display.setBacklight(true);

  tmp_enc = digitalRead(ENC_A);

}

void loop() {
  uint8_t i;
  char tmp;
  uint8_t cur_enc, zzz;
  uint16_t cur_btn;

  cur_btn = analogRead(A0);

  cur_enc = digitalRead(ENC_A);
  if (cur_enc != tmp_enc) {
    if (digitalRead(ENC_B) != cur_enc) {
      cnt_enc++; 
      cur_cmd = CMD_VOL_UP; 
    } else {
      cnt_enc--;
      cur_cmd = CMD_VOL_DOWN;
    }

    tmp_enc = cur_enc;
  }

  if (Serial.available()){
    tmp = Serial.read();
    tmp_btn = 0;
    switch (tmp){
      case 'c':
        cur_btn = BTN_RGHT;
        break;
      case 'z':
        cur_btn = BTN_LEFT;
        break;
      case 's':
        cur_btn = BTN_UP;
        break;
      case 'x':
        cur_btn = BTN_DOWN;
        break;
      case 'd':
        cur_cmd = CMD_SEEK_UP;
        break;
      case 'a':
        cur_cmd = CMD_SEEK_DOWN;
        break;
      default:
        cur_cmd = CMD_INFO;
    }
  }

//  if (tmp_btn!=0 && cur_btn!=0){
//    Serial.print(cur_btn);
//    Serial.print(" ");
//    Serial.println(tmp_btn);
//  }
  
  if (abs((int16_t) tmp_btn-cur_btn)>3){
    if (cur_btn <= BTN_LEFT+1 && cur_btn >= BTN_LEFT-1) {
      cur_cmd = CMD_CHAN_DOWN;
    }
    if (cur_btn <= BTN_RGHT+1 && cur_btn >= BTN_RGHT-1) {
      cur_cmd = CMD_CHAN_UP;
    }
    tmp_btn = cur_btn;
  }
  
  if (cur_btn != 0){
    if (cur_btn <= BTN_DOWN+1 && cur_btn >= BTN_DOWN-1) {
      cur_cmd = CMD_VOL_DOWN;
    }
    if (cur_btn <= BTN_UP+1 && cur_btn >= BTN_UP-1) {
      cur_cmd = CMD_VOL_UP;
    }
  }
  
  cur_mil = millis();
  if (cur_mil - tmp_mil > 500){
    tmp_mil = cur_mil;

    display.setCursor(0, 5);  
    display.print("btn:");
    display.print(cur_btn);
    display.print(" ");
    display.print("enc:");
    display.print(cnt_enc);
    display.print("   ");
          
    RTC.readTime();

    display.setCursor(0, 0);
    display.print(RTC.yyyy, DEC); 
    display.print("-");
    if (RTC.mm < 10) display.print(0);
    display.print(RTC.mm, DEC); 
    display.print("-"); 
    if (RTC.dd < 10) display.print(0);
    display.print(RTC.dd, DEC);
    display.setCursor(0, 1);
    if (RTC.h < 10) display.print(0);
    display.print(RTC.h, DEC);
    display.print(":"); 
    if (RTC.m < 10) display.print(0);
    display.print(RTC.m, DEC);
    display.print(":"); 
    if (RTC.s < 10) display.print(0);
    display.print(RTC.s, DEC);


    cl5767.read_status();
    //cl5767.print_state_hex();
    
    display.setCursor(0, 2);
    display.print("freq:");
    display.print(cl5767.get_freq());
    //display.print(tmp);
    display.print(" ");
  
    display.setCursor(0, 3);
    display.print("vol:");
    display.print(cl5767.get_vol());
    if (cl5767.get_stereo()) display.print(" STEREO");
    else display.print(" MONO   "); 
  
    display.setCursor(0, 4);
    display.print("rssi:");
    display.print(cl5767.get_rssi());
    display.print(" e:");
    display.print(cl5767.get_envlp_num());
    display.print("  ");
            
    if (blnk) {
      digitalWrite(LED, HIGH);
      blnk = false;
    }
    else {
      digitalWrite(LED, LOW);
      blnk = true;
    }


    switch (cur_cmd){
      case CMD_CHAN_DOWN:
        if (--cur_sttn<0) cur_sttn=7; 
        cl5767.tune(sttns[cur_sttn]);
        break;
      case CMD_CHAN_UP:
        if (++cur_sttn>=8) cur_sttn=0;
        cl5767.tune(sttns[cur_sttn]);
        break;
      case CMD_SEEK_DOWN:
        cl5767.seek_chanel(false);
        break;
      case CMD_SEEK_UP: 
        cl5767.seek_chanel(true);
        break;
      case CMD_VOL_DOWN:
        if (--cur_vol<0) cur_vol=0;
        cl5767.set_vol(cur_vol);
        cl5767.write_func();
        break;
      case CMD_VOL_UP:
        if (++cur_vol>=MAX_VOL) cur_vol=MAX_VOL;
        cl5767.set_vol(cur_vol);
        cl5767.write_func();
        break;    
    }
    if (cur_cmd!=0){
      cl5767.read_status();
      cl5767.print_state_hex();
      Serial.print("cmd:");
      Serial.println(cur_cmd);
      cl5767.print_state();

      Serial.print(RTC.yyyy, DEC); 
      Serial.print("-"); 
      Serial.print(RTC.mm, DEC); 
      Serial.print("-"); 
      Serial.print(RTC.dd, DEC);
      Serial.print(" "); 
      Serial.print(RTC.h, DEC);
      Serial.print(":"); 
      Serial.print(RTC.m, DEC);
      Serial.print(":"); 
      Serial.print(RTC.s, DEC);
      Serial.println("");  
          
      cur_cmd = 0;
    }    

  }




}
