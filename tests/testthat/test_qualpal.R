library(qualpalr)

test_that("qualpal() returns the proper list object", {
  fit <- qualpal(2)
  expect_s3_class(fit, "list")
  expect_s3_class(fit, "qualpal")
  expect_equal(length(fit), 6)
  expect_equal(length(fit$hex), 2)
})

test_that("erroneous input to qualpal() returns errors", {
  expect_error(qualpal("cvd"))
  expect_error(qualpal(2, cvd_severity = 1.1))
  expect_error(qualpal(3, cvd_severity = -5))
  expect_error(qualpal(5, cvd = "normal", cvd_severity = 0.2))
  expect_error(qualpal(n = 0))
  expect_error(qualpal(n = 2, cvd = "deutrenop", cvd_severity = 0.4))
  expect_error(qualpal(n = 10^4))
  expect_error(qualpal(n = 2, "prety"))
  expect_error(qualpal(n = 3, colorspace = c(0, 200)))
  expect_error(qualpal(n = 1.4))
  expect_error(qualpal(n = 2, list(h = c(0, 200), s = c(0, 100), l = c(0, 1))))
  expect_error(qualpal(n = 2, list(h = c(-200, 200), s = c(0, 1), l = c(0, 1))))
  expect_error(qualpal(n = 2, list(h = c(0, 200), s = c(0, 1))))
  expect_error(qualpal(n = 2, list(h = 2, s = c(0, 1), l = c(0, 1))))
  expect_error(qualpal(3, matrix(1:9, ncol = 3)))
  expect_error(qualpal(3, matrix(runif(10), ncol = 5)))
  expect_error(qualpal(3, bg = "notacolor"))
  expect_error(qualpal(3, metric = "HSL"))
})

test_that("proper use of qualpal() works", {
  expect_silent(qualpal(3))
  expect_silent(qualpal(3, cvd = "deutan", cvd_severity = 0.487))
  expect_silent(qualpal(3, list(h = c(0, 360), s = c(0, 1), l = c(0, 1))))
  expect_silent(qualpal(3, colorspace = matrix(runif(90), ncol = 3)))
  expect_silent(qualpal(
    3,
    colorspace = data.frame(
      r = runif(30),
      g = runif(30),
      b = runif(30)
    )
  ))
  expect_silent(qualpal(3, bg = "white"))
  expect_silent(qualpal(9, bg = "#00ff00"))
  expect_silent(qualpal(9, metric = "ciede2000"))
})

test_that("extending a palette", {
  base_pal <- qualpal(3)

  base_pal_hex <- base_pal$hex
  base_pal_rgb <- t(col2rgb(base_pal_hex)) / 255

  expect_no_failure(qualpal(3, extend = base_pal_hex))
  expect_no_failure(qualpal(5, extend = base_pal_hex))
  expect_no_failure(qualpal(9, extend = base_pal_rgb))

  expect_error(qualpal(5, extend = "notacolor"))
  expect_error(qualpal(2, extend = base_pal_hex))
})

test_that("colorspace input", {
  expect_silent(qualpal(
    5,
    colorspace = list(h = c(0, 360), s = c(0.2, 0.5), l = c(0.6, 0.85))
  ))
  expect_silent(qualpal(
    5,
    colorspace = list(h = c(0, 360), c = c(0, 40), l = c(0, 85))
  ))

  expect_error(qualpal(
    5,
    colorspace = list(
      h = c(0, 360),
      c = c(-5, 25),
      l = c(0.6, 0.85)
    )
  ))

  expect_error(qualpal(
    5,
    colorspace = list(
      h = c(0, 360),
      c = c(5, 25),
      s = c(0.6, 0.85),
      l = c(0.6, 0.85)
    )
  ))
})
