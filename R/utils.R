# Test approximate equality

almost_equal <- function(x, y, tol = .Machine$double.eps ^ 0.5) {
  ifelse(abs(x - y) < tol, TRUE, FALSE)
}

