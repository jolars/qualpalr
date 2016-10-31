#' Generate qualitative color palettes
#'
#' Takes a subset of the HSL color space, projects it to the DIN99d color space,
#' and generates a color palette from the most distinct colors.
#'
#' \code{qualpal} takes a color subspace in the HSL color space, where lightness
#' and saturation take values from 0 to 1. Hue take values from -360 to 360,
#' although negative values are brought to lie in the range \{0, 360\}; this
#' behavior exists to enable color subspaces that span all hues being tha the
#' hue space is circular.
#'
#' The HSL color subspace that the user provides is projected into the DIN99d
#' color space, which is approximately perceptually uniform, i.e. color
#' difference is proportional to the euclidean distance between two colors. A
#' distance matrix is computed and, as an additional step, is transformed using
#' power transformations discovered by Huang 2015 in order to fine tune
#' differences.
#'
#' \code{qualpal} then greedily searches the distance matrix for the most
#' distinct colors; it does this iteratively by first selecting the two most
#' distinct colors, then finding the third that has the smallest pairwise
#' distance to the previously selected points, repeating this until `n` colors
#' have been collected.
#'
#' Optionally, \code{qualpal} can adapt palettes to cater to color blindness, or
#' more specifically dichromacy. This is accomplished by taking the colors
#' provided by the user and transforming them to colors that a protan or deutan
#' would see, that is, simulating dichromacy. qualpal then chooses colors from
#' these new colors.
#'
#' \code{qualpal} currently only supports the sRGB color space with the D65
#' white point reference.
#'
#' @section Predefined color spaces: Instead of specifying a color space
#'   manually, the following predefined color spaces can by accessed by
#'   specifying their name as a character vector to \code{colorspace}.
#'   \describe{ \item{\code{pretty}}{Tries to provide aesthetically pleasing,
#'   but still distinct color palettes. Hue ranges from 0 to 360, saturation
#'   from 0.1 to 0.5, and lightness from 0.5 to 0.85. Is not suitable for large
#'   numbers of \code{n}} \item{\code{pretty_dark}}{Like \code{pretty} but
#'   darker. Hue ranges from 0 to 360, saturation from 0.1 to 0.5, and lightness
#'   from 0.2 to 0.4.} \item{\code{rainbow}}{Uses all hues, chromas, and most of
#'   the lightness range. Provides distinct but not aesthetically pleasing
#'   colors.} \item{\code{pastels}}{Pastel colors from the complete range of
#'   hues (0-360), with saturation between 0.2 and 0.4, and lightness between
#'   0.8 and 0.9.} }
#'
#' @param n The number of colors to generate.
#' @param colorspace Either 1) a list of three named numeric vectors: \code{h}
#'   (hue), \code{s} (saturation), and \code{l} (lightness), all of length 2
#'   specifying a min and max value for the range. The values has to be in the
#'   range -360 to 360 for \code{h}, and 0 to 1 for \code{s} and \code{l} 2), or
#'   2) a \emph{character vector} specifying one of the predefined color spaces
#'   (see below).
#' @param colorblind Set to \code{"protan"} or \code{"deutan"} to adapt the
#'   color palette to protanopia or deuteranopia respectively. The default is no
#'   adapation (\code{"normal"}).
#' @return qualpal returns a list of class "qualpal" with the following
#'   components. \item{HSL}{A matrix of the colors in the HSL color space.}
#'   \item{DIN99d}{A matrix of the colors in the DIN99d color space.}
#'   \item{RGB}{A matrix of the colors in the sRGB color space.} \item{hex}{A
#'   character vector of the colors in hex notation.} \item{de_DIN99d}{A
#'   distance matrix of color differenes according to delta E DIN99d.}
#'   \item{min_de_DIN99d}{The smallest pairwise delta E DIN99d.}
#' @seealso \code{\link{plot.qualpal}}, \code{\link{pairs.qualpal}}
#' @examples
#' # generate 3 distinct colors from the default color subspace
#' qualpal(3)
#' qualpal(n = 3, list(h = c(35, 360), s = c(.5, .7), l = c(0, .45)))
#'
#' qualpal(3, "pretty")
#'
#' # Adapt color palette to deuteranopia
#' qualpal(5, colorspace = "pretty_dark", colorblind = "deutan")
#'
#' \dontrun{
#' # The range of hue cannot exceed 360
#' qualpal(n = 3, list(h = c(-20, 360), s = c(.5, .7), l = c(0, .45)))
#' }
#' @export

qualpal <- function(n, colorspace = "pretty",
                    colorblind = c("normal", "protan", "deutan")) {
  if (inherits(colorspace, "list")) {
    if (!(all(c("h", "s", "l") %in% names(colorspace)))) {
      stop("You forgot to specify h, s, or l.")
    }
  } else if (is.character(colorspace)) {
    colorspace <- predefined_colorspaces(colorspace)
    if (is.null(colorspace)) {
      stop("The predefined color space does not exist.")
    }
  } else {
    stop("colorspace must be a list or a character vector specifying one of
         the color space templates.")
  }

  h <- colorspace[["h"]]
  s <- colorspace[["s"]]
  l <- colorspace[["l"]]

  if((!n %% 1 == 0)) stop("n must be an integer.")

  stopifnot(
    diff(range(h)) <= 360,
    min(h) >= - 360 & max(h) <= 360,
    min(s) >= 0 & max(s) <= 1,
    min(l) >= 0 & max(l) <= 1,
    length(h) == 2,
    length(s) == 2,
    length(l) == 2,
    length(n) == 1,
    is.numeric(h),
    is.numeric(s),
    is.numeric(l),
    is.numeric(n),
    n > 1,
    n < 10 ^ 3
  )

  rnd <- randtoolbox::sobol(1000, dim = 3, scrambling = 3)

  H <- scale_runif(rnd[, 1], min(h), max(h))
  S <- scale_runif(sqrt(rnd[, 2]), min(s), max(s))
  L <- scale_runif(rnd[, 3], min(l), max(l))

  HSL <- cbind(H, S, L)

  HSL[HSL[, 1] < 0, 1] <- HSL[HSL[, 1] < 0, 1] + 360
  RGB <- HSL_RGB(HSL)

  if (match.arg(colorblind) != "normal") {
    RGB2 <- RGB ^ 2.2
    LMS <- switch(
      match.arg(colorblind),
      protan = do.call(LMS_protan,
                       args = list(RGB_LMS(RGB2 * 0.992052 + 0.003974))),
      deutan = do.call(LMS_deutan,
                       args = list(RGB_LMS(RGB2 * 0.957237 + 0.0213814)))
    )
    RGB <- LMS_RGB(LMS) ^ (1 / 2.2)
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

  col_diff <- stats::dist(DIN99d)
  col_diff <- 1.28 * (col_diff ^ 0.74)

  structure(
    list(
      HSL           = round(HSL, 2),
      RGB           = round(RGB, 2),
      DIN99d        = round(DIN99d, 1),
      hex           = hex,
      de_DIN99d     = round(col_diff, 0),
      min_de_DIN99d = min(col_diff)
    ),
    class = c("qualpal", "list")
  )
}

#' Multidimensional scaling map of qualitative color palette
#'
#' Uses the colors in a \code{qualpalr} object to compute and plot a
#' multidimensional scaling (MDS) map using \code{\link[stats]{cmdscale}} on the
#' Delta E DIN99d distance matrix.
#'
#' @param x A list object of class \code{"qualpal"} generated from
#'   \code{\link{qualpal}}.
#' @param ... Arguments to pass on to \code{\link[graphics]{plot}}.
#' @seealso \code{\link{qualpal}}, \code{\link{pairs.qualpal}},
#'   \code{\link[graphics]{plot}}
#'
#' @examples
#' col_pal <- qualpal(n = 3)
#' plot(col_pal)
#' @export

plot.qualpal <- function(x, ...) {
  args <- list(
    x = stats::cmdscale(x$de_DIN99d, k = ifelse(length(x$hex) == 2, 1, 2)),
    col = x$hex,
    ylab = "dE DIN99d",
    xlab = "dE DIN99d",
    ...
    )

  if (is.null(args[["cex"]])) args[["cex"]] <- 2
  if (is.null(args[["pch"]])) args[["pch"]] <- 19

  do.call(graphics::plot, args)
}

#' Scatter matrix plot of qualitative color palette
#'
#' Plots the colors in a \code{qualpal} object as a scatter matrix plot on
#' either the DIN99d (the default) or HSL color space.
#'
#' @param x A list object of class \code{"qualpal"} generated from
#'   \code{\link{qualpal}}.
#' @param colorspace The color space in which to plot the colors ("DIN99d" or
#'   "HSL").
#' @param ... Arguments to pass on to \code{\link[graphics]{pairs}}.
#' @seealso
#'   \code{\link{qualpal}},
#'   \code{\link{plot.qualpal}},
#'   \code{\link[graphics]{pairs}}
#'
#' @examples
#' col_pal <- qualpal(n = 3)
#' pairs(col_pal)
#' pairs(col_pal, colorspace = "HSL")
#' @export

pairs.qualpal <- function(x, colorspace = c("DIN99d", "HSL"), ...) {
  args <- list(
    x   = switch(match.arg(colorspace), DIN99d = x$DIN99d, HSL = x$HSL),
    col = x$hex,
    ...
  )
  if (is.null(args[["cex"]])) args[["cex"]] <- 2
  if (is.null(args[["pch"]])) args[["pch"]] <- 19

  do.call(graphics::pairs, args)
}

# Predefined colorspaces ----------------------------------------------------

predefined_colorspaces <- function(colorspace) {
  list(
    pretty      = list(h = c(0, 360), s = c(0.2, 0.5), l = c(0.6, 0.85)),
    pretty_dark = list(h = c(0, 360), s = c(0.1, 0.5), l = c(0.2, 0.4)),
    rainbow     = list(h = c(0, 360), s = c(0,   1),   l = c(0,   1)),
    pastels     = list(h = c(0, 360), s = c(0.2, 0.4), l = c(0.8, 0.9))
  )[[colorspace]]
}

# Helper functions --------------------------------------------------------

scale_runif <- function(x, new_min, new_max) {
  (new_max - new_min) * (x - 1) + new_max
}
