#ifndef _Trigger_H_
#define _Trigger_H_

#define TRIG_OK    0
#define TRIG_ERROR 1
#define TRIG_UNDEF 2

//int trig_init();
int trig_init(char*, unsigned short int);
int trig_end();
int trig_get_data(void*,unsigned int*); // buffer,length
int trig_get_temperature(float*); // temperature

int trig_start_run();
int trig_stop_run();

int trig_get_fw_version(unsigned int*); // 32b firmware version

int trig_get_trigbusymask(unsigned char*); // 32b register 0x02: obsolete do not use

int trig_get_trigmask(unsigned char*); // 8b trigger mask
int trig_set_trigmask(unsigned char); // 8b trigger mask

int trig_enable_trigger(unsigned char); // trigger number (0-7)
int trig_disable_trigger(unsigned char); // trigger number (0-7)

int trig_get_busymask(unsigned char*); // 8b busy mask
int trig_set_busymask(unsigned char); // 8b busy mask

int trig_get_correlated_delay(unsigned short int*); // 16b correlated trigger delay in clock counts
int trig_set_correlated_delay(unsigned short int); // 16b correlated trigger delay in clock counts

int trig_get_timepix_delay(unsigned char*); // 8b timepix shutter delay wrt trigger in clock counts
int trig_set_timepix_delay(unsigned char); // 8b timepix shutter delay wrt trigger in clock counts

int trig_get_timepix_width(unsigned char*); // 8b timepix shutter width in 100ns counts
int trig_set_timepix_width(unsigned char); // 8b timepix shutter width in 100ns counts

int trig_get_trigger0_delay(unsigned char*); // 8b trigger0 distribution delay in clock counts
int trig_set_trigger0_delay(unsigned char); // 8b trigger0 distribution delay in clock counts

int trig_get_trigger_global_factor(unsigned char,unsigned short int*); // trigger (0-7),16b global demultiplication factor
int trig_set_trigger_global_factor(unsigned char,unsigned short int); // trigger (0-7),16b global demultiplication factor

int trig_get_trigger_autopass_factor(unsigned char,unsigned short int*); // trigger (0-7),16b autopass demultiplication factor
int trig_set_trigger_autopass_factor(unsigned char,unsigned short int); // trigger (0-7),16b autopass demultiplication factor

int trig_get_register(unsigned char,unsigned char*); // register number, 32b word output
int trig_set_register(unsigned char,unsigned char*); // register number, 32b word input
#endif
