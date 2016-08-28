#' Generate qualitative color palettes
#'
#' Takes a subset of the hsl color space and generates a qualitative color
#' palette by optimizing color difference via the CIEDE 2000 Delta-E formula.
#'
#' \code{qualpal} takes a color subspace in the hsl color space, where lightness
#' and saturation take values from 0 to 1. Hue take values from -360 to 360,
#' although negative values are brought to lie in the range \{0, 360\} under the
#' hood. This behavior exists to enable color subspaces that span any hue range
#' (since the hue range is circular).
#'
#' The hsl color subspace is passed to a \emph{Nelder-Mead optimizer}:
#' (\code{dfoptim::nmkb}), which attempts to maximize the smallest pairwise
#' color difference among all \code{n} colors using the Delta-E CIEDE2000 color
#' difference algorithm. However, the optimizer gets easily stuck in local
#' minimi (since the optimization function is far from smooth) so multiple
#' restarts can be useful, particularly for high values of \code{n}.
#'
#' @section Predefined color spaces: Instead of specifying a color space
#'   manually, the following predefined color spaces can by accessed by
#'   specifying their name as a character vector to \code{colorspace}.
#'   \describe{ \item{"colorblind"}{Caters to red-green color blindness by
#'   simply avoiding most of the green hues. Hue ranges from -180 to 5,
#'   saturation from 0.2 to 0.7, and lightness from 0.2 to 0.9}
#'   \item{"rainbow"}{Uses the entire spectrum of colors. Provides distinct, but
#'   not always aesthetically pleasing colors.} \item{"pastels"}{Pastel colors
#'   from the complete range of hues (0-360), with saturation between 0.2 and
#'   0.4, and lightness between 0.8 and 0.9.}}
#'
#' @param n The number of colors to generate. Has to lie between 2 and 50.
#' @param colorspace Either 1) a list of three named numeric vectors: \code{h}
#'   (hue), \code{s} (saturation), and \code{l} (lightness), all of length 2
#'   specifying a min and max value for the range. The values has to be in the
#'   range -360 to 360 for \code{h}, and 0 to 1 for \code{s} and \code{l} 2), or
#'   2) a \emph{character vector} specifying one of the predefined color spaces
#'   (see below).
#' @param ... Control parameters to pass on to the Nelder-Mead optimizer (see
#'   \code{\link[dfoptim]{nmkb}}).
#' @return qualpal returns a list of class "qualpal" with the following
#'   components. \item{hsl}{A matrix of the colors in the hsl color space.}
#'   \item{Lab}{A matrix of the colors in the CIE Lab color space.} \item{RGB}{A
#'   matrix of the colors in the sRGB color space.} \item{hex}{A character
#'   vector of the colors in hex notation.} \item{dE_CIEDE2000}{A distance
#'   matrix of color differences according to the Delta-E CIEDE2000 formula.}
#'   \item{min_dE_CIEDE2000}{The smallest pairwise Delta-E CIEDE2000 (the
#'   maximization objective of the optimizer.}
#' @seealso \code{\link{plot.qualpal}}, \code{\link{pairs.qualpal}},
#'   \code{\link[dfoptim]{nmkb}}
#' @examples
#' # generates 3 distinct colors from the default color subspace
#' qualpal(3)
#' qualpal(n = 4, list(h = c(35, 360), s = c(.5, .7), l = c(0, .45)))
#'
#' # Trace the optimizer
#' qualpal(5, "colorblind", trace = TRUE)
#'
#' # Rigorous (and slower) evaluation using control parameters of nmkb
#' qualpal(5, "colorblind", maxfeval = 10000, tol = 1e-10)
#' \dontrun{
#' # The range of hue cannot exceed 360
#' qualpal(n = 4, list(h = c(-20, 360), s = c(.5, .7), l = c(0, .45)))
#' }
#' @export

qualpal <- function(n, colorspace = "colorblind", ...) {
  if (is.list(colorspace)) {
    if (!(all(c("h", "s", "l") %in% names(colorspace)))) {
      stop("You forgot to specify h, s, or l.")
    }
  } else if (is.character(colorspace)) {
    colorspace <- predefined_colorspaces(colorspace)
    if (is.null(colorspace)) {
      stop("The predefined color space does not exist.")
    }
  } else {
    stop("colorspace should be a list or a character vector specifying one of
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
    n > 1 & n < 51
  )

  suppressWarnings(
    fit <-
      dfoptim::nmkb(
        par = c(stats::runif(n, min(h), max(h)),
                stats::runif(n, min(s), max(s)),
                stats::runif(n, min(l), max(l))),
        fn = opt_dE,
        n = n,
        lower = c(rep(min(h), n), rep(min(s), n), rep(min(l), n)),
        upper = c(rep(max(h), n), rep(max(s), n), rep(max(l), n)),
        control = list(maximize = T, ...)
      )
  )

  hsl_cols <- matrix(fit$par, ncol = 3)
  hsl_cols[, 1] <- ifelse(hsl_cols[, 1] < 0, hsl_cols[, 1] + 360, hsl_cols[, 1])
  rgb_cols <- t(apply(hsl_cols, 1, hsl_rgb))
  lab_cols <- grDevices::convertColor(rgb_cols, from = "sRGB", to = "Lab")
  hex_cols <- grDevices::rgb(rgb_cols[, 1], rgb_cols[, 2], rgb_cols[, 3])

  dimnames(hsl_cols) <- list(hex_cols, c("h", "s", "l"))
  dimnames(lab_cols) <- list(hex_cols, c("L", "a", "b"))
  dimnames(rgb_cols) <- list(hex_cols, c("Red", "Green", "Blue"))

  col_diff <- measure_CIEDE2000(lab_cols)

  structure(
    list(
      hsl = hsl_cols,
      RGB = rgb_cols,
      Lab = lab_cols,
      hex = hex_cols,
      dE_CIEDE2000 = col_diff,
      min_dE_CIEDE2000 = min(col_diff)
    ),
    class = c("qualpal", "list")
  )
}

#' Multidimensional scaling map of qualitative color palette
#'
#' Uses the colors in a \code{qualpal} object to compute and plot a
#' multidimensional scaling (MDS) map using \code{\link[stats]{cmdscale}} on the
#' Delta-E distance matrix.
#'
#' @param x A list object of class \code{"qualpal"} generated from
#'   \code{\link{qualpal}}.
#' @param ... Arguments to pass on to \code{plot()}.
#' @seealso \code{\link{qualpal}}, \code{\link{pairs.qualpal}},
#'   \code{\link[graphics]{plot}}
#'
#' @examples
#' col_pal <- qualpal(n = 5)
#' plot(col_pal)
#' @export

plot.qualpal <- function(x, ...) {
  K <- ifelse(length(x$hex) < 3, 1, 2)
  graphics::plot(stats::cmdscale(x$dE_CIEDE2000, k = K),
                 main = "Multidimensional Scaling",
                 col = x$hex,
                 pch = 19,
                 cex = 2,
                 ylab = "Delta-E CIEDE2000",
                 xlab = "Delta-E CIEDE2000",
                 ...)
}

#' Scatter matrix plot of qualitative color palette
#'
#' Plots the colors in a \code{qualpal} object as a scatter matrix plot on
#' either the Lab (the default) or hsl color space.
#'
#' @param x A list object of class \code{"qualpal"} generated from
#'   \code{\link{qualpal}}.
#' @param colorspace The color space in which to plot the colors ("Lab" or
#'   "hsl").
#' @param ... Arguments to pass on to \code{\link[graphics]{pairs}}.
#' @seealso \code{\link{qualpal}}, \code{\link{plot.qualpal}},
#'   \code{\link[graphics]{pairs}}
#'
#' @examples
#' col_pal <- qualpal(n = 5)
#' pairs(col_pal)
#' pairs(col_pal, colorspace = "hsl")
#' @export

pairs.qualpal <- function(x, colorspace = c("Lab", "hsl"), ...) {
  graphics::pairs(switch(match.arg(colorspace),
                         Lab = x$Lab,
                         hsl = x$hsl),
                  col = x$hex,
                  pch = 19,
                  cex = 2,
                  ...)
}

# Predefined colorspaces ----------------------------------------------------

predefined_colorspaces <- function(colorspace) {
  list(
    colorblind = list(h = c(-180, 55), s = c(0.2, 0.7), l = c(0.2, 0.9)),
    rainbow    = list(h = c(0, 360), s = c(0, 1), l = c(0, 1)),
    pastels    = list(h = c(0, 360), s = c(0.2, 0.4), l = c(0.8, 0.9))
  )[[colorspace]]
}
