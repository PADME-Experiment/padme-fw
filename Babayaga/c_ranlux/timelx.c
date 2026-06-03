
/*******************************************************************************
*
* File timelx.c
*
* Copyright (C) 2019 Martin Luescher
*
* This software is distributed under the terms of the GNU General Public
* License (GPL)
*
* Measurement of the processor time required to produce random numbers
* using ranlxs() and ranlxd().
*
*******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "ranlux.h"

#define N 100
#define NLOOPS 1000000


int main(void)
{
   int k,level;
   float t1,t2,dt;
   float rs[N];
   double rd[N];

   printf("\n");
   printf("Timing of ranlxs() and ranlxd()\n");
   printf("===============================\n\n");

#if (defined AVX2)
   printf("Using AVX2 inline assembly.\n\n");
#elif (defined SSE2)
   printf("Using SSE2 inline assembly.\n\n");
#endif

   printf("Time per random number in nanoseconds:\n\n");
   printf("  Program      level 0      level 1      level 2\n");
   printf("---------------------------------------------------\n");
   printf("  ranlxs");

   for (level=0;level<=2;level++)
   {
      rlxs_init(level,1);

      t1=(float)(clock());
      for (k=1;k<=NLOOPS;k++)
         ranlxs(rs,N);
      t2=(float)(clock());

      dt=(t2-t1)/(float)(CLOCKS_PER_SEC);
      dt*=1.0e9f/((float)(N)*(float)(NLOOPS));

      printf("       %6.2f",dt);
   }

   printf("\n");
   printf("  ranlxd          -  ");

   for (level=1;level<=2;level++)
   {
      rlxd_init(level,1);

      t1=(float)(clock());
      for (k=1;k<=NLOOPS;k++)
         ranlxd(rd,N);
      t2=(float)(clock());

      dt=(t2-t1)/(float)(CLOCKS_PER_SEC);
      dt*=1.0e9f/((float)(N)*(float)(NLOOPS));

      printf("       %6.2f",dt);
   }

   printf("\n");
   printf("---------------------------------------------------\n\n");
   printf("The RANLUX p-values used at level 0, 1 and 2 are \n");
   printf("218, 404 and 794, respectively.\n\n");
   exit(0);
}
