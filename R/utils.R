# Test approximate equality -----------------------------------------------

almost_equal <- function(x, y, tol = .Machine$double.eps ^ 0.5) {
  ifelse(abs(x - y) < tol, TRUE, FALSE)
}

# Scale to range ----------------------------------------------------------

scale_runif <- function(x, new_min, new_max) {
  (new_max - new_min) * (x - 1) + new_max
}

