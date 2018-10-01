#ifndef _FAKE_H_
#define _FAKE_H_

int FAKE_readdata();
unsigned int create_fake_event(unsigned int,unsigned int,void *); // event nr, trigger time tag, out buffer
void generate_trigger(void *); // out buffer
void generate_channel(unsigned int, void *); // channel, out buffer

#endif
