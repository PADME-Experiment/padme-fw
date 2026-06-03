
/*******************************************************************************
*
* File ranlux_common.c
*
* Copyright (C) 2019 Martin Luescher
*
* This software is distributed under the terms of the GNU General Public
* License (GPL)
*
* Collection of common functions used by the programs implementing the
* random number generator RANLUX.
*
* The externally accessible functions are:
*
*   int rlx_check_machine(void)
*     Returns 0 if the data types required by the RANLUX programs are
*     supported and 1 if not.
*
*   void rlx_error(int test,int no,char *name,char *message)
*     Checks whether "test"=0 and, if not, aborts the program with exit(no)
*     after printing the "name" of the calling program and an error "message"
*     to stdout.
*
*   void rlx_alloc_state(rlx_state_t *s)
*     Allocates the state array in the structure "s". The  array is aligned
*     to a 32 byte boundary if the macro SSE2 or AVX2 is defined.
*
*   void rlx_init(rlx_state_t *s,int seed,int flag)
*     Initializes the state "s" of RANLUX using a bit generator with the
*     given "seed". Setting "flag" to a non-zero value indicates that
*     the initialization is for the double-precision generator.
*
*   void rlx_update(rlx_state_t *s)
*     Updates the state "s" of the random number generator (see the notes).
*
*   void rlx_converts(rlx_state_t *s,float *rs)
*     Extracts 96 random single-precision floating-point numbers from the
*     state "s" and assigns them to rs[0],..,rs[95]. Each number is of the
*     form n/2^24, where n is an integer ranging from 0 to 2^24-1.
*
*   void rlx_convertd(rlx_state_t *state,double *rd)
*     Extracts 48 random double-precision floating-point numbers from the
*     state "s" and assigns them to rd[0],..,rd[47]. Each number is of the
*     form n/2^48, where n is an integer ranging from 0 to 2^48-1.
*
*   void rlx_get_state(rlx_state_t *s,int *is)
*     Maps the state vectors and the carry bits contained in the state "s"
*     to an array is[0],..,is[99] of integers in the range from 0 to 2^24-1.
*
*   void rlx_set_state(int *is,rlx_state_t *s)
*     Restores the state vectors and the carry bits in the state "s" from
*     an array is[0],..,is[99] of integers, assuming the array was obtained
*     from a previous call of rlx_get_state(). Various checks are performed
*     to ensure that the state "s" is properly restored.
*
* These programs are used by the functions in the modules ranlxd.c and
* ranlxs.c. They are not intended to be called from any other program and
* their prototypes in ranlux.h are masked accordingly.
*
* A structure of type rlx_state_t contains the states of 4 copies of the
* RANLUX generator, which are initialized differently and updated in parallel.
* The double-word algorithm is used, where the state of a single generator
* is an array of 12 integers plus a carry bit. The elements of the structure
* are:
*
*   int pr;                      Half of the RANLUX p-value.
*   int ir;                      Index of the random number to be
*                                updated next (0<=ir<12).
*   int8_t (*state)[4];          Current state vectors and carry bits.
*
* The last index of the state array labels the 4 copies of the generator.
* After allocation, the state array has length 13, the last element being
* reserved for the carry bits. The data type int8_t is defined in ranlux.h
* and usually is an integer data type of size 8.
*
*******************************************************************************/

#define RANLUX_COMMON_C

#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include <math.h>
#include "ranlux.h"

static const int8_t base=(int8_t)(0x1000000000000);
static const int8_t mask=(int8_t)(0xffffffffffff);


int rlx_check_machine(void)
{
#if ((defined AVX2)||(defined SSE2))
   if ((sizeof(int*)>sizeof(unsigned long))||(sizeof(int8_t)!=8))
      return 1;
#endif

   if (sizeof(int)<4)
      return 1;

   if ((FLT_RADIX!=2)||(FLT_MANT_DIG<24)||(DBL_MANT_DIG<48))
      return 1;

   return 0;
}


void rlx_error(int test,int no,char *name,char *message)
{
   if (test)
   {
      printf("\nError in %s (error no=%d):\n",name,no);
      printf("%s\n",message);
      printf("Program aborted\n");
      exit(no);
   }
}


void rlx_alloc_state(rlx_state_t *s)
{
   int8_t (*state)[4];

#if ((defined AVX2)||(defined SSE2))
   char *addr;
   unsigned long mask;

   addr=malloc(13*sizeof(*state)+32);
   rlx_error(addr==NULL,1,"rlx_alloc_state [ranlux_common.c]",
             "Unable to allocate state array");

   mask=(unsigned long)(31);
   (*s).state=(int8_t(*)[4])(((unsigned long)(addr+32))&(~mask));
#else
   (*s).state=malloc(13*sizeof(*state));
   rlx_error((*s).state==NULL,1,"rlx_alloc_state [ranlux_common.c]",
             "Unable to allocate state array");
#endif
}


void rlx_init(rlx_state_t *s,int seed,int flag)
{
   int i,k,l,ix,iy;
   int ibit,jbit,xbit[31];
   int8_t (*state)[4];

   for (k=0;k<31;k++)
   {
      xbit[k]=seed&0x1;
      seed/=2;
   }

   ibit=0;
   jbit=18;
   state=(*s).state;

   for (i=0;i<4;i++)
   {
      for (k=0;k<24;k++)
      {
         ix=0;

         for (l=0;l<24;l++)
         {
            iy=xbit[ibit];
            ix=2*ix+iy;

            xbit[ibit]=(xbit[ibit]+xbit[jbit])%2;
            ibit=(ibit+1)%31;
            jbit=(jbit+1)%31;
         }

         if (((flag==0)&&((k%4)==i))||((flag!=0)&&((k%4)!=i)))
            ix=16777215-ix;

         if (k&0x1)
            state[k/2][i]+=((int8_t)(ix)<<24);
         else
            state[k/2][i]=(int8_t)(ix);
      }
   }

   (*s).state[12][0]=(int8_t)(0);
   (*s).state[12][1]=(int8_t)(0);
   (*s).state[12][2]=(int8_t)(0);
   (*s).state[12][3]=(int8_t)(0);
   (*s).ir=0;
}

#if (defined AVX2)

#define avx_load_state(reg1,reg2) \
   __asm__ __volatile__ ("vmovdqa %0, "#reg1 \
                         : \
                         : \
                         "m" (pi[0][0]), \
                         "m" (pi[0][1]), \
                         "m" (pi[0][2]), \
                         "m" (pi[0][3]) \
                         : \
                         #reg2); \
   pi+=1; \
   if (pi==pmax) pi=pmin

#define avx_store_state(reg1) \
   __asm__ __volatile__ ("vmovdqa "#reg1", %0" \
                         : \
                         "=m" (pi[0][0]), \
                         "=m" (pi[0][1]), \
                         "=m" (pi[0][2]), \
                         "=m" (pi[0][3])); \
   pi+=1; \
   if (pi==pmax) pi=pmin


void rlx_update(rlx_state_t *s)
{
   int pr,ir,k;
   int8_t shift,(*pmin)[4],(*pmax)[4],(*pi)[4],(*pj)[4];

   pr=(*s).pr;
   ir=(*s).ir;

   pmin=(*s).state;
   pmax=pmin+12;
   pi=pmin+ir;
   if (ir>=5)
      pj=pi-5;
   else
      pj=pi+7;

   shift=(int8_t)(63);

   avx_load_state(%%ymm0,xmm0);
   avx_load_state(%%ymm1,xmm1);
   avx_load_state(%%ymm2,xmm2);
   avx_load_state(%%ymm3,xmm3);
   avx_load_state(%%ymm4,xmm4);
   avx_load_state(%%ymm5,xmm5);
   avx_load_state(%%ymm6,xmm6);
   avx_load_state(%%ymm7,xmm7);
   avx_load_state(%%ymm8,xmm8);
   avx_load_state(%%ymm9,xmm9);
   avx_load_state(%%ymm10,xmm10);
   avx_load_state(%%ymm11,xmm11);

   __asm__ __volatile__ ("vmovdqa %0, %%ymm12 \n\t"
                         "vbroadcastsd %4, %%ymm13 \n\t"
                         "vbroadcastsd %5, %%ymm14 \n\t"
                         "vbroadcastsd %6, %%ymm15"
                         :
                         :
                         "m" (pmax[0][0]),
                         "m" (pmax[0][1]),
                         "m" (pmax[0][2]),
                         "m" (pmax[0][3]),
                         "m" (shift),
                         "m" (base),
                         "m" (mask)
                         :
                         "xmm12", "xmm13", "xmm14", "xmm15");

   for (k=0;k<=(pr-12);k+=12)
   {
      __asm__ __volatile__ ("vpsubq %%ymm0, %%ymm7, %%ymm0 \n\t"
                            "vpsubq %%ymm1, %%ymm8, %%ymm1 \n\t"
                            "vpsubq %%ymm2, %%ymm9, %%ymm2 \n\t"
                            "vpsubq %%ymm3, %%ymm10, %%ymm3 \n\t"
                            "vpsubq %%ymm12, %%ymm0, %%ymm0 \n\t"
                            "vpsrlq %%xmm13, %%ymm0, %%ymm12 \n\t"
                            "vpsubq %%ymm12, %%ymm1, %%ymm1 \n\t"
                            "vpsrlq %%xmm13, %%ymm1, %%ymm12 \n\t"
                            "vpsubq %%ymm12, %%ymm2, %%ymm2 \n\t"
                            "vpsrlq %%xmm13, %%ymm2, %%ymm12 \n\t"
                            "vpsubq %%ymm12, %%ymm3, %%ymm3 \n\t"
                            "vpsrlq %%xmm13, %%ymm3, %%ymm12 \n\t"
                            "vpaddq %%ymm14, %%ymm0, %%ymm0 \n\t"
                            "vpaddq %%ymm14, %%ymm1, %%ymm1 \n\t"
                            "vpaddq %%ymm14, %%ymm2, %%ymm2 \n\t"
                            "vpaddq %%ymm14, %%ymm3, %%ymm3 \n\t"
                            "vpand %%ymm15, %%ymm0, %%ymm0 \n\t"
                            "vpand %%ymm15, %%ymm1, %%ymm1 \n\t"
                            "vpand %%ymm15, %%ymm2, %%ymm2 \n\t"
                            "vpand %%ymm15, %%ymm3, %%ymm3"
                            :
                            :
                            :
                            "xmm0", "xmm1", "xmm2", "xmm3", "xmm12");

      __asm__ __volatile__ ("vpsubq %%ymm4, %%ymm11, %%ymm4 \n\t"
                            "vpsubq %%ymm5, %%ymm0, %%ymm5 \n\t"
                            "vpsubq %%ymm6, %%ymm1, %%ymm6 \n\t"
                            "vpsubq %%ymm7, %%ymm2, %%ymm7 \n\t"
                            "vpsubq %%ymm12, %%ymm4, %%ymm4 \n\t"
                            "vpsrlq %%xmm13, %%ymm4, %%ymm12 \n\t"
                            "vpsubq %%ymm12, %%ymm5, %%ymm5 \n\t"
                            "vpsrlq %%xmm13, %%ymm5, %%ymm12 \n\t"
                            "vpsubq %%ymm12, %%ymm6, %%ymm6 \n\t"
                            "vpsrlq %%xmm13, %%ymm6, %%ymm12 \n\t"
                            "vpsubq %%ymm12, %%ymm7, %%ymm7 \n\t"
                            "vpsrlq %%xmm13, %%ymm7, %%ymm12 \n\t"
                            "vpaddq %%ymm14, %%ymm4, %%ymm4 \n\t"
                            "vpaddq %%ymm14, %%ymm5, %%ymm5 \n\t"
                            "vpaddq %%ymm14, %%ymm6, %%ymm6 \n\t"
                            "vpaddq %%ymm14, %%ymm7, %%ymm7 \n\t"
                            "vpand %%ymm15, %%ymm4, %%ymm4 \n\t"
                            "vpand %%ymm15, %%ymm5, %%ymm5 \n\t"
                            "vpand %%ymm15, %%ymm6, %%ymm6 \n\t"
                            "vpand %%ymm15, %%ymm7, %%ymm7"
                            :
                            :
                            :
                            "xmm4", "xmm5", "xmm6", "xmm7", "xmm12");

      __asm__ __volatile__ ("vpsubq %%ymm8, %%ymm3, %%ymm8 \n\t"
                            "vpsubq %%ymm9, %%ymm4, %%ymm9 \n\t"
                            "vpsubq %%ymm10, %%ymm5, %%ymm10 \n\t"
                            "vpsubq %%ymm11, %%ymm6, %%ymm11 \n\t"
                            "vpsubq %%ymm12, %%ymm8, %%ymm8 \n\t"
                            "vpsrlq %%xmm13, %%ymm8, %%ymm12 \n\t"
                            "vpsubq %%ymm12, %%ymm9, %%ymm9 \n\t"
                            "vpsrlq %%xmm13, %%ymm9, %%ymm12 \n\t"
                            "vpsubq %%ymm12, %%ymm10, %%ymm10 \n\t"
                            "vpsrlq %%xmm13, %%ymm10, %%ymm12 \n\t"
                            "vpsubq %%ymm12, %%ymm11, %%ymm11 \n\t"
                            "vpsrlq %%xmm13, %%ymm11, %%ymm12 \n\t"
                            "vpaddq %%ymm14, %%ymm8, %%ymm8 \n\t"
                            "vpaddq %%ymm14, %%ymm9, %%ymm9 \n\t"
                            "vpaddq %%ymm14, %%ymm10, %%ymm10 \n\t"
                            "vpaddq %%ymm14, %%ymm11, %%ymm11 \n\t"
                            "vpand %%ymm15, %%ymm8, %%ymm8 \n\t"
                            "vpand %%ymm15, %%ymm9, %%ymm9 \n\t"
                            "vpand %%ymm15, %%ymm10, %%ymm10 \n\t"
                            "vpand %%ymm15, %%ymm11, %%ymm11"
                            :
                            :
                            :
                            "xmm8", "xmm9", "xmm10", "xmm11", "xmm12");
   }

   avx_store_state(%%ymm0);
   avx_store_state(%%ymm1);
   avx_store_state(%%ymm2);
   avx_store_state(%%ymm3);
   avx_store_state(%%ymm4);
   avx_store_state(%%ymm5);
   avx_store_state(%%ymm6);
   avx_store_state(%%ymm7);
   avx_store_state(%%ymm8);
   avx_store_state(%%ymm9);
   avx_store_state(%%ymm10);
   avx_store_state(%%ymm11);

   for (;k<pr;k++)
   {
      __asm__ __volatile__ ("vmovdqa %0, %%ymm0"
                            :
                            :
                            "m" (pj[0][0]),
                            "m" (pj[0][1]),
                            "m" (pj[0][2]),
                            "m" (pj[0][3])
                            :
                            "xmm0");

      __asm__ __volatile__ ("vpsubq %4, %%ymm0, %%ymm1 \n\t"
                            "vpsubq %%ymm12, %%ymm1, %%ymm2 \n\t"
                            "vpaddq %%ymm14, %%ymm2, %%ymm1 \n\t"
                            "vpsrlq %%xmm13, %%ymm2, %%ymm12 \n\t"
                            "vpand %%ymm15, %%ymm1, %%ymm3 \n\t"
                            "vmovdqa %%ymm3, %0"
                            :
                            "=m" (pi[0][0]),
                            "=m" (pi[0][1]),
                            "=m" (pi[0][2]),
                            "=m" (pi[0][3])
                            :
                            "m" (pi[0][0]),
                            "m" (pi[0][1]),
                            "m" (pi[0][2]),
                            "m" (pi[0][3])
                            :
                            "xmm1", "xmm2", "xmm3", "xmm12");

      pj+=1;
      if (pj==pmax)
         pj=pmin;

      pi+=1;
      if (pi==pmax)
         pi=pmin;
   }

   __asm__ __volatile__ ("vmovdqa %%ymm12, %0"
                         :
                         "=m" (pmax[0][0]),
                         "=m" (pmax[0][1]),
                         "=m" (pmax[0][2]),
                         "=m" (pmax[0][3]));

   (*s).ir=(ir+pr)%12;
}

#elif (defined SSE2)

void rlx_update(rlx_state_t *s)
{
   int pr,ir,k;
   int8_t shift,(*pmin)[4],(*pmax)[4],(*pi)[4],(*pj)[4];

   pr=(*s).pr;
   ir=(*s).ir;

   pmin=(*s).state;
   pmax=pmin+12;
   pi=pmin+ir;
   if (ir>=5)
      pj=pi-5;
   else
      pj=pi+7;

   shift=(int8_t)(63);

   __asm__ __volatile__ ("movq %0, %%xmm5 \n\t"
                         "movq %1, %%xmm6 \n\t"
                         "movq %2, %%xmm7 \n\t"
                         "movdqa %3, %%xmm3 \n\t"
                         "movdqa %5, %%xmm4 \n\t"
                         "shufpd $0x0, %%xmm6, %%xmm6 \n\t"
                         "shufpd $0x0, %%xmm7, %%xmm7"
                         :
                         :
                         "m" (shift),
                         "m" (base),
                         "m" (mask),
                         "m" (pmax[0][0]),
                         "m" (pmax[0][1]),
                         "m" (pmax[0][2]),
                         "m" (pmax[0][3])
                         :
                         "xmm3", "xmm4", "xmm5",
                         "xmm6", "xmm7");

   for (k=0;k<pr;k++)
   {
      __asm__ __volatile__ ("movdqa %0, %%xmm0 \n\t"
                            "movdqa %2, %%xmm1"
                            :
                            :
                            "m" (pj[0][0]),
                            "m" (pj[0][1]),
                            "m" (pj[0][2]),
                            "m" (pj[0][3])
                            :
                            "xmm0", "xmm1");

      __asm__ __volatile__ ("psubq %0, %%xmm0 \n\t"
                            "psubq %2, %%xmm1 \n\t"
                            "psubq %%xmm3, %%xmm0 \n\t"
                            "psubq %%xmm4, %%xmm1 \n\t"
                            "movdqa %%xmm0, %%xmm3 \n\t"
                            "movdqa %%xmm1, %%xmm4"
                            :
                            :
                            "m" (pi[0][0]),
                            "m" (pi[0][1]),
                            "m" (pi[0][2]),
                            "m" (pi[0][3])
                            :
                            "xmm0", "xmm1", "xmm3", "xmm4");

      __asm__ __volatile__ ("paddq %%xmm6, %%xmm0 \n\t"
                            "paddq %%xmm6, %%xmm1 \n\t"
                            "psrlq %%xmm5, %%xmm3 \n\t"
                            "psrlq %%xmm5, %%xmm4 \n\t"
                            "pand %%xmm7, %%xmm0 \n\t"
                            "pand %%xmm7, %%xmm1 \n\t"
                            "movdqa %%xmm0, %0 \n\t"
                            "movdqa %%xmm1, %2"
                            :
                            "=m" (pi[0][0]),
                            "=m" (pi[0][1]),
                            "=m" (pi[0][2]),
                            "=m" (pi[0][3])
                            :
                            :
                            "xmm0", "xmm1", "xmm3", "xmm4");
      pj+=1;
      if (pj==pmax)
         pj=pmin;

      pi+=1;
      if (pi==pmax)
         pi=pmin;
   }

   __asm__ __volatile__ ("movdqa %%xmm3, %0 \n\t"
                         "movdqa %%xmm4, %2"
                         :
                         "=m" (pmax[0][0]),
                         "=m" (pmax[0][1]),
                         "=m" (pmax[0][2]),
                         "=m" (pmax[0][3]));

   (*s).ir=(ir+pr)%12;
}

#else

void rlx_update(rlx_state_t *s)
{
   int pr,ir,k;
   int8_t d[4],(*pmin)[4],(*pmax)[4],(*pi)[4],(*pj)[4];

   pr=(*s).pr;
   ir=(*s).ir;

   pmin=(*s).state;
   pmax=pmin+12;
   pi=pmin+ir;
   if (ir>=5)
      pj=pi-5;
   else
      pj=pi+7;

   for (k=0;k<pr;k++)
   {
      d[0]=pj[0][0]-pi[0][0]-pmax[0][0];
      d[1]=pj[0][1]-pi[0][1]-pmax[0][1];
      d[2]=pj[0][2]-pi[0][2]-pmax[0][2];
      d[3]=pj[0][3]-pi[0][3]-pmax[0][3];
      pmax[0][0]=(d[0]<0);
      pmax[0][1]=(d[1]<0);
      pmax[0][2]=(d[2]<0);
      pmax[0][3]=(d[3]<0);
      pi[0][0]=(d[0]+base)&mask;
      pi[0][1]=(d[1]+base)&mask;
      pi[0][2]=(d[2]+base)&mask;
      pi[0][3]=(d[3]+base)&mask;

      pj+=1;
      if (pj==pmax)
         pj=pmin;

      pi+=1;
      if (pi==pmax)
         pi=pmin;
   }

   (*s).ir=(ir+pr)%12;
}

#endif

#if ((defined AVX2)||(defined SSE2))

void rlx_converts(rlx_state_t *s,float *rs)
{
   int k;
   int8_t lmask,shift,(*state)[4];
   float onebit;

   onebit=(float)(ldexp(1.0,-24));
   lmask=(int8_t)(0xffffff);
   shift=(int8_t)(24);
   state=(*s).state;

   __asm__ __volatile__ ("movq %0, %%xmm5 \n\t"
                         "movq %1, %%xmm6 \n\t"
                         "movss %2, %%xmm7 \n\t"
                         "shufpd $0x0, %%xmm5, %%xmm5 \n\t"
                         "shufpd $0x0, %%xmm6, %%xmm6 \n\t"
                         "shufps $0x0, %%xmm7, %%xmm7"
                         :
                         :
                         "m" (lmask),
                         "m" (shift),
                         "m" (onebit)
                         :
                         "xmm5", "xmm6", "xmm7");

   for (k=0;k<12;k++)
   {
      __asm__ __volatile__ ("movdqa %0, %%xmm0 \n\t"
                            "movdqa %2, %%xmm1 \n\t"
                            "movdqa %%xmm0, %%xmm2 \n\t"
                            "movdqa %%xmm1, %%xmm3"
                            :
                            :
                            "m" (state[0][0]),
                            "m" (state[0][1]),
                            "m" (state[0][2]),
                            "m" (state[0][3])
                            :
                            "xmm0", "xmm1", "xmm2", "xmm3");

      __asm__ __volatile__ ("pand %%xmm5, %%xmm0 \n\t"
                            "pand %%xmm5, %%xmm1 \n\t"
                            "psrlq %%xmm6, %%xmm2 \n\t"
                            "psrlq %%xmm6, %%xmm3 \n\t"
                            "shufps $0x88, %%xmm1, %%xmm0 \n\t"
                            "shufps $0x88, %%xmm3, %%xmm2 \n\t"
                            "cvtdq2ps %%xmm0, %%xmm1 \n\t"
                            "cvtdq2ps %%xmm2, %%xmm3 \n\t"
                            "mulps %%xmm7, %%xmm1 \n\t"
                            "mulps %%xmm7, %%xmm3 \n\t"
                            "movaps %%xmm1, %0 \n\t"
                            "movaps %%xmm3, %4"
                            :
                            "=m" (rs[0]),
                            "=m" (rs[1]),
                            "=m" (rs[2]),
                            "=m" (rs[3]),
                            "=m" (rs[4]),
                            "=m" (rs[5]),
                            "=m" (rs[6]),
                            "=m" (rs[7])
                            :
                            :
                            "xmm0", "xmm1", "xmm2", "xmm3");

      state+=1;
      rs+=8;
   }
}


void rlx_convertd(rlx_state_t *s,double *rd)
{
   int k;
   int8_t sexp,(*state)[4];

   sexp=(int8_t)(0x4030000000000000);
   state=(*s).state;

   __asm__ __volatile__ ("movq %0, %%xmm6 \n\t"
                         "movq %0, %%xmm7 \n\t"
                         "shufpd $0x0, %%xmm6, %%xmm6 \n\t"
                         "shufpd $0x0, %%xmm7, %%xmm7"
                         :
                         :
                         "m" (sexp)
                         :
                         "xmm6", "xmm7");

   for (k=0;k<6;k++)
   {
      __asm__ __volatile__ ("movdqa %0, %%xmm0 \n\t"
                            "movdqa %2, %%xmm1 \n\t"
                            "movdqa %4, %%xmm2 \n\t"
                            "movdqa %6, %%xmm3"
                            :
                            :
                            "m" (state[0][0]),
                            "m" (state[0][1]),
                            "m" (state[0][2]),
                            "m" (state[0][3]),
                            "m" (state[1][0]),
                            "m" (state[1][1]),
                            "m" (state[1][2]),
                            "m" (state[1][3])
                            :
                            "xmm0", "xmm1", "xmm2", "xmm3");

      __asm__ __volatile__ ("pxor %%xmm6, %%xmm0 \n\t"
                            "pxor %%xmm7, %%xmm1 \n\t"
                            "pxor %%xmm6, %%xmm2 \n\t"
                            "pxor %%xmm7, %%xmm3 \n\t"
                            "subpd %%xmm6, %%xmm0 \n\t"
                            "subpd %%xmm7, %%xmm1 \n\t"
                            "subpd %%xmm6, %%xmm2 \n\t"
                            "subpd %%xmm7, %%xmm3 \n\t"
                            "movapd %%xmm0, %0 \n\t"
                            "movapd %%xmm1, %2 \n\t"
                            "movapd %%xmm2, %4 \n\t"
                            "movapd %%xmm3, %6"
                            :
                            "=m" (rd[0]),
                            "=m" (rd[1]),
                            "=m" (rd[2]),
                            "=m" (rd[3]),
                            "=m" (rd[4]),
                            "=m" (rd[5]),
                            "=m" (rd[6]),
                            "=m" (rd[7])
                            :
                            :
                            "xmm0", "xmm1", "xmm2", "xmm3");

      state+=2;
      rd+=8;
   }
}

#else

void rlx_converts(rlx_state_t *s,float *rs)
{
   int k;
   int8_t lmask,(*state)[4];
   float onebit;

   onebit=(float)(ldexp(1.0,-24));
   lmask=(int8_t)(0xffffff);
   state=(*s).state;

   for (k=0;k<12;k++)
   {
      rs[0]=(float)((*state)[0]&lmask)*onebit;
      rs[1]=(float)((*state)[1]&lmask)*onebit;
      rs[2]=(float)((*state)[2]&lmask)*onebit;
      rs[3]=(float)((*state)[3]&lmask)*onebit;

      rs[4]=(float)((*state)[0]>>24)*onebit;
      rs[5]=(float)((*state)[1]>>24)*onebit;
      rs[6]=(float)((*state)[2]>>24)*onebit;
      rs[7]=(float)((*state)[3]>>24)*onebit;

      rs+=8;
      state+=1;
   }
}


void rlx_convertd(rlx_state_t *s,double *rd)
{
   int k;
   int8_t (*state)[4];
   double onebit;

   onebit=ldexp(1.0,-48);
   state=(*s).state;

   for (k=0;k<12;k++)
   {
      rd[0]=(double)((*state)[0])*onebit;
      rd[1]=(double)((*state)[1])*onebit;
      rd[2]=(double)((*state)[2])*onebit;
      rd[3]=(double)((*state)[3])*onebit;

      rd+=4;
      state+=1;
   }
}

#endif

void rlx_get_state(rlx_state_t *s,int *is)
{
   int k;
   int8_t lmask,(*state)[4];

   lmask=(int8_t)(0xffffff);
   state=(*s).state;

   for (k=0;k<12;k++)
   {
      is[0]=(int)((*state)[0]&lmask);
      is[1]=(int)((*state)[1]&lmask);
      is[2]=(int)((*state)[2]&lmask);
      is[3]=(int)((*state)[3]&lmask);

      is[4]=(int)((*state)[0]>>24);
      is[5]=(int)((*state)[1]>>24);
      is[6]=(int)((*state)[2]>>24);
      is[7]=(int)((*state)[3]>>24);

      is+=8;
      state+=1;
   }

   is[0]=(int)((*state)[0]);
   is[1]=(int)((*state)[1]);
   is[2]=(int)((*state)[2]);
   is[3]=(int)((*state)[3]);
}


void rlx_set_state(int *is,rlx_state_t *s)
{
   int ie,k,lbase;
   int8_t (*state)[4];

   ie=0;
   lbase=0x1000000;
   state=(*s).state;

   for (k=0;k<96;k++)
      ie|=((is[k]<0)||(is[k]>=lbase));

   ie|=((is[96]<0)||(is[96]>1));
   ie|=((is[97]<0)||(is[97]>1));
   ie|=((is[98]<0)||(is[98]>1));
   ie|=((is[99]<0)||(is[99]>1));

   rlx_error(ie,1,"rlx_set_state [ranlux_common.c]",
             "Input numbers are out of range");

   for (k=0;k<12;k++)
   {
      (*state)[0]=(int8_t)(is[0]);
      (*state)[1]=(int8_t)(is[1]);
      (*state)[2]=(int8_t)(is[2]);
      (*state)[3]=(int8_t)(is[3]);

      (*state)[0]+=((int8_t)(is[4])<<24);
      (*state)[1]+=((int8_t)(is[5])<<24);
      (*state)[2]+=((int8_t)(is[6])<<24);
      (*state)[3]+=((int8_t)(is[7])<<24);

      is+=8;
      state+=1;
   }

   (*state)[0]=(int8_t)(is[0]);
   (*state)[1]=(int8_t)(is[1]);
   (*state)[2]=(int8_t)(is[2]);
   (*state)[3]=(int8_t)(is[3]);
}
