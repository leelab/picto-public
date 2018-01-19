//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
// File: binopdfCoderWrap.cpp
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
#include "gammaln.h"
#include "binodeviance.h"
#include "stirlerr.h"
#include "binocdfCoderWrap_emxutil.h"
#include "fprintf.h"

// Function Definitions

//
// Arguments    : const emxArray_real_T *x
//                const emxArray_real_T *N
//                const emxArray_real_T *p
//                emxArray_real_T *w
// Return Type  : void
//
void binopdfCoderWrap(const emxArray_real_T *x, const emxArray_real_T *N, const
                      emxArray_real_T *p, emxArray_real_T *w)
{
  int c;
  emxArray_real_T *ztemp;
  int i1;
  emxArray_real_T *b_ztemp;
  double lc;
  double d3;
  double d4;
  double d5;

  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% 
  //
  //   binopdfCoderWrap.m
  //
  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% 
  b_cfprintf();
  if (x->size[1] <= N->size[1]) {
    c = x->size[1];
  } else {
    c = N->size[1];
  }

  emxInit_real_T(&ztemp, 2);
  i1 = ztemp->size[0] * ztemp->size[1];
  ztemp->size[0] = 1;
  ztemp->size[1] = c;
  emxEnsureCapacity((emxArray__common *)ztemp, i1, (int)sizeof(double));
  if (ztemp->size[1] <= p->size[1]) {
    c = ztemp->size[1];
  } else {
    c = p->size[1];
  }

  emxFree_real_T(&ztemp);
  emxInit_real_T(&b_ztemp, 2);
  i1 = b_ztemp->size[0] * b_ztemp->size[1];
  b_ztemp->size[0] = 1;
  b_ztemp->size[1] = c;
  emxEnsureCapacity((emxArray__common *)b_ztemp, i1, (int)sizeof(double));
  i1 = w->size[0] * w->size[1];
  w->size[0] = 1;
  w->size[1] = c;
  emxEnsureCapacity((emxArray__common *)w, i1, (int)sizeof(double));
  for (c = 0; c < b_ztemp->size[1]; c++) {
    if ((p->data[c] >= 0.0) && (p->data[c] <= 1.0) && (N->data[c] == std::floor
         (N->data[c])) && (!rtIsNaN(x->data[c]))) {
      if ((x->data[c] >= 0.0) && (x->data[c] == std::floor(x->data[c])) &&
          (x->data[c] <= N->data[c])) {
        if (p->data[c] == 0.0) {
          w->data[c] = (x->data[c] == 0.0);
        } else if (p->data[c] == 1.0) {
          w->data[c] = (x->data[c] == N->data[c]);
        } else if (x->data[c] == 0.0) {
          w->data[c] = std::exp(N->data[c] * std::log(1.0 - p->data[c]));
        } else if (x->data[c] == N->data[c]) {
          w->data[c] = std::exp(N->data[c] * std::log(p->data[c]));
        } else if (N->data[c] < 10.0) {
          lc = -p->data[c];
          if (!(std::abs(-p->data[c]) < 2.2204460492503131E-16)) {
            lc = std::log(1.0 + -p->data[c]) * (-p->data[c] / ((1.0 + -p->data[c])
              - 1.0));
          }

          d3 = N->data[c] + 1.0;
          gammaln(&d3);
          d4 = x->data[c] + 1.0;
          gammaln(&d4);
          d5 = (N->data[c] - x->data[c]) + 1.0;
          gammaln(&d5);
          lc = (((d3 - d4) - d5) + x->data[c] * std::log(p->data[c])) + (N->
            data[c] - x->data[c]) * lc;
          w->data[c] = std::exp(lc);
        } else {
          lc = (((stirlerr(N->data[c]) - stirlerr(x->data[c])) - stirlerr
                 (N->data[c] - x->data[c])) - binodeviance(x->data[c], N->data[c]
                 * p->data[c])) - binodeviance(N->data[c] - x->data[c], N->
            data[c] * (1.0 - p->data[c]));
          w->data[c] = std::exp(lc) * std::sqrt(N->data[c] / (6.2831853071795862
            * x->data[c] * (N->data[c] - x->data[c])));
        }
      } else {
        w->data[c] = 0.0;
      }
    } else {
      w->data[c] = rtNaN;
    }
  }

  emxFree_real_T(&b_ztemp);
}

//
// File trailer for binopdfCoderWrap.cpp
//
// [EOF]
//
