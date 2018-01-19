//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
// File: normrndCoderWrap.cpp
//
// MATLAB Coder version            : 3.2
// C/C++ source code generated on  : 01-Nov-2017 16:14:32
//

// Include Files
#include "rt_nonfinite.h"
#include "binocdfCoderWrap.h"
#include "binopdfCoderWrap.h"
#include "normrndCoderWrap.h"
#include "randCoderWrap.h"
#include "binocdfCoderWrap_emxutil.h"
#include "randn.h"
#include "fprintf.h"

// Function Definitions

//
// Arguments    : const emxArray_real_T *mu
//                const emxArray_real_T *sigma
//                emxArray_real_T *R
// Return Type  : void
//
void normrndCoderWrap(const emxArray_real_T *mu, const emxArray_real_T *sigma,
                      emxArray_real_T *R)
{
  int c;
  double dv0[2];
  int i2;

  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% 
  //
  //   normrndCoderWrap.m
  //
  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% 
  c_cfprintf();
  if (mu->size[1] <= sigma->size[1]) {
    c = mu->size[1];
  } else {
    c = sigma->size[1];
  }

  dv0[0] = 1.0;
  dv0[1] = c;
  randn(dv0, R);
  i2 = R->size[0] * R->size[1];
  R->size[0] = 1;
  R->size[1] = c;
  emxEnsureCapacity((emxArray__common *)R, i2, (int)sizeof(double));
  for (i2 = 0; i2 < c; i2++) {
    R->data[R->size[0] * i2] = R->data[i2] * sigma->data[sigma->size[0] * i2] +
      mu->data[mu->size[0] * i2];
  }

  if (sigma->size[1] == 1) {
    if (sigma->data[0] < 0.0) {
      i2 = R->size[0] * R->size[1];
      R->size[0] = 1;
      emxEnsureCapacity((emxArray__common *)R, i2, (int)sizeof(double));
      c = R->size[1];
      for (i2 = 0; i2 < c; i2++) {
        R->data[R->size[0] * i2] = rtNaN;
      }
    }
  } else {
    i2 = R->size[1];
    for (c = 0; c < i2; c++) {
      if (sigma->data[c] < 0.0) {
        R->data[c] = rtNaN;
      }
    }
  }
}

//
// File trailer for normrndCoderWrap.cpp
//
// [EOF]
//
