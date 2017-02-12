#' Multidimensional scaling map of qualitative color palette
#'
#' Uses the colors in a \link{qualpal} object to compute and plot a
#' multidimensional scaling (MDS) map using \code{\link[stats]{cmdscale}} on the
#' Delta E DIN99d distance matrix.
#'
#' @param x An object of class \code{"qualpal"} generated from
#'   \code{\link{qualpal}}.
#' @param ... Arguments to pass on to \code{\link[graphics]{plot}}.
#' @seealso \code{\link{qualpal}}, \code{\link{pairs.qualpal}},
#'   \code{\link[graphics]{plot}}
#'
#' @examples
#' col_pal <- qualpal(3)
#' plot(col_pal)
#' @export

plot.qualpal <- function(x, ...) {
  args <- list(
    x = stats::cmdscale(x[["de_DIN99d"]],
                        k = ifelse(length(x[["hex"]]) == 2, 1, 2)),
    col = x[["hex"]],
    ...
  )

  if (is.null(args[["cex"]])) args[["cex"]] <- 3
  if (is.null(args[["pch"]])) args[["pch"]] <- 19
  if (is.null(args[["xlab"]])) args[["xlab"]] <- "dE DIN99d"
  if (is.null(args[["ylab"]])) args[["ylab"]] <- "dE DIN99d"

  do.call(graphics::plot, args)
}

#' Scatterplot matrix of qualitative color palette
#'
#' Plots the colors in a \code{qualpal} object as a scatterplot matrix on
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
    x = switch(match.arg(colorspace), DIN99d = x[["DIN99d"]], HSL = x[["HSL"]]),
    col = x[["hex"]],
    ...
  )
  if (is.null(args[["cex"]])) args[["cex"]] <- 3
  if (is.null(args[["pch"]])) args[["pch"]] <- 19

  do.call(graphics::pairs, args)
}