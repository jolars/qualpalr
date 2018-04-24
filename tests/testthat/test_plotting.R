library(qualpalr)
context("plotting tests")

test_that("plotting checks for errors", {
  f <- qualpal(5)

  expect_error(pairs(f, colorspace = "f"))
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
