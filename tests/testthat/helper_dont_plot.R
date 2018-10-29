# Capture plots without plotting
dont_plot <- function(x, f = graphics::plot, ...) {
  tmp <- tempfile()
  grDevices::png(tmp)
  p <- f(x, ...)
  grDevices::dev.off()
  unlink(tmp)
  invisible(p)
}