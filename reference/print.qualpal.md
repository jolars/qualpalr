# Print qualpal palette

Print the result from a call to
[`qualpal`](https://jolars.github.io/qualpalr/reference/qualpal.md).

## Usage

``` r
# S3 method for class 'qualpal'
print(x, colorspace = c("HSL", "DIN99d", "RGB"), digits = 2, ...)
```

## Arguments

- x:

  An object of class `"qualpal"`.

- colorspace:

  Color space to print colors in.

- digits:

  Number of significant digits for the output. (See
  [print.default](https://rdrr.io/r/base/print.default.html).) Setting
  it to `NULL` uses
  [`getOption`](https://rdrr.io/r/base/options.html)`("digits")`.

- ...:

  Arguments to pass to
  [`print.default`](https://rdrr.io/r/base/print.default.html).

## Value

Prints the colors as a matrix in the specified color space as well as a
distance matrix of the color differences. Invisibly returns x.

## Examples

``` r
f <- qualpal(3)
print(f, colorspace = "DIN99d", digits = 3)
#> ---------------------------------------- 
#> Colors in the DIN99d color space 
#> 
#>      [,1]   [,2]   [,3]
#> [1,] 59.3  31.62   7.44
#> [2,] 82.5  -3.77  32.19
#> [3,] 69.3 -17.89 -17.90
#> 
#>  ---------------------------------------- 
#> DIN99d color difference distance matrix 
#> 
#>      1    2
#> 2 22.8     
#> 3 25.3 24.4
```
