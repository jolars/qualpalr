# Optimization wrapper for dE CIEDE2000 -----------------------------------

opt_dE <- function(par, n) {
  Lab <- grDevices::convertColor(t(apply(matrix(par, ncol = 3), 1, hsl_rgb)),
                                 from = "sRGB", to = "Lab")

  L <- utils::combn(Lab[, 1], 2)
  a <- utils::combn(Lab[, 2], 2)
  b <- utils::combn(Lab[, 3], 2)

  min(ciede2000(L1 = L[1, ], a1 = a[1, ], b1 = b[1, ],
                L2 = L[2, ], a2 = a[2, ], b2 = b[2, ]))
}

# Compute distance matrix of delta E CIEDE 2000 ---------------------------

measure_ciede2000 <- function(lab_data, n) {
  L <- utils::combn(lab_data[, 1], 2)
  a <- utils::combn(lab_data[, 2], 2)
  b <- utils::combn(lab_data[, 3], 2)

  col_diff <- ciede2000(L1 = L[1, ], a1 = a[1, ], b1 = b[1,],
                        L2 = L[2, ], a2 = a[2, ], b2 = b[2, ])
  diff_mat <- matrix(
    0,
    ncol = nrow(lab_data),
    nrow = nrow(lab_data),
    dimnames = list(rownames(lab_data), rownames(lab_data))
  )

  diff_mat[lower.tri(diff_mat)] <- col_diff
  diff_mat[upper.tri(diff_mat)] <- t(diff_mat)[upper.tri(diff_mat)]

  stats::as.dist(diff_mat)
}
