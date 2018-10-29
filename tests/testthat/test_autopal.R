library(qualpalr)
context("autopal() tests")

test_that("autopal() throws errors when it should", {
  expect_error(autopal(c(2,3)))
  expect_error(autopal(3, target = 1:4))
  expect_error(autopal(3, target = "string"))
  expect_error(autopal(3, cvd = 1))
})

test_that("autopal() works as expected", {
  expect_silent(autopal(3))
  pal <- autopal(3, target = 14)
  expect_s3_class(pal, "qualpal")
})
