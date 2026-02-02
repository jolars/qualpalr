#' Convert colors between colorspaces
#'
#' @param colors A matrix of colors
#' @param from The colorspace of the input colors, one of "rgb", "hsl",
#'   "din99d", "lab", "xyz"
#' @param to The colorspace of the output colors,
#'   one of "rgb", "hsl", "lab", "xyz"
#'
#' @return The colors converted to the new colorspace
#' @export
convert_colors <- function(colors, from, to) {
  convert_colors_cpp(colors, from, to)
}
