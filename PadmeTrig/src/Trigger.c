#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>

#include "Trigger.h"

#define MSG_SIZE 4096
#define SERVER_PORT 7
#define SERVER_ADDRESS "192.168.60.100"

#define TRIG_CMD_READ 0x00
#define TRIG_CMD_WRITE 0x01
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
//         [15:8] TimePix3 shutter window width in clock cycles (def: 0x16 = 22 = 275ns)
//
// 0x06 RW trigger 0 (BTF trigger) start of pre-veto wrt start of previous trigger
//         [31:0] pre-veto width in clock cycles (def: 0x001829ef = 1583599 = 19.794988ms
//                i.e. 205us before next trigger assuming a 50Hz trigger rate)
//
// 0x07 RW trigger 0 (BTF trigger) pre-veto timeout if next BTF trigger does not arrive
//         [31:0] pre-veto timeout in clock cycles (def: 0x04c4b3ff = 79999999 = 1s)
// 0x08 RW trigger 0 (BTF trigger) global demultiplication factor and autopass demultiplication factor
//         [15:0] global demultiplication (def: 1), [31:16] autopass demultiplication (def: 0, autopass off)
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

// === Trigger data format (64bit) ===
// [39:0]  timestamp in clock cycles, resets in 13744s = 3h49m
// [47:40] trigger map (7:0), 0 = BTF, 6 = uncorrelated, 7 = correlated
// [55:48] trigger counter, resets at 256 triggers
// [56]    fifo empty (technical detail, ignore)
// [57]    trigger type (0: standard, 1: autopass)

#define TRIG_REG_START_RUN           0x00
#define TRIG_REG_BUSY_AND_TRIG_MASKS 0x02

int Trig_SockFD = 0;

int trig_init()
{

  struct sockaddr_in serv_addr;

  // Create a socket for the client
  Trig_SockFD = socket(AF_INET,SOCK_STREAM,0);

  // Reset server address (just in case)
  memset(&serv_addr,'0',sizeof(serv_addr));
 
  // Set protocol and port
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(SERVER_PORT);
     
  // Convert IPv4 and IPv6 addresses from text to binary form
  if ( inet_pton(AF_INET,SERVER_ADDRESS,&serv_addr.sin_addr) <= 0 ) {
    perror("Error converting address");
    return TRIG_ERROR;
  }
 
  // Connect to server
  if ( connect(Trig_SockFD,(struct sockaddr*)&serv_addr,sizeof(serv_addr)) < 0 ) {
    perror("Error connecting");
    return TRIG_ERROR;
  }

  return TRIG_OK;

}

int trig_end()
{
  close(Trig_SockFD);
  return TRIG_OK;
}

int trig_get_data(void* buffer,unsigned int* buf_len)
{

  int rc;

  // Send command to read timestamp buffer
  char cmd[2]; sprintf(cmd,"%c%c",TRIG_CMD_TIMESTAMP,TRIG_REG_TIMESTAMP);
  //printf("trig_get_data cmd = 0x");
  //int i; for(i=0;i<2;i++) printf("%02x",cmd[i]);
  //printf("\n");
  rc = write(Trig_SockFD,cmd,2);
  if (rc<0) {
    perror("Error issuing timestamp command");
    return TRIG_ERROR;
  }

  // Get ready to read the answer from Trigger

  void* pointer = buffer;
  unsigned char buff[4],head[4],tail[4],biff[4];
  head[0] = 0xB0; head[1] = 0xF0; head[2] = 0xB0; head[3] = 0xF0;
  tail[0] = 0xE0; tail[1] = 0xF0; tail[2] = 0xE0; tail[3] = 0xF0;

  // Read first 4 bytes from Trigger (must be B0F0B0F0 pattern)
  rc = read(Trig_SockFD,(char*)buff,4);
  if (rc<0) {
    perror("Reading timestamp data (header)");
    return TRIG_ERROR;
  }
  if (rc!=4) {
    rc = read(Trig_SockFD,(char*)(buff+rc),4-rc);
  }
  //printf("buff 0x%02x%02x%02x%02x\n",buff[0],buff[1],buff[2],buff[3]);
  //printf("head 0x%02x%02x%02x%02x\n",head[0],head[1],head[2],head[3]);
  if ( memcmp((void*)buff,(void*)head,4) != 0 ) {
    printf("Trigger::trig_get_data - ERROR - First timestamp word: expected 0x%02x%02x%02x%02x received 0x%02x%02x%02x%02x\n",head[0],head[1],head[2],head[3],buff[0],buff[1],buff[2],buff[3]);
    return TRIG_ERROR;
  }

  while(1){

    // Read next 4 bytes from Trigger
    rc = read(Trig_SockFD,(char*)buff,4);
    if (rc<0) {
      perror("Reading timestamp data (first half)");
      return TRIG_ERROR;
    }
    if (rc!=4) {
      rc = read(Trig_SockFD,(char*)(buff+rc),4-rc);
    }
    //printf("0x%02x%02x%02x%02x\n",buff[0],buff[1],buff[2],buff[3]);

    // Check if we reached the E0F0E0F0 final tag
    if ( memcmp((void*)buff,(void*)tail,4) == 0 ) {
      //printf("Footer found\n");
      break; // We are done: exit main loop
    }

    // Read second half of packet from Trigger
    rc = read(Trig_SockFD,(char*)biff,4);
    if (rc<0) {
      perror("Reading timestamp data (second half)");
      return TRIG_ERROR;
    }
    if (rc!=4) {
      rc = read(Trig_SockFD,(char*)(biff+rc),4-rc);
    }
    //printf("0x%02x%02x%02x%02x\n",biff[0],biff[1],biff[2],biff[3]);

    // Copy two halves of 8bytes packet to final buffer
    memcpy(pointer,buff,4); pointer += 4;
    memcpy(pointer,biff,4); pointer += 4;

  }

  unsigned int length = (unsigned int)(pointer-buffer);
  //printf("Length is %u\n",length);
  memcpy(buf_len,&length,4);

  return TRIG_OK;

}

int trig_get_temperature(float* temp)
{
  return TRIG_OK;
}

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

int trig_get_register(unsigned char reg, unsigned int* word)
{

  int rc;
  char cmd[2];
  char errmsg[80];

  // Send command to read given register
  sprintf(cmd,"%c%c",TRIG_CMD_READ,reg);
  //printf("trig_get_register cmd = 0x");
  //int i; for(i=0;i<2;i++) printf("%02x",cmd[i]);
  //printf("\n");
  rc = write(Trig_SockFD,cmd,2);
  if (rc<0) {
    sprintf(errmsg,"trig_get_register - Error requesting register %2u - Command %02x%02x",
	    reg,cmd[0],cmd[1]);
    perror(errmsg);
    return TRIG_ERROR;
  }

  // Get response (make sure we read 4 bytes)
  rc = read(Trig_SockFD,(char*)word,4);
  if (rc<0) {
    sprintf(errmsg,"trig_get_register - Error reading register %2u",reg);
    perror(errmsg);
    return TRIG_ERROR;
  }
  if (rc!=4) {
    rc = read(Trig_SockFD,(char*)(word+rc),4-rc);
    if (rc<0) {
      sprintf(errmsg,"trig_get_register - Error reading second part of register %2u",reg);
      perror(errmsg);
      return TRIG_ERROR;
    }
  }

  return TRIG_OK;

}

int trig_set_register(unsigned char reg,unsigned int word)
{

  int rc;
  char errmsg[80];

  char cmd[6]; sprintf(cmd,"%c%c%08x",TRIG_CMD_WRITE,reg,word);
  //printf("trig_set_register cmd = 0x");
  //int i; for(i=0;i<6;i++) printf("%02x",cmd[i]);
  //printf("\n");
  rc = write(Trig_SockFD,cmd,6);
  if (rc<0) {
    sprintf(errmsg,"trig_set_register - Error setting register %2u - Command %02x%02x%02x%02x%02x%02x",
	    reg,cmd[0],cmd[1],cmd[2],cmd[3],cmd[4],cmd[5]);
    perror(errmsg);
    return TRIG_ERROR;
  }

  return TRIG_OK;

}
