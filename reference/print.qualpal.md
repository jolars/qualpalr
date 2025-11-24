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
#>         L(99d) a(99d) b(99d)
#> #c9cb70   82.2   -4.3  30.24
#> #ca6c74   60.3   30.4   6.17
#> #70a4c8   68.4  -14.6 -18.49
#> 
#>  ---------------------------------------- 
#> DIN99d color difference distance matrix 
#> 
#>         #c9cb70 #ca6c74
#> #ca6c74    22.3        
#> #70a4c8    23.7    23.8
```
