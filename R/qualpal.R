#' Generate qualitative color palettes
#'
#' Given a collection of colors, \code{qualpal()} algorithmically
#' tries to select to `n` most distinct colors from the provided
#' input colors, optionally taking color vision deficiency into account.
#'
#' The main idea is to compute a distance matrix from all the input colors, and
#' then try to select the most distinct colors based on the color differences
#' between them. It does this iteratively by first selecting the first `n`
#' colors from the input colors, then iterates over the palette, putting colors
#' back into the total set and replaces it with a new color until it has gone
#' through the whole range without changing any of the colors.
#'
#' Optionally, \code{qualpal} can adapt palettes to cater to color vision
#' deficiency (CVD). This is accomplished by taking the colors
#' provided by the user and transforming them to colors that someone with CVD
#' would see, that is, simulating CVD qualpal then chooses colors from
#' these new colors.
#'
#' @param n The number of colors to generate.
#' @param colorspace A color space to generate colors from. Can be any of the
#'   following:
#'   \itemize{
#'     \item A \code{\link{list}} that describes a color space in either
#'     HSL or LCHab color space. In the first case (HSL), the list must
#'     contain the following \emph{named} vectors,
#'       each of length two, giving a range for each item.
#'       \describe{
#'         \item{\code{h}}{Hue, in range from -360 to 360}
#'         \item{\code{s}}{Saturation, in the range from 0 to 1}
#'         \item{\code{l}}{Lightness, in the range from 0 to 1}
#'       }
#'       In the second case (LCHab), the list must contain the following
#'       \emph{named} vectors,
#'       each of length two, giving a range for each item.
#'       \describe{
#'         \item{\code{h}}{Hue, in range from -360 to 360}
#'         \item{\code{c}}{Chroma, in the range from 0 to infinity}
#'         \item{\code{l}}{Lightness, in the range from 0 to 100}
#'       }
#'       In these cases, `qualpal()` will generate
#'     \item A \code{\link{character}} vector of length one in
#'       the form of "Source:Palette", where \emph{Domain} is the name of a
#'       source that provides a color palette, and \emph{Palette} is the name of
#'       a color palette from that source. See [list_palettes()] for
#'       available palettes.
#'     \item A matrix or data frame of RGB values (with values between 0 and
#'       1).
#'   }
#' @param cvd Color vision deficiency adaptation. This must be a named
#'   vector with names `protan`, `deutan`, and `tritan` and values
#'   between 0 and 1, where 0 means no adaptation and 1 means full
#'   adaptation.
#' @param cvd_severity DEPRECATED. Use a named `cvd` vector instead, e.g.
#'  `c(protan = 0.5, deutan = 0.2, tritan = 0)`.
#' @param bg Background color to consider (but not include) when
#'   generating the palette. This is useful to avoid colors that are too
#'   close to the background/canvas color. If \code{NULL} (the default), the
#'   background color is not considered at all. Any color that is convertable
#'   via \code{\link[grDevices]{col2rgb}} is acceptable, including hex colors.
#' @param metric The color metric to use for the color distance
#'   matrix.
#' @param extend A palette of colors to use as a fixed set of initial
#'   colors in the palette, which can be either a matrix or data frame
#'   of RGB values (with values between 0 and 1) or a character vector
#'   of hex colors (or any other format that's acceptable in
#'   [grDevices::col2rgb()]).
#'
#' @return A list of class \code{qualpal} with the following
#'   components.
#'   \item{HSL}{
#'     A matrix of the colors in the HSL color space.
#'   }
#'   \item{RGB}{
#'     A matrix of the colors in the sRGB color space.
#'   }
#'   \item{hex}{A
#'     character vector of the colors in hex notation.
#'   }
#'   \item{de_DIN99d}{
#'     A distance matrix of color differences according to the
#'     metric used. The name is misleading, bu kept for
#'     backwards compatibility.
#'   }
#'   \item{hex}{
#'     A character vector of the colors in hex notation.
#'   }
#'   \item{min_de_DIN99d}{
#'     The minimum pairwise DIN99d color difference among all colors in the
#'     palette.
#'   }
#' @seealso [plot.qualpal()], [pairs.qualpal()], [list_palettes()]
#' @examples
#' # Generate 3 distinct colors from the default color space
#' qualpal(3)
#'
#' # Provide a custom color space
#' qualpal(n = 3, list(h = c(35, 360), s = c(0.5, 0.7), l = c(0, 0.45)))
#'
#' qualpal(3, "ColorBrewer:Set2")
#'
#' # Adapt palette to deuteranopia
#' qualpal(5, "ColorBrewer:Dark2", cvd = c(deutan = 1))
#'
#' # Adapt palette to protanomaly with severity 0.4
#' qualpal(8, cvd = c(protan = 0.4))
#'
#' # Generate and extend a palette with 3 colors, using the DIN99d
#' # metric
#' pal <- qualpal(3)
#' qualpal(5, extend = pal$hex, metric = "din99d")
#'
#' \dontrun{
#' # The range of hue cannot exceed 360
#' qualpal(3, list(h = c(-20, 360), s = c(0.5, 0.7), l = c(0, 0.45)))
#' }
#'
#' @export
qualpal <- function(
  n,
  colorspace = list(h = c(0, 360), s = c(0.2, 0.5), l = c(0.6, 0.85)),
  cvd = c(protan = 0, deutan = 0, tritan = 0),
  cvd_severity,
  bg = NULL,
  metric = c("ciede2000", "din99d", "cie76"),
  extend = NULL
) {
  UseMethod("qualpal", colorspace)
}

#' @export
qualpal.matrix <- function(
  n,
  colorspace,
  ...
) {
  stopifnot(
    is.matrix(colorspace),
    ncol(colorspace) == 3,
    max(colorspace) <= 1,
    min(colorspace) >= 0
  )

  validate_args(n)
  opts <- make_options(...)
  res <- qualpal_cpp_rgb(n, colorspace, opts)
  res$de_DIN99d <- stats::as.dist(res$de_DIN99d)

  res
}

#' @export
qualpal.data.frame <- function(
  n,
  colorspace,
  ...
) {
  mat <- data.matrix(colorspace)
  qualpal(
    n = n,
    colorspace = mat,
    ...
  )
}

#' @export
qualpal.character <- function(
  n,
  colorspace = "ColorBrewer:Set2",
  ...
) {
  stopifnot(
    is.character(colorspace)
  )
  validate_args(n)

  if (colorspace %in% c("pretty", "pretty_dark", "rainbow", "pastels")) {
    warning(
      "The use of predefined colorspaces is deprecated. This method now ",
      "expects the name of a color palette. Please use the list method and ",
      "provide the color space explicitly instead."
    )
    colorspace <- predefined_colorspaces(colorspace)
  }

  opts <- make_options(...)
  res <- qualpal_cpp_palette(n, colorspace, opts)
  res$de_DIN99d <- stats::as.dist(res$de_DIN99d)

  res
}


#' @export
qualpal.list <- function(
  n,
  colorspace = list(h = c(0, 360), s = c(0.2, 0.5), l = c(0.6, 0.85)),
  ...
) {
  nc <- names(colorspace)

  stopifnot(
    !is.null(attr(colorspace, "names")),
    (("h" %in% nc) && ("s" %in% nc) && ("l" %in% nc)) ||
      (("h" %in% nc) && ("c" %in% nc) && ("l" %in% nc)),
    length(nc) == 3
  )
  validate_args(n)

  colorspace_type <- if ("s" %in% nc) {
    "hsl"
  } else {
    "lchab"
  }

  h <- colorspace[["h"]]
  l <- colorspace[["l"]]

  if (colorspace_type == "hsl") {
    s_or_c <- colorspace[["s"]]
    stopifnot(
      min(s_or_c) >= 0,
      max(s_or_c) <= 1,
      min(l) >= 0,
      max(l) <= 1
    )
  } else {
    s_or_c <- colorspace[["c"]]
    stopifnot(
      s_or_c >= 0,
      min(l) >= 0,
      max(l) <= 100
    )
  }

  n_points <- 1000

  stopifnot(
    diff(range(h)) <= 360,
    min(h) >= -360,
    max(h) <= 360,
    length(h) == 2,
    length(l) == 2,
    is.numeric(h),
    is.numeric(l)
  )

  colorspace[["s_or_c"]] <- s_or_c
  colorspace[["type"]] <- colorspace_type

  opts <- make_options(...)

  res <- qualpal_cpp_colorspace(n, colorspace, n_points, opts)
  res$de_DIN99d <- stats::as.dist(res$de_DIN99d)

  res
}


#' Print qualpal palette
#'
#' Print the result from a call to \code{\link{qualpal}}.
#'
#' @param x An object of class \code{"qualpal"}.
#' @param colorspace Color space to print colors in.
#' @param digits Number of significant digits for the output.
#'   (See \link{print.default}.) Setting it to \code{NULL} uses
#'   \code{\link{getOption}("digits")}.
#' @param \dots Arguments to pass to \code{\link{print.default}}.
#'
#' @return Prints the colors as a matrix in the specified color space as well
#'   as a distance matrix of the color differences. Invisibly returns x.
#' @export
#'
#' @examples
#' f <- qualpal(3)
#' print(f, colorspace = "DIN99d", digits = 3)
print.qualpal <- function(
  x,
  colorspace = c("HSL", "DIN99d", "RGB"),
  digits = 2,
  ...
) {
  vsep <- strrep("-", 0.5 * getOption("width"))

  cat(vsep, "\n")
  cat("Colors in the", match.arg(colorspace), "color space", "\n\n")
  print(x[[match.arg(colorspace)]], digits = digits, ...)

  cat("\n", vsep, "\n")
  cat("DIN99d color difference distance matrix", "\n\n")
  print(x[["de_DIN99d"]], digits = digits, ...)

  invisible(x)
}

# Predefined color spaces -------------------------------------------------

predefined_colorspaces <- function(colorspace) {
  spaces <- list(
    pretty = list(h = c(0, 360), s = c(0.2, 0.5), l = c(0.6, 0.85)),
    pretty_dark = list(h = c(0, 360), s = c(0.1, 0.5), l = c(0.2, 0.4)),
    rainbow = list(h = c(0, 360), s = c(0, 1), l = c(0, 1)),
    pastels = list(h = c(0, 360), s = c(0.2, 0.4), l = c(0.8, 0.9))
  )

  stopifnot(colorspace %in% names(spaces))

  spaces[[colorspace]]
}

validate_args <- function(n) {
  stopifnot(
    is.numeric(n),
    n %% 1 == 0,
    n > 1
  )
}
