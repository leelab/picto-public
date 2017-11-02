//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
// File: fprintf.cpp
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
#include "fprintf.h"
#include "fileManager.h"

// Function Definitions

//
// Arguments    : void
// Return Type  : int
//
int b_cfprintf()
{
  int nbytesint;
  FILE * b_NULL;
  FILE * filestar;
  boolean_T autoflush;
  static const char cfmt[19] = { 'b', 'i', 'n', 'o', 'p', 'd', 'f', 'C', 'o',
    'd', 'e', 'r', 'W', 'r', 'a', 'p', '.', '\x0a', '\x00' };

  b_NULL = NULL;
  nbytesint = 0;
  b_fileManager(&filestar, &autoflush);
  if (!(filestar == b_NULL)) {
    nbytesint = fprintf(filestar, cfmt);
    fflush(filestar);
  }

  return nbytesint;
}

//
// Arguments    : void
// Return Type  : int
//
int c_cfprintf()
{
  int nbytesint;
  FILE * b_NULL;
  FILE * filestar;
  boolean_T autoflush;
  static const char cfmt[19] = { 'n', 'o', 'r', 'm', 'r', 'n', 'd', 'C', 'o',
    'd', 'e', 'r', 'W', 'r', 'a', 'p', '.', '\x0a', '\x00' };

  b_NULL = NULL;
  nbytesint = 0;
  c_fileManager(&filestar, &autoflush);
  if (!(filestar == b_NULL)) {
    nbytesint = fprintf(filestar, cfmt);
    fflush(filestar);
  }

  return nbytesint;
}

//
// Arguments    : void
// Return Type  : int
//
int cfprintf()
{
  int nbytesint;
  FILE * b_NULL;
  FILE * filestar;
  boolean_T autoflush;
  static const char cfmt[19] = { 'b', 'i', 'n', 'o', 'c', 'd', 'f', 'C', 'o',
    'd', 'e', 'r', 'W', 'r', 'a', 'p', '.', '\x0a', '\x00' };

  b_NULL = NULL;
  nbytesint = 0;
  fileManager(&filestar, &autoflush);
  if (!(filestar == b_NULL)) {
    nbytesint = fprintf(filestar, cfmt);
    fflush(filestar);
  }

  return nbytesint;
}

//
// Arguments    : void
// Return Type  : int
//
int d_cfprintf()
{
  int nbytesint;
  FILE * b_NULL;
  FILE * filestar;
  boolean_T autoflush;
  static const char cfmt[16] = { 'r', 'a', 'n', 'd', 'C', 'o', 'd', 'e', 'r',
    'W', 'r', 'a', 'p', '.', '\x0a', '\x00' };

  b_NULL = NULL;
  nbytesint = 0;
  d_fileManager(&filestar, &autoflush);
  if (!(filestar == b_NULL)) {
    nbytesint = fprintf(filestar, cfmt);
    fflush(filestar);
  }

  return nbytesint;
}

//
// File trailer for fprintf.cpp
//
// [EOF]
//
