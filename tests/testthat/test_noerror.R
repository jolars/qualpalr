library(qualpalr)
context("normal (no error) tests")

test_that("proper use of the functions work", {
  expect_error(qualpal(3), NA)
  expect_error(qualpal(3, cvd = "deutan", cvd_severity = 0.487), NA)
  expect_error(qualpal(5, "pretty"), NA)
  expect_error(qualpal(3, "pretty_dark", cvd_severity = 0.1), NA)
})

test_that("plotting functions work as expected", {
  f <- qualpal(4)

  ff <- tempfile()
  png(ff)

  expect_error(plot(f), NA)
  expect_error(pairs(f), NA)
  expect_error(pairs(f, colorspace = "DIN99d"), NA)

  dev.off()
  unlink(ff)
})
