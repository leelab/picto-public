//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
// File: binopdf.cpp
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
#include "binopdf.h"
#include "gammaln.h"
#include "binodeviance.h"
#include "stirlerr.h"

// Function Definitions

//
// Arguments    : double x
//                double n
//                double p
// Return Type  : double
//
double binopdf(double x, double n, double p)
{
  double y;
  double lc;
  double d0;
  double d1;
  double d2;
  if ((n == std::floor(n)) && (!rtIsNaN(x))) {
    if ((x >= 0.0) && (x == std::floor(x)) && (x <= n)) {
      if (p == 0.0) {
        y = (x == 0.0);
      } else if (p == 1.0) {
        y = (x == n);
      } else if (x == 0.0) {
        y = std::exp(n * std::log(1.0 - p));
      } else if (x == n) {
        y = std::exp(n * std::log(p));
      } else if (n < 10.0) {
        lc = -p;
        if (!(std::abs(-p) < 2.2204460492503131E-16)) {
          lc = std::log(1.0 + -p) * (-p / ((1.0 + -p) - 1.0));
        }

        d0 = n + 1.0;
        gammaln(&d0);
        d1 = x + 1.0;
        gammaln(&d1);
        d2 = (n - x) + 1.0;
        gammaln(&d2);
        y = std::exp((((d0 - d1) - d2) + x * std::log(p)) + (n - x) * lc);
      } else {
        lc = (((stirlerr(n) - stirlerr(x)) - stirlerr(n - x)) - binodeviance(x,
               n * p)) - binodeviance(n - x, n * (1.0 - p));
        y = std::exp(lc) * std::sqrt(n / (6.2831853071795862 * x * (n - x)));
      }
    } else {
      y = 0.0;
    }
  } else {
    y = rtNaN;
  }

  return y;
}

//
// File trailer for binopdf.cpp
//
// [EOF]
//
