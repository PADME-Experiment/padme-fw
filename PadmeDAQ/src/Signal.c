#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>

#include "DAQ.h"
#include "Signal.h"

int InBurst; // =0 DAQ not running, >0 DAQ running

int BreakSignal = 0; // If >0 an interrupt was received and DAQ should exit

// Function to handle interrupts
void termination_handler (int signum)
{
  if (InBurst > 0) {

    // If DAQ is running, don't stop abruptly
    // Just tell the main to exit when DAQ allows it
    printf ("\n### Interrupt Received while in burst mode - Signal: %d\n", signum);
    printf ("--- Waiting for DAQ to terminate gracefully... \n");
    if (signum!=0) {
      BreakSignal = signum;
    } else {
      BreakSignal = -1;
    }
    // Ignore additional interrupts
    signal (SIGINT,  SIG_IGN);
    signal (SIGHUP,  SIG_IGN);
    signal (SIGTERM, SIG_IGN);
    signal (SIGFPE,  SIG_DFL);

  } else {

    printf ("\n### Interrupt Received in idle mode - Signal: %d\n", signum);
    printf ("--- Terminating DAQ program... \n");
    signal (SIGINT,  SIG_DFL); // Reset interrupt handler
    signal (SIGHUP,  SIG_DFL);
    signal (SIGTERM, SIG_DFL);
    signal (SIGFPE,  SIG_DFL);
    DAQ_close ();
    //remove_lock(); // will leave dangling lock file
    printf("= Hope you enjoyed the ride. Bye!\n");
    exit(0);

  }
}

// Set interrupt handling functions
void set_signal_handlers()
{
  printf ("Trap signals:");
  signal (SIGINT, termination_handler);
  printf (" SIGINT %d", SIGINT);
  signal (SIGHUP, termination_handler);
  printf (" SIGHUP %d", SIGHUP);
  signal (SIGTERM, termination_handler);
  printf (" SIGTERM %d", SIGTERM);
  //signal (SIGUSR2, termination_handler); // do we need this?
  //printf (" %d", SIGUSR2);
  signal (SIGFPE, termination_handler);
  printf (" SIGFPE %d", SIGFPE);
  printf ("\n");
}
