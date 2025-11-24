# Scatterplot matrix of qualitative color palette

Plots the colors in an object of class `"qualpal"` as a scatterplot
matrix on either the DIN99d (the default) or HSL color space.

## Usage

``` r
# S3 method for class 'qualpal'
pairs(x, colorspace = c("DIN99d", "HSL", "RGB"), ...)
```

## Arguments

- x:

  A list object of class `"qualpal"` generated from
  [`qualpal`](https://jolars.github.io/qualpalr/reference/qualpal.md).

- colorspace:

  The color space in which to plot the colors ("DIN99d", "HSL", or
  "RGB").

- ...:

  Arguments to pass on to
  [`pairs`](https://rdrr.io/r/graphics/pairs.html).

## See also

[`qualpal`](https://jolars.github.io/qualpalr/reference/qualpal.md),
[`plot.qualpal`](https://jolars.github.io/qualpalr/reference/plot.qualpal.md),
[`pairs`](https://rdrr.io/r/graphics/pairs.html)

## Examples

``` r
col_pal <- qualpal(3)
pairs(col_pal)

pairs(col_pal, colorspace = "HSL")
```
