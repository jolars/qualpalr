library(qualpalr)
context("qualpal() tests")

test_that("qualpal() returns the proper list object", {
  fit <- qualpal(2)
  expect_s3_class(fit, "list")
  expect_s3_class(fit, "qualpal")
  expect_equal(length(fit), 6)
  expect_equal(length(fit$hex), 2)
})

test_that("erroneous input to qualpal() returns errors", {
  expect_error(qualpal("cvd"))
  expect_error(qualpal(2, cvd_severity = 1.5))
  expect_error(qualpal(3, cvd_severity = -5))
  expect_error(qualpal(5, cvd = "normal", cvd_severity = 0.2))
  expect_error(qualpal(1))
  expect_error(qualpal(n = 0))
  expect_error(qualpal(n = 2, cvd = "deutrenop", cvd_severity = 0.4))
  expect_error(qualpal(n = 10 ^ 3))
  expect_error(qualpal(n = 2, "prety"))
  expect_error(qualpal(n = 3, colorspace = c(0, 200)))
  expect_error(qualpal(n = 1.4))
  expect_error(qualpal(n = 2, list(h = c(0, 200), s = c(0, 100), l = c(0, 1))))
  expect_error(qualpal(n = 2, list(h = c(-200, 200), s = c(0, 1), l = c(0, 1))))
  expect_error(qualpal(n = 2, list(h = c(0, 200), s = c(0, 1))))
  expect_error(qualpal(n = 2, list(h = 2, s = c(0, 1), l = c(0, 1))))
  expect_error(qualpal(n = 500))
  expect_error(qualpal(3, matrix(1:9, ncol = 3)))
  expect_error(qualpal(3, matrix(runif(10), ncol = 5)))
})

test_that("proper use of qualpal() works", {
  expect_silent(qualpal(3))
  expect_silent(qualpal(3, cvd = "deutan", cvd_severity = 0.487))
  expect_silent(qualpal(5, "pretty"))
  expect_silent(qualpal(3, "pretty_dark", cvd_severity = 0.1))
  expect_silent(qualpal(3, list(h = c(0, 360), s = c(0, 1), l = c(0, 1))))
  expect_silent(qualpal(3, colorspace = matrix(runif(90), ncol = 3)))
  expect_silent(qualpal(3, colorspace = data.frame(r = runif(30),
                                                   g = runif(30),
                                                   b = runif(30))))
})
