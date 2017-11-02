//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
// File: stirlerr.cpp
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
#include "stirlerr.h"
#include "gammaln.h"

// Function Definitions

//
// Arguments    : double n
// Return Type  : double
//
double stirlerr(double n)
{
  double delta;
  double nn;
  static const double sfe[31] = { 0.0, 0.1534264097200273, 0.081061466795327261,
    0.054814121051917651, 0.04134069595540929, 0.033162873519936291,
    0.027677925684998338, 0.0237461636562975, 0.02079067210376509,
    0.01848845053267319, 0.01664469118982119, 0.01513497322191738,
    0.01387612882307075, 0.012810465242920231, 0.01189670994589177,
    0.01110455975820868, 0.0104112652619721, 0.0097994161261588022,
    0.0092554621827127329, 0.0087687001341393844, 0.00833056343336287,
    0.0079341145643140217, 0.0075736754879518406, 0.007244554301320383,
    0.00694284010720953, 0.0066652470327076821, 0.0064089941880042071,
    0.0061717122630394576, 0.0059513701127588484, 0.0057462165130101163,
    0.0055547335519628011 };

  nn = n * n;
  if (n <= 15.0) {
    nn = 2.0 * n;
    if (nn == std::floor(nn)) {
      delta = sfe[(int)(nn + 1.0) - 1];
    } else {
      nn = n + 1.0;
      gammaln(&nn);
      delta = ((nn - (n + 0.5) * std::log(n)) + n) - 0.91893853320467278;
    }
  } else if (n <= 35.0) {
    delta = (0.083333333333333329 - (0.0027777777777777779 -
              (0.00079365079365079365 - (0.00059523809523809518 -
                0.00084175084175084182 / nn) / nn) / nn) / nn) / n;
  } else if (n <= 80.0) {
    delta = (0.083333333333333329 - (0.0027777777777777779 -
              (0.00079365079365079365 - 0.00059523809523809518 / nn) / nn) / nn)
      / n;
  } else if (n <= 500.0) {
    delta = (0.083333333333333329 - (0.0027777777777777779 -
              0.00079365079365079365 / nn) / nn) / n;
  } else {
    delta = (0.083333333333333329 - 0.0027777777777777779 / nn) / n;
  }

  return delta;
}

//
// File trailer for stirlerr.cpp
//
// [EOF]
//
