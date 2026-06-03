
/*wrappers for ranlux (C version), which is much faster than fortran implementation*/

#include <stdio.h>
#include <ranlux.h>
/*#include <ranlx.h>*/
/*#include <ranlxs.h>*/

int iinirnlx = 0;

void drnlx_c_init_(int* ilux, int* n){
  /* int ilux;*/
  /* for double precision rlx ilux = 1 or 2 */
  /* according to doc, ilux=0 or 1 correspond to 3 or 4 in the fortran ranlux */
  /* ilux=1 is adequate, ilux=2 is twice slower*/


  /*printf("iinirnlx %u\n",iinirnlx);*/
    
  if (iinirnlx == 0){
#if (defined AVX2)
   printf("\nc_ranlux: using AVX2 inline assembly.\n\n");
#elif (defined SSE2)
   printf("\nc_ranlux: using SSE2 inline assembly.\n\n");
#endif  
  }
  iinirnlx = iinirnlx+1;
  
  rlxd_init(*ilux,*n);
}

void srnlx_c_init_(int* ilux, int* n){
  /* int ilux; */
  /* ilux = 1; */

  if (iinirnlx == 0){
#if (defined AVX2)
   printf("c_ranlux: using AVX2 inline assembly.\n\n");
#elif (defined SSE2)
   printf("c_ranlux: using SSE2 inline assembly.\n\n");
#endif  
  }
  iinirnlx = iinirnlx+1;
  
  rlxs_init(*ilux,*n);
}

void drnlx_c_(double* x, int* n){
  /*  printf("%u\n",*n);*/
  ranlxd(x,*n);
  /*  printf("%f\n",x[2]);*/
}


void srnlx_c_(float* x, int* n){
  /*  printf("%u\n",*n);*/
  ranlxs(x,*n);
  /*  printf("%f\n",x[2]);*/
}

void rnlxdstate_c_(int* n){
  rlxd_get(n);
}

void rnlxsstate_c_(int* n){
  rlxs_get(n);
}

void rnlxdreset_c_(int* n){
  printf("\nResetting c_ranlux from\n");
  for(int i = 0; i < rlxd_size(); i++){
    printf("%u ",n[i]);
  }
  printf("\n\n");
  rlxd_reset(n);
}

void rnlxsreset_c_(int* n){
  printf("\nResetting c_ranlux from\n");
  for(int i = 0; i < rlxd_size(); i++){
    printf("%u ",n[i]);
  }
  printf("\n\n");
  rlxs_reset(n);
}

int rnlxstatesize_c_(){
  return rlxd_size();
}
