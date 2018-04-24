# Scale to range ----------------------------------------------------------

scale_runif <- function(x, new_min, new_max) {
  (new_max - new_min) * (x - 1) + new_max
}
