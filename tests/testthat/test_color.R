library(qualpalr)
context("Color formulas")

test_that("the CIEDE2000 formula works correctly", {
  # CIEDE2000 test data from http://www.ece.rochester.edu/~gsharma/ciede2000/
  dat <- readRDS("ciede2000testdata.RData")
  res <- qualpalr::CIEDE2000(dat[, 1], dat[, 2], dat[, 3],
                             dat[, 4], dat[, 5], dat[, 6])
  expect_true(all(abs(res - dat[, 7]) < 1e-4))
})

test_that("hsl to rgb color conversion works correctly", {
  # Data taken from http://www.on-the-matrix.com/webtools/HtmlColorCodes.aspx
  dat <- readRDS("hsl_rgb_hex.RData")

  expect_true(all(abs(t(apply(dat[, 5:7], 1, hsl_rgb)) - dat[, 1:3]) < 1e-2))

})

test_that("colors are within their respective color spaces", {
  fit <- qualpal(n = 3)
  expect_true(all(fit$RGB >= 0 & fit$RGB <= 1))
  expect_true(all(fit$hsl[, 1] >= 0 & fit$hsl[, 1] <= 360))
  expect_true(all(fit$hsl[, 2] >= 0 & fit$hsl[, 2] <= 1))
  expect_true(all(fit$hsl[, 2] >= 0 & fit$hsl[, 2] <= 1))
  expect_true(all(fit$Lab[, 1] >= 0 & fit$Lab[, 1] <= 100))
})

