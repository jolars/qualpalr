#pragma once

#include <array>
#include <cassert>
#include <vector>

namespace qualpal {

template<typename T>
class Matrix
{
public:
  Matrix(int rows, int cols)
    : rows(rows)
    , cols(cols)
    , data(rows * cols)
  {
  }

  Matrix(int rows, int cols, const std::vector<T>& data)
    : rows(rows)
    , cols(cols)
    , data(data)
  {
    assert(rows * cols == data.size());
  }

  T& operator()(int row, int col) { return data[col * rows + row]; }

  const T& operator()(int row, int col) const { return data[col * rows + row]; }

  Matrix<T> transpose() const
  {
    Matrix<T> result(cols, rows);
    for (int i = 0; i < rows; ++i) {
      for (int j = 0; j < cols; ++j) {
        result(j, i) = (*this)(i, j);
      }
    }
    return result;
  }

  int ncol() const { return cols; }
  int nrow() const { return rows; }

private:
  int rows, cols;
  std::vector<T> data;
};

template<typename T, int rows, int cols>
class FixedMatrix
{
public:
  FixedMatrix() {}

  FixedMatrix(std::initializer_list<std::initializer_list<T>> list)
  {
    auto it = list.begin();
    for (int i = 0; i < rows; ++i) {
      std::copy(it->begin(), it->end(), data.begin() + i * cols);
      ++it;
    }
  }

  FixedMatrix<T, cols, rows> t() const
  {
    FixedMatrix<T, cols, rows> result;
    for (int i = 0; i < rows; ++i) {
      for (int j = 0; j < cols; ++j) {
        result(j, i) = (*this)(i, j);
      }
    }
    return result;
  }

  T& operator()(int row, int col) { return data[row * cols + col]; }

  const T& operator()(int row, int col) const { return data[row * cols + col]; }

  // Scalar multiplication
  FixedMatrix<T, rows, cols> operator*(const T& scalar) const
  {
    FixedMatrix<T, rows, cols> result;
    for (int i = 0; i < rows * cols; ++i) {
      result.data[i] = this->data[i] * scalar;
    }
    return result;
  }

  // Vector multiplication
  std::array<T, rows> operator*(const std::array<T, cols>& vec) const
  {
    std::array<T, rows> result{};
    for (int i = 0; i < rows; ++i) {
      for (int j = 0; j < cols; ++j) {
        result[i] += (*this)(i, j) * vec[j];
      }
    }
    return result;
  }

  // Fixed matrix multiplication
  template<int other_cols>
  FixedMatrix<T, rows, other_cols> operator*(
    const FixedMatrix<T, cols, other_cols>& other) const
  {
    FixedMatrix<T, rows, other_cols> result;
    result.zeros();
    for (int i = 0; i < rows; ++i) {
      for (int j = 0; j < other_cols; ++j) {
        for (int k = 0; k < cols; ++k) {
          result(i, j) += (*this)(i, k) * other(k, j);
        }
      }
    }
    return result;
  }

  // Dynamic matrix multiplication
  Matrix<T> operator*(const Matrix<T>& other) const
  {
    assert(cols == other.nrow());
    Matrix<T> result(rows, other.ncol());
    for (int i = 0; i < rows; ++i) {
      for (int j = 0; j < other.ncol(); ++j) {
        for (int k = 0; k < cols; ++k) {
          result(i, j) += (*this)(i, k) * other(k, j);
        }
      }
    }
    return result;
  }

  // Matrix - matrix addition
  FixedMatrix<T, rows, cols> operator+(
    const FixedMatrix<T, rows, cols>& other) const
  {
    FixedMatrix<T, rows, cols> result;
    for (int i = 0; i < rows * cols; ++i) {
      result.data[i] = this->data[i] + other.data[i];
    }
    return result;
  }

  // Matrix - matrix subtraction
  FixedMatrix<T, rows, cols> operator-(
    const FixedMatrix<T, rows, cols>& other) const
  {
    FixedMatrix<T, rows, cols> result;
    for (int i = 0; i < rows * cols; ++i) {
      result.data[i] = this->data[i] - other.data[i];
    }
    return result;
  }

  void fill(const T& value) { data.fill(value); }

  void zeros() { data.fill(0); }

private:
  std::array<T, rows * cols> data;
};

} // namespace qualpal
