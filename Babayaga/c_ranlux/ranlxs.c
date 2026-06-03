
/*******************************************************************************
*
* File ranlxs.c
*
* Copyright (C) 2005, 2019 Martin Luescher
*
* This software is distributed under the terms of the GNU General Public
* License (GPL)
*
* Random number generator "ranlxs". See the notes
*
*   "User's guide for ranlxs and ranlxd v3.4" (May 2019)
*
*   "Algorithms used in ranlxs and ranlxd v3.4" (May 2019)
*
* for a detailed description.
*
* The externally accessible functions are:
*
*   void ranlxs(float *r,int n)
*     Computes the next n single-precision random numbers and assigns
*     them to r[0],...,r[n-1].
*
*   void rlxs_init(int level,int seed)
*     Initialization of the generator with the specified luxury "level"
*     and "seed". Admissible levels are 0, 1 and 2 and the seed must be
*     in the range 1<=seed<2^31.
*
*   int rlxs_size(void)
*     Returns the number of integers required to save the state of
*     the generator.
*
*   void rlxs_get(int *state)
*     Extracts the current state of the generator and stores the
*     information in the array elements state[0],..,state[N-1], where
*     N is the value returned by rlxs_size().
*
*   void rlxs_reset(int *state)
*     Resets the generator to the state defined by state[0],..,state[N-1],
*     assuming the state was previously saved to this array by the program
*     rlxs_get().
*
* The state of the generator is stored in the static memory of this module.
* If rlxs_init() is called a second time, the state is reinitialized.
*
*******************************************************************************/

#define RANLXS_C

#include <stdlib.h>
#include "ranlux.h"

static int init=0,is,is_old,next[96];
#if ((defined AVX2)||(defined SSE2))
static float rs[96] __attribute__ ((aligned (16)));
#else
static float rs[96];
#endif
static rlx_state_t rstate;


void rlxs_init(int level,int seed)
{
   int k;

   rlx_error(rlx_check_machine(),1,"rlxs_init [ranlxs.c]",
             "Machine does not support the required data types");
   rlx_error((level<0)||(level>2)||(seed<1)||(seed>0x7fffffff),1,
             "rlxs_init [ranlxs.c]","Arguments are out of range");

   if (level==0)
      rstate.pr=109;
   else if (level==1)
      rstate.pr=202;
   else
      rstate.pr=397;

   if (init==0)
      rlx_alloc_state(&rstate);
   rlx_init(&rstate,seed,0);

   for (k=0;k<95;k++)
      next[k]=k+1;
   next[95]=0;

   is=95;
   is_old=0;
   init=1;
}


void ranlxs(float *r,int n)
{
   int k;

   if (init==0)
      rlx_error(1,1,"ranlxs [ranlxs.c]","ranlxs is not initialized");

   for (k=0;k<n;k++)
   {
      is=next[is];

      if (is==is_old)
      {
         rlx_update(&rstate);
         rlx_converts(&rstate,rs);
         is=8*rstate.ir;
         is_old=is;
      }

      r[k]=rs[is];
   }
}


int rlxs_size(void)
{
   return 104;
}


void rlxs_get(int *state)
{
   rlx_error(init==0,1,"rlxs_get [ranlxs.c]","ranlxs is not initialized");

   state[0]=rlxs_size();
   rlx_get_state(&rstate,state+1);
   state[101]=rstate.pr;
   state[102]=rstate.ir;
   state[103]=is;
}


void rlxs_reset(int *state)
{
   int ie,k;

   rlx_error(rlx_check_machine(),1,"rlxs_reset [ranlxs.c]",
             "Machine does not support the required data types");
   rlx_error(state[0]!=rlxs_size(),1,"rlxs_reset [ranlxs.c]",
             "Unexpected input data");
   ie=((state[101]!=109)&&(state[101]!=202)&&(state[101]!=397));
   ie|=((state[102]<0)||(state[102]>=12));
   ie|=((state[103]<0)||(state[103]>=96));
   rlx_error(ie,2,"rlxs_reset [ranlxs.c]","Unexpected input data");

   if (init==0)
      rlx_alloc_state(&rstate);

   rlx_set_state(state+1,&rstate);
   rstate.pr=state[101];
   rstate.ir=state[102];
   is=state[103];
   is_old=8*rstate.ir;

   for (k=0;k<95;k++)
      next[k]=k+1;
   next[95]=0;

   rlx_converts(&rstate,rs);
   init=1;
}
