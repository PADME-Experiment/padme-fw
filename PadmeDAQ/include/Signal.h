#ifndef _Signal_H_
#define _Signal_H_

//int InBurst; // =0 DAQ not running, >0 DAQ running

//int BreakSignal = 0; // If >0 an interrupt was received and DAQ should exit

void termination_handler(int); // signal id
void set_signal_handlers();

#endif
