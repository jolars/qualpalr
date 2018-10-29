# qualpalr 0.4.3

## Minor changes
* `qualpal()` gains an argument, `n_threads`, for specifying the number
of threads to use when computing the distance matrix between colors.
* C++ functions call namespaces explicitly using `::`.
* Documentation for hue and saturation in `qualpal()` has been fixed. 
  (Closes #2, thanks @jflycn).

# qualpalr 0.4.2

## Major changes
* Palettes are no longer generated randomly. **qualpalr** previuosly started
with a random sample of colors before running the optimization scheme but now
instead picks a starting set of linearly spaced colors.

## Minor changes
* C++ functions are registered via Rcpp.

## Bug fixes
* `autopal()` erroneously required `colorspace` to be a string.

# qualpalr 0.4.1
## Bug fixes
* Fixed `autopal()` which was broken since the minimum color difference
returned was always 0 due to a bug in `qualpal()`.

## Minor changes
* Now registers compiled functions.

# qualpalr 0.4.0
## Major changes
* `autopal()` is a new function that tweaks the amount of color vision
deficiency adapation to match a target color difference.
* `qualpal()` argument `colorspace` now also accepts a `matrix` or 
`data.frame` of RGB colors.

## Minor  changes
* `qualpal()` sorts palettes in order of increasing color distinctness.
* `qualpal()` argument `colorblind` has been made defunct.
* Documentation for `qualpal()` has been improved.
* Colors are now generated with `randtoolbox::torus()` instead of
`randtoolbox::sobol()`.

# qualpalr 0.3.1
## Bug fixes
* Dropped a C++ header that caused the package build to fail
on some platforms.
* Fixed issues with unitialized variables in the internal farthest points
optimizer.

# qualpalr 0.3.0
## New features
* Improved algorithm for finding distinct colors. (For details
[see this](https://larssonjohan.com/post/2016-10-30-farthest-points/).)
* Revamped the color deficiency handling to include almost all cases of color
deficiency using the methods described in
[Machado 2010](https://www.lume.ufrgs.br/bitstream/handle/10183/26950/000761444.pdf), 
now including tritanopia as well as anomalous trichromacies (deuteranomaly,
tritanomaly, and protanomaly). This is controlled via the `cvd_severity`
argument to `qualpal()` that allows the user to set the severity of color
deficiency to adapt to -- `0` for normal vision and `1` for dichromatic vision
(protanopia, deuteranopia, or tritanopia).

## Minor improvements
* Distance and color picking algorithms have been rewritten in C++ using Rcpp,
RcppParallel, and RcppArmadillo.
* Phased out the `...` argument to `qualpal`.
* Lightness range of the predefined rainbow palette increased to [0, 1].
* Changed argument name of `colorblind` to `cvd` (for color vision deficiency)
since the function now adapts to less severe versions of color deficiency. Using
`colorblind` is deprecated and will throw a warning.

## Bug fixes
* Fixed typos and invalid links in the _Introduction to qualpalr_ vignette.

# qualpalr 0.2.1
## New features
* Dropped daltonization since it effectively transposed the color
subspace given by the user. qualpalr now instead only transforms the given color
subspace to *simulate* protanopia or deuteranopia and then picks colors. This
has the side-effect of decreasing the distinctness of color palettes when
`colorblind` is used, but is more consistent with user input.

## Bug fixes and minor improvements
* Simulations for tritanopia were dropped since there is no reliable source
to explain how sRGB ranges should be converted (as there is for deuteranopia
and protanopia in Vienot et al 1999).
* Added tests using data from Vienot et al 1999 to check that color blind
simulations work properly.
* Fixed a sampling bug wherein the square root of saturation was taken after
scaling to the provided range, which generated different ranges than intended.
* Switched to the sobol quasi-random sequence instead of torus.

# qualpalr 0.2.0
## New features
* Redesigned the method by which qualpal picks colors. Now initializes a point
cloud of colors, projects it to DIN99d space, and picks points greedily.
* Introduced *real* methods of adapting colors to color blindness by daltonizing
color subspaces before picking colors from them.
* The introduction to qualpalr vignette has been expanded with a thorough
description of how qualpalr picks colors.

## Bug fixes and minor improvements
* Moved from using `grDevices::convertColor` to formulas from
[Bruce Lindbloom](http://www.brucelindbloom.com/) for color conversions,
since the former function inaccurately converts colors.
* Deprecated `...` in `qualpal` since the function no longer uses an optimizer.
