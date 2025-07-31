#' Retrieve one of the built-in color palettes
#'
#' This function retrieves a color palette from the built-in palettes. To
#' see the available palettes, use [list_palettes()].
#'
#' @param palette A character string specifying the name of the color palette to
#'
#' @return A character vector of colors in hex format.
#'
#' @seealso [list_palettes()]
#'
#' @examples
#' get_palette("Vermeer:LittleStreet")
#'
#' @export
get_palette <- function(palette = "ColorBrewer:Accent") {
  get_palette_cpp(palette)
}
