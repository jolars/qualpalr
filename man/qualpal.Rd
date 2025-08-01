% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/qualpal.R
\name{qualpal}
\alias{qualpal}
\title{Generate qualitative color palettes}
\usage{
qualpal(
  n,
  colorspace = list(h = c(0, 360), s = c(0.2, 0.5), l = c(0.6, 0.85)),
  cvd = c("protan", "deutan", "tritan"),
  cvd_severity = 0,
  bg = NULL,
  metric = c("din99d", "ciede2000", "cie76"),
  extend = NULL
)
}
\arguments{
\item{n}{The number of colors to generate.}

\item{colorspace}{A color space to generate colors from. Can be any of the
following:
\itemize{
\item A \code{\link{list}} that describes a color space in either
HSL or LCHab color space. In the first case (HSL), the list must
contain the following \emph{named} vectors,
each of length two, giving a range for each item.
\describe{
\item{\code{h}}{Hue, in range from -360 to 360}
\item{\code{s}}{Saturation, in the range from 0 to 1}
\item{\code{l}}{Lightness, in the range from 0 to 1}
}
In the second case (LCHab), the list must contain the following
\emph{named} vectors,
each of length two, giving a range for each item.
\describe{
\item{\code{h}}{Hue, in range from -360 to 360}
\item{\code{c}}{Chroma, in the range from 0 to infinity}
\item{\code{l}}{Lightness, in the range from 0 to 100}
}
In these cases, \code{qualpal()} will generate
\item A \code{\link{character}} vector of length one in
the form of "Source:Palette", where \emph{Domain} is the name of a
source that provides a color palette, and \emph{Palette} is the name of
a color palette from that source. See \code{\link[=list_palettes]{list_palettes()}} for
available palettes.
}}

\item{cvd}{Color vision deficiency adaptation. Use \code{cvd_severity}
to set the severity of color vision deficiency to adapt to. Permissible
values are \code{"protan", "deutan",} and \code{"tritan"}.}

\item{cvd_severity}{Severity of color vision deficiency to adapt to. Can take
any value from 0, for normal vision (the default), and 1, for dichromatic
vision.}

\item{bg}{Backgrouind color to consider (but not include) when
generating the palette. This is useful to avoid colros that are too
close to the background/canvas color. If \code{NULL} (the default), the
background color is not considered at all. Any color that is convertable
via \code{\link[grDevices]{col2rgb}} is acceptable, including hex colors.}

\item{metric}{The color metric to use for the color distance
matrix.}

\item{extend}{A palette of colors to use as a fixed set of initial
colors in the palette, which can be either a matrix or data frame
of RGB values (with values between 0 and 1) or a character vector
of hex colors (or any other format that's acceptable in
\code{\link[grDevices:col2rgb]{grDevices::col2rgb()}}).}
}
\value{
A list of class \code{qualpal} with the following
components.
\item{HSL}{
A matrix of the colors in the HSL color space.
}
\item{RGB}{
A matrix of the colors in the sRGB color space.} \item{hex}{A
character vector of the colors in hex notation.} \item{de_DIN99d}{A
distance matrix of color differences according to delta E DIN99d.
}
\item{DIN99d}{
A color difference matrix of the colors corrosponding to the
\code{metric} used.
}
\item{de_DIN99d}{
The minimum pairwise DIN99d color difference for each color in the
palette.
}
\item{hex}{
A character vector of the colors in hex notation.
}
\item{min_de_DIN99d}{
The minimum pairwise DIN99d color difference among all colors in the
palette.
}
}
\description{
Given a collection of colors, \code{qualpal()} algorithmically
tries to select to \code{n} most distinct colors from the provided
input colors, optionally taking color vision deficiency into account.
}
\details{
In the default \code{metric} setting, the colors that the user provides are
projected into the DIN99d color space, which is approximately perceptually
uniform, i.e. color difference is proportional to the euclidean distance
between two colors. A distance matrix is computed and, as an additional
step, is transformed using power transformations discovered by Huang 2015 in
order to fine tune differences.

\code{qualpal} then searches the distance matrix for the most distinct
colors; it does this iteratively by first selecting a random set of colors
and then iterates over each color, putting colors back into the total set
and replaces it with a new color until it has gone through the whole range
without changing any of the colors.

Optionally, \code{qualpal} can adapt palettes to cater to color vision
deficiency (CVD). This is accomplished by taking the colors
provided by the user and transforming them to colors that someone with CVD
would see, that is, simulating CVD qualpal then chooses colors from
these new colors.
}
\examples{
# Generate 3 distinct colors from the default color space
qualpal(3)

# Provide a custom color space
qualpal(n = 3, list(h = c(35, 360), s = c(0.5, 0.7), l = c(0, 0.45)))

qualpal(3, "ColorBrewer:Set2")

# Adapt palette to deuteranopia
qualpal(5, "ColorBrewer:Dark2", cvd = "deutan", cvd_severity = 1)

# Adapt palette to protanomaly with severity 0.4
qualpal(8, cvd = "protan", cvd_severity = 0.4)

# Generate and extend a palette with 3 colors
pal <- qualpal(3)
qualpal(5, extend = pal$hex)

\dontrun{
# The range of hue cannot exceed 360
qualpal(3, list(h = c(-20, 360), s = c(0.5, 0.7), l = c(0, 0.45)))
}

}
\seealso{
\code{\link[=plot.qualpal]{plot.qualpal()}}, \code{\link[=pairs.qualpal]{pairs.qualpal()}}, \code{\link[=list_palettes]{list_palettes()}}
}
