# Scale to range ----------------------------------------------------------

scale_runif <- function(x, new_min, new_max) {
  (new_max - new_min) * (x - 1) + new_max
}

make_options <- function(
  cvd = c("protan", "deutan", "tritan"),
  cvd_severity = 0,
  bg = NULL,
  metric = c("din99d", "ciede2000", "cie76"),
  extend = NULL
) {
  cvd <- match.arg(cvd)
  metric <- match.arg(metric)

  if (is.null(extend)) {
    extend <- matrix(0, nrow = 0, ncol = 3)
  } else if (is.character(extend)) {
    extend <- grDevices::col2rgb(extend)
    extend <- t(extend) / 255
  } else if (is.data.frame(extend)) {
    extend <- as.matrix(extend)
  }

  stopifnot(
    is.character(cvd),
    length(cvd) == 1,
    is.numeric(cvd_severity),
    length(cvd_severity) == 1,
    cvd_severity >= 0,
    cvd_severity <= 1,
    is.null(bg) || (is.character(bg) && length(bg) == 1),
    is.matrix(extend),
    ncol(extend) == 3
  )

  cvd_list <- list()
  cvd_list[[cvd]] <- cvd_severity

  if (!is.null(bg)) {
    bg <- grDevices::col2rgb(bg)
    bg <- as.vector(bg) / 255
  } else {
    bg <- double(0)
  }
  list(cvd = cvd_list, bg = bg, metric = metric, extend = extend)
}
