#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>

#include "Trigger.h"

#define MSG_SIZE 4096
//#define SERVER_PORT 7
//#define SERVER_ADDRESS "192.168.60.100"

#define TRIG_CMD_READ 0x00
#define TRIG_CMD_WRITE 0x01

#define TRIG_REG_START_STOP_RUN 0x00
#define TRIG_BIT_START_RUN 0x01
#define TRIG_BIT_STOP_RUN 0x02

#define TRIG_REG_BUSY_TRIG_MASKS 0x02
#define TRIG_REG_TIMEPIX_SHUTTER 0x05

#define TRIG_REG_TRIGGER_BASE 0x08

#define TRIG_REG_FW_VERSION 0x1D

#define TRIG_CMD_TIMESTAMP 0x02
#define TRIG_REG_TIMESTAMP 0x00

#define TRIG_CMD_READ_I2C_TEMP 0x04
#define TRIG_REG_READ_I2C_TEMP 0x00

#define TRIG_CMD_READ_XADC_TEMP 0x04
#define TRIG_REG_READ_XADC_TEMP 0x01

// === Trigger registers ===
//
// 0x00 SC Start/stop run, busy reset
//         [0] start run, [1] stop run, [7:4] reset busy_in counters (3:0), [8] reset CPU busy counter
//
// 0x01 RW Reset programmable logic, define post trigger dead time
//         [0] programmable logic reset, [31:16] dead time in clock cycles (def: 0x3e7f = 15999 = 200us)
//
// 0x02 RW Trigger mask, busy mask, correlated trigger delay
//         [5:0] input triggers mask (def: 0 = all off)
//         [6] uncorrelated trigger (def: 0 = off), [7] correlated trigger (def: 0 = off)
//         [11:8] busy_in mask (3:0) (def: 0 = all off), [12] CPU busy (def: 1 = on)
//         [31:16] correlated trigger delay in us (def: 0x01f4 = 500 us)
//
// 0x03 RW run window duration (low 32bit)
// 0x04 RW run window duration (high 32bit)
//         [63:0] run duration in clock cycles (def: 0xffffffffffffffff = almost unlimited)
//
// 0x05 RW TimePix3 shutter delay and width
//         [7:0] TimePix3 shutter delay from trig out in clock counts (def: 0x02 = 2 = 25ns)
//         [15:8] TimePix3 shutter window width in 100ns counts (def: 0x64 = 100 = 10us)
//         [23:16] Reserved
//         [31:24] Trigger 0 (BTF trigger) distribution delay in clock counts (def: 0x7E = 126 = 1.575us)
//
// 0x06 RW trigger 0 (BTF trigger) start of pre-veto wrt start of previous trigger
//         [31:0] pre-veto width in clock cycles (def: 0x001829ef = 1583599 = 19.794988ms
//                i.e. 205us before next trigger assuming a 50Hz trigger rate)
//
// 0x07 RW trigger 0 (BTF trigger) pre-veto timeout if next BTF trigger does not arrive
//         [31:0] pre-veto timeout in clock cycles (def: 0x04c4b3ff = 79999999 = 1s)
//
// 0x08 RW trigger 0 (BTF trigger) autopass demultiplication factor (WARNING: no global demultiplication!)
//                                                  [31:16] autopass demultiplication (def: 0, autopass off)
// 0x09 RW trigger 1 global demultiplication factor and autopass demultiplication factor
//         [15:0] global demultiplication (def: 1), [31:16] autopass demultiplication (def: 0, autopass off)
// 0x0a RW trigger 2 global demultiplication factor and autopass demultiplication factor
//         [15:0] global demultiplication (def: 1), [31:16] autopass demultiplication (def: 0, autopass off)
// 0x0b RW trigger 3 global demultiplication factor and autopass demultiplication factor
//         [15:0] global demultiplication (def: 1), [31:16] autopass demultiplication (def: 0, autopass off)
// 0x0c RW trigger 4 global demultiplication factor and autopass demultiplication factor
//         [15:0] global demultiplication (def: 1), [31:16] autopass demultiplication (def: 0, autopass off)
// 0x0d RW trigger 5 global demultiplication factor and autopass demultiplication factor
//         [15:0] global demultiplication (def: 1), [31:16] autopass demultiplication (def: 0, autopass off)
// 0x0e RW trigger 6 (uncorrelated trigger) global demultiplication factor and autopass demultiplication factor
//         [15:0] global demultiplication (def: 1), [31:16] autopass demultiplication (def: 0, autopass off)
// 0x0f RW trigger 7 (correlated trigger) global demultiplication factor and autopass demultiplication factor
//         [15:0] global demultiplication (def: 1), [31:16] autopass demultiplication (def: 0, autopass off)
//
// 0x14 RO busy_in 0 counter in clock cycles
// 0x15 RO busy_in 1 counter in clock cycles
// 0x16 RO busy_in 2 counter in clock cycles
// 0x17 RO busy_in 3 counter in clock cycles
// 0x18 RO CPU busy counter in clock cycles
//
// 0x19 RO busy register
//      [3:0] busy_in (3:0), [4] CPU busy
//
// 0x1D RO firmware version

// === Trigger data format (64bit) ===
// [39:0]  timestamp in clock cycles, resets in 13744s = 3h49m
// [47:40] trigger map (7:0), 0 = BTF, 6 = uncorrelated, 7 = correlated
// [55:48] trigger counter, resets at 256 triggers
// [56]    fifo empty (technical detail, ignore)
// [57]    trigger type (0: standard, 1: autopass)

int Trig_SockFD = 0;

// This function creates a socket and connects to the trigger server board
// Parameters:
//   trigger_addr: a const char* with the IP address of the trigger server board
//   trigger_port: an unsigned short int with the port number of the trigger server board
// Returns:
//   TRIG_OK if successful, TRIG_ERROR if not
int trig_init(char* trigger_addr, unsigned short int trigger_port)
{

  struct sockaddr_in serv_addr;

  // Create a socket for the client
  Trig_SockFD = socket(AF_INET,SOCK_STREAM,0);
  // Check if socket creation was successful
  if (Trig_SockFD < 0) {
    perror("Trigger::trig_init - Error creating socket");
    return TRIG_ERROR;
  }

  // Reset server address (just in case)
  memset(&serv_addr,0,sizeof(serv_addr));
  //bzero(&serv_addr,sizeof(serv_addr));

  // Set protocol and port
  serv_addr.sin_family = AF_INET;
  //serv_addr.sin_port = htons(SERVER_PORT);
  serv_addr.sin_port = htons(trigger_port);
     
  // Convert IPv4 and IPv6 addresses from text to binary form
  //if ( inet_pton(AF_INET,SERVER_ADDRESS,&serv_addr.sin_addr) <= 0 ) {
  if ( inet_pton(AF_INET,trigger_addr,&serv_addr.sin_addr) <= 0 ) {
    perror("Trigger::trig_init - Error converting address");
    return TRIG_ERROR;
  }
 
  // Connect to server
  if ( connect(Trig_SockFD,(struct sockaddr*)&serv_addr,sizeof(serv_addr)) < 0 ) {
    perror("Trigger::trig_init - Error connecting");
    return TRIG_ERROR;
  }

  return TRIG_OK;

}

int trig_end()
{
  close(Trig_SockFD);
  return TRIG_OK;
}

// This function reads timestamp data from a trigger socket
// and copies it to a buffer
// Parameters:
//   buffer: an unsigned char array to store the data
//   buf_len: a pointer to an unsigned int to store the length of the data
// Returns:
//   TRIG_OK if successful, TRIG_ERROR if not
int trig_get_data(void* buffer,unsigned int* buf_len)
{

  int rc;

  // Send command to read timestamp buffer
  //char cmd[2]; sprintf(cmd,"%c%c",TRIG_CMD_TIMESTAMP,TRIG_REG_TIMESTAMP);
  //printf("trig_get_data cmd = 0x");
  //int i; for(i=0;i<2;i++) printf("%02x",cmd[i]);
  //printf("\n");
  const unsigned char cmd[2] = {TRIG_CMD_TIMESTAMP, TRIG_REG_TIMESTAMP};
  rc = write(Trig_SockFD,cmd,2);
  if (rc<0) {
    perror("Trigger::trig_get_data - Error issuing timestamp command");
    return TRIG_ERROR;
  }

  // Get ready to read the answer from Trigger

  void* pointer = buffer;
  //unsigned char buff[4],head[4],tail[4],biff[4];
  //head[0] = 0xB0; head[1] = 0xF0; head[2] = 0xB0; head[3] = 0xF0;
  //tail[0] = 0xE0; tail[1] = 0xF0; tail[2] = 0xE0; tail[3] = 0xF0;
  unsigned char buff[4], biff[4];
  const unsigned char head[4] = {0xB0, 0xF0, 0xB0, 0xF0};
  const unsigned char tail[4] = {0xE0, 0xF0, 0xE0, 0xF0};

  // Read first 4 bytes from Trigger (must be B0F0B0F0 pattern)
  rc = read(Trig_SockFD,(char*)buff,4);
  if (rc<0) {
    perror("Trigger::trig_get_data - Error reading timestamp data (header)");
    return TRIG_ERROR;
  }
  //if (rc!=4) {
  //  rc = read(Trig_SockFD,(char*)(buff+rc),4-rc);
  //}
  // Use a loop to read until we get enough bytes
  while (rc < 4) {
    int n = read(Trig_SockFD,(char*)(buff+rc),4-rc);
    if (n < 0) {
      perror("Trigger::trig_get_data - Error reading timestamp data (header)");
      return TRIG_ERROR;
    }
    rc += n;
  }

  //printf("buff 0x%02x%02x%02x%02x\n",buff[0],buff[1],buff[2],buff[3]);
  //printf("head 0x%02x%02x%02x%02x\n",head[0],head[1],head[2],head[3]);
  //if ( memcmp((void*)buff,(void*)head,4) != 0 ) {
  if ( (buff[0] ^ head[0]) | (buff[1] ^ head[1]) | (buff[2] ^ head[2]) | (buff[3] ^ head[3]) ) {
    printf("Trigger::trig_get_data - ERROR - First timestamp word: expected 0x%02x%02x%02x%02x received 0x%02x%02x%02x%02x\n",head[0],head[1],head[2],head[3],buff[0],buff[1],buff[2],buff[3]);
    return TRIG_ERROR;
  }

  // Use a while loop to read the data until we reach the tail pattern
  while(1){

    // Read next 4 bytes from Trigger
    rc = read(Trig_SockFD,(char*)buff,4);
    if (rc<0) {
      perror("Trigger::trig_get_data - Error reading timestamp data (first half)");
      return TRIG_ERROR;
    }
    //if (rc!=4) {
    //  rc = read(Trig_SockFD,(char*)(buff+rc),4-rc);
    //}
    // Use a loop to read until we get enough bytes
    while (rc < 4) {
      int n = read(Trig_SockFD,(char*)(buff+rc),4-rc);
      if (n < 0) {
	perror("Trigger::trig_get_data - Error reading timestamp data (first half)");
	return TRIG_ERROR;
      }
      rc += n;
    }
   //printf("0x%02x%02x%02x%02x\n",buff[0],buff[1],buff[2],buff[3]);

    // Check if we reached the E0F0E0F0 final tag
    //if ( memcmp((void*)buff,(void*)tail,4) == 0 ) {
    if ( !( (buff[0] ^ tail[0]) | (buff[1] ^ tail[1]) | (buff[2] ^ tail[2]) | (buff[3] ^ tail[3]) ) ) {
      //printf("Footer found\n");
      break; // We are done: exit main loop
    }

    // Read second half of packet from Trigger
    rc = read(Trig_SockFD,(char*)biff,4);
    if (rc<0) {
      perror("Trigger::trig_get_data - Error reading timestamp data (second half)");
      return TRIG_ERROR;
    }
    //if (rc!=4) {
    //  rc = read(Trig_SockFD,(char*)(biff+rc),4-rc);
    //}
    // Use a loop to read until we get enough bytes
    while (rc < 4) {
      int n = read(Trig_SockFD,(char*)(biff+rc),4-rc);
      if (n < 0) {
	perror("Trigger::trig_get_data - Error reading timestamp data (second half)");
	return TRIG_ERROR;
      }
      rc += n;
    }
    //printf("0x%02x%02x%02x%02x\n",biff[0],biff[1],biff[2],biff[3]);

    // Copy two halves of 8bytes packet to final buffer
    memcpy(pointer,buff,4); pointer += 4;
    memcpy(pointer,biff,4); pointer += 4;

  }

  unsigned int length = (unsigned int)(pointer-buffer);
  //printf("Length is %u\n",length);
  memcpy(buf_len,&length,sizeof(length));

  return TRIG_OK;

}

int trig_get_temperature(float* temp)
{
  return TRIG_OK;
}

/*
int trig_set_trigmask(char trig)
{

  int rc;

  // Get current busy mask
  char busy; rc = trig_get_busymask(&busy);
  if (rc != TRIG_OK) {
    printf("Error reading busy mask\n");
    return TRIG_ERROR;
  }

  char cmd[6]; sprintf(cmd,"%c%c%c%c%c%c",TRIG_CMD_WRITE,TRIG_REG_BUSY_AND_TRIG_MASKS,0,0,busy,trig);
  //printf("trig_set_trigmask cmd = 0x");
  //int i; for(i=0;i<6;i++) printf("%02x",cmd[i]);
  //printf("\n");
  rc = write(Trig_SockFD,cmd,6);
  if (rc<0) {
    perror("Error setting trigger/busy mask");
    return TRIG_ERROR;
  }

  return TRIG_OK;

}
*/

/*
int trig_set_busymask(char busy)
{

  int rc;

  // Get current trigger mask
  char trig; rc = trig_get_trigmask(&trig);
  if (rc != TRIG_OK) {
    printf("Error reading trigger mask\n");
    return TRIG_ERROR;
  }

  char cmd[6]; sprintf(cmd,"%c%c%c%c%c%c",TRIG_CMD_WRITE,TRIG_REG_BUSY_AND_TRIG_MASKS,0,0,busy,trig);
  //printf("trig_set_busymask cmd = 0x");
  //int i; for(i=0;i<6;i++) printf("%02x",cmd[i]);
  //printf("\n");
  rc = write(Trig_SockFD,cmd,6);
  if (rc<0) {
    perror("Error setting trigger/busy mask");
    return TRIG_ERROR;
  }

  return TRIG_OK;

}
*/

 /*
int trig_get_trigbusymask(char* mask)
{

  int rc;
  char cmd[2];

  // Send command to read trigger/busy mask
  sprintf(cmd,"%c%c",TRIG_CMD_READ,TRIG_REG_BUSY_AND_TRIG_MASKS);
  //printf("trig_get_trigbusymask cmd = 0x");
  //int i; for(i=0;i<2;i++) printf("%02x",cmd[i]);
  //printf("\n");
  rc = write(Trig_SockFD,cmd,2);
  if (rc<0) {
    perror("Error asking for trigger/busy mask");
    return TRIG_ERROR;
  }

  // Get response
  rc = read(Trig_SockFD,mask,4);
  if (rc<0) {
    perror("Error reading trigger/busy mask");
    return TRIG_ERROR;
  }
  //mask[0] = 0x3f;
  //mask[1] = 0x1f;

  return TRIG_OK;

}
 */

/*
int trig_get_trigmask(char* mask)
{

  int rc;
  char fullmask[4];

  rc = trig_get_trigbusymask(fullmask);
  if (rc!=TRIG_OK) {
    printf("Error reading trigger mask\n");
    return TRIG_ERROR;
  }
  mask[0] = fullmask[3];

  return TRIG_OK;

}
*/

/*
int trig_get_busymask(char* mask)
{

  int rc;
  char fullmask[4];

  rc = trig_get_trigbusymask(fullmask);
  if (rc!=TRIG_OK) {
    printf("Error reading busy mask\n");
    return TRIG_ERROR;
  }
  mask[0] = fullmask[2];

  return TRIG_OK;

}
*/

/*
int trig_start_run()
{

  int rc;
  char cmd[3];

  sprintf(cmd,"%c%c%c",TRIG_CMD_WRITE,TRIG_REG_START_RUN,0x01);
  printf("trig_start_run cmd = 0x");
  int i; for(i=0;i<3;i++) printf("%02x",cmd[i]);
  printf("\n");
  rc = write(Trig_SockFD,cmd,3);
  if (rc<0) {
    perror("Error starting run");
    return TRIG_ERROR;
  }
  return TRIG_OK;
}
*/

// This function sets the start run bit in the trigger start/stop run register
// Returns:
//   The return value of trig_set_register function
int trig_start_run()
{
  //unsigned char fullmask[4];
  //fullmask[0] = 0x00;
  //fullmask[1] = 0x00;
  //fullmask[2] = 0x00;
  //fullmask[3] = (1 << 0); // bit 0: start run
  unsigned char fullmask[] = {0x00, 0x00, 0x00, 0x00};
  fullmask[3] = (fullmask[3] | TRIG_BIT_START_RUN); // bit 0: start run
  return trig_set_register(TRIG_REG_START_STOP_RUN,fullmask);
}

// This function sets the stop run bit in the trigger start/stop run register
// Returns:
//   The return value of trig_set_register function
int trig_stop_run()
{
  //unsigned char fullmask[4];
  //fullmask[0] = 0x00;
  //fullmask[1] = 0x00;
  //fullmask[2] = 0x00;
  //fullmask[3] = (1 << 1); // bit 1: stop run
  unsigned char fullmask[] = {0x00, 0x00, 0x00, 0x00};
  fullmask[3] = (fullmask[3] | TRIG_BIT_STOP_RUN); // bit 1: stop run
  return trig_set_register(TRIG_REG_START_STOP_RUN,fullmask);
}

// This function reads the firmware version from the trigger register
// Parameters:
//   fw_version: a pointer to an unsigned int to store the firmware version
// Returns:
//   The return value of trig_get_register function
int trig_get_fw_version(unsigned int* fw_version)
{
  int rc;
  unsigned char fullmask[4];
  rc = trig_get_register(TRIG_REG_FW_VERSION,fullmask);
  if (rc == TRIG_OK) {
    //*fw_version = fullmask[0]*(1<<24)+fullmask[1]*(1<<16)+fullmask[2]*(1<<8)+fullmask[3];
    *fw_version = (fullmask[0] << 24) | (fullmask[1] << 16) | (fullmask[2] << 8) | fullmask[3];
  }
  return rc;
}

int trig_get_trigbusymask(unsigned char* mask)
{
  // WARNING this function is obsolete and will be removed
  return trig_get_register(TRIG_REG_BUSY_TRIG_MASKS,mask);
}

int trig_get_trigmask(unsigned char* mask)
{
  int rc;
  unsigned char fullmask[4];
  rc = trig_get_register(TRIG_REG_BUSY_TRIG_MASKS,fullmask);
  if (rc == TRIG_OK) mask[0] = fullmask[3];
  return rc;
}

int trig_set_trigmask(unsigned char mask)
{
  int rc;
  unsigned char fullmask[4];
  rc = trig_get_register(TRIG_REG_BUSY_TRIG_MASKS,fullmask);
  if (rc != TRIG_OK) return rc;
  // Replace old trigger mask with new one (8bits)
  fullmask[3] = mask;
  return trig_set_register(TRIG_REG_BUSY_TRIG_MASKS,fullmask);
}

int trig_enable_trigger(unsigned char trigger)
{

  int rc;
  unsigned char fullmask[4];
  char errmsg[80];

  if (trigger >= 8) {
    sprintf(errmsg,"Trigger::trig_enable_trigger - Error request to enable trigger %u",trigger);
    perror(errmsg);
    return TRIG_ERROR;
  }

  rc = trig_get_register(TRIG_REG_BUSY_TRIG_MASKS,fullmask);
  if (rc != TRIG_OK) return rc;
  fullmask[0] = ( fullmask[0] | (1 << trigger) );
  return trig_set_register(TRIG_REG_BUSY_TRIG_MASKS,fullmask);

}

int trig_disable_trigger(unsigned char trigger)
{

  int rc;
  unsigned char fullmask[4];
  char errmsg[80];

  if (trigger >= 8) {
    sprintf(errmsg,"Trigger::trig_disable_trigger - Error request to disable trigger %u",trigger);
    perror(errmsg);
    return TRIG_ERROR;
  }

  rc = trig_get_register(TRIG_REG_BUSY_TRIG_MASKS,fullmask);
  if (rc != TRIG_OK) return rc;
  fullmask[0] = ( fullmask[0] & ~(1 << trigger) );
  return trig_set_register(TRIG_REG_BUSY_TRIG_MASKS,fullmask);

}

int trig_get_busymask(unsigned char* mask)
{
  int rc;
  unsigned char fullmask[4];
  rc = trig_get_register(TRIG_REG_BUSY_TRIG_MASKS,fullmask);
  if (rc == TRIG_OK) mask[0] = fullmask[2];
  return rc;
}

int trig_set_busymask(unsigned char mask)
{
  int rc;
  unsigned char fullmask[4];
  rc = trig_get_register(TRIG_REG_BUSY_TRIG_MASKS,fullmask);
  if (rc != TRIG_OK) return rc;
  // Replace old busy mask with new one (bits 3:0)
  // Reminder: bit 4 (CPU busy) must never be touched
  fullmask[2] = ( (fullmask[2] & 0xf0) | (mask & 0x0f) );
  return trig_set_register(TRIG_REG_BUSY_TRIG_MASKS,fullmask);
}

int trig_get_correlated_delay(unsigned short int* delay)
{
  int rc;
  unsigned char fullmask[4];
  rc = trig_get_register(TRIG_REG_BUSY_TRIG_MASKS,fullmask);
  //if (rc == TRIG_OK) *delay = fullmask[0]*256+fullmask[1];
  if (rc == TRIG_OK) *delay = (fullmask[0] << 8) | fullmask[1];
  return rc;
}

int trig_set_correlated_delay(unsigned short int delay)
{
  int rc;
  unsigned char fullmask[4];
  rc = trig_get_register(TRIG_REG_BUSY_TRIG_MASKS,fullmask);
  if (rc != TRIG_OK) return rc;
  fullmask[0] = ( (delay/256) & 0xff );
  fullmask[1] = ( (delay%256) & 0xff );
  return trig_set_register(TRIG_REG_BUSY_TRIG_MASKS,fullmask);
}

int trig_get_timepix_delay(unsigned char* delay)
{
  int rc;
  unsigned char fullmask[4];
  rc = trig_get_register(TRIG_REG_TIMEPIX_SHUTTER,fullmask);
  if (rc == TRIG_OK) delay[0] = fullmask[3];
  return rc;
}

int trig_set_timepix_delay(unsigned char delay)
{
  int rc;
  unsigned char fullmask[4];
  if (delay == 0) {
    printf("Trigger::trig_set_timepix_delay - WARNING - delay set to 0: timepix shutter will start ~25us after the BTF trigger. Are you sure?\n");
  }
  rc = trig_get_register(TRIG_REG_TIMEPIX_SHUTTER,fullmask);
  if (rc != TRIG_OK) return rc;
  // Replace old timepix delay with new one
  fullmask[3] = delay;
  return trig_set_register(TRIG_REG_TIMEPIX_SHUTTER,fullmask);
}

int trig_get_timepix_width(unsigned char* width)
{
  int rc;
  unsigned char fullmask[4];
  rc = trig_get_register(TRIG_REG_TIMEPIX_SHUTTER,fullmask);
  if (rc == TRIG_OK) width[0] = fullmask[2];
  return rc;
}

int trig_set_timepix_width(unsigned char width)
{
  int rc;
  unsigned char fullmask[4];
  rc = trig_get_register(TRIG_REG_TIMEPIX_SHUTTER,fullmask);
  if (rc != TRIG_OK) return rc;
  // Replace old timepix width with new one
  fullmask[2] = width;
  return trig_set_register(TRIG_REG_TIMEPIX_SHUTTER,fullmask);
}

int trig_get_trigger0_delay(unsigned char* delay)
{
  int rc;
  unsigned char fullmask[4];
  rc = trig_get_register(TRIG_REG_TIMEPIX_SHUTTER,fullmask);
  if (rc == TRIG_OK) delay[0] = fullmask[0];
  return rc;
}

int trig_set_trigger0_delay(unsigned char delay)
{
  int rc;
  unsigned char fullmask[4];
  rc = trig_get_register(TRIG_REG_TIMEPIX_SHUTTER,fullmask);
  if (rc != TRIG_OK) return rc;
  // Replace old trigger0 delay with new one
  fullmask[0] = delay;
  return trig_set_register(TRIG_REG_TIMEPIX_SHUTTER,fullmask);
}

// This function reads the global scale factor for a given trigger
// Parameters:
//   trigger: an unsigned char with the trigger number
//   factor: a pointer to an unsigned short int to store the factor
// Returns:
//   The return value of trig_get_register function
int trig_get_trigger_global_factor(unsigned char trigger,unsigned short int* factor)
{
  int rc;
  if (trigger == 0) return TRIG_UNDEF; // Global factor is not used for trigger 0 (BTF)
  unsigned char fullmask[4];
  unsigned char reg = TRIG_REG_TRIGGER_BASE + trigger; // Define register for this trigger
  rc = trig_get_register(reg,fullmask);
  //if (rc == TRIG_OK) *factor = fullmask[2]*256+fullmask[3];
  if (rc == TRIG_OK) *factor = (fullmask[2] << 8) | fullmask[3];
  return rc;
}

// This function sets the global scale factor for a given trigger
// Parameters:
//   trigger: an unsigned char with the trigger number
//   factor: an unsigned short int with the factor to write
// Returns:
//   The return value of trig_set_register function
int trig_set_trigger_global_factor(unsigned char trigger,unsigned short int factor)
{
  int rc;
  if (trigger == 0) return TRIG_UNDEF; // Global factor is not used for trigger 0 (BTF)
  unsigned char fullmask[4];
  unsigned char reg = TRIG_REG_TRIGGER_BASE + trigger; // Define register for this trigger
  rc = trig_get_register(reg,fullmask);
  if (rc != TRIG_OK) return rc;
  // Replace old global factor with new one
  //fullmask[2] = ( (factor/256) & 0xff );
  //fullmask[3] = ( (factor%256) & 0xff );
  fullmask[2] = ( (factor >> 8) & 0xff );
  fullmask[3] = ( factor & 0xff );
  return trig_set_register(reg,fullmask);
}

// This function reads the autopass factor for a given trigger
// Parameters:
//   trigger: an unsigned char with the trigger number
//   factor: a pointer to an unsigned short int to store the factor
// Returns:
//   The return value of trig_get_register function
int trig_get_trigger_autopass_factor(unsigned char trigger,unsigned short int* factor)
{
  int rc;
  unsigned char fullmask[4];
  unsigned char reg = TRIG_REG_TRIGGER_BASE + trigger; // Define register for this trigger
  rc = trig_get_register(reg,fullmask);
  //if (rc == TRIG_OK) *factor = fullmask[0]*256+fullmask[1];
  if (rc == TRIG_OK) *factor = (fullmask[0] << 8) | fullmask[1];
  return rc;
}

// This function sets the autopass factor for a given trigger
// Parameters:
//   trigger: an unsigned char with the trigger number
//   factor: an unsigned short int with the factor to write
// Returns:
//   The return value of trig_set_register function
int trig_set_trigger_autopass_factor(unsigned char trigger,unsigned short int factor)
{
  int rc;
  unsigned char fullmask[4];
  unsigned char reg = TRIG_REG_TRIGGER_BASE + trigger; // Define register for this trigger
  rc = trig_get_register(reg,fullmask);
  if (rc != TRIG_OK) return rc;
  // Replace old autopass factor with new one
  //fullmask[0] = ( (factor/256) & 0xff );
  //fullmask[1] = ( (factor%256) & 0xff );
  fullmask[0] = ( (factor >> 8) & 0xff );
  fullmask[1] = ( factor & 0xff );
  return trig_set_register(reg,fullmask);
}

// This function reads a 4-byte word from a given trigger register
// Parameters:
//   reg: an unsigned char with the register address
//   word: an unsigned char array to store the word
// Returns:
//   TRIG_OK if successful, TRIG_ERROR if not
int trig_get_register(unsigned char reg, unsigned char* word)
{

  int rc;
  unsigned char cmd[2];
  char errmsg[80];

  // Send command to read given register
  cmd[0] = TRIG_CMD_READ;
  cmd[1] = reg;

  //printf("trig_get_register cmd = 0x");
  //int i; for(i=0;i<2;i++) printf("%02x",cmd[i]);
  //printf("\n");

  rc = write(Trig_SockFD,cmd,2);
  if (rc<0) {
    //sprintf(errmsg,"trig_get_register - Error requesting register %2u - Command %02x%02x",
    //	    reg,cmd[0],cmd[1]);
    snprintf(errmsg,sizeof(errmsg),"Trigger::trig_get_register - Error requesting register %2u - Command %02x%02x",
	    reg,cmd[0],cmd[1]);
    perror(errmsg);
    return TRIG_ERROR;
  }

  // Get response (make sure we read 4 bytes)
  rc = read(Trig_SockFD,word,4);
  if (rc<0) {
    //sprintf(errmsg,"Trigger::trig_get_register - Error reading register %2u",reg);
    snprintf(errmsg,sizeof(errmsg),"Trigger::trig_get_register - Error reading register %2u",reg);
    perror(errmsg);
    return TRIG_ERROR;
  }
  //if (rc!=4) {
  //  rc = read(Trig_SockFD,word+rc,4-rc);
  //  if (rc<0) {
  //    sprintf(errmsg,"trig_get_register - Error reading second part of register %2u",reg);
  //    perror(errmsg);
  //    return TRIG_ERROR;
  //  }
  //}
  // Use a loop to read until we get enough bytes
  while (rc < 4) {
    int n = read(Trig_SockFD,word+rc,4-rc);
    if (n < 0) {
      snprintf(errmsg,sizeof(errmsg),"Trigger::trig_get_register - Error reading second part of register %2u",reg);
      perror(errmsg);
      return TRIG_ERROR;
    }
    rc += n;
  }

  return TRIG_OK;

}

// This function writes a 4-byte word to a given trigger register
// Parameters:
//   reg: an unsigned char with the register address
//   word: an unsigned char array with the word to write
// Returns:
//   TRIG_OK if successful, TRIG_ERROR if not
int trig_set_register(unsigned char reg,unsigned char* word)
{

  int rc;
  unsigned char cmd[6];
  char errmsg[80];

  cmd[0] = TRIG_CMD_WRITE;
  cmd[1] = reg;
  memcpy((void*)(cmd+2),(void*)word,4);

  printf("trig_set_register cmd = 0x");
  int i; for(i=0;i<6;i++) printf("%02x",cmd[i]);
  printf("\n");

  rc = write(Trig_SockFD,cmd,6);
  if (rc<0) {
    snprintf(errmsg,sizeof(errmsg),"Trigger::trig_set_register - Error setting register %2u - Command %02x%02x%02x%02x%02x%02x",
	    reg,cmd[0],cmd[1],cmd[2],cmd[3],cmd[4],cmd[5]);
    perror(errmsg);
    return TRIG_ERROR;
  }

  return TRIG_OK;

}
