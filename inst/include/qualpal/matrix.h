/**
 * @file
 * @brief Matrix classes for qualpal.
 *
 * Provides both dynamic (runtime-sized) and fixed-size (compile-time-sized)
 * matrix implementations for color difference calculations and general linear
 * algebra operations. Supports element access, transposition, arithmetic, and
 * multiplication with vectors/matrices.
 */

#pragma once

#include <array>
#include <cassert>
#include <vector>

namespace qualpal {

/**
 * @brief Dynamic matrix class with runtime-determined dimensions.
 * @tparam T Element type (typically double or float).
 *
 * Stores elements in column-major order (Fortran-style).
 * Provides element access, transposition, and dimension queries.
 *
 * Example:
 * @code
 * Matrix<double> mat(2, 3); // 2 rows, 3 columns
 * mat(0, 1) = 5.0;          // Set element at row 0, column 1
 * double v = mat(1, 2);     // Get element at row 1, column 2
 * @endcode
 *
 * @see FixedMatrix for compile-time sized matrices
 */
template<typename T>
class Matrix
{
public:
  /**
   * @brief Construct matrix with specified dimensions
   * @param rows Number of rows
   * @param cols Number of columns
   */
  Matrix(std::size_t rows, std::size_t cols)
    : rows(rows)
    , cols(cols)
    , data(rows * cols)
  {
  }

  /**
   * @brief Default constructor creates an empty matrix (0x0)
   */
  Matrix()
    : rows(0)
    , cols(0)
    , data()
  {
  }

  /**
   * @brief Construct matrix with specified dimensions and data
   * @param rows Number of rows
   * @param cols Number of columns
   * @param data Vector containing matrix elements in column-major order
   */
  Matrix(std::size_t rows, std::size_t cols, const std::vector<T>& data)
    : rows(rows)
    , cols(cols)
    , data(data)
  {
    assert(rows * cols == data.size());
  }

  /**
   * @brief Access matrix element (mutable).
   * @param row Row index (0-based).
   * @param col Column index (0-based).
   * @return Reference to element at (row, col).
   * @note Elements are stored in column-major order: data[col * nrow + row].
   * @throws std::out_of_range if indices are out of bounds (in debug builds).
   */
  T& operator()(std::size_t row, std::size_t col)
  {
    return data[col * rows + row];
  }

  /**
   * @brief Access matrix element (const).
   * @param row Row index (0-based).
   * @param col Column index (0-based).
   * @return Const reference to element at (row, col).
   * @note Elements are stored in column-major order: data[col * nrow + row].
   * @throws std::out_of_range if indices are out of bounds (in debug builds).
   */
  const T& operator()(std::size_t row, std::size_t col) const
  {
    return data[col * rows + row];
  }

  /**
   * @brief Create transpose of this matrix
   * @return New matrix that is the transpose of this matrix
   */
  Matrix<T> transpose() const
  {
    Matrix<T> result(cols, rows);
    for (std::size_t i = 0; i < rows; ++i) {
      for (std::size_t j = 0; j < cols; ++j) {
        result(j, i) = (*this)(i, j);
      }
    }
    return result;
  }

  /** @brief Get number of columns */
  std::size_t ncol() const { return cols; }

  /** @brief Get number of rows */
  std::size_t nrow() const { return rows; }

private:
  std::size_t rows, cols;
  std::vector<T> data;
};

/**
 * @brief Fixed-size matrix class with compile-time dimensions.
 * @tparam T Element type (typically double or float).
 * @tparam rows Number of rows (compile-time constant).
 * @tparam cols Number of columns (compile-time constant).
 *
 * Stores elements in row-major order (C-style).
 * Provides constexpr element access, arithmetic, and multiplication.
 *
 * Example:
 * @code
 * FixedMatrix<double, 2, 3> mat = { {1.0, 2.0, 3.0}, {4.0, 5.0, 6.0} };
 * double v = mat(1, 2); // Access element at row 1, column 2
 * @endcode
 *
 * @see Matrix for dynamic-sized matrices
 */
template<typename T, std::size_t rows, std::size_t cols>
class FixedMatrix
{
public:
  /** @brief Default constructor - elements are zero-initialized */
  constexpr FixedMatrix()
    : data{}
  {
  }

  /**
   * @brief Construct from nested initializer list
   * @param list Nested initializer list of matrix elements
   */
  constexpr FixedMatrix(std::initializer_list<std::initializer_list<T>> list)
    : data{}
  {
    auto it = list.begin();
    for (std::size_t i = 0; i < rows && it != list.end(); ++i, ++it) {
      auto col_it = it->begin();
      for (std::size_t j = 0; j < cols && col_it != it->end(); ++j, ++col_it) {
        data[i * cols + j] = *col_it;
      }
    }
  }

  /**
   * @brief Create transpose of this matrix
   * @return New FixedMatrix that is the transpose of this matrix
   */
  constexpr FixedMatrix<T, cols, rows> t() const
  {
    FixedMatrix<T, cols, rows> result;
    for (std::size_t i = 0; i < rows; ++i) {
      for (std::size_t j = 0; j < cols; ++j) {
        result(j, i) = (*this)(i, j);
      }
    }
    return result;
  }

  /**
   * @brief Access matrix element (mutable).
   * @param row Row index (0-based).
   * @param col Column index (0-based).
   * @return Reference to element at (row, col).
   * @note Elements are stored in row-major order: data[row * cols + col].
   * @throws std::out_of_range if indices are out of bounds (in debug builds).
   */
  constexpr T& operator()(std::size_t row, std::size_t col)
  {
    return data[row * cols + col];
  }

  /**
   * @brief Access matrix element (const).
   * @param row Row index (0-based).
   * @param col Column index (0-based).
   * @return Const reference to element at (row, col).
   * @note Elements are stored in row-major order: data[row * cols + col].
   * @throws std::out_of_range if indices are out of bounds (in debug builds).
   */
  constexpr const T& operator()(std::size_t row, std::size_t col) const
  {
    return data[row * cols + col];
  }

  /**
   * @brief Scalar multiplication
   * @param scalar Value to multiply all elements by
   * @return New matrix with all elements multiplied by scalar
   */
  constexpr FixedMatrix<T, rows, cols> operator*(const T& scalar) const
  {
    FixedMatrix<T, rows, cols> result;
    for (std::size_t i = 0; i < rows * cols; ++i) {
      result.data[i] = this->data[i] * scalar;
    }
    return result;
  }

  /**
   * @brief Matrix-vector multiplication
   * @param vec Vector to multiply with (must have 'cols' elements)
   * @return Result vector with 'rows' elements
   */
  constexpr std::array<T, rows> operator*(const std::array<T, cols>& vec) const
  {
    std::array<T, rows> result{};
    for (std::size_t i = 0; i < rows; ++i) {
      for (std::size_t j = 0; j < cols; ++j) {
        result[i] += (*this)(i, j) * vec[j];
      }
    }
    return result;
  }

  /**
   * @brief Matrix-matrix multiplication (FixedMatrix * FixedMatrix).
   * @tparam other_cols Number of columns in the other matrix.
   * @param other The matrix to multiply with (must have 'cols' rows).
   * @return Result matrix with dimensions (rows × other_cols).
   */
  template<std::size_t other_cols>
  constexpr FixedMatrix<T, rows, other_cols> operator*(
    const FixedMatrix<T, cols, other_cols>& other) const
  {
    FixedMatrix<T, rows, other_cols> result;
    result.zeros();
    for (std::size_t i = 0; i < rows; ++i) {
      for (std::size_t j = 0; j < other_cols; ++j) {
        for (std::size_t k = 0; k < cols; ++k) {
          result(i, j) += (*this)(i, k) * other(k, j);
        }
      }
    }
    return result;
  }

  /**
   * @brief Matrix-matrix multiplication (FixedMatrix * dynamic Matrix)
   * @param other Dynamic matrix to multiply with
   * @return Result as dynamic Matrix
   */
  constexpr Matrix<T> operator*(const Matrix<T>& other) const
  {
    assert(cols == other.nrow());
    Matrix<T> result(rows, other.ncol());
    for (std::size_t i = 0; i < rows; ++i) {
      for (std::size_t j = 0; j < other.ncol(); ++j) {
        for (std::size_t k = 0; k < cols; ++k) {
          result(i, j) += (*this)(i, k) * other(k, j);
        }
      }
    }
    return result;
  }

  /**
   * @brief Matrix addition
   * @param other Matrix to add (must have same dimensions)
   * @return New matrix containing element-wise sum
   */
  constexpr FixedMatrix<T, rows, cols> operator+(
    const FixedMatrix<T, rows, cols>& other) const
  {
    FixedMatrix<T, rows, cols> result;
    for (std::size_t i = 0; i < rows * cols; ++i) {
      result.data[i] = this->data[i] + other.data[i];
    }
    return result;
  }

  /**
   * @brief Matrix subtraction
   * @param other Matrix to subtract (must have same dimensions)
   * @return New matrix containing element-wise difference
   */
  constexpr FixedMatrix<T, rows, cols> operator-(
    const FixedMatrix<T, rows, cols>& other) const
  {
    FixedMatrix<T, rows, cols> result;
    for (std::size_t i = 0; i < rows * cols; ++i) {
      result.data[i] = this->data[i] - other.data[i];
    }
    return result;
  }

  /** @brief Fill all elements with specified value */
  constexpr void fill(const T& value) { data.fill(value); }

  /** @brief Set all elements to zero */
  constexpr void zeros() { data.fill(0); }

private:
  std::array<T, rows * cols> data;
};

} // namespace qualpal
