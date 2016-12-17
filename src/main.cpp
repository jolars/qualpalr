// Adopted from http://gallery.rcpp.org/articles/parallel-distance-matrix/

#include <Rcpp.h>
using namespace Rcpp;

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

// [[Rcpp::depends(RcppParallel)]]
#include <RcppParallel.h>
using namespace RcppParallel;

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
NumericMatrix edist(NumericMatrix mat) {

  NumericMatrix rmat(mat.nrow(), mat.nrow());
  dist_worker dist_worker(mat, rmat);
  parallelFor(0, mat.nrow(), dist_worker);

  return rmat;
}
