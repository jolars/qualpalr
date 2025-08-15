/**
 * @file
 * @brief Utilities for analyzing color palettes, including color difference
 * matrices and color vision deficiency simulation.
 *
 * Provides data structures and functions to analyze color palettes for
 * distinguishability, supporting different color difference metrics and
 * simulation of common color vision deficiencies.
 */
#pragma once

#include <map>
#include <optional>
#include <qualpal/color_difference.h>
#include <qualpal/colors.h>
#include <qualpal/matrix.h>
#include <qualpal/metrics.h>

namespace qualpal {

/**
 * @brief Holds analysis results for a color palette.
 *
 * Contains the color difference matrix and the minimum color distances for each
 * color in the palette, for a given vision type.
 */
struct PaletteAnalysis
{
  Matrix<double> difference_matrix; ///< Pairwise color difference matrix.
  std::vector<double>
    min_distances; ///< Minimum distance to any other color for each color.
  double bg_min_distance =
    std::numeric_limits<double>::quiet_NaN(); ///< Minimum distance from
                                              ///< background to palette.
};

/**
 * @brief Map from vision deficiency type to palette analysis results.
 *
 * The key is a string representing the vision type ("normal", "deutan",
 * "protan", "tritan").
 */
using PaletteAnalysisMap = std::map<std::string, PaletteAnalysis>;

/**
 * @brief Analyze a color palette for normal and simulated color vision
 * deficiencies.
 *
 * Computes color difference matrices and minimum color distances for the input
 * palette, for normal vision as well as common color vision deficiencies
 * (deutan, protan, tritan) at the specified severity. Results are returned as a
 * map keyed by deficiency type.
 *
 * @param colors      Vector of RGB colors to analyze.
 * @param metric      Color difference metric to use.
 * @param cvd         Map of CVD type to severity.
 * @param bg          Optional background color to include in the analysis.
 * @param max_memory  Maximum memory (in GB) allowed for the difference matrix.
 * @return            Map from CvdType to PaletteAnalysisResult for each vision
 * type.
 */
PaletteAnalysisMap
analyzePalette(
  const std::vector<colors::RGB>& colors,
  const metrics::MetricType& metric = metrics::MetricType::CIEDE2000,
  const std::map<std::string, double>& cvd = { { "deutan", 1.0 },
                                               { "protan", 1.0 },
                                               { "tritan", 1.0 } },
  const std::optional<colors::RGB>& bg = std::nullopt,
  double max_memory = 1.0);

} // namespace qualpal
