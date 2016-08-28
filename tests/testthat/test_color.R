library(stringr)
context("Color formulas")

test_that("the CIEDE2000 formula works correctly", {
  # CIEDE2000 test data from http://www.ece.rochester.edu/~gsharma/ciede2000/
  dat <- read.delim("ciede2000testdata.txt")
  res <- CIEDE2000(dat[, 1], dat[, 2], dat[, 3],
                             dat[, 4], dat[, 5], dat[, 6])
  testthat::expect_true(all(abs(res - dat[, 7]) < 1e-4))
})

test_that("hsl to rgb color conversion works correctly", {
  # Test data taken from http://www.rapidtables.com/convert/color/rgb-to-hsl.htm
  rgb_dat <- matrix(c(0, 0, 0, 255, 255, 255, 255, 0, 0, 0, 255, 0, 0, 0, 255,
                      255, 255, 0, 0, 255, 255, 255, 0, 255, 192, 192, 192,
                      128, 128, 128, 128, 0, 0, 128, 128, 0, 0, 128, 0, 128, 0,
                      128, 0, 128, 128, 0, 0, 128),
                    byrow = TRUE, ncol = 3) / 255

  hsl_dat <- matrix(c(0, 0, 0, 0, 0, 100, 0, 100, 50, 120, 100, 50, 240, 100,
                      50, 60, 100, 50, 180, 100, 50, 300, 100, 50, 0, 0, 75, 0,
                      0, 50, 0, 100, 25, 60, 100, 25, 120, 100, 25, 300, 100,
                      25, 180, 100, 25, 240, 100, 25),
                    byrow = TRUE,  ncol = 3)
  hsl_dat[, 2:3] <- hsl_dat[, 2:3] / 100

  testthat::expect_true(all(abs(rgb_dat - t(apply(hsl_dat, 1, hsl_rgb))) <
                          1e-2))
})





