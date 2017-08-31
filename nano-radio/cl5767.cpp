#include "cl5767.h" 

void ser_print_u16h(uint16_t v){
  uint16_t i;
  for(i=0xfff;i;i>>=4) if (v < i) Serial.print('0');
  Serial.print(v, HEX);
}

void ser_print_u16b(uint16_t v){
  uint16_t i;
  for(i=0x8000;i;i>>=1) {
    if (i == 0x80) Serial.print(' ');
    if (i & v) Serial.print('1'); else Serial.print('0');
  }
}

void ser_print_u16hb(uint16_t v){
  ser_print_u16h(v);
  Serial.print(' ');
  ser_print_u16b(v);
}


CL5767::CL5767(){
  if (TWCR == 0) Wire.begin();
}


void CL5767::read_buf(){
  //readDevAddr(CL5767_BASE_ADDR, 0, this->buf, CL5767_CTRL_DATA_LEN);
  readDev(CL5767_BASE_ADDR, this->buf, CL5767_CTRL_DATA_LEN);
  this->buf_to_ctrl(); 
}


void CL5767::read_status(){
  //readDevAddr(CL5767_BASE_ADDR, 0, this->buf, 6);
  readDev(CL5767_BASE_ADDR, this->buf, 6);
  this->buf_to_ctrl(); 
}

void CL5767::init(){
  CL5767::read_buf(); 
}

void CL5767::buf_to_ctrl(){

  uint8_t i;
  uint16_t * ptr;

  ptr = (uint16_t*) &this->ctrl_data;
  for (i=0; i<9; i++){
    *ptr = (this->buf[i*2]  << 8) + this->buf[i*2+1];
    ptr++;
  }
}

void CL5767::ctrl_to_buf(){

  uint8_t i;
  uint16_t * ptr;

  ptr = (uint16_t*) &this->ctrl_data;
  for (i=0; i<9; i++){
    this->buf[i*2+1] = *ptr & 0xff;
    this->buf[i*2] = *ptr >> 8;
    ptr++;
  }
}

void CL5767::print_buf(){
  uint8_t i, m, n;
  for (i=0;i<CL5767_CTRL_DATA_LEN;i++){ 
    m = i % 2;
    n = i / 2;
    if (!m){
      Serial.print(n);
      Serial.print(":");
    }   
    if (this->buf[i]<16) Serial.print("0");
    Serial.print(this->buf[i], HEX);     
    Serial.print(" ");
    if (m) {
      Serial.println("");
    }
  }
  Serial.println(" "); 
}

void CL5767::set_vol(uint8_t vol){
  this->ctrl_data.vol_freq &= ~CL5767_MASK_VOL; 
  this->ctrl_data.vol_freq |= (vol << 10) & CL5767_MASK_VOL;
}  

void CL5767::set_freq(uint32_t freq){
  uint16_t tmp = 0;
  
  tmp = (freq - 70000)/50 & CL5767_MASK_FREQ;
  this->ctrl_data.vol_freq &= ~CL5767_MASK_FREQ;   
  this->ctrl_data.vol_freq |= tmp;
}  
  
uint32_t CL5767::get_freq(){
//  return 70000 + (this->ctrl_data.vol_freq & CL5767_MASK_FREQ) * 50;
  return 70000 + (this->ctrl_data.read_chan & CL5767_MASK_FREQ) * 50;
}
  
uint8_t CL5767::get_vol(){
  return (uint8_t) ((this->ctrl_data.vol_freq & CL5767_MASK_VOL) >> 10);  
}  
  
uint8_t CL5767::get_rssi(){
  return (uint8_t) (this->ctrl_data.status_rssi & CL5767_MASK_RSSI) - 162;  
}  
  
bool CL5767::get_stereo(){
  return (bool) ((this->ctrl_data.status_rssi & CL5767_MASK_STEREO) >> 8);  
}  

uint8_t CL5767::get_envlp_num(){
  return (uint8_t) ((this->ctrl_data.read_chan & CL5767_MASK_ENVN) >>10);  
} 

void CL5767::set_func(uint16_t v){
  this->ctrl_data.func = v;
}


void CL5767::enable(){
  this->ctrl_data.func &= ~CL5767_FN_DISABLE;
}

void CL5767::disable(){
  this->ctrl_data.func |= CL5767_FN_DISABLE;
}

void CL5767::set_dsp(bool st){
  if (st) this->ctrl_data.func |=  CL5767_FN_DSP;
  else    this->ctrl_data.func &= ~CL5767_FN_DSP; 
}

void CL5767::dsp_on(){
  this->set_dsp(true);
}

void CL5767::turn_on(){
  this->enable();
  this->dsp_on();
  this->set_mute(false);
  this->set_lmute(false);
  this->set_rmute(false);
  this->set_smute(true);
  this->set_stereo(false);
  this->write_func();
}

void CL5767::tune(uint32_t freq){
  this->set_tune(false);
  this->set_seek(false, false);
  this->set_freq(freq);
  this->write_func();
  delay(5);  
  this->set_tune(true);
  this->write_func();  
  delay(50);
}

void CL5767::seek_chanel(bool dir){
  this->set_tune(false);
  this->set_seek(false, dir);
  this->write_func();
  delay(5);  
  this->set_seek(true, dir);
  this->write_func();  
}

void CL5767::set_mute(bool mute){ 
  if (mute) this->ctrl_data.func |=  CL5767_FN_MUTE; 
  else      this->ctrl_data.func &= ~CL5767_FN_MUTE; 
}

void CL5767::set_lmute(bool mute){
  if (mute) this->ctrl_data.func |=  CL5767_FN_LMUTE; 
  else      this->ctrl_data.func &= ~CL5767_FN_LMUTE; 
}

void CL5767::set_rmute(bool mute){
  if (mute) this->ctrl_data.func |=  CL5767_FN_RMUTE; 
  else      this->ctrl_data.func &= ~CL5767_FN_RMUTE; 
}

void CL5767::set_smute(bool mute){
  if (mute) this->ctrl_data.func &= ~CL5767_FN_SMUTE; 
  else      this->ctrl_data.func |=  CL5767_FN_SMUTE; 
}

void CL5767::set_tune(bool tune){
  if (tune) this->ctrl_data.func |=  CL5767_FN_TUNE; 
  else      this->ctrl_data.func &= ~CL5767_FN_TUNE; 
}

void CL5767::set_stereo(bool st){
  if (st) this->ctrl_data.func |=  CL5767_FN_STEREO; 
  else    this->ctrl_data.func &= ~CL5767_FN_STEREO; 
}

void CL5767::set_bass(bool st){
  if (st) this->ctrl_data.func |=  CL5767_FN_BB; 
  else    this->ctrl_data.func &= ~CL5767_FN_BB; 
}

void CL5767::set_seek(bool st, bool dir){
  if (st)  this->ctrl_data.func |=  CL5767_FN_SEEK; 
  else     this->ctrl_data.func &= ~CL5767_FN_SEEK; 
  if (dir) this->ctrl_data.func |=  CL5767_FN_SEEK_UP; 
  else     this->ctrl_data.func &= ~CL5767_FN_SEEK_UP; 
}

void CL5767::write_func(){
  this->ctrl_to_buf();
  writeDev(0x10, this->buf+6, 4);
}
  
void CL5767::print_state(){

  Serial.println("---");

  Serial.print("vol:");
  Serial.println(this->get_vol());
    
  Serial.print("freq:");
  Serial.println(this->get_freq());

  Serial.print("rssi:");
  Serial.println(this->get_rssi());
  
  Serial.print("envrlop number:");
  Serial.println(this->get_envlp_num());
  
  if (this->get_stereo()) Serial.println("STEREO");
  else Serial.println("MONO"); 
   
}

void CL5767::print_state_hex(){
  uint8_t i;
  uint16_t * ptr;

  Serial.println("---");

  ptr = (uint16_t*) &this->ctrl_data;
  for (i=0; i<9; i++){
    Serial.print(i);
    Serial.print(":");
    ser_print_u16hb(*ptr++);
    Serial.println("");
  } 
   
}


