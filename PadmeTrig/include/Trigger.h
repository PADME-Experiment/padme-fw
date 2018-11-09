#ifndef _Trigger_H_
#define _Trigger_H_

#define TRIG_OK 0
#define TRIG_ERROR 1

int trig_init();
int trig_end();
int trig_get_data(void*,unsigned int*); // buffer,length
int trig_get_temperature(float*); // temperature
int trig_get_trigbusymask(char*); // 8b+8b trigger+busy mask
int trig_get_trigmask(char*); // 8b trigger mask
int trig_get_busymask(char*); // 8b busy mask
int trig_set_trigmask(char); // 8b trigger mask
int trig_set_busymask(char); // 8b busy mask
int trig_start_run();
int trig_get_register(unsigned char,unsigned int*); // register number, 4B word output
int trig_set_register(unsigned char,unsigned int); // register number, 4B word input
#endif
