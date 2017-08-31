#include "Arduino.h"
#include <Wire.h>
#include "i2c.h"

#define CL5767_CTRL_DATA_LEN 18
#define CL5767_BASE_ADDR 0x10

#define CL5767_MASK_VOL    0x3c00 //0011110000000000
#define CL5767_MASK_FREQ   0x03ff //0000001111111111
#define CL5767_MASK_RSSI   0x00ff //
#define CL5767_MASK_ENVN   0xfc00 //1111110000000000
#define CL5767_MASK_STEREO 0x0100 //0000000100000000

#define CL5767_FN_DISABLE   0x8000
#define CL5767_FN_DSP       0x4000
#define CL5767_FN_MUTE      0x2000
#define CL5767_FN_LMUTE     0x1000
#define CL5767_FN_RMUTE     0x0800
#define CL5767_FN_SMUTE     0x0400
#define CL5767_FN_TUNE      0x0200
#define CL5767_FN_SEEK      0x0100
#define CL5767_FN_SEEK_UP   0x0080
#define CL5767_FN_MONO      0x0040
#define CL5767_FN_STEREO    0x0020
#define CL5767_FN_HCC       0x0010
#define CL5767_FN_BB        0x0008
#define CL5767_FN_HLSI_AUTO 0x0002
#define CL5767_FN_HLSI_HIGH 0x0002

#define MAX_VOL 0x0f

struct cl5767_ctrl
{
  uint16_t status_rssi;
  uint16_t read_chan;
  uint16_t qos;
  uint16_t func;
  uint16_t vol_freq;
  uint16_t sys_config;
  uint16_t aplln;  
  uint16_t apllk;
  uint16_t seek_th;      
};


class CL5767 
{
  private:
  
    uint8_t vol;
    uint16_t freq;
  
    uint8_t buf[CL5767_CTRL_DATA_LEN];

    void buf_to_ctrl();    
    void ctrl_to_buf();

    cl5767_ctrl ctrl_data;
  
  public: 
    CL5767();

    void init();

    void read_buf();
    void read_status();
    
    void set_vol(uint8_t vol);
    void set_freq(uint32_t freq);
    
    void set_func(uint16_t v);
       
    void write_func();

    void enable();
    void disable();
    void set_dsp(bool st);
    void dsp_on();
    void set_mute(bool mute);
    void set_lmute(bool mute);
    void set_rmute(bool mute);
    void set_smute(bool mute);
    void set_tune(bool tune); 
    void set_stereo(bool st);
    void set_bass(bool st);
    void set_seek(bool st, bool dir);

    void turn_on();
    void tune(uint32_t freq);
    void seek_chanel(bool dir);
        
    uint32_t get_freq();
    uint8_t get_vol();  
    uint8_t get_rssi();
    uint8_t get_envlp_num();
    bool get_stereo();

    void print_state();

    void print_state_hex();
    void print_buf();
};

