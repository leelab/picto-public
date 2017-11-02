//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
// File: binocdfCoderWrap_emxutil.h
//
// MATLAB Coder version            : 3.2
// C/C++ source code generated on  : 01-Nov-2017 16:14:32
//
#ifndef BINOCDFCODERWRAP_EMXUTIL_H
#define BINOCDFCODERWRAP_EMXUTIL_H

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
extern void emxEnsureCapacity(emxArray__common *emxArray, int oldNumel, int
  elementSize);
extern void emxFree_real_T(emxArray_real_T **pEmxArray);
extern void emxInit_real_T(emxArray_real_T **pEmxArray, int numDimensions);

#endif

//
// File trailer for binocdfCoderWrap_emxutil.h
//
// [EOF]
//
