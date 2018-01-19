//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
// File: binodeviance.cpp
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
#include "binodeviance.h"

// Function Definitions

//
// Arguments    : double x
//                double np
// Return Type  : double
//
double binodeviance(double x, double np)
{
  double bd0;
  double v;
  double e;
  double j;
  double vv;
  double d;
  int exponent;
  if (std::abs(x - np) < 0.1 * (x + np)) {
    bd0 = (x - np) * (x - np) / (x + np);
    v = (x - np) / (x + np);
    e = 2.0 * x * v;
    j = 1.0;
    vv = v * v;
    do {
      e *= vv;
      j += 2.0;
      d = e / j;
      bd0 += d;
      v = std::abs(bd0);
      if ((!rtIsInf(v)) && (!rtIsNaN(v))) {
        if (v <= 2.2250738585072014E-308) {
          v = 4.94065645841247E-324;
        } else {
          frexp(v, &exponent);
          v = std::ldexp(1.0, exponent - 53);
        }
      } else {
        v = rtNaN;
      }
    } while (!(std::abs(d) <= v));
  } else {
    bd0 = (x * std::log(x / np) + np) - x;
  }

  return bd0;
}

//
// File trailer for binodeviance.cpp
//
// [EOF]
//
