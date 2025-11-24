# Analyze a categorical color palette

Analyze a categorical color palette with respect to the differences
between the colors in the palette.

## Usage

``` r
analyze_palette(
  palette,
  cvd = c(protan = 0, deutan = 0, tritan = 0),
  bg = NULL,
  metric = c("ciede2000", "din99d", "cie76")
)
```

## Arguments

- palette:

  Either a matrix of RGB values (with values between 0 and 1), a data
  frame with RGB values, or a character vector of hex colors.

- cvd:

  Color vision deficiency adaptation. This must be a named vector with
  names `protan`, `deutan`, and `tritan` and values between 0 and 1,
  where 0 means no adaptation and 1 means full adaptation.

- bg:

  Background color to consider (but not include) when generating the
  palette. This is useful to avoid colors that are too close to the
  background/canvas color. If `NULL` (the default), the background color
  is not considered at all. Any color that is convertable via
  [`col2rgb`](https://rdrr.io/r/grDevices/col2rgb.html) is acceptable,
  including hex colors.

- metric:

  The color metric to use for the color distance matrix.

## Value

A list of lists, one for each type of color vision deficiency plus
normal vision. Each list contains `difference_matrix`, `min_distances`,
and `bg_min_distance`

## See also

[`qualpal()`](https://jolars.github.io/qualpalr/reference/qualpal.md)

## Examples

``` r
pal <- qualpal(5)
analyze_palette(pal$hex, cvd = c(protan = 1))
#> $deutan
#> $deutan$difference_matrix
#>          #c9cb70  #7cc4cc  #cb7469  #6e6cca
#> #7cc4cc 31.95148                           
#> #cb7469 41.40243 43.64204                  
#> #6e6cca 64.37811 33.37803 35.19014         
#> #d8c3e9 45.21974 31.62615 29.96390 29.99927
#> 
#> $deutan$min_distances
#>  #c9cb70  #7cc4cc  #cb7469  #6e6cca  #d8c3e9 
#> 31.95148 31.62615 29.96390 29.99927 29.96390 
#> 
#> $deutan$bg_min_distance
#> [1] NaN
#> 
#> 
#> $normal
#> $normal$difference_matrix
#>          #c9cb70  #7cc4cc  #cb7469  #6e6cca
#> #7cc4cc 31.95148                           
#> #cb7469 41.40243 43.64204                  
#> #6e6cca 64.37811 33.37803 35.19014         
#> #d8c3e9 45.21974 31.62615 29.96390 29.99927
#> 
#> $normal$min_distances
#>  #c9cb70  #7cc4cc  #cb7469  #6e6cca  #d8c3e9 
#> 31.95148 31.62615 29.96390 29.99927 29.96390 
#> 
#> $normal$bg_min_distance
#> [1] NaN
#> 
#> 
#> $protan
#> $protan$difference_matrix
#>          #c9cb70  #7cc4cc  #cb7469  #6e6cca
#> #7cc4cc 31.75402                           
#> #cb7469 26.29280 26.85895                  
#> #6e6cca 57.52869 26.64757 36.71673         
#> #d8c3e9 38.33444  6.73337 33.51401 26.53771
#> 
#> $protan$min_distances
#>  #c9cb70  #7cc4cc  #cb7469  #6e6cca  #d8c3e9 
#> 26.29280  6.73337 26.29280 26.53771  6.73337 
#> 
#> $protan$bg_min_distance
#> [1] NaN
#> 
#> 
#> $tritan
#> $tritan$difference_matrix
#>          #c9cb70  #7cc4cc  #cb7469  #6e6cca
#> #7cc4cc 31.95148                           
#> #cb7469 41.40243 43.64204                  
#> #6e6cca 64.37811 33.37803 35.19014         
#> #d8c3e9 45.21974 31.62615 29.96390 29.99927
#> 
#> $tritan$min_distances
#>  #c9cb70  #7cc4cc  #cb7469  #6e6cca  #d8c3e9 
#> 31.95148 31.62615 29.96390 29.99927 29.96390 
#> 
#> $tritan$bg_min_distance
#> [1] NaN
#> 
#> 
```
