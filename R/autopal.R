#' @title Optimize color palette for color vision deficiency
#'
#' **Deprecated**: This function is deprecated and will be removed in a future
#' release. There is no direct replacement.
#'
#' This function adapts color palettes to color vision deficiency (CVD) by
#' optimizing the CVD severity to try reach a target color difference (DIN99d
#' \eqn{\delta E}) of the user's choosing. Basically, it will choose a color
#' palette that is as close as possible to the target \eqn{\delta E} by
#' tweaking the CVD severity option in \code{\link{qualpal}}.
#'
#' The rationale for this function is that when there are few colors in a color
#' palette, there is no cost involved in adapting colors to CVD -- the colors
#' will still remain distinct. As more an more colors are added to the palette,
#' however, adapting the color palette to CVD will eventually lead to colors
#' that are too similar. This function gradually loosens the adaptation to CVDs
#' by lowering the severity of CVD to simulate to before picking colors (the
#' \code{cvd_severity} argument in \code{\link{qualpal}})
#'
#' @param n Number of colors to generate.
#' @param colorspace Either 1) a list of three named numeric vectors: \code{h}
#'   (hue), \code{s} (saturation), and \code{l} (lightness), all of length 2
#'   specifying a min and max value for the range. The values has to be in the
#'   range -360 to 360 for \code{h}, and 0 to 1 for \code{s} and \code{l} 2), or
#'   2) a \emph{character vector} specifying one of the predefined color spaces
#'   (see below).
#' @param cvd Color vision deficiency adaptation to adapt the color palette to.
#' @param target Target color difference.
#'
#' @name autopal-deprecated
#' @inherit qualpal return
#' @keywords internal
#' @seealso \code{\link{qualpal-deprecated}}
NULL

#' @rdname qualpal-deprecated
#' @section \code{autopal}:
#' This function is deprecated and will be removed in a future release.
#' No direct replacement is available.
#' @export
autopal <- function(
  n,
  colorspace = list(h = c(0, 360), s = c(0.2, 0.5), l = c(0.6, 0.85)),
  cvd = c("protan", "deutan", "tritan"),
  target = 20
) {
  .Deprecated(new = NULL, old = "autopal")

  cvd <- match.arg(cvd)

  stopifnot(
    is.numeric(target) && length(target) == 1,
    n > 0 && length(n) == 1 && is.finite(n),
    is.character(cvd)
  )

  # Run the optimizer
  fit <- stats::optimize(
    costfun,
    target = target,
    n = n,
    colorspace = colorspace,
    cvd = match.arg(cvd),
    lower = 0,
    upper = 1
  )

  # Generate a new qualpal with the optimized cvd_severity value
  qualpal(
    n = n,
    colorspace = colorspace,
    cvd = match.arg(cvd),
    cvd_severity = fit$minimum
  )
}


# Set up a cost function --------------------------------------------------

costfun <- function(x, target, n, colorspace, cvd) {
  fit <- qualpal(n, colorspace = colorspace, cvd = cvd, cvd_severity = x)

  # Return cost
  (fit$min_de_DIN99d - target)^2
}
