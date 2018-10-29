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
#'     \item{A \code{\link{list}} with the following \emph{named} vectors,
#'       each of length two, giving a range for each item.}{
#'       \describe{
#'         \item{\code{h}}{Hue, in the range [-360, 360]}
#'         \item{\code{s}}{Saturation, in the range [0, 1]}
#'         \item{\code{l}}{Lightness, in the range [0, 1]}
#'       }
#'     }
#'     \item{A \code{\link{character}} vector of length one specifying one of
#'       these predefined color spaces:}{
#'       \describe{
#'         \item{\code{pretty}}{
#'           Tries to provide aesthetically pleasing,
#'           but still distinct color palettes. Hue ranges from 0 to 360,
#'           saturation from 0.1 to 0.5, and lightness from 0.5 to 0.85. This
#'           palette is not suitable for high \code{n}}
#'         \item{\code{pretty_dark}}{
#'           Like \code{pretty} but darker. Hue ranges from 0 to 360, saturation
#'           from 0.1 to 0.5, and lightness from 0.2 to 0.4.
#'         }
#'         \item{\code{rainbow}}{
#'           Uses all hues, chromas, and most of the lightness range. Provides
#'           distinct but not aesthetically pleasing colors.
#'         }
#'         \item{\code{pastels}}{
#'           Pastel colors from the complete range of hues (0-360), with
#'           saturation between 0.2 and 0.4, and lightness between 0.8 and 0.9.
#'         }
#'       }
#'     }
#'     \item{A \code{\link{matrix}} of colors from the sRGB color space, each
#'       row representing a unique color.}
#'     \item{A \code{\link{data.frame}} that can be converted to a matrix via
#'       \link{data.matrix}}
#'   }
#'
#' @param cvd Color vision deficiency adaptation. Use \code{cvd_severity}
#'   to set the severity of color vision deficiency to adapt to. Permissible
#'   values are \code{"protan", "deutan",} and \code{"tritan"}.
#' @param cvd_severity Severity of color vision deficiency to adapt to. Can take
#'   any value from 0, for normal vision (the default), and 1, for dichromatic
#'   vision.
#' @param n_threads The number of threads to use, provided to
#' \link[RcppParallel]{setThreadOptions} if non-null.
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
#' qualpal(3, "pretty")
#'
#' # Adapt palette to deuteranopia
#' qualpal(5, colorspace = "pretty_dark", cvd = "deutan", cvd_severity = 1)
#'
#' # Adapt palette to protanomaly with severity 0.4
#' qualpal(8, colorspace = "pretty_dark", cvd = "protan", cvd_severity = 0.4)
#'
#' \dontrun{
#' # The range of hue cannot exceed 360
#' qualpal(3, list(h = c(-20, 360), s = c(0.5, 0.7), l = c(0, 0.45)))
#' }
#'
#' @export
qualpal <- function(n,
                    colorspace = "pretty",
                    cvd = c("protan", "deutan", "tritan"),
                    cvd_severity = 0,
                    n_threads = NULL) {
  UseMethod("qualpal", colorspace)
}

#' @export
qualpal.matrix <- function(n,
                           colorspace,
                           cvd = c("protan", "deutan", "tritan"),
                           cvd_severity = 0,
                           n_threads = NULL) {
  assertthat::assert_that(
    assertthat::is.count(n),
    is.character(cvd),
    assertthat::is.number(cvd_severity),
    is.matrix(colorspace),
    max(colorspace) <= 1,
    min(colorspace) >= 0,
    n < 100,
    n > 1,
    cvd_severity >= 0,
    cvd_severity <= 1,
    ncol(colorspace) == 3
  )

  if (!is.null(n_threads)) {
    RcppParallel::setThreadOptions(numThreads = n_threads)
    on.exit(RcppParallel::defaultNumThreads())
  }

  RGB <- colorspace
  HSL <- RGB_HSL(RGB)

  # Simulate color deficiency if required
  if (cvd_severity > 0) {
    RGB <- sRGB_CVD(RGB, cvd = match.arg(cvd), cvd_severity = cvd_severity)
  }

  XYZ    <- sRGB_XYZ(RGB)
  DIN99d <- XYZ_DIN99d(XYZ)

  col_ind <- farthest_points(DIN99d, n)

  RGB    <- RGB[col_ind, ]
  HSL    <- HSL[col_ind, ]
  DIN99d <- DIN99d[col_ind, ]
  hex    <- grDevices::rgb(RGB)

  dimnames(HSL)    <- list(hex, c("Hue", "Saturation", "Lightness"))
  dimnames(DIN99d) <- list(hex, c("L(99d)", "a(99d)", "b(99d)"))
  dimnames(RGB)    <- list(hex, c("Red", "Green", "Blue"))

  col_diff           <- edist(DIN99d)
  dimnames(col_diff) <- list(hex, hex)
  de_DIN99d <- stats::as.dist(col_diff)

  structure(
    list(
      HSL           = HSL,
      RGB           = RGB,
      DIN99d        = DIN99d,
      hex           = hex,
      de_DIN99d     = de_DIN99d,
      min_de_DIN99d = min(de_DIN99d)
    ),
    class = c("qualpal", "list")
  )
}

#' @export
qualpal.data.frame <- function(n, colorspace,
                               cvd = c("protan", "deutan", "tritan"),
                               cvd_severity = 0,
                               n_threads = NULL) {
  mat <- data.matrix(colorspace)
  qualpal(n = n, colorspace = mat, cvd = cvd, cvd_severity = cvd_severity)
}

#' @export
qualpal.character <- function(n, colorspace = "pretty",
                              cvd = c("protan", "deutan", "tritan"),
                              cvd_severity = 0,
                              n_threads = NULL) {
  assertthat::assert_that(
    assertthat::is.string(colorspace)
  )
  colorspace <- predefined_colorspaces(colorspace)
  qualpal(n = n, colorspace = colorspace, cvd = cvd,
          cvd_severity = cvd_severity)
}


#' @export
qualpal.list <- function(n, colorspace,
                         cvd = c("protan", "deutan", "tritan"),
                         cvd_severity = 0,
                         n_threads = NULL) {
  assertthat::assert_that(
    assertthat::has_attr(colorspace, "names"),
    "h" %in% names(colorspace),
    "s" %in% names(colorspace),
    "l" %in% names(colorspace)
  )

  h <- colorspace[["h"]]
  s <- colorspace[["s"]]
  l <- colorspace[["l"]]

  assertthat::assert_that(
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

  rnd <- randtoolbox::torus(1000, dim = 3)

  H <- scale_runif(rnd[, 1], min(h), max(h))
  S <- scale_runif(sqrt(rnd[, 2]), min(s), max(s))
  L <- scale_runif(rnd[, 3], min(l), max(l))

  HSL <- cbind(H, S, L)

  HSL[HSL[, 1] < 0, 1] <- HSL[HSL[, 1] < 0, 1] + 360
  RGB <- HSL_RGB(HSL)

  qualpal(n = n, colorspace = RGB, cvd = cvd, cvd_severity = cvd_severity)
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
print.qualpal <- function(x,
                          colorspace = c("HSL", "DIN99d", "RGB"),
                          digits = 2,
                          ...) {
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
    pretty      = list(h = c(0, 360), s = c(0.2, 0.5), l = c(0.6, 0.85)),
    pretty_dark = list(h = c(0, 360), s = c(0.1, 0.5), l = c(0.2, 0.4)),
    rainbow     = list(h = c(0, 360), s = c(0,   1),   l = c(0,   1)),
    pastels     = list(h = c(0, 360), s = c(0.2, 0.4), l = c(0.8, 0.9))
  )

  assertthat::assert_that(colorspace %in% names(spaces))

  spaces[[colorspace]]
}


