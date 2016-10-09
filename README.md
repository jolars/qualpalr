
<!-- README.md is generated from README.Rmd. Please edit that file -->
qualpalr
========

[![Travis-CI Build Status](https://travis-ci.org/jolars/qualpalr.svg?branch=master)](https://travis-ci.org/jolars/qualpalr) [![AppVeyor Build Status](https://ci.appveyor.com/api/projects/status/github/jolars/qualpalr?branch=master&svg=true)](https://ci.appveyor.com/project/jolars/qualpalr) [![Coverage Status](https://codecov.io/github/jolars/qualpalr/coverage.svg?branch=master)](https://codecov.io/github/jolars/qualpalr?branch=master) [![CRAN\_Status\_Badge](http://www.r-pkg.org/badges/version/qualpalr)](https://cran.r-project.org/package=qualpalr)

`qualpalr` generates distinct qualitative color palettes, primarily for use in R graphics. Given `n` (the number of colors to generate), along with a subset in the [hsl color space](https://en.wikipedia.org/wiki/HSL_and_HSV) (a cylindrical representation of the RGB color space) `qualpalr` attempts to find the `n` colors in the provided color subspace that *maximize the smallest pairwise color difference*. This is done by projecting the color subset from the HSL color space to the DIN99d space. DIN99d is (approximately) perceptually uniform, that is, the euclidean distance between two colors in the space is proportional to their perceived difference.

`qualpalr` was inspired by [i want hue](http://tools.medialab.sciences-po.fr/iwanthue/).

Usage
-----

``` r
library(qualpalr)

# Create a palette of 4 colors of hues from 0 to 360, saturations between
# 0.1 and 0.5, and lightness from 0.6 to 0.85
pal <- qualpal(n = 4, list(h = c(0, 360), s = c(0.1, 0.5), l = c(0.6, 0.85)))

# Look at the colors in hex format
pal$hex
#> [1] "#6BCB69" "#C86DC5" "#77AACE" "#CF9D74"

# Create a palette using one of the predefined color subspaces
pal2 <- qualpal(n = 4, colorspace = "pretty")

# Distance matrix of the DIN99d color differences
pal2$de_DIN99d
#>         #6ACB69 #C96CC6 #76AACE
#> #C96CC6      30                
#> #76AACE      21      21        
#> #CF9D74      20      21      23
```

`pairs()` and `plot()` methods have been prepared for objects created with `qualpal`.

``` r
plot(pal)
pairs(pal, colorspace = "hsl")
```

Installation
------------

The current CRAN release can be installed by running

``` r
install.packages("qualpalr")
```

The development version can be installed by running

``` r
devtools::install_github("jolars/qualpalr")
```

License
-------

`qualpalr` is open source software, licensed under GPL-3.
