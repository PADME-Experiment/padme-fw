#ifndef _ZSUP_H_
#define _ZSUP_H_

int ZSUP_readdata();
unsigned int apply_zero_suppression (unsigned int,unsigned int,void *,void *); // flag, algorithm, in buffer, out buffer
unsigned int zsup_algorithm_1(void *,void *); // in buffer, out buffer
unsigned int zsup_algorithm_2(unsigned int,void *,void *); // channel, in buffer, out buffer

#endif
