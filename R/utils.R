# Scale to range ----------------------------------------------------------

scale_runif <- function(x, new_min, new_max) {
  (new_max - new_min) * (x - 1) + new_max
}

make_options <- function(
  cvd = c(protan = 0, deutan = 0, tritan = 0),
  cvd_severity,
  bg = NULL,
  metric = c("ciede2000", "din99d", "cie76"),
  extend = NULL
) {
  metric <- match.arg(metric)

  if (!missing(cvd_severity)) {
    message(
      "The argument 'cvd_severity' is deprecated and will be
      removed in a future version. Please use a named 'cvd' object instead."
    )
    stopifnot(
      length(cvd_severity) == 1,
      cvd_severity >= 0,
      cvd_severity <= 1
    )
  }

  if (is.null(extend)) {
    extend <- matrix(0, nrow = 0, ncol = 3)
  } else if (is.character(extend)) {
    extend <- grDevices::col2rgb(extend)
    extend <- t(extend) / 255
  } else if (is.data.frame(extend)) {
    extend <- as.matrix(extend)
  }

  stopifnot(
    is.character(cvd) || is.numeric(cvd) || is.null(cvd),
    is.null(bg) || (is.character(bg) && length(bg) == 1),
    is.matrix(extend),
    ncol(extend) == 3
  )

  if (is.null(cvd)) {
    cvd_list <- list(protan = 0, deutan = 0, tritan = 0)
  } else if (is.character(cvd)) {
    cvd <- match.arg(cvd, c("protan", "deutan", "tritan"))
    cvd_list <- list()
    cvd_list[[cvd]] <- cvd_severity
  } else {
    stopifnot(
      is.numeric(cvd),
      length(cvd) <= 3,
      all(cvd >= 0),
      all(cvd <= 1),
      names(cvd) %in% c("protan", "deutan", "tritan")
    )
    cvd_list <- list(
      protan = ifelse("protan" %in% names(cvd), cvd["protan"], 0),
      deutan = ifelse("deutan" %in% names(cvd), cvd["deutan"], 0),
      tritan = ifelse("tritan" %in% names(cvd), cvd["tritan"], 0)
    )
  }

  if (!is.null(bg)) {
    bg <- grDevices::col2rgb(bg)
    bg <- as.vector(bg) / 255
  } else {
    bg <- double(0)
  }
  list(cvd = cvd_list, bg = bg, metric = metric, extend = extend)
}
