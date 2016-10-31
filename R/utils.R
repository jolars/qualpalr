
# Find the farthest n points ----------------------------------------------

farthest_points <- function (data, n) {
  # DIN99d distances with power tranformations from Huang 2014
  dm <- as.matrix((stats::dist(data) ^ 0.74) * 1.28)
  r <- sample.int(nrow(dm), n)

  repeat {
    r_old <- r
    for (i in 1:n) {
      mm <- dm[r[-i], -r[-i], drop = FALSE]
      k <- which.max(mm[(1:ncol(mm) - 1) * nrow(mm) + max.col(t(-mm))])
      r[i] <- as.numeric(dimnames(mm)[[2]][k])
    }
    if (identical(r_old, r)) return(r)
  }
}







