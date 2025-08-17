#' List available color palettes
#'
#' @return A list of available color palettes. Each palette is a named list
#' with a character vector.
#'
#' @seealso [qualpal()]
#'
#' @examples
#' list_palettes()
#'
#' @export
list_palettes <- function() {
  list_palettes_cpp()
}
