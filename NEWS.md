# qualpalr 0.2.1
## New features

## Minor improvements
* Phase out the ... argument to `qualpal`.
* Lightness range of the predefined rainbow palette increased to [0, 1].

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
