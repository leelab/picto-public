//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
// File: randCoderWrap.cpp
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
#include "rand.h"
#include "fprintf.h"

// Function Definitions

//
// Arguments    : double N
//                emxArray_real_T *X
// Return Type  : void
//
void randCoderWrap(double N, emxArray_real_T *X)
{
  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% 
  //
  //   randCoderWrap.m
  //
  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% 
  d_cfprintf();
  b_rand(N, X);
}

//
// File trailer for randCoderWrap.cpp
//
// [EOF]
//
