/**
 * @file
 * @brief Utilities for analyzing color palettes, including color difference
 * matrices and color vision deficiency (CVD) simulation.
 *
 * Provides data structures and functions to analyze color palettes for
 * distinguishability, supporting multiple color difference metrics and
 * simulation of common color vision deficiencies (protan, deutan, tritan).
 * Enables quantitative assessment of palette quality under normal and simulated
 * vision conditions.
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
 * @brief Holds analysis results for a color palette under a specific vision
 * condition.
 *
 * Contains:
 * - The pairwise color difference matrix for all palette colors.
 * - The minimum color distance to any other color for each palette color.
 * - The minimum color distance from the background (if provided) to the
 * palette.
 *
 * All distances are computed using the specified color difference metric and
 * simulated color vision deficiency (CVD) if applicable.
 */
struct PaletteAnalysis
{
  Matrix<double>
    difference_matrix; ///< Pairwise color difference matrix [size: n x n].
  std::vector<double> min_distances; ///< For each color, the minimum distance
                                     ///< to any other color [size: n].
  double bg_min_distance =
    std::numeric_limits<double>::quiet_NaN(); ///< Minimum distance from
                                              ///< background to palette, or NaN
                                              ///< if no background.
};

/**
 * @brief Map from vision deficiency type to palette analysis results.
 *
 * The key is a string representing the vision type:
 *   - "normal"  : Normal color vision (no simulation)
 *   - "deutan"  : Deuteranomaly simulation
 *   - "protan"  : Protanomaly simulation
 *   - "tritan"  : Tritanomaly simulation
 *
 * The value is a PaletteAnalysis struct with results for that vision type.
 */
using PaletteAnalysisMap = std::map<std::string, PaletteAnalysis>;

/**
 * @brief Analyze a color palette for normal and simulated color vision
 * deficiencies (CVD).
 *
 * Computes color difference matrices and minimum color distances for the input
 * palette, for normal vision as well as common color vision deficiencies
 * ("deutan", "protan", "tritan") at the specified severity. Results are
 * returned as a map keyed by vision type.
 *
 * For each vision type:
 *   - Simulates the palette (and optional background) under the specified CVD
 * severity.
 *   - Computes the pairwise color difference matrix using the selected metric.
 *   - Computes the minimum distance to any other color for each palette color.
 *   - Computes the minimum distance from the background (if provided) to the
 * palette.
 *
 * @param colors      Vector of RGB colors to analyze. Each channel should be in
 *                    [0, 1].
 * @param metric      Color difference metric to use (e.g., CIEDE2000, DIN99d,
 *                    CIE76).
 * @param cvd         Map of CVD type ("deutan", "protan", "tritan") to severity
 *                    [0, 1]. If a type is omitted or severity is 0, simulation
 *                    is skipped for that type.
 * @param bg          Optional background color to include in the analysis (RGB
 *                    in [0, 1]).
 * @param max_memory  Maximum memory (in GB) allowed for the difference matrix
 *                    computation.
 * @return            Map from vision type string to PaletteAnalysis for each
 *                    simulated vision.
 *
 * @throws std::invalid_argument if input parameters are invalid.
 *
 * @see PaletteAnalysis, PaletteAnalysisMap
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
