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
#define SERVER_ADDRESS "192.168.60.200"

#define TRIG_CMD_READ 0x00
#define TRIG_CMD_WRITE 0x01
#define TRIG_CMD_READ_I2C_TEMP 0x04
#define TRIG_CMD_READ_XADC_TEMP 0x04

int Trig_SockFD = 0;

int trig_init()
{

  //int port;
  //char hostname[MSG_SIZE];
  //struct hostent* hostinfo;
  struct sockaddr_in serv_addr;

  // Create a socket for the client
  Trig_SockFD = socket(AF_INET,SOCK_STREAM,0);

  //// Get hostname and port from parameters
  //strcpy(hostname,HOSTNAME); // IP address of Trigger board
  //port = MYPORT; // Port on which the Trigger board is listening
  //
  //// Name the socket, as agreed with the server
  //hostinfo = gethostbyname(hostname);  /* look for host's name */
  //serv_addr.sin_addr = *(struct in_addr *)*hostinfo->h_addr_list;
  //serv_addr.sin_family = AF_INET;
  //serv_addr.sin_port = htons(port);
  //
  //// Connect the socket to the server's socket
  //if ( connect(Trig_SockFD,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0 ) {
  //  perror("Error connecting");
  //  return TRIG_ERROR;
  //}

  memset(&serv_addr,'0',sizeof(serv_addr));
 
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(SERVER_PORT);
     
  // Convert IPv4 and IPv6 addresses from text to binary form
  if ( inet_pton(AF_INET,SERVER_ADDRESS,&serv_addr.sin_addr) <= 0 ) {
    perror("Error converting address");
    //printf("\nInvalid address/ Address not supported \n");
    return TRIG_ERROR;
  }
 
  // Connect to server
  //if ( connect(sock,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0 ) {
  //  perror("Error connecting");
  //  //printf("\nConnection Failed \n");
  //  return TRIG_ERROR;
  //}

  return TRIG_OK;

}

int trig_end()
{
  close(Trig_SockFD);
  return TRIG_OK;
}

int trig_get_data(void* buffer)
{
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
  if (rc!=TRIG_OK) {
    printf("Error reading busy mask\n");
    return TRIG_ERROR;
  }

  char cmd[4]; sprintf(cmd,"%c%c%c%c",TRIG_CMD_WRITE,0x02,trig,busy);
  printf("trig_set_trigmask cmd = 0x");
  int i; for(i=0;i<4;i++) printf("%02x",cmd[i]);
  printf("\n");
  //rc = write(Trig_SockFD,cmd,4);
  //if (rc<0) {
  //  perror("Error setting trigger/busy mask");
  //  return TRIG_ERROR;
  //}

  return TRIG_OK;

}

int trig_set_busymask(char busy)
{

  int rc;

  // Get current trigger mask
  char trig; rc = trig_get_trigmask(&trig);
  if (rc!=TRIG_OK) {
    printf("Error reading trigger mask\n");
    return TRIG_ERROR;
  }

  char cmd[4]; sprintf(cmd,"%c%c%c%c",TRIG_CMD_WRITE,0x02,trig,busy);
  printf("trig_set_busymask cmd = 0x");
  int i; for(i=0;i<4;i++) printf("%02x",cmd[i]);
  printf("\n");
  //rc = write(Trig_SockFD,cmd,4);
  //if (rc<0) {
  //  perror("Error setting trigger/busy mask");
  //  return TRIG_ERROR;
  //}

  return TRIG_OK;

}

int trig_get_trigbusymask(char* mask)
{

  int rc;
  char cmd[3];

  // Send command to read trigger/busy mask (register 2)
  sprintf(cmd,"%c%c%c",TRIG_CMD_READ,0x02,0x02);
  printf("trig_get_trigbusymask cmd = 0x");
  int i; for(i=0;i<3;i++) printf("%02x",cmd[i]);
  printf("\n");
  //rc = write(Trig_SockFD,cmd,4);
  //if (rc<0) {
  //  perror("Error asking for trigger/busy mask");
  //  return TRIG_ERROR;
  //}

  // Get response (byte 0: trigger mask, byte 1: busy mask)
  //rc = read(Trig_SockFD,mask,2);
  //if (rc<0) {
  //  perror("Error reading trigger/busy mask");
  //  return TRIG_ERROR;
  //}
  mask[0] = 0x3f;
  mask[1] = 0x1f;

  return TRIG_OK;

}

int trig_get_trigmask(char* mask)
{

  int rc;
  char fullmask[2];

  rc = trig_get_trigbusymask(fullmask);
  if (rc!=TRIG_OK) {
    printf("Error reading trigger mask\n");
    return TRIG_ERROR;
  }
  mask[0] = fullmask[0];

  return TRIG_OK;

}

int trig_get_busymask(char* mask)
{

  int rc;
  char fullmask[2];

  rc = trig_get_trigbusymask(fullmask);
  if (rc!=TRIG_OK) {
    printf("Error reading busy mask\n");
    return TRIG_ERROR;
  }
  mask[0] = fullmask[1];

  return TRIG_OK;

}

int trig_start_run()
{

  int rc;
  char cmd[3];

  sprintf(cmd,"%c%c%c",TRIG_CMD_WRITE,0x00,0x01);
  printf("trig_start_run cmd = 0x");
  int i; for(i=0;i<3;i++) printf("%02x",cmd[i]);
  printf("\n");
  //rc = write(Trig_SockFD,cmd,2);
  //if (rc<0) {
  //  perror("Error starting run");
  //  return TRIG_ERROR;
  //}
  return TRIG_OK;
}
