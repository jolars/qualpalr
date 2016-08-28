library(qualpalr)
context("qualpal() tests")

test_that("qualpal() returns the proper list object", {
  fit <- qualpal(n = 2)
  expect_is(fit, "list")
  expect_is(fit, "qualpal")
  expect_equal(length(fit), 6)
  expect_equal(length(fit$hex), 2)
  })

test_that("erroneous input to qualpal() returns errors", {
  expect_error(qualpal(n = 0))
  expect_error(qualpal("colorblind"))
  expect_error(qualpal(2, "clorblind"))
  expect_error(qualpal(list(h = c(-200, 200), s = c(0, 1), l = c(0, 1))))
  expect_error(qualpal(list(h = c(0, 200), s = c(0, 1))))
  expect_error(qualpal(2, list(h = 2, s = c(0, 1), l = c(0, 1))))
})
