//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
// File: randn.h
//
// MATLAB Coder version            : 3.2
// C/C++ source code generated on  : 01-Nov-2017 16:14:32
//
#ifndef RANDN_H
#define RANDN_H

// Include Files
#include <cmath>
#include <math.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "rt_nonfinite.h"
#include "rtwtypes.h"
#include "binocdfCoderWrap_types.h"

// Function Declarations
extern void genrand_uint32_vector(unsigned int mt[625], unsigned int u[2]);
extern boolean_T is_valid_state(const unsigned int mt[625]);
extern void randn(const double varargin_1[2], emxArray_real_T *r);

#endif

//
// File trailer for randn.h
//
// [EOF]
//
