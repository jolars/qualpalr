#' Analyze a categorical color palette
#'
#' Analyze a categorical color palette with respect to
#' the differences between the colors in the palette.
#'
#' @param palette Either a matrix of RGB values (with values between 0 and 1),
#'   a data frame with RGB values, or a character vector of hex colors.
#' @inheritParams qualpal
#'
#' @return A list of lists, one for each type of color vision deficiency plus
#' normal vision. Each list contains `difference_matrix`, `min_distances`, and
#' `bg_min_distance`
#'
#' @seealso [qualpal()[qualpal()]
#'
#' @examples
#' pal <- qualpal(5)
#' analyze_palette(pal$hex, cvd = "protan", cvd_severity = 0.5)
#'
#' @export
analyze_palette <- function(
  palette,
  cvd = c("protan", "deutan", "tritan"),
  cvd_severity = 1,
  bg = NULL,
  metric = c("din99d", "ciede2000", "cie76")
) {
  UseMethod("analyze_palette")
}

#' @export
analyze_palette.matrix <- function(
  palette,
  cvd = c("protan", "deutan", "tritan"),
  cvd_severity = 1,
  bg = NULL,
  metric = c("din99d", "ciede2000", "cie76")
) {
  stopifnot(
    is.matrix(palette),
    ncol(palette) == 3,
    max(palette) <= 1,
    min(palette) >= 0
  )

  opts <- make_options(
    cvd = cvd,
    cvd_severity = cvd_severity,
    bg = bg,
    metric = metric
  )

  res <- analyze_palette_cpp(palette, opts)
  res

  lapply(res, function(x) {
    x[["difference_matrix"]] <- stats::as.dist(x[["difference_matrix"]])
    x
  })
}

#' @export
analyze_palette.data.frame <- function(
  palette,
  cvd = c("protan", "deutan", "tritan"),
  cvd_severity = 1,
  bg = NULL,
  metric = c("din99d", "ciede2000", "cie76")
) {
  analyze_palette(
    as.matrix(palette),
    cvd = cvd,
    cvd_severity = cvd_severity,
    bg = bg,
    metric = metric
  )
}

#' @export
analyze_palette.character <- function(
  palette,
  cvd = c("protan", "deutan", "tritan"),
  cvd_severity = 1,
  bg = NULL,
  metric = c("din99d", "ciede2000", "cie76")
) {
  pal <- t(grDevices::col2rgb(palette, alpha = FALSE)) / 255

  analyze_palette(
    pal,
    cvd = cvd,
    cvd_severity = cvd_severity,
    bg = bg,
    metric = metric
  )
}
