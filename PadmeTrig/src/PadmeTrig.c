#include <stdio.h>

#include "Trigger.h"

int main(int argc, char *argv[]) {

  int rc;
  char mask[4];

  rc = trig_get_trigbusymask(mask);
  if (rc != TRIG_OK) {
    printf("Error getting trigger/busy mask\n");
  }
  printf("trigbusymask = 0x%02x%02x\n",mask[0],mask[1]);

  rc = trig_get_trigmask(mask);
  if (rc != TRIG_OK) {
    printf("Error getting trigger mask\n");
  }
  printf("trigmask = 0x%02x\n",mask[0]);

  rc = trig_get_busymask(mask);
  if (rc != TRIG_OK) {
    printf("Error getting busy mask\n");
  }
  printf("busymask = 0x%02x\n",mask[0]);

  rc = trig_set_trigmask(0x0f);
  if (rc != TRIG_OK) {
    printf("Error setting trigger mask\n");
  }

  rc = trig_set_busymask(0x03);
  if (rc != TRIG_OK) {
    printf("Error setting busy mask\n");
  }

  rc = trig_start_run();
  if (rc != TRIG_OK) {
    printf("Error starting run\n");
  }
  return 1;
}
