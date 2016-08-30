#' Generate qualitative color palettes
#'
#' Takes a subset of the hsl color space and generates a qualitative color
#' palette by optimizing color difference via the CIEDE 2000 formula.
#'
#' \code{qualpal} takes a color subspace in the hsl color space, where lightness
#' and saturation take values from 0 to 1. Hue take values from -360 to 360,
#' although negative values are brought to lie in the range \{0, 360\} under the
#' hood. This behavior exists to enable color subspaces that span any hue range
#' (since the hue range is circular).
#'
#' The hsl color subspace is passed to a \emph{Nelder-Mead optimizer}:
#' (\code{dfoptim::nmkb}), which attempts to maximize the smallest pairwise
#' color difference among all \code{n} colors using the CIEDE 2000 color
#' difference algorithm. However, the optimizer gets easily stuck in local
#' minimi (since the optimization function is far from smooth) so multiple
#' restarts can be useful, particularly for high values of \code{n}.
#'
#' @section Predefined color spaces: Instead of specifying a color space
#'   manually, the following predefined color spaces can by accessed by
#'   specifying their name as a character vector to \code{colorspace}.
#'   \describe{
#'   \item{\code{pretty}}{Tries to provide aesthetically pleasing, but still
#'   distinct color palettes. Hue ranges from 0 to 360, saturation from 0.1 to
#'   0.5, and lightness from 0.5 to 0.85. Does not handle large numbers of
#'   colors well.}
#'   \item{\code{pretty_dark}}{Like \code{pretty} but darker. Hue ranges from 0
#'   to 360, saturation from 0.1 to 0.5, and lightness from 0.2 to 0.4.}
#'   \item{\code{colorblind}}{Caters to red-green color blindness by
#'   simply avoiding most of the green hues. Hue ranges from -180 to 55,
#'   saturation from 0.1 to 0.6, and lightness from 0.2 to 0.85.}
#'   \item{\code{rainbow}}{Uses all hues, chromas, and most of the lightness
#'   range. Provides distinct, but not asethetically pleasing colors.}
#'   \item{\code{pastels}}{Pastel colors from the complete range of hues
#'   (0-360), with saturation between 0.2 and 0.4, and lightness between 0.8 and
#'   0.9.}
#'   }
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
#'   vector of the colors in hex notation.} \item{ciede2000}{A distance
#'   matrix of color differences according to the CIEDE 2000 formula.}
#'   \item{min_ciede2000}{The smallest pairwise CIEDE 2000 (the
#'   maximization objective of the optimizer.}
#' @seealso \code{\link{plot.qualpal}}, \code{\link{pairs.qualpal}},
#'   \code{\link[dfoptim]{nmkb}}
#' @examples
#' # generates 3 distinct colors from the default color subspace
#' qualpal(3)
#' qualpal(n = 3, list(h = c(35, 360), s = c(.5, .7), l = c(0, .45)))
#'
#' # Trace the optimizer
#' qualpal(3, "colorblind", trace = TRUE)
#'
#' # Rigorous (and slow) evaluation using control parameters of nmkb
#' \dontrun{
#' qualpal(6, "rainbow", maxfeval = 10000, tol = 1e-10)
#' }
#' \dontrun{
#' # The range of hue cannot exceed 360
#' qualpal(n = 3, list(h = c(-20, 360), s = c(.5, .7), l = c(0, .45)))
#' }
#' @export

qualpal <- function(n, colorspace = "pretty", ...) {
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

  col_diff <- measure_ciede2000(lab_cols)

  structure(
    list(
      hsl = hsl_cols,
      RGB = rgb_cols,
      Lab = lab_cols,
      hex = hex_cols,
      ciede2000 = col_diff,
      min_ciede2000 = min(col_diff)
    ),
    class = c("qualpal", "list")
  )
}

#' Multidimensional scaling map of qualitative color palette
#'
#' Uses the colors in a \code{qualpal} object to compute and plot a
#' multidimensional scaling (MDS) map using \code{\link[stats]{cmdscale}} on the
#' CIEDE2000 distance matrix.
#'
#' @param x A list object of class \code{"qualpal"} generated from
#'   \code{\link{qualpal}}.
#' @param ... Arguments to pass on to \code{plot()}.
#' @seealso \code{\link{qualpal}}, \code{\link{pairs.qualpal}},
#'   \code{\link[graphics]{plot}}
#'
#' @examples
#' col_pal <- qualpal(n = 3)
#' plot(col_pal)
#' @export

plot.qualpal <- function(x, ...) {
  stopifnot(inherits(x, "qualpal"))
  args <- list(
    x = stats::cmdscale(x$ciede2000, k = ifelse(length(x$hex) == 2, 1, 2)),
    col = x$hex,
    ylab = "CIEDE2000",
    xlab = "CIEDE2000",
    ...
    )
  if (is.null(args[["cex"]])) args[["cex"]] <- 2
  if (is.null(args[["pch"]])) args[["pch"]] <- 19

  do.call(graphics::plot, args)
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
#' col_pal <- qualpal(n = 3)
#' pairs(col_pal)
#' pairs(col_pal, colorspace = "hsl")
#' @export

pairs.qualpal <- function(x, colorspace = c("Lab", "hsl"), ...) {
  stopifnot(inherits(x, "qualpal"))
  args <- list(
    x = switch(match.arg(colorspace), Lab = x$Lab, hsl = x$hsl),
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
    pretty      = list(h = c(0, 360), s = c(0.1, 0.5), l = c(0.6, 0.85)),
    pretty_dark = list(h = c(0, 360), s = c(0.1, 0.5), l = c(0.2, 0.4)),
    colorblind  = list(h = c(-180, 55), s = c(0.1, 0.6), l = c(0.2, 0.85)),
    rainbow     = list(h = c(0, 360), s = c(0, 1), l = c(0, .7)),
    pastels     = list(h = c(0, 360), s = c(0.2, 0.4), l = c(0.8, 0.9))
  )[[colorspace]]
}
