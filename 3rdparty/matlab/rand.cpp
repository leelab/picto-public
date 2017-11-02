//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
// File: rand.cpp
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
#include "randn.h"
#include "binocdfCoderWrap_emxutil.h"
#include "binocdfCoderWrap_data.h"

// Function Declarations
static double b_eml_rand_mt19937ar(unsigned int b_state[625]);

// Function Definitions

//
// Arguments    : unsigned int b_state[625]
// Return Type  : double
//
static double b_eml_rand_mt19937ar(unsigned int b_state[625])
{
  double r;
  int exitg1;
  unsigned int u[2];
  unsigned int b_r;
  int mti;

  // ========================= COPYRIGHT NOTICE ============================
  //  This is a uniform (0,1) pseudorandom number generator based on:
  //
  //  A C-program for MT19937, with initialization improved 2002/1/26.
  //  Coded by Takuji Nishimura and Makoto Matsumoto.
  //
  //  Copyright (C) 1997 - 2002, Makoto Matsumoto and Takuji Nishimura,
  //  All rights reserved.
  //
  //  Redistribution and use in source and binary forms, with or without
  //  modification, are permitted provided that the following conditions
  //  are met:
  //
  //    1. Redistributions of source code must retain the above copyright
  //       notice, this list of conditions and the following disclaimer.
  //
  //    2. Redistributions in binary form must reproduce the above copyright
  //       notice, this list of conditions and the following disclaimer
  //       in the documentation and/or other materials provided with the
  //       distribution.
  //
  //    3. The names of its contributors may not be used to endorse or
  //       promote products derived from this software without specific
  //       prior written permission.
  //
  //  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
  //  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
  //  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
  //  A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT
  //  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
  //  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  //  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
  //  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
  //  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  //  (INCLUDING  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  //  OF THIS  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  //
  // =============================   END   =================================
  do {
    exitg1 = 0;
    genrand_uint32_vector(b_state, u);
    r = 1.1102230246251565E-16 * ((double)(u[0] >> 5U) * 6.7108864E+7 + (double)
      (u[1] >> 6U));
    if (r == 0.0) {
      if (!is_valid_state(b_state)) {
        b_r = 5489U;
        b_state[0] = 5489U;
        for (mti = 0; mti < 623; mti++) {
          b_r = (b_r ^ b_r >> 30U) * 1812433253U + (1 + mti);
          b_state[mti + 1] = b_r;
        }

        b_state[624] = 624U;
      }
    } else {
      exitg1 = 1;
    }
  } while (exitg1 == 0);

  return r;
}

//
// Arguments    : double varargin_1
//                emxArray_real_T *r
// Return Type  : void
//
void b_rand(double varargin_1, emxArray_real_T *r)
{
  int i4;
  int k;
  i4 = r->size[0] * r->size[1];
  r->size[0] = (int)varargin_1;
  r->size[1] = (int)varargin_1;
  emxEnsureCapacity((emxArray__common *)r, i4, (int)sizeof(double));
  i4 = r->size[0] * r->size[1];
  for (k = 0; k < i4; k++) {
    r->data[k] = b_eml_rand_mt19937ar(state);
  }
}

//
// File trailer for rand.cpp
//
// [EOF]
//
