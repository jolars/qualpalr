# Optimize color palette for color vision deficiency

**Deprecated**: This function is deprecated and will be removed in a
future release. There is no direct replacement.

## Arguments

- n:

  Number of colors to generate.

- colorspace:

  Either 1) a list of three named numeric vectors: `h` (hue), `s`
  (saturation), and `l` (lightness), all of length 2 specifying a min
  and max value for the range. The values has to be in the range -360 to
  360 for `h`, and 0 to 1 for `s` and `l` 2), or 2) a *character vector*
  specifying one of the predefined color spaces (see below).

- cvd:

  Color vision deficiency adaptation to adapt the color palette to.

- target:

  Target color difference.

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

This function adapts color palettes to color vision deficiency (CVD) by
optimizing the CVD severity to try reach a target color difference
(DIN99d \\\delta E\\) of the user's choosing. Basically, it will choose
a color palette that is as close as possible to the target \\\delta E\\
by tweaking the CVD severity option in
[`qualpal`](https://jolars.github.io/qualpalr/reference/qualpal.md).

The rationale for this function is that when there are few colors in a
color palette, there is no cost involved in adapting colors to CVD – the
colors will still remain distinct. As more an more colors are added to
the palette, however, adapting the color palette to CVD will eventually
lead to colors that are too similar. This function gradually loosens the
adaptation to CVDs by lowering the severity of CVD to simulate to before
picking colors (the `cvd_severity` argument in
[`qualpal`](https://jolars.github.io/qualpalr/reference/qualpal.md))

## See also

[`qualpal-deprecated`](https://jolars.github.io/qualpalr/reference/qualpal-deprecated.md)
