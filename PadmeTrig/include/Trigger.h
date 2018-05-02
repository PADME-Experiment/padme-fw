#ifndef _Trigger_H_
#define _Trigger_H_

#define TRIG_OK 0
#define TRIG_ERROR 1

int trig_init();
int trig_end();
int trig_get_data(void*); // buffer
int trig_get_temperature(float*); // temperature
int trig_get_trigbusymask(char*); // 8b+8b trigger+busy mask
int trig_get_trigmask(char*); // 8b trigger mask
int trig_get_busymask(char*); // 8b busy mask
int trig_set_trigmask(char); // 8b trigger mask
int trig_set_busymask(char); // 8b busy mask
int trig_start_run();

#endif
