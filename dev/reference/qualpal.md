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
  white_point = c("D65", "D50", "D55", "A", "E")
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
#>         Hue Saturation Lightness
#> #c9cb70  62       0.47      0.62
#> #ca6c74 355       0.47      0.61
#> #70a4c8 205       0.45      0.61
#> 
#>  ---------------------------------------- 
#> DIN99d color difference distance matrix 
#> 
#>         #c9cb70 #ca6c74
#> #ca6c74      22        
#> #70a4c8      24      24

# Provide a custom color space
qualpal(n = 3, list(h = c(35, 360), s = c(0.5, 0.7), l = c(0, 0.45)))
#> ---------------------------------------- 
#> Colors in the HSL color space 
#> 
#>         Hue Saturation Lightness
#> #b32eb7 298       0.60     0.449
#> #9ebb26  71       0.66     0.440
#> #071f06 117       0.67     0.073
#> 
#>  ---------------------------------------- 
#> DIN99d color difference distance matrix 
#> 
#>         #b32eb7 #9ebb26
#> #9ebb26      32        
#> #071f06      30      29

qualpal(3, "ColorBrewer:Set2")
#> ---------------------------------------- 
#> Colors in the HSL color space 
#> 
#>         Hue Saturation Lightness
#> #a6d854  83       0.63      0.59
#> #fc8d62  17       0.96      0.69
#> #8da0cb 222       0.37      0.67
#> 
#>  ---------------------------------------- 
#> DIN99d color difference distance matrix 
#> 
#>         #a6d854 #fc8d62
#> #fc8d62      22        
#> #8da0cb      26      24

# Adapt palette to deuteranopia
qualpal(5, "ColorBrewer:Dark2", cvd = c(deutan = 1))
#> ---------------------------------------- 
#> Colors in the HSL color space 
#> 
#>         Hue Saturation Lightness
#> #e6ab02  44       0.98      0.45
#> #a6761d  39       0.70      0.38
#> #1b9e77 162       0.71      0.36
#> #7570b3 244       0.31      0.57
#> #e7298a 329       0.80      0.53
#> 
#>  ---------------------------------------- 
#> DIN99d color difference distance matrix 
#> 
#>         #e6ab02 #a6761d #1b9e77 #7570b3
#> #a6761d      12                        
#> #1b9e77      24      23                
#> #7570b3      29      26      24        
#> #e7298a      26      22      31      20

# Adapt palette to protanomaly with severity 0.4
qualpal(8, cvd = c(protan = 0.4))
#> ---------------------------------------- 
#> Colors in the HSL color space 
#> 
#>         Hue Saturation Lightness
#> #ce836c  14       0.50      0.62
#> #d074a2 330       0.50      0.64
#> #6e6cca 241       0.47      0.61
#> #77b6c6 192       0.41      0.62
#> #e7c7e7 299       0.41      0.84
#> #6ec7a1 154       0.44      0.61
#> #bcc76c  67       0.45      0.60
#> #e4d7ca  30       0.33      0.84
#> 
#>  ---------------------------------------- 
#> DIN99d color difference distance matrix 
#> 
#>         #ce836c #d074a2 #6e6cca #77b6c6 #e7c7e7 #6ec7a1 #bcc76c
#> #d074a2      13                                                
#> #6e6cca      24      18                                        
#> #77b6c6      24      24      19                                
#> #e7c7e7      18      15      19      18                        
#> #6ec7a1      24      27      26      12      22                
#> #bcc76c      19      25      29      21      22      15        
#> #e4d7ca      16      19      24      18      13      18      14

# Generate and extend a palette with 3 colors, using the DIN99d
# metric
pal <- qualpal(3)
qualpal(5, extend = pal$hex, metric = "din99d")
#> ---------------------------------------- 
#> Colors in the HSL color space 
#> 
#>         Hue Saturation Lightness
#> #c9cb70  61       0.47      0.62
#> #ca6c74 355       0.47      0.61
#> #70a4c8 205       0.44      0.61
#> #9468ca 267       0.48      0.60
#> #e6c6e6 301       0.39      0.84
#> 
#>  ---------------------------------------- 
#> DIN99d color difference distance matrix 
#> 
#>         #c9cb70 #ca6c74 #70a4c8 #9468ca
#> #ca6c74      22                        
#> #70a4c8      24      24                
#> #9468ca      29      19      18        
#> #e6c6e6      22      18      18      17

# Use a different white point (D50, common in printing)
qualpal(5, white_point = "D50")
#> ---------------------------------------- 
#> Colors in the HSL color space 
#> 
#>         Hue Saturation Lightness
#> #c89c71  30       0.44      0.61
#> #d074a2 330       0.50      0.64
#> #7086c7 225       0.43      0.61
#> #8acc6d 102       0.48      0.61
#> #c7eadc 155       0.46      0.85
#> 
#>  ---------------------------------------- 
#> DIN99d color difference distance matrix 
#> 
#>         #c89c71 #d074a2 #7086c7 #8acc6d
#> #d074a2      17                        
#> #7086c7      24      19                
#> #8acc6d      18      28      26        
#> #c7eadc      20      25      22      14

if (FALSE) { # \dontrun{
# The range of hue cannot exceed 360
qualpal(3, list(h = c(-20, 360), s = c(0.5, 0.7), l = c(0, 0.45)))
} # }
```
