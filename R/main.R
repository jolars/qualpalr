#' Generate qualitative color palettes
#'
#' Takes a subset of the hsl color space and generates a qualitative color
#' palette by optimizing color difference via the CIEDE 2000 Delta-E formula.
#'
#' \code{qualpal} takes a color subspace in the hsl color space, where lightness
#' and saturation take values from 0 to 1. Hue takes values from -360 to 360,
#' although negative values are brought to lie in the range \{0, 360\} under the
#' hood. This behavior exists to enable color subspaces that span any hue range,
#' since the hue range is circular.
#'
#' The hsl color subspace is passed to a \emph{Nelder-Mead optimizer}:
#' (\code{dfoptim::nmkb}), which attempts to maximize the smallest pairwise
#' color difference among all \code{n} colors using the Delta-E CIEDE2000 color
#' difference algorithm. However, the optimizer gets easily stuck in local
#' minimi (since the optimization function is far from smooth) so multiple
#' restarts can be useful, particularly for high values of \code{n}.
#'
#' @section Predfined color spaces: Instead of specifying a color space
#'   manually, the following predefined color spaces can by accessed by
#'   specifying their name as a character vector to \code{colorspace}.
#'   \describe{ \item{"colorblind"}{Caters to red-green color blindness by
#'   simply avoiding most of the green hues. Hue ranges from -160 to 60,
#'   saturation from 0.4 to 0.7, and lightness from 0.2 to 0.85}
#'   \item{"rainbow"}{Uses the entire spectrum of colors. Provides distinct, but
#'   not always aesthetically pleasing colors.} \item{"pastels"}{Pastel colors
#'   from the complete range of hues (0-360), with saturation between 0.2 and
#'   0.4, and lightness between 0.8 and 0.9.}}
#'
#' @param n The number of colors to generate.
#' @param colorspace Either 1) a \emph{list} of three named numeric vectors:
#'   \code{h} (hue), \code{s} (saturation), and \code{l} (lightness), all of
#'   length 2 specifying a min and max value for the range. The values has to be
#'   in the range -360 to 360 for \code{h}, and 0 to 1 for \code{s} and \code{l}
#'   2), or 2) a \emph{character vector} specifying one of the predefined color
#'   spaces (see below).
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
#' @seealso \code{\link{plot.qualpal}}, \code{\link[dfoptim]{nmkb}}
#' @examples
#' qualpal(3) # generates 3 distinct colors from the default color subspace
#' qualpal(n = 4, list(h = c(35, 360), s = c(.5, .7), l = c(0, .45)))
#' @export

qualpal <- function(n, colorspace = "colorblind", ...) {
  if (is.list(colorspace) & all(c("h", "s", "l") %in% names(colorspace))) {
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

  stopifnot(
    diff(range(h)) <= 360,
    min(h) >= - 360 & max(h) <= 360,
    min(s) >= 0 & max(s) <= 1,
    min(l) >= 0 & max(l) <= 1,
    length(h) == 2,
    length(s) == 2,
    length(l) == 2,
    is.numeric(h),
    is.numeric(s),
    is.numeric(l)
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

#' Demos and diagnostic plots of qualitative color palettes
#'
#' Uses the colors from a \code{qualpal} object to plot \enumerate{\item a
#' grouped scatter plot (\code{kmeans} on \code{mtcars}), \item a heatmap of the
#' correlation matrix given by the the Delta-E CIEDE2000 difference computations
#' (darker greys mean more correlated), \item a multidimensional scaling (MDS)
#' map computed by \code{cmdscale} from the Delta-E distance matrix, and \item a
#' \code{pairs} plot of the hsl color space dimensions. }
#' @param x A list object of class \code{"qualpal"} generated from
#'   \code{\link{qualpal}}.
#' @param ... Does nothing.
#'
#' @examples
#' col_pal <- qualpal(n = 8)
#' plot(col_pal)
#' @export

plot.qualpal <- function(x, ...) {
  # kmeans plot
  clus <- stats::kmeans(datasets::mtcars[, c("mpg", "disp")],
                        length(x$hex))
  with(datasets::mtcars, plot(x = mpg, y = disp,
                              col = x$hex[clus$cluster],
                              pch = 19, main = "K-means clustering"))
  graphics::legend("topright", legend = rownames(clus$centers),
                   col = x$hex[seq_along(clus$centers)], pch = 19)

  # Correlation map of colors
  stats::heatmap(as.matrix(x$dE_CIEDE2000),
                 col = grDevices::gray(0:8 / 8))

  # Multidimensional scaling
  K <- ifelse(length(x$hex) < 3, 1, 2)
  graphics::plot(stats::cmdscale(x$dE_CIEDE2000, k = K),
                 main = "Multidimensional Scaling",
                 col = x$hex, pch = 19, cex = 2, ylab = "Delta-E CIEDE2000",
                 xlab = "Delta-E CIEDE2000")

  # hsl pairs plot
  graphics::pairs(x$hsl, pch = 19, cex = 2, col = x$hex)
}

# Predefined colorspaces ----------------------------------------------------

predefined_colorspaces <- function(colorspace) {
  list(
    colorblind = list(h = c(-150, 60), s = c(0.4, 0.7), l = c(0.2, 0.9)),
    rainbow    = list(h = c(0, 360), s = c(0, 1), l = c(0, 1)),
    pastels    = list(h = c(0, 360), s = c(0.2, 0.4), l = c(0.8, 0.9))
  )[[colorspace]]
}
