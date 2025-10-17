/**
 * @file
 * @brief Thread management for parallel computations
 */

#pragma once

#include <stdexcept>

#ifdef _OPENMP
#include <omp.h>
#endif

namespace qualpal {

/**
 * @brief Centralized management of thread settings for parallel computations.
 *
 * The Threads class provides a unified interface to control the number of
 * threads used for parallel computations throughout qualpal. It supports both
 * OpenMP and non-OpenMP builds:
 * - When OpenMP is available, it sets and queries the OpenMP thread count.
 * - When OpenMP is not available, all computations run sequentially
 * (single-threaded).
 *
 * By default, the thread count is set to half the number of available hardware
 * threads (typically the number of physical CPU cores), but can be changed at
 * runtime.
 *
 * @note This class is not thread-safe. Set the thread count before starting
 * parallel work.
 *
 * @code
 * qualpal::Threads::set(4);  // Set to use 4 threads
 * std::size_t n = qualpal::Threads::get();  // Get current thread count
 * @endcode
 */
class Threads
{
public:
  /**
   * @brief Set the number of threads to use for parallel computations
   *
   * @param n Number of threads. Must be positive.
   * @throws std::invalid_argument if n is zero.
   * @note When OpenMP is enabled, this sets the OpenMP thread count globally.
   */
  static void set(const std::size_t n)
  {
    if (n > 0) {
      num_threads = n;
#ifdef _OPENMP
      omp_set_num_threads(n);
#endif
    } else {
      throw std::invalid_argument("Number of threads must be positive");
    }
  }

  /**
   * @brief Get the current number of threads used for parallel computations.
   *
   * @return The current thread count as set by set(), or the default value.
   */
  static std::size_t get() { return num_threads; }

private:
#ifdef _OPENMP
  /// Number of threads to use for parallel computations.
  /// Defaults to half of the maximum available threads (typically the number of
  /// physical CPU cores).
  inline static std::size_t num_threads =
    std::max(1, omp_get_max_threads() / 2);
#else
  /// Default to single thread when OpenMP is not available
  inline static std::size_t num_threads = 1;
#endif
};

} // namespace qualpal
