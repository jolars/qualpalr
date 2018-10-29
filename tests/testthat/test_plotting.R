library(qualpalr)
context("plotting tests")

test_that("plotting checks for errors", {
  f <- qualpal(5)

  expect_error(dont_plot(f, pairs, colorspace = "f"))
})

test_that("plotting functions work as expected", {
  f <- qualpal(4)

  expect_silent(dont_plot(f))
  expect_silent(dont_plot(f, pairs))
  expect_silent(dont_plot(f, pairs, colorspace = "DIN99d"))
})
