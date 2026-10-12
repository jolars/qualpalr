# Generate qualitative color palettes

Given a collection of colors, `qualpal()` algorithmically tries to
select to `n` most distinct colors from the provided input colors,
optionally taking color vision deficiency into account.

## Usage

``` r
qualpal(
  n,
  colorspace = list(h = c(0, 360), s = c(0.2, 0.5), l = c(0.6, 0.85)),
  cvd = c(protan = 0, deutan = 0, tritan = 0),
  cvd_severity,
  bg = NULL,
  metric = c("ciede2000", "din99d", "cie76"),
  extend = NULL,
  white_point = c("D65", "D50", "D55", "A", "E"),
  refinement_starts = 5L
)
```

## Arguments

- n:

  The number of colors to generate.

- colorspace:

  A color space to generate colors from. Can be any of the following:

  - A [`list`](https://rdrr.io/r/base/list.html) that describes a color
    space in either HSL or LCHab color space. In the first case (HSL),
    the list must contain the following *named* vectors, each of length
    two, giving a range for each item.

    `h`

    :   Hue, in range from -360 to 360

    `s`

    :   Saturation, in the range from 0 to 1

    `l`

    :   Lightness, in the range from 0 to 1

    In the second case (LCHab), the list must contain the following
    *named* vectors, each of length two, giving a range for each item.

    `h`

    :   Hue, in range from -360 to 360

    `c`

    :   Chroma, in the range from 0 to infinity

    `l`

    :   Lightness, in the range from 0 to 100

    In these cases, `qualpal()` will generate

  - A [`character`](https://rdrr.io/r/base/character.html) vector of
    length one in the form of "Source:Palette", where *Domain* is the
    name of a source that provides a color palette, and *Palette* is the
    name of a color palette from that source. See
    [`list_palettes()`](https://jolars.github.io/qualpalr/dev/reference/list_palettes.md)
    for available palettes.

  - A matrix or data frame of RGB values (with values between 0 and 1).

- cvd:

  Color vision deficiency adaptation. This must be a named vector with
  names `protan`, `deutan`, and `tritan` and values between 0 and 1,
  where 0 means no adaptation and 1 means full adaptation.

- cvd_severity:

  DEPRECATED. Use a named `cvd` vector instead, e.g.
  `c(protan = 0.5, deutan = 0.2, tritan = 0)`.

- bg:

  Background color to consider (but not include) when generating the
  palette. This is useful to avoid colors that are too close to the
  background/canvas color. If `NULL` (the default), the background color
  is not considered at all. Any color that is convertable via
  [`col2rgb`](https://rdrr.io/r/grDevices/col2rgb.html) is acceptable,
  including hex colors.

- metric:

  The color metric to use for the color distance matrix.

- extend:

  A palette of colors to use as a fixed set of initial colors in the
  palette, which can be either a matrix or data frame of RGB values
  (with values between 0 and 1) or a character vector of hex colors (or
  any other format that's acceptable in
  [`grDevices::col2rgb()`](https://rdrr.io/r/grDevices/col2rgb.html)).

- white_point:

  The white point to use for color space conversions. Can be one of
  "D65" (default, daylight at 6500K), "D50" (daylight at 5000K), "D55"
  (daylight at 5500K), "A" (incandescent tungsten at 2856K), or "E"
  (equal energy).

- refinement_starts:

  A nonnegative integer controlling experimental continuous refinement.
  The default, `5L`, uses the discrete selection plus four random
  starting palettes and keeps the best result. Use `1L` to refine only
  the discrete selection or `0L` to disable refinement. Applies only to
  color space regions with `metric = "ciede2000"`. Random starts use
  fixed C++ seeds and are unaffected by
  [`set.seed()`](https://rdrr.io/r/base/Random.html). Refinement can
  change the generated palette and increase runtime.

## Value

A list of class `qualpal` with the following components.

- HSL:

  A matrix of the colors in the HSL color space.

- RGB:

  A matrix of the colors in the sRGB color space.

- hex:

  A character vector of the colors in hex notation.

- de_DIN99d:

  A distance matrix of color differences according to the metric used.
  The name is misleading, bu kept for backwards compatibility.

- hex:

  A character vector of the colors in hex notation.

- min_de_DIN99d:

  The minimum pairwise DIN99d color difference among all colors in the
  palette.

## Details

The main idea is to compute a distance matrix from all the input colors,
and then try to select the most distinct colors based on the color
differences between them. It does this iteratively by first selecting
the first `n` colors from the input colors, then iterates over the
palette, putting colors back into the total set and replaces it with a
new color until it has gone through the whole range without changing any
of the colors.

For HSL and LCHab color space regions, the selected colors are then
refined continuously in CIE L*a*b\* to improve their minimum CIEDE2000
distance. This experimental step preserves fixed colors supplied through
`extend` and respects the input region. Refinement is disabled for other
metrics and for explicit RGB inputs or named palettes.

Optionally, `qualpal` can adapt palettes to cater to color vision
deficiency (CVD). This is accomplished by taking the colors provided by
the user and transforming them to colors that someone with CVD would
see, that is, simulating CVD qualpal then chooses colors from these new
colors.

## See also

[`plot.qualpal()`](https://jolars.github.io/qualpalr/dev/reference/plot.qualpal.md),
[`pairs.qualpal()`](https://jolars.github.io/qualpalr/dev/reference/pairs.qualpal.md),
[`list_palettes()`](https://jolars.github.io/qualpalr/dev/reference/list_palettes.md)

## Examples

``` r
# Generate 3 distinct colors from the default color space
qualpal(3)
#> ---------------------------------------- 
#> Colors in the HSL color space 
#> 
#>      [,1] [,2] [,3]
#> [1,]  356  0.5  0.6
#> [2,]   60  0.5  0.6
#> [3,]  201  0.5  0.6
#> 
#>  ---------------------------------------- 
#> DIN99d color difference distance matrix 
#> 
#>    1  2
#> 2 23   
#> 3 25 24

# Provide a custom color space
qualpal(n = 3, list(h = c(35, 360), s = c(0.5, 0.7), l = c(0, 0.45)))
#> ---------------------------------------- 
#> Colors in the HSL color space 
#> 
#>      [,1] [,2]  [,3]
#> [1,]   60  0.7 0.450
#> [2,]  300  0.7 0.450
#> [3,]  120  0.7 0.073
#> 
#>  ---------------------------------------- 
#> DIN99d color difference distance matrix 
#> 
#>    1  2
#> 2 33   
#> 3 31 31

qualpal(3, "ColorBrewer:Set2")
#> ---------------------------------------- 
#> Colors in the HSL color space 
#> 
#>      [,1] [,2] [,3]
#> [1,]   83 0.63 0.59
#> [2,]   17 0.96 0.69
#> [3,]  222 0.37 0.67
#> 
#>  ---------------------------------------- 
#> DIN99d color difference distance matrix 
#> 
#>    1  2
#> 2 22   
#> 3 26 24

# Adapt palette to deuteranopia
qualpal(5, "ColorBrewer:Dark2", cvd = c(deutan = 1))
#> ---------------------------------------- 
#> Colors in the HSL color space 
#> 
#>      [,1] [,2] [,3]
#> [1,]   44 0.98 0.45
#> [2,]   39 0.70 0.38
#> [3,]  162 0.71 0.36
#> [4,]  244 0.31 0.57
#> [5,]  329 0.80 0.53
#> 
#>  ---------------------------------------- 
#> DIN99d color difference distance matrix 
#> 
#>    1  2  3  4
#> 2 12         
#> 3 24 23      
#> 4 29 26 24   
#> 5 26 22 31 20

# Adapt palette to protanomaly with severity 0.4
qualpal(8, cvd = c(protan = 0.4))
#> ---------------------------------------- 
#> Colors in the HSL color space 
#> 
#>      [,1] [,2] [,3]
#> [1,]   25  0.5 0.60
#> [2,]  358  0.5 0.60
#> [3,]  120  0.5 0.60
#> [4,]  344  0.5 0.79
#> [5,]  240  0.5 0.60
#> [6,]  209  0.5 0.68
#> [7,]  171  0.5 0.61
#> [8,]   38  0.5 0.85
#> 
#>  ---------------------------------------- 
#> DIN99d color difference distance matrix 
#> 
#>    1  2  3  4  5  6  7
#> 2 12                  
#> 3 22 27               
#> 4 14 13 25            
#> 5 26 23 30 21         
#> 6 23 24 22 19 16      
#> 7 24 28 14 23 25 14   
#> 8 15 20 18 14 27 20 19

# Generate and extend a palette with 3 colors, using the DIN99d
# metric
pal <- qualpal(3)
qualpal(5, extend = pal$hex, metric = "din99d")
#> ---------------------------------------- 
#> Colors in the HSL color space 
#> 
#>      [,1] [,2] [,3]
#> [1,]  356 0.50 0.60
#> [2,]   60 0.50 0.60
#> [3,]  201 0.50 0.60
#> [4,]  268 0.45 0.62
#> [5,]  332 0.25 0.84
#> 
#>  ---------------------------------------- 
#> DIN99d color difference distance matrix 
#> 
#>    1  2  3  4
#> 2 23         
#> 3 25 24      
#> 4 19 29 18   
#> 5 18 19 18 18

# Use a different white point (D50, common in printing)
qualpal(5, white_point = "D50")
#> ---------------------------------------- 
#> Colors in the HSL color space 
#> 
#>      [,1] [,2] [,3]
#> [1,]   65 0.54 0.55
#> [2,]  179 0.48 0.60
#> [3,]  243 0.44 0.62
#> [4,]   10 0.42 0.85
#> [5,]  354 0.49 0.60
#> 
#>  ---------------------------------------- 
#> DIN99d color difference distance matrix 
#> 
#>    1  2  3  4
#> 2 21         
#> 3 30 22      
#> 4 18 20 22   
#> 5 24 27 21 17

if (FALSE) { # \dontrun{
# The range of hue cannot exceed 360
qualpal(3, list(h = c(-20, 360), s = c(0.5, 0.7), l = c(0, 0.45)))
} # }
```
