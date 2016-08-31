
<!-- README.md is generated from README.Rmd. Please edit that file -->
qualpalr
========

[![Build Status](https://travis-ci.org/jolars/qualpalr.svg?branch=master)](https://travis-ci.org/jolars/qualpalr) [![AppVeyor Build Status](https://ci.appveyor.com/api/projects/status/github/jolars/qualpalr?branch=master&svg=true)](https://ci.appveyor.com/project/jolars/qualpalr) [![Coverage Status](https://codecov.io/github/jolars/qualpalr/coverage.svg?branch=master)](https://codecov.io/github/jolars/qualpalr?branch=master) [![CRAN\_Status\_Badge](http://www.r-pkg.org/badges/version/qualpalr)](http://cran.r-project.org/package=qualpalr)

`qualpalr` generates distinct qualitative color palettes, primarily for use in R graphics. The user inputs `n` (the number of colors to generate), along with a subset in the [hsl color space](https://en.wikipedia.org/wiki/HSL_and_HSV) (a cylindrical representation of the RGB color space). Using a [Nelder-Mead optimizer](https://en.wikipedia.org/wiki/Nelder%E2%80%93Mead_method), `qualpalr` then attempts to find the `n` colors in the provided color subspace that *maximize the smallest pairwise color difference*, computed via the [CIEDE2000 color difference formula](https://en.wikipedia.org/wiki/Color_difference#CIEDE2000).

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
#> [1] "#CC66A8" "#CC9766" "#669CCC" "#6ACC66"

# Create a palette using one of the predefined color subspaces
pal2 <- qualpal(n = 4, colorspace = "colorblind")

# Produce a distance matrix of the CIEDE2000 color differences
pal2$ciede2000
#>          #524316  #2D2CAC  #E0D683
#> #2D2CAC 51.89358                  
#> #E0D683 51.89241 82.47010         
#> #E7A9E7 62.51856 51.89242 51.89242
```

Methods for `pairs()` and `plot()` have been prepared for objects created with `qualpal`.

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
