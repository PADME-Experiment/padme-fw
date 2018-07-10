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
#define TRIG_REG_START_RUN 0x00
#define TRIG_REG_BUSY_AND_TRIG_MASKS 0x02
#define TRIG_CMD_TIMESTAMP 0x02
#define TRIG_REG_TIMESTAMP 0x00
#define TRIG_CMD_READ_I2C_TEMP 0x04
#define TRIG_REG_READ_I2C_TEMP 0x00
#define TRIG_CMD_READ_XADC_TEMP 0x04
#define TRIG_REG_READ_XADC_TEMP 0x01

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
