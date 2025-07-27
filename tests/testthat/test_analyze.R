library(qualpalr)
library(testthat)

test_that("analyze_palette() works as expected", {
  colors <- c("#FF5733", "#33FF57", "#3357FF", "#F0F0F0", "#000000")

  expect_no_error(analyze_palette(colors))
  expect_no_error(analyze_palette(colors, cvd = NULL))

  res_protan <- analyze_palette(colors, cvd = "protan")

  expect_in("protan", names(res_protan))

  rgb <- t(col2rgb(colors) / 255)

  res_hex <- analyze_palette(colors)
  res_rgb <- analyze_palette(rgb)

  expect_equal(res_hex, res_rgb)
})
