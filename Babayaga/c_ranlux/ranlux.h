
/*******************************************************************************
*
* File ranlux.h
*
* Copyright (C) 2019 Martin Luescher
*
* This software is distributed under the terms of the GNU General Public
* License (GPL)
*
*******************************************************************************/

#ifndef RANLUX_H
#define RANLUX_H

/* RANLXS */
extern void ranlxs(float *r,int n);
extern void rlxs_init(int level,int seed);
extern int rlxs_size(void);
extern void rlxs_get(int *state);
extern void rlxs_reset(int *state);

/* RANLXD */
extern void ranlxd(double *r,int n);
extern void rlxd_init(int level,int seed);
extern int rlxd_size(void);
extern void rlxd_get(int *state);
extern void rlxd_reset(int *state);

#if ((defined RANLXS_C)||(defined RANLXD_C)||(defined RANLUX_COMMON_C))

#include <limits.h>

#if (INT_MAX>=0x1000000000000)
typedef int int8_t;
#elif (LONG_MAX>=0x1000000000000)
typedef long int8_t;
#elif (LLONG_MAX>=0x1000000000000)
typedef long long int8_t;
#else
#error There is no integer data type of size greater than 6
#endif

typedef struct
{
   int pr,ir;
   int8_t (*state)[4];
} rlx_state_t;

/* RANLUX_COMMON_C */
extern int rlx_check_machine(void);
extern void rlx_error(int test,int no,char *name,char *format);
extern void rlx_alloc_state(rlx_state_t *s);
extern void rlx_init(rlx_state_t *s,int seed,int flag);
extern void rlx_update(rlx_state_t *s);
extern void rlx_converts(rlx_state_t *s,float *rs);
extern void rlx_convertd(rlx_state_t *s,double *rd);
extern void rlx_get_state(rlx_state_t *s,int *is);
extern void rlx_set_state(int *is,rlx_state_t *s);

#endif
#endif
