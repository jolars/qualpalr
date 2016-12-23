library(qualpalr)
context("normal (no error) tests")

test_that("proper use of the functions work", {
  expect_error(qualpal(3), NA)
  expect_error(qualpal(3, cvd = "deutan", cvd_severity = 0.487), NA)
  expect_error(qualpal(5, "pretty"), NA)
  expect_error(qualpal(3, "pretty_dark", cvd_severity = 0.1), NA)

  f <- qualpal(4)

  expect_error(plot(f), NA)
  expect_error(pairs(f), NA)
  expect_error(pairs(f, colorspace = "DIN99d"), NA)
})
