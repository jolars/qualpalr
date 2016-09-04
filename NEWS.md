# qualpalr 0.2.0
## New features
* Redesigned the method by which qualpal picks colors. Now initializes a point
cloud of colors, projects it to DIN99d space, and picks points greedily.
* Introduced *real* methods adapting colors to color blindness by daltonizing
color subspaces before picking colors from them.
* The introduction to qualpalr vignette has been expanded with a thorough
description of how qualpalr picks colors.

## Bug fixes and minor improvements
* Moved from using `grDevices::convertColor` to formulas from
[Bruce Lindbloom](http://www.brucelindbloom.com/) for color conversions,
since the former function inaccurately converts colors.
* Deprecated `...` in `qualpal` since the function no longer uses an optimizer.
