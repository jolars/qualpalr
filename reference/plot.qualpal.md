# Multidimensional scaling map of qualitative color palette

Uses the colors in a
[qualpal](https://jolars.github.io/qualpalr/reference/qualpal.md) object
to compute and plot a multidimensional scaling (MDS) map using
[`cmdscale`](https://rdrr.io/r/stats/cmdscale.html) on the Delta E
DIN99d distance matrix.

## Usage

``` r
# S3 method for class 'qualpal'
plot(x, ...)
```

## Arguments

- x:

  An object of class `"qualpal"` generated from
  [`qualpal`](https://jolars.github.io/qualpalr/reference/qualpal.md).

- ...:

  Arguments to pass on to
  [`plot`](https://rdrr.io/r/graphics/plot.default.html).

## See also

[`qualpal`](https://jolars.github.io/qualpalr/reference/qualpal.md),
[`pairs.qualpal`](https://jolars.github.io/qualpalr/reference/pairs.qualpal.md),
[`plot`](https://rdrr.io/r/graphics/plot.default.html)

## Examples

``` r
col_pal <- qualpal(3)
plot(col_pal)
```
