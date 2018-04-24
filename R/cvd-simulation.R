
# Adapt RGB matrix to color deficiency ------------------------------------

sRGB_CVD <- function(RGB, cvd, cvd_severity) {
  cvd_severity <- cvd_severity * 10

  fl <- as.integer(floor(cvd_severity))
  ce <- as.integer(ceiling(cvd_severity))

  cvd_ind <- cvd_type_dat == cvd

  cvd_mat1 <- cvd_mats[fl == cvd_severity_dat & cvd_ind, ]
  cvd_mat2 <- cvd_mats[ce == cvd_severity_dat & cvd_ind, ]

  # Interpolate nearest matching matrices
  cvd_mat <- cvd_mat1 + (ce - cvd_severity) * (cvd_mat2 - cvd_mat1)

  out <- tcrossprod(cvd_mat, RGB)

  return(t(out))
}
