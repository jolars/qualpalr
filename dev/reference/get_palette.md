# Retrieve one of the built-in color palettes

This function retrieves a color palette from the built-in palettes. To
see the available palettes, use
[`list_palettes()`](https://jolars.github.io/qualpalr/dev/reference/list_palettes.md).

## Usage

``` r
get_palette(palette = "ColorBrewer:Accent")
```

## Arguments

- palette:

  A character string specifying the name of the color palette to

## Value

A character vector of colors in hex format.

## See also

[`list_palettes()`](https://jolars.github.io/qualpalr/dev/reference/list_palettes.md)

## Examples

``` r
get_palette("Vermeer:LittleStreet")
#>  [1] "#9D3D2D" "#99A8A5" "#8F4C2D" "#6A6A54" "#EADAC5" "#27221F" "#727C73"
#>  [8] "#C9AA82" "#3A2D22" "#8F5144" "#C9BDA3"
```
