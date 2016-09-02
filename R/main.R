#' Generate qualitative color palettes
#'
#' Takes a subset of the HSL color space, projects it to the DIN99d color space,
#' and generates a color palette from the most distinct colors.
#'
#' \code{qualpal} takes a color subspace in the HSL color space, where lightness
#' and saturation take values from 0 to 1. Hue take values from -360 to 360,
#' although negative values are brought to lie in the range \{0, 360\} under the
#' hood. This behavior exists to enable color subspaces that span any hue range
#' (since the hue range is circular).
#'
#' The HSL color subspace that the user provides is projected into the DIN99d
#' color space, which is approximately perceptually uniform, i.e. color
#' difference is proportional to the euclidean distance between colors. A
#' distance matrix is computed and transformed using power transformations
#' discovered by Huang 2015 in order to fine tune differences.
#'
#' `qualpal` then greedily searches this distance matrix for the most distinct
#' colors; it does this iteratively by first selecting the two most distinct
#' colors, then finding the third that has the smallest pairwise distance to the
#' previously selected points, repeating this until `n` colors have been
#' collected.
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
#'   \item{\code{rainbow}}{Uses all hues, chromas, and most of the lightness
#'   range. Provides distinct, but not asethetically pleasing colors.}
#'   \item{\code{pastels}}{Pastel colors from the complete range of hues
#'   (0-360), with saturation between 0.2 and 0.4, and lightness between 0.8 and
#'   0.9.}
#'   }
#'
#' @param n The number of colors to generate.
#' @param colorspace Either 1) a list of three named numeric vectors: \code{h}
#'   (hue), \code{s} (saturation), and \code{l} (lightness), all of length 2
#'   specifying a min and max value for the range. The values has to be in the
#'   range -360 to 360 for \code{h}, and 0 to 1 for \code{s} and \code{l} 2), or
#'   2) a \emph{character vector} specifying one of the predefined color spaces
#'   (see below).
#' @return qualpal returns a list of class "qualpal" with the following
#'   components. \item{HSL}{A matrix of the colors in the HSL color space.}
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

qualpal <- function(n,
                    colorspace = "pretty",
                    colorblind = c("normal", "deutan", "protan", "tritan"),
                    ...) {
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

  if (length(list(...) > 0)) warning("... is deprecated since the optimizer is no
                                   longer used.")

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
    n > 1
  )

  HSL <- expand.grid(seq(min(h), max(h), length.out = 13),
                     seq(min(s), max(s), length.out = 13),
                     seq(min(l), max(l), length.out = 13))

  HSL[HSL[, 1] < 0, 1] <- HSL[HSL[, 1] < 0, 1] + 360
  RGB <- HSL_RGB(HSL)

  if (match.arg(colorblind) != "normal") {
    LMS <- RGB_LMS(RGB)
    LMS <- switch(
      match.arg(colorblind),
      deutan = do.call(LMS_deutan, args = list(LMS)),
      protan = do.call(LMS_protan, args = list(LMS)),
      tritan = do.call(LMS_tritan, args = list(LMS))
    )
    RGB_div <- LMS_RGB(LMS)
    RGB_err <- RGB - RGB_div
    RGB <- RGB + daltonize(RGB_err)
    RGB <- RGB
    RGB[RGB > 1] <- 1
    RGB[RGB < 0] <- 0
  }

  XYZ <- convertColor(RGB, from = "sRGB", to = "XYZ")

  # Apply correction to xyz in preparation of converting to DIN99d
  XYZ_mod <- XYZ
  XYZ_mod[, 1] <- 1.12 * XYZ[, 1] - 0.12 * XYZ[, 3]
  Lab_mod <- convertColor(XYZ_mod, from = "XYZ", to = "Lab")
  DIN99d <- Lab_DIN99d(Lab_mod)

  DIN99d_dist <- as.matrix(stats::dist(DIN99d))
  # Apply power transformations from Huang 2015
  DIN99d_dist <- 1.28 * (DIN99d_dist ^ 0.74)

  # Start by finding the two most distant points
  col_ind <-
    as.vector(arrayInd(which.max(DIN99d_dist), .dim = dim(DIN99d_dist)))

  # Iterate over all distances, each time finding the point most distant to
  # the already selected points
  while (length(col_ind) < n) {
    new_col <- which.max(apply(DIN99d_dist[, col_ind], 1, min))
    col_ind <- c(col_ind, new_col)
  }

  RGB    <- convertColor(XYZ[col_ind, ], from = "XYZ", to = "sRGB")
  HSL    <- RGB_HSL(RGB)
  DIN99d <- DIN99d[col_ind, ]
  hex    <- grDevices::rgb(RGB[, 1], RGB[, 2], RGB[, 3])

  dimnames(HSL)    <- list(hex, c("Hue", "Saturation", "Lightness"))
  dimnames(DIN99d) <- list(hex, c("L(99d)", "a(99d)", "b(99d)"))
  dimnames(RGB)    <- list(hex, c("Red", "Green", "Blue"))

  col_diff <- stats::dist(DIN99d)
  col_diff <- 1.28 * (col_diff ^ 0.74)

  structure(
    list(
      RGB = HSL,
      RGB = RGB,
      DIN99d = DIN99d,
      hex = hex,
      de_DIN99d = col_diff,
      min_de_DIN99d = min(col_diff)
    ),
    class = c("qualpal", "list")
  )
}

#' Multidimensional scaling map of qualitative color palette
#'
#' Uses the colors in a \code{qualpal} object to compute and plot a
#' multidimensional scaling (MDS) map using \code{\link[stats]{cmdscale}} on the
#' Delta E DIN99d distance matrix.
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
#' either the Lab (the default) or HSL color space.
#'
#' @param x A list object of class \code{"qualpal"} generated from
#'   \code{\link{qualpal}}.
#' @param colorspace The color space in which to plot the colors ("Lab" or
#'   "HSL").
#' @param ... Arguments to pass on to \code{\link[graphics]{pairs}}.
#' @seealso \code{\link{qualpal}}, \code{\link{plot.qualpal}},
#'   \code{\link[graphics]{pairs}}
#'
#' @examples
#' col_pal <- qualpal(n = 3)
#' pairs(col_pal)
#' pairs(col_pal, colorspace = "HSL")
#' @export

pairs.qualpal <- function(x, colorspace = c("DIN99d", "HSL"), ...) {
  stopifnot(inherits(x, "qualpal"))
  args <- list(
    x = switch(match.arg(colorspace),
               DIN99d = x$DIN99d,
               HSL = x$HSL),
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
    rainbow     = list(h = c(0, 360), s = c(0, 1), l = c(0, .7)),
    pastels     = list(h = c(0, 360), s = c(0.2, 0.4), l = c(0.8, 0.9))
  )[[colorspace]]
}
