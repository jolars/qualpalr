#' Generate qualitative color palettes
#'
#' Given a color space or collection of colors, \code{qualpal()} projects
#' these colors to the DIN99d color space, where it generates a color palette
#' from the most visually distinct colors, optionally taking color vision
#' deficiency into account.
#'
#' The function takes a color subspace in the HSL color space, where lightness
#' and saturation take values from 0 to 1. Hue take values from -360 to 360,
#' although negative values are brought to lie in the range \{0, 360\}; this
#' behavior exists to enable color subspaces that span all hues being that the
#' hue space is circular.
#'
#' The HSL color subspace that the user provides is projected into the DIN99d
#' color space, which is approximately perceptually uniform, i.e. color
#' difference is proportional to the euclidean distance between two colors. A
#' distance matrix is computed and, as an additional step, is transformed using
#' power transformations discovered by Huang 2015 in order to fine tune
#' differences.
#'
#' \code{qualpal} then searches the distance matrix for the most
#' distinct colors; it does this iteratively by first selecting a random set of
#' colors and then iterates over each color, putting colors back into the total
#' set and replaces it with a new color until it has gone through the whole
#' range without changing any of the colors.
#'
#' Optionally, \code{qualpal} can adapt palettes to cater to color vision
#' deficiency (cvd). This is accomplished by taking the colors
#' provided by the user and transforming them to colors that someone with cvd
#' would see, that is, simulating cvd. qualpal then chooses colors from
#' these new colors.
#'
#' \code{qualpal} currently only supports the sRGB color space with the D65
#' white point reference.
#'
#' @param n The number of colors to generate.
#' @param colorspace A color space to generate colors from. Can be any of the
#'   following:
#'   \itemize{
#'     \item A \code{\link{list}} with the following \emph{named} vectors,
#'       each of length two, giving a range for each item.
#'       \describe{
#'         \item{\code{h}}{Hue, in range from -360 to 360}
#'         \item{\code{s}}{Saturation, in the range from 0 to 1}
#'         \item{\code{l}}{Lightness, in the range from 0 to 1}
#'       }
#'     \item A \code{\link{character}} vector of length one in
#'     the form of "Source:Palette", where \emph{Domain} is the name of a
#'     source that provides a color palette, and \emph{Palette} is the name of
#'     a color palette from that source. The following source are
#'     currently
#'     supported:
#'     \itemize{
#'         \item \code{"ColorBrewer"}
#'     }
#'   }
#'
#' @param cvd Color vision deficiency adaptation. Use \code{cvd_severity}
#'   to set the severity of color vision deficiency to adapt to. Permissible
#'   values are \code{"protan", "deutan",} and \code{"tritan"}.
#' @param cvd_severity Severity of color vision deficiency to adapt to. Can take
#'   any value from 0, for normal vision (the default), and 1, for dichromatic
#'   vision.
#' @param bg Backgrouind color to consider (but not include) when
#'  generating the palette. This is useful to avoid colros that are too
#'  close to the background/canvas color. If \code{NULL} (the default), the
#'  background color is not considered at all. Any color that is convertable
#'  via \code{\link{col2rgb}} is acceptable, including hex colors.
#'
#' @return A list of class \code{qualpal} with the following
#'   components.
#'   \item{HSL}{
#'     A matrix of the colors in the HSL color space.
#'   }
#'   \item{DIN99d}{
#'     A matrix of the colors in the DIN99d color space (after power
#'     transformations).
#'   }
#'   \item{RGB}{
#'     A matrix of the colors in the sRGB color space.} \item{hex}{A
#'     character vector of the colors in hex notation.} \item{de_DIN99d}{A
#'     distance matrix of color differences according to delta E DIN99d.
#'   }
#'   \item{min_de_DIN99d}{
#'     The smallest pairwise DIN99d color difference.
#'   }
#' @seealso \code{\link{plot.qualpal}}, \code{\link{pairs.qualpal}}
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
#' qualpal(5, "ColorBrewer:Dark2", cvd = "deutan", cvd_severity = 1)
#'
#' # Adapt palette to protanomaly with severity 0.4
#' qualpal(8, cvd = "protan", cvd_severity = 0.4)
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
  cvd = c("protan", "deutan", "tritan"),
  cvd_severity = 0,
  bg = NULL
) {
  UseMethod("qualpal", colorspace)
}

#' @export
qualpal.matrix <- function(
  n,
  colorspace,
  cvd = c("protan", "deutan", "tritan"),
  cvd_severity = 0,
  bg = NULL
) {
  stopifnot(
    is.matrix(colorspace),
    ncol(colorspace) == 3,
    max(colorspace) <= 1,
    min(colorspace) >= 0
  )

  validate_args(n)

  opts <- make_options(cvd, cvd_severity, bg)

  res <- qualpal_cpp_rgb(n, colorspace, opts)

  res$de_DIN99d <- stats::as.dist(res$de_DIN99d)

  res
}

#' @export
qualpal.data.frame <- function(
  n,
  colorspace,
  cvd = c("protan", "deutan", "tritan"),
  cvd_severity = 0,
  bg = NULL
) {
  mat <- data.matrix(colorspace)
  qualpal(
    n = n,
    colorspace = mat,
    cvd = cvd,
    cvd_severity = cvd_severity,
    bg = bg
  )
}

#' @export
qualpal.character <- function(
  n,
  colorspace = "pretty",
  cvd = c("protan", "deutan", "tritan"),
  cvd_severity = 0,
  bg = NULL
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

  opts <- make_options(cvd, cvd_severity, bg)

  res <- qualpal_cpp_palette(n, colorspace, opts)

  res$de_DIN99d <- stats::as.dist(res$de_DIN99d)

  res
}


#' @export
qualpal.list <- function(
  n,
  colorspace = list(h = c(0, 360), s = c(0.2, 0.5), l = c(0.6, 0.85)),
  cvd = c("protan", "deutan", "tritan"),
  cvd_severity = 0,
  bg = NULL
) {
  stopifnot(
    !is.null(attr(colorspace, "names")),
    "h" %in% names(colorspace),
    "s" %in% names(colorspace),
    "l" %in% names(colorspace)
  )
  validate_args(n)

  h <- colorspace[["h"]]
  s <- colorspace[["s"]]
  l <- colorspace[["l"]]

  n_points <- 1000

  stopifnot(
    diff(range(h)) <= 360,
    min(h) >= -360,
    max(h) <= 360,
    min(s) >= 0,
    max(s) <= 1,
    min(l) >= 0,
    max(l) <= 1,
    length(h) == 2,
    length(s) == 2,
    length(l) == 2,
    is.numeric(h),
    is.numeric(s),
    is.numeric(l)
  )

  opts <- make_options(cvd, cvd_severity, bg)

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

make_options <- function(
  cvd = c("protan", "deutan", "tritan"),
  cvd_severity = 0,
  bg = NULL
) {
  cvd <- match.arg(cvd)

  stopifnot(
    is.character(cvd),
    length(cvd) == 1,
    is.numeric(cvd_severity),
    length(cvd_severity) == 1,
    cvd_severity >= 0,
    cvd_severity <= 1,
    is.null(bg) || (is.character(bg) && length(bg) == 1)
  )

  cvd_list <- list(protan = 0, deutan = 0, tritan = 0)
  cvd_list[[cvd]] <- cvd_severity

  if (!is.null(bg)) {
    bg <- col2rgb(bg)
    bg <- as.vector(bg) / 255
  } else {
    bg <- double(0)
  }
  list(cvd = cvd_list, bg = bg)
}

validate_args <- function(n) {
  stopifnot(
    is.numeric(n),
    n %% 1 == 0,
    n > 1
  )
}
