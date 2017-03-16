#include <R.h>
#include <Rinternals.h>
#include <stdlib.h> // for NULL
#include <R_ext/Rdynload.h>

/* FIXME:
Check these declarations against the C/Fortran source code.
*/

/* .Call calls */
extern SEXP qualpalr_edist(SEXP);
extern SEXP qualpalr_farthest_points(SEXP, SEXP);

static const R_CallMethodDef CallEntries[] = {
  {"qualpalr_edist",           (DL_FUNC) &qualpalr_edist,           1},
  {"qualpalr_farthest_points", (DL_FUNC) &qualpalr_farthest_points, 2},
  {NULL, NULL, 0}
};

void R_init_qualpalr(DllInfo *dll)
{
  R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
  R_useDynamicSymbols(dll, FALSE);
}