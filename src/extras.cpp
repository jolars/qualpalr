#include <RcppArmadillo.h>
// [[Rcpp::depends(RcppArmadillo)]]

using namespace Rcpp;

// [[Rcpp::export]]
NumericMatrix dist2(NumericMatrix Ar) {
  int m = Ar.nrow(),
    k = Ar.ncol();
  arma::mat A = arma::mat(Ar.begin(), m, k, false);

  arma::colvec An = sum(square(A),1);

  arma::mat C = -2 * (A * A.t());
  C.each_col() += An;
  C.each_row() += An.t();

  return wrap(pow(sqrt(C), 0.74) * 1.28);
}
