
<!-- README.md is generated from README.Rmd. Please edit that file -->
qualpalr
========

[![Build Status](https://travis-ci.org/jolars/qualpalr.svg?branch=master)](https://travis-ci.org/jolars/qualpalr) [![AppVeyor Build Status](https://ci.appveyor.com/api/projects/status/github/jolars/qualpalr?branch=master&svg=true)](https://ci.appveyor.com/project/jolars/qualpalr) [![Coverage Status](https://codecov.io/github/jolars/qualpalr/coverage.svg?branch=master)](https://codecov.io/github/jolars/qualpalr?branch=master)

`qualpalr` generates distinct qualitative color palettes, primarily for use in R graphics. The user inputs `n`: the number of colors to generate, along with a subset in the [hls color space](https://en.wikipedia.org/wiki/HSL_and_HSV): a cylindrical representation of the RGB color space. Using a [Nelder-Mead optimizer](https://en.wikipedia.org/wiki/Nelder%E2%80%93Mead_method), `qualpalr` then attempts to find the `n` colors in the provided color subspace that *maximize the smallest pairwise color difference*, computed via the [CIEDE2000 color difference formula](https://en.wikipedia.org/wiki/Color_difference#CIEDE2000).

Example
-------

``` r
library(qualpalr)
set.seed(1)
cols <- qualpal(n = 4, list(h = c(10, 350), s = c(0.4, 0.6), l = c(0.6, 0.7)))
mosaicplot(~ Survived + Class, data = Titanic, color = cols$hex)
```

![](README-unnamed-chunk-2-1.png)

``` r

# Diagnostic plotting
pairs(cols, colorspace = "hsl")
```

![](README-unnamed-chunk-2-2.png)

Installation
------------

The development version can be installed by running

``` r
devtools::install_github("jolars/qualpalr")
```

A version for CRAN is being prepared.
