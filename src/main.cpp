// Adopted from http://gallery.rcpp.org/articles/parallel-distance-matrix/

#include <RcppArmadilloExtensions/sample.h>
#include <RcppParallel.h>

using namespace Rcpp;
using namespace RcppParallel;

// [[Rcpp::depends(RcppParallel, RcppArmadillo)]]

template <typename InputIterator1, typename InputIterator2>
inline double euclid(InputIterator1 begin1, InputIterator1 end1,
                     InputIterator2 begin2, InputIterator2 end2) {

  double out = 0;

  InputIterator1 it1 = begin1;
  InputIterator2 it2 = begin2;

  while (it1 != end1)
    out += pow(*it1++ - *it2++, 2);

  return pow(sqrt(out), 0.74) * 1.28;
}

struct dist_worker : public Worker {
  const RMatrix<double> mat;
  RMatrix<double> rmat;
  dist_worker(const NumericMatrix mat, NumericMatrix rmat)
    : mat(mat), rmat(rmat) {}

  void operator()(std::size_t begin, std::size_t end) {
    for (std::size_t i = begin; i < end; i++) {
      for (std::size_t j = 0; j < i; j++) {
        RMatrix<double>::Row row1 = mat.row(i);
        RMatrix<double>::Row row2 = mat.row(j);
        double d = euclid(row1.begin(), row1.end(), row2.begin(), row2.end());
        rmat(i, j) = d;
        rmat(j, i) = d;
      }
    }
  }
};

// [[Rcpp::export]]
NumericMatrix edist(const NumericMatrix mat) {

  NumericMatrix rmat(mat.nrow(), mat.nrow());
  dist_worker dist_worker(mat, rmat);
  parallelFor(0, mat.nrow(), dist_worker);

  return rmat;
}

arma::uvec std_setdiff(const arma::uvec & x, const arma::uvec & y) {

  std::vector<int> a = arma::conv_to< std::vector<int> >::from(arma::sort(x));
  std::vector<int> b = arma::conv_to< std::vector<int> >::from(arma::sort(y));
  std::vector<int> out;

  std::set_difference(a.begin(), a.end(), b.begin(), b.end(),
                      std::inserter(out, out.end()));

  return arma::conv_to<arma::uvec>::from(out);
}

// Farthest point optimization

// [[Rcpp::export]]
arma::uvec farthest_points(const Rcpp::NumericMatrix &data,
                           const arma::uword n) {

  arma::mat dm = as<arma::mat>(edist(data));
  arma::uword N = dm.n_cols;

  arma::uvec full_range = arma::linspace<arma::uvec>(0, N - 1, N);
  arma::uvec r = RcppArmadillo::sample(full_range, n, false);
  arma::uvec r_old(n);

  do {
    r_old = r;

    for (arma::uword i = 0; i < n; i++) {

      arma::uvec::fixed<1> ri;

      ri.fill(r(i));

      arma::uvec rr = std_setdiff(r, ri);
      arma::uvec excl = std_setdiff(full_range, rr);
      arma::uvec indices = full_range(excl);
      arma::rowvec mins = arma::min(dm.submat(rr, excl), 0);
      r(i) = indices(mins.index_max());

    }
  } while (!all(r_old == r));

  return r + 1;
}
