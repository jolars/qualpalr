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
  expect_error(qualpal("colorblind"))
  expect_error(qualpal(n = 0))
  expect_error(qualpal(n = 2, colorblind = "deutrenop"))
  expect_error(qualpal(n = 10 ^ 3))
  expect_error(qualpal(n = 2, "prety"))
  expect_error(qualpal(n = 3, colorspace = c(0, 200)))
  expect_error(qualpal(n = 1.4))
  expect_error(qualpal(n = 2, list(h = c(0, 200), s = c(0, 100), l = c(0, 1))))
  expect_error(qualpal(n = 2, list(h = c(-200, 200), s = c(0, 1), l = c(0, 1))))
  expect_error(qualpal(n = 2, list(h = c(0, 200), s = c(0, 1))))
  expect_error(qualpal(n = 2, list(h = 2, s = c(0, 1), l = c(0, 1))))
})

test_that("plots return errors when used inappropriately", {
  fit <- qualpal(n = 2)
  expect_error(pairs.qualpal(fit, colorspace = "ABC"))
})
