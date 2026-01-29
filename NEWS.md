# qualpalr (development version)

## Breaking Changes

- The previous method for catering to multiple types of color
  vision deficiencies has been revised. Previously, it
  incorrectly chained simulations of all the deficiencies together,
  which does not work as intended since the operations are not
  commutative and hence depend on the order in which they are applied.
  Now, instead, the color differences are computed separately for
  each type of deficiency (protanopia, deuteranopia, and tritanopia)
  as well as normal vision, and the minimum color distance across
  all types is maximized. The input of the `cvd` argument is
  unchanged, but the behavior is different. This change may lead
  to different palettes being generated when multiple types of
  color vision deficiencies are specified.

## New Features

- Added `white_point` parameter to `qualpal()` to allow users to
  specify the reference white point for color space conversions.
  Available options are "D65" (default, daylight at 6500K), "D50"
  (daylight at 5000K, common in printing), "D55" (daylight at
  5500K), "A" (incandescent tungsten at 2856K), and "E" (equal
  energy). The white point affects how perceptual color distances
  are calculated.

# qualpalr 1.0.1

## Minor Changes

- The project has been published as a paper in JOSS:
  <https://joss.theoj.org/papers/10.21105/joss.08936>, and the
  citation information has been updated in the package.
- Qualpalr has been updated to use v3.3.0 of the underlying qualpal C++ library.

## Bug Fixes

- Fixes an edge case when converting RGB colors to a hex representation.

# qualpalr 1.0.0

## New or Modified Features

- `qualpal()` now uses the CIEDE2000 metric instead of the DIN99d one
  by default. Users can still specify the DIN99d metric by passing
  `metric = "din99d"` to `qualpal()`.
- The `character` method of the `qualpal()` function now accepts
  a name for a predefined palette, such as `"ColorBrewer::Set1"` or
  `"Tableau::10"`, which will use the corresponding palette's
  colors as input. Passing one of the old colorspace names
  now throws a warning and will be removed in the next major release.
- `qualpal()` now accepts a background color through the `bg` argument,
  which is considered when generating the palette, but not included in the
  final palette.
- The color difference metric in `qualpal()` can now be specified
  using the `metric` argument, which accepts either `"din99d"` (the default),
  `"ciede2000"`, or `"cie76"`. Expect slower performance for `"ciede2000"`.
- The `autopal()` function is now deprecated and will be removed int the next major
  release. There is no direct replacement.
- There's a new function, `analyze_palette()`, which analyzes a categorical
  color palette.
- `qualpal()` now has a `extend` method that allows the user to extend
  an existing palette by adding more colors to it. This is useful for
  expanding an existing palette to include more colors.
- The color space (`list`) method of `qualpal()` now accepts
  a specification in the LCHab (HCL) color space, which is
  a cylindrical representation of the Lab color space. This is mostly
  added to simplify comparisons with other packages that use LCHab
  as input. We still recommend using the HSL color space, which is
  more intuitive for most users.
- There's a new function, `list_palettes()`, which lists all built-in palettes
  available in the package, which can thus be used in the new character method
  for `qualpal()`. This includes the predefined palettes from ColorBrewer,
  Tableau, and other sources.
- There's a new function, `get_palette()`, which retrieves a palette
  by name from the built-in palettes.
- The `cvd` argument now accepts a named numeric vector, which allows specifying
  the severity of color vision deficiency for each type of deficiency.
  The names are `"protan"`, `"deuter"`, and `"tritan"`.
- The `cvd_severity` argument has been deprecated and will be removed
  in the next major release. The `cvd` argument now handles all cases of color
  vision deficiency adaptation directly.

## Refactoring

- The entire C++ backend for the package has been rewritten and
  outsourced to a separate C++ library, [qualpal](https://github.com/jolars/qualpal).

## Bug Fixes

- An inconsistency in the conversion from Lab to DIN99d color space,
  where the reference of the white point was not modified correctly,
  has been fixed.

## Performance Improvements

- The color distance computation has been parallelized using
  OpenMP.
- All of the heavy lifting is now done in C++, which should
  significantly improve performance, especially for larger palettes.

# qualpalr 0.4.4

## Minor changes

- The dependency on RcppParallel has been dropped. As a result, the
  argument `n_threads` is now deprecated (and has no effect).

# qualpalr 0.4.3

## Minor changes

- `qualpal()` gains an argument, `n_threads`, for specifying the number
  of threads to use when computing the distance matrix between colors.
- C++ functions call namespaces explicitly using `::`.
- Documentation for hue and saturation in `qualpal()` has been fixed.
  (Closes #2, thanks @jflycn).

# qualpalr 0.4.2

## Major changes

- Palettes are no longer generated randomly. **qualpalr** previously started
  with a random sample of colors before running the optimization scheme but now
  instead picks a starting set of linearly spaced colors.

## Minor changes

- C++ functions are registered via Rcpp.

## Bug fixes

- `autopal()` erroneously required `colorspace` to be a string.

# qualpalr 0.4.1

## Bug fixes

- Fixed `autopal()` which was broken since the minimum color difference
  returned was always 0 due to a bug in `qualpal()`.

## Minor changes

- Now registers compiled functions.

# qualpalr 0.4.0

## Major changes

- `autopal()` is a new function that tweaks the amount of color vision
  deficiency adaptation to match a target color difference.
- `qualpal()` argument `colorspace` now also accepts a `matrix` or
  `data.frame` of RGB colors.

## Minor changes

- `qualpal()` sorts palettes in order of increasing color distinctness.
- `qualpal()` argument `colorblind` has been made defunct.
- Documentation for `qualpal()` has been improved.
- Colors are now generated with `randtoolbox::torus()` instead of
  `randtoolbox::sobol()`.

# qualpalr 0.3.1

## Bug fixes

- Dropped a C++ header that caused the package build to fail
  on some platforms.
- Fixed issues with uninitialized variables in the internal farthest points
  optimizer.

# qualpalr 0.3.0

## New features

- Improved algorithm for finding distinct colors. (For details
  [see this](https://jolars.co/blog/2016-10-30-farthest-points/).)
- Revamped the color deficiency handling to include almost all cases of color
  deficiency using the methods described in
  Machado et al. 2009,
  now including tritanopia as well as anomalous trichromacies (deuteranomaly,
  tritanomaly, and protanomaly). This is controlled via the `cvd_severity`
  argument to `qualpal()` that allows the user to set the severity of color
  deficiency to adapt to -- `0` for normal vision and `1` for dichromatic vision
  (protanopia, deuteranopia, or tritanopia).

## Minor improvements

- Distance and color picking algorithms have been rewritten in C++ using Rcpp,
  RcppParallel, and RcppArmadillo.
- Phased out the `...` argument to `qualpal`.
- Lightness range of the predefined rainbow palette increased to [0, 1].
- Changed argument name of `colorblind` to `cvd` (for color vision deficiency)
  since the function now adapts to less severe versions of color deficiency. Using
  `colorblind` is deprecated and will throw a warning.

## Bug fixes

- Fixed typos and invalid links in the _Introduction to qualpalr_ vignette.

# qualpalr 0.2.1

## New features

- Dropped daltonization since it effectively transposed the color
  subspace given by the user. qualpalr now instead only transforms the given color
  subspace to _simulate_ protanopia or deuteranopia and then picks colors. This
  has the side-effect of decreasing the distinctness of color palettes when
  `colorblind` is used, but is more consistent with user input.

## Bug fixes and minor improvements

- Simulations for tritanopia were dropped since there is no reliable source
  to explain how sRGB ranges should be converted (as there is for deuteranopia
  and protanopia in Vienot et al 1999).
- Added tests using data from Vienot et al 1999 to check that color blind
  simulations work properly.
- Fixed a sampling bug wherein the square root of saturation was taken after
  scaling to the provided range, which generated different ranges than intended.
- Switched to the sobol quasi-random sequence instead of torus.

# qualpalr 0.2.0

## New features

- Redesigned the method by which qualpal picks colors. Now initializes a point
  cloud of colors, projects it to DIN99d space, and picks points greedily.
- Introduced _real_ methods of adapting colors to color blindness by daltonizing
  color subspaces before picking colors from them.
- The introduction to qualpalr vignette has been expanded with a thorough
  description of how qualpalr picks colors.

## Bug fixes and minor improvements

- Moved from using `grDevices::convertColor` to formulas from
  [Bruce Lindbloom](http://www.brucelindbloom.com/) for color conversions,
  since the former function inaccurately converts colors.
- Deprecated `...` in `qualpal` since the function no longer uses an optimizer.
