//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
// File: eml_rand_mt19937ar_stateful.cpp
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
#include "eml_rand_mt19937ar_stateful.h"
#include "binocdfCoderWrap_data.h"

// Function Definitions

//
// Arguments    : void
// Return Type  : void
//
void c_eml_rand_mt19937ar_stateful_i()
{
  unsigned int r;
  int mti;
  memset(&state[0], 0, 625U * sizeof(unsigned int));
  r = 5489U;
  state[0] = 5489U;
  for (mti = 0; mti < 623; mti++) {
    r = (r ^ r >> 30U) * 1812433253U + (1 + mti);
    state[mti + 1] = r;
  }

  state[624] = 624U;
}

//
// File trailer for eml_rand_mt19937ar_stateful.cpp
//
// [EOF]
//
