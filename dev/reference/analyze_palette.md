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

[`qualpal()`](https://jolars.github.io/qualpalr/dev/reference/qualpal.md)

## Examples

``` r
pal <- qualpal(5)
analyze_palette(pal$hex, cvd = c(protan = 1))
#> $deutan
#> $deutan$difference_matrix
#>          1        2        3        4
#> 2 37.64796                           
#> 3 33.99326 33.54151                  
#> 4 46.47404 33.82091 60.26837         
#> 5 33.95615 37.94976 54.83337 33.87781
#> 
#> $deutan$min_distances
#>  #cc9466  #68bfcd  #98cc66  #6766cc  #eac2eb 
#> 33.95615 33.54151 33.54151 33.82091 33.87781 
#> 
#> $deutan$bg_min_distance
#> [1] NaN
#> 
#> 
#> $normal
#> $normal$difference_matrix
#>          1        2        3        4
#> 2 37.64796                           
#> 3 33.99326 33.54151                  
#> 4 46.47404 33.82091 60.26837         
#> 5 33.95615 37.94976 54.83337 33.87781
#> 
#> $normal$min_distances
#>  #cc9466  #68bfcd  #98cc66  #6766cc  #eac2eb 
#> 33.95615 33.54151 33.54151 33.82091 33.87781 
#> 
#> $normal$bg_min_distance
#> [1] NaN
#> 
#> 
#> $protan
#> $protan$difference_matrix
#>           1         2         3         4
#> 2 30.903017                              
#> 3 13.643532 35.523128                    
#> 4 49.081899 25.673314 59.513139          
#> 5 36.008174  6.163088 39.200820 28.657278
#> 
#> $protan$min_distances
#>   #cc9466   #68bfcd   #98cc66   #6766cc   #eac2eb 
#> 13.643532  6.163088 13.643532 25.673314  6.163088 
#> 
#> $protan$bg_min_distance
#> [1] NaN
#> 
#> 
#> $tritan
#> $tritan$difference_matrix
#>          1        2        3        4
#> 2 37.64796                           
#> 3 33.99326 33.54151                  
#> 4 46.47404 33.82091 60.26837         
#> 5 33.95615 37.94976 54.83337 33.87781
#> 
#> $tritan$min_distances
#>  #cc9466  #68bfcd  #98cc66  #6766cc  #eac2eb 
#> 33.95615 33.54151 33.54151 33.82091 33.87781 
#> 
#> $tritan$bg_min_distance
#> [1] NaN
#> 
#> 
```
