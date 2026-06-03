
/*******************************************************************************
*
* File ranlxd.c
*
* Copyright (C) 2005, 2019 Martin Luescher
*
* This software is distributed under the terms of the GNU General Public
* License (GPL)
*
* Random number generator "ranlxd". See the notes
*
*   "User's guide for ranlxs and ranlxd v3.4" (May 2019)
*
*   "Algorithms used in ranlxs and ranlxd v3.4" (May 2019)
*
* for a detailed description.
*
* The externally accessible functions are:
*
*   void ranlxd(double *r,int n)
*     Computes the next n double-precision random numbers and assigns
*     them to r[0],...,r[n-1].
*
*   void rlxd_init(int level,int seed)
*     Initialization of the generator with the specified luxury "level"
*     and "seed". Admissible levels are 1 and 2 and the seed must be in
*     the range 1<=seed<2^31.
*
*   int rlxd_size(void)
*     Returns the number of integers required to save the state of
*     the generator.
*
*   void rlxd_get(int *state)
*     Extracts the current state of the generator and stores the
*     information in the array elements state[0],..,state[N-1], where
*     N is the value returned by rlxd_size().
*
*   void rlxd_reset(int *state)
*     Resets the generator to the state defined by state[0],..,state[N-1],
*     assuming the state was previously saved to this array by the program
*     rlxd_get().
*
* The state of the generator is stored in the static memory of this module.
* If rlxd_init() is called a second time, the state is reinitialized.
*
*******************************************************************************/

#define RANLXD_C

#include <stdlib.h>
#include "ranlux.h"

static int init=0,is,is_old,next[48];
#if ((defined AVX2)||(defined SSE2))
static double rd[48] __attribute__ ((aligned (16)));
#else
static double rd[48];
#endif
static rlx_state_t rstate;


void rlxd_init(int level,int seed)
{
   int k;

   rlx_error(rlx_check_machine(),1,"rlxd_init [ranlxd.c]",
             "Machine does not support the required data types");
   rlx_error((level<1)||(level>2)||(seed<1)||(seed>0x7fffffff),1,
             "rlxd_init [ranlxd.c]","Arguments are out of range");

   if (level==1)
      rstate.pr=202;
   else
      rstate.pr=397;

   if (init==0)
      rlx_alloc_state(&rstate);
   rlx_init(&rstate,seed,1);

   for (k=0;k<47;k++)
      next[k]=k+1;
   next[47]=0;

   is=47;
   is_old=0;
   init=1;
}


void ranlxd(double *r,int n)
{
   int k;

   if (init==0)
      rlx_error(1,1,"ranlxd [ranlxd.c]","ranlxd is not initialized");

   for (k=0;k<n;k++)
   {
      is=next[is];

      if (is==is_old)
      {
         rlx_update(&rstate);
         rlx_convertd(&rstate,rd);
         is=4*rstate.ir;
         is_old=is;
      }

      r[k]=rd[is];
   }
}


int rlxd_size(void)
{
   return 104;
}


void rlxd_get(int *state)
{
   rlx_error(init==0,1,"rlxd_get [ranlxd.c]","ranlxd is not initialized");

   state[0]=rlxd_size();
   rlx_get_state(&rstate,state+1);
   state[101]=rstate.pr;
   state[102]=rstate.ir;
   state[103]=is;
}


void rlxd_reset(int *state)
{
   int ie,k;

   rlx_error(rlx_check_machine(),1,"rlxd_reset [ranlxd.c]",
             "Machine does not support the required data types");
   rlx_error(state[0]!=rlxd_size(),1,"rlxd_reset [ranlxd.c]",
             "Unexpected input data");
   ie=((state[101]!=202)&&(state[101]!=397));
   ie|=((state[102]<0)||(state[102]>=12));
   ie|=((state[103]<0)||(state[103]>=48));
   rlx_error(ie,2,"rlxd_reset [ranlxd.c]","Unexpected input data");

   if (init==0)
      rlx_alloc_state(&rstate);

   rlx_set_state(state+1,&rstate);
   rstate.pr=state[101];
   rstate.ir=state[102];
   is=state[103];
   is_old=4*rstate.ir;

   for (k=0;k<47;k++)
      next[k]=k+1;
   next[47]=0;

   rlx_convertd(&rstate,rd);
   init=1;
}
