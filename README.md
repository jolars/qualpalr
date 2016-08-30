
<!-- README.md is generated from README.Rmd. Please edit that file -->
qualpalr
========

[![Build Status](https://travis-ci.org/jolars/qualpalr.svg?branch=master)](https://travis-ci.org/jolars/qualpalr) [![AppVeyor Build Status](https://ci.appveyor.com/api/projects/status/github/jolars/qualpalr?branch=master&svg=true)](https://ci.appveyor.com/project/jolars/qualpalr) [![Coverage Status](https://codecov.io/github/jolars/qualpalr/coverage.svg?branch=master)](https://codecov.io/github/jolars/qualpalr?branch=master)

`qualpalr` generates distinct qualitative color palettes, primarily for use in R graphics. The user inputs `n` (the number of colors to generate), along with a subset in the [hls color space](https://en.wikipedia.org/wiki/HSL_and_HSV) (a cylindrical representation of the RGB color space). Using a [Nelder-Mead optimizer](https://en.wikipedia.org/wiki/Nelder%E2%80%93Mead_method), `qualpalr` then attempts to find the `n` colors in the provided color subspace that *maximize the smallest pairwise color difference*, computed via the [CIEDE2000 color difference formula](https://en.wikipedia.org/wiki/Color_difference#CIEDE2000).

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
#> [1] "#CCC8C0" "#929FDB" "#66CC66" "#D58484"

# Create a palette using one of the predefined color subspaces
pal2 <- qualpal(n = 4, colorspace = "colorblind")

# Produce a distance matrix of the CIEDE2000 color differences
pal2$ciede2000
#>          #CBC04E  #DEBCEC  #1A5E66
#> #DEBCEC 50.92516                  
#> #1A5E66 51.16246 50.51355         
#> #932727 58.10369 50.39859 50.39859
```

Methods for `pairs()` and `plot()` exist for objects created with `qualpal()`.

Installation
------------

The development version can be installed by running

``` r
devtools::install_github("jolars/qualpalr")
```

A version for CRAN is being prepared.

License
-------

`qualpalr` is open source software, licensed under GPL-3.
