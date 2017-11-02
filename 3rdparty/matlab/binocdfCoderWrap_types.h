//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
// File: binocdfCoderWrap_types.h
//
// MATLAB Coder version            : 3.2
// C/C++ source code generated on  : 01-Nov-2017 16:14:32
//
#ifndef BINOCDFCODERWRAP_TYPES_H
#define BINOCDFCODERWRAP_TYPES_H

// Include Files
#include "rtwtypes.h"

// Type Definitions
#include <stdio.h>
#ifndef struct_emxArray__common
#define struct_emxArray__common

struct emxArray__common
{
  void *data;
  int *size;
  int allocatedSize;
  int numDimensions;
  boolean_T canFreeData;
};

#endif                                 //struct_emxArray__common

#ifndef struct_emxArray_real_T
#define struct_emxArray_real_T

struct emxArray_real_T
{
  double *data;
  int *size;
  int allocatedSize;
  int numDimensions;
  boolean_T canFreeData;
};

#endif                                 //struct_emxArray_real_T
#endif

//
// File trailer for binocdfCoderWrap_types.h
//
// [EOF]
//
