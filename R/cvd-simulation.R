
# Adapt RGB matrix to color deficiency ------------------------------------

sRGB_CVD <- function(RGB, cvd, cvd_severity) {
  sev_ind <- almost_equal(cvd_severity, cvd_severity_dat)

  if (any(sev_ind)) {

    cvd_mat <- cvd_mats[cvd_type_dat == cvd & sev_ind, ]

  } else {

    fl <- floor(cvd_severity * 10) / 10
    ce <- ceiling(cvd_severity * 10) / 10

    fl_ind <- almost_equal(fl, cvd_severity_dat) & cvd_type_dat == cvd
    ce_ind <- almost_equal(ce, cvd_severity_dat) & cvd_type_dat == cvd

    cvd_mat1 <- cvd_mats[fl_ind, ]
    cvd_mat2 <- cvd_mats[ce_ind, ]

    # Interpolate nearest matching matrices
    cvd_mat <- cvd_mat1 + ((ce - cvd_severity) * 10) * (cvd_mat2 - cvd_mat1)

  }

  out <- tcrossprod(cvd_mat, RGB)

  return(t(out))
}
