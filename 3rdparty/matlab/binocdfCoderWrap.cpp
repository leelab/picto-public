//
// Academic License - for use in teaching, academic research, and meeting
// course requirements at degree granting institutions only.  Not for
// government, commercial, or other organizational use.
// File: binocdfCoderWrap.cpp
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
void binocdfCoderWrap(const emxArray_real_T *x, const emxArray_real_T *N, const
                      emxArray_real_T *p, emxArray_real_T *w)
{
  int c;
  emxArray_real_T *ztemp;
  int i0;
  emxArray_real_T *b_ztemp;
  double nk;
  double pk;
  double xf;
  double mu;
  double sd;
  double x2;
  double t2;
  int j;

  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% 
  //
  //   binocdfCoderWrap.m
  //
  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% 
  cfprintf();
  if (x->size[1] <= N->size[1]) {
    c = x->size[1];
  } else {
    c = N->size[1];
  }

  emxInit_real_T(&ztemp, 2);
  i0 = ztemp->size[0] * ztemp->size[1];
  ztemp->size[0] = 1;
  ztemp->size[1] = c;
  emxEnsureCapacity((emxArray__common *)ztemp, i0, (int)sizeof(double));
  if (ztemp->size[1] <= p->size[1]) {
    c = ztemp->size[1];
  } else {
    c = p->size[1];
  }

  emxFree_real_T(&ztemp);
  emxInit_real_T(&b_ztemp, 2);
  i0 = b_ztemp->size[0] * b_ztemp->size[1];
  b_ztemp->size[0] = 1;
  b_ztemp->size[1] = c;
  emxEnsureCapacity((emxArray__common *)b_ztemp, i0, (int)sizeof(double));
  i0 = w->size[0] * w->size[1];
  w->size[0] = 1;
  w->size[1] = c;
  emxEnsureCapacity((emxArray__common *)w, i0, (int)sizeof(double));
  for (c = 0; c < b_ztemp->size[1]; c++) {
    nk = N->data[c];
    pk = p->data[c];
    if ((p->data[c] >= 0.0) && (p->data[c] <= 1.0) && (N->data[c] == std::floor
         (N->data[c])) && (!rtIsNaN(x->data[c]))) {
      if (x->data[c] >= N->data[c]) {
        w->data[c] = 1.0;
      } else if ((p->data[c] == 0.0) && (x->data[c] >= 0.0)) {
        w->data[c] = 1.0;
      } else if (p->data[c] == 1.0) {
        w->data[c] = (x->data[c] >= N->data[c]);
      } else if (x->data[c] >= 0.0) {
        xf = std::floor(x->data[c]);
        if (N->data[c] < 100000.0) {
          if ((x->data[c] <= N->data[c] * p->data[c]) || (p->data[c] < 0.0001))
          {
            t2 = 0.0;
            for (j = 0; j < (int)(xf + 1.0); j++) {
              t2 += binopdf((double)j, nk, pk);
            }

            if (t2 > 1.0) {
              t2 = 1.0;
            }

            w->data[c] = t2;
          } else {
            x2 = (N->data[c] - xf) - 1.0;
            t2 = 0.0;
            for (j = 0; j < (int)(x2 + 1.0); j++) {
              t2 += binopdf((double)j, nk, 1.0 - pk);
            }

            if (t2 > 1.0) {
              t2 = 1.0;
            }

            w->data[c] = 1.0 - t2;
          }
        } else {
          mu = N->data[c] * p->data[c];
          sd = std::sqrt(N->data[c] * p->data[c] * (1.0 - p->data[c]));
          t2 = 10.0;
          x2 = std::ceil(mu + 10.0 * sd);
          while ((xf <= x2) && (binopdf(x2, nk, pk) > 0.0)) {
            t2 *= 1.5;
            x2 = std::ceil(mu + t2 * sd);
          }

          if (xf >= x2) {
            t2 = 1.0;
          } else {
            t2 = 40.0;
            x2 = std::floor(mu - 40.0 * sd);
            while ((0.0 < x2) && (binopdf(x2, nk, pk) > 2.2204460492503131E-16))
            {
              t2 *= 1.5;
              x2 = std::floor(mu - t2 * sd);
            }

            if (x2 < 0.0) {
              x2 = 0.0;
            }

            if (xf < x2) {
              t2 = 0.0;
            } else {
              t2 = 0.0;
              i0 = (int)(xf + (1.0 - x2));
              for (j = 0; j < i0; j++) {
                t2 += binopdf(x2 + (double)j, nk, pk);
              }

              if (t2 > 1.0) {
                t2 = 1.0;
              }
            }
          }

          w->data[c] = t2;
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
// File trailer for binocdfCoderWrap.cpp
//
// [EOF]
//
