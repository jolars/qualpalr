library(qualpalr)
context("color formula tests")

test_that("color formula conversions work properly", {
  # Data retrieved from https://en.wikipedia.org/wiki/HSL_and_HSV
  dat <- readRDS("rgb_hsl.RData")
  expect_true(all(HSL_RGB(cbind(dat$H, dat$SHSL, dat$L)) -
                  cbind(dat$R, dat$G, dat$B) <
                  1e-2))
  expect_true(all(RGB_HSL(cbind(dat$R, dat$G, dat$B)) -
                  cbind(dat$H, dat$SHSL, dat$L) <
                  1e-1))
})

test_that("color conversions work back and forth", {
  test_rgb <- matrix(runif(9), ncol = 3)
  expect_true(all(HSL_RGB(RGB_HSL(test_rgb)) -
                    test_rgb <
                    .Machine$double.eps ^ 0.5))

  test_lms <- RGB_LMS(matrix(runif(9), ncol = 3))
  expect_true(all(XYZ_LMS(LMS_XYZ(test_lms)) -
                    test_lms <
                    .Machine$double.eps ^ 0.5))
  test_xyz <- matrix(runif(9), ncol = 3)
  expect_equal(sum(test_xyz), sum(Lab_XYZ(XYZ_Lab(test_xyz))))

  expect_equal(sum(test_rgb), sum(XYZ_sRGB(sRGB_XYZ(test_rgb))))
})

test_that("colors are within their respective color spaces", {
  fit <- qualpal(n = 3)
  expect_true(all(fit$RGB >= 0 & fit$RGB <= 1))
  expect_true(all(fit$HSL[, 1] >= 0 & fit$HSL[, 1] <= 360))
  expect_true(all(fit$HSL[, 2] >= 0 & fit$HSL[, 2] <= 1))
  expect_true(all(fit$HSL[, 2] >= 0 & fit$HSL[, 2] <= 1))
})

test_that("formulas perform as expected on worked example from Cui 2002", {
  xyz1 <- cbind(0.049600, 0.037200, 0.195900)
  xyz2 <- cbind(0.046651, 0.038100, 0.177848)

  lab1 <- XYZ_Lab(xyz1, Xr = 0.9481, Yr = 1, Zr = 1.0733)
  lab2 <- XYZ_Lab(xyz2, Xr = 0.9481, Yr = 1, Zr = 1.0733)

  expect_equal(as.numeric(dist(rbind(
    XYZ_DIN99d(xyz1, Xr = 0.9481, Yr = 1, Zr = 1.0733),
    XYZ_DIN99d(xyz2, Xr = 0.9481, Yr = 1, Zr = 1.0733)
  ))), 3.1210819361106368)
})

test_that("color blindness simulation performs on test data", {
  # Test data from Vienot et al 1999
  test_rgb <-   rbind(c(255, 255, 255),
                      c(  0, 255, 255),
                      c(255,   0, 255),
                      c(255, 255,   0),
                      c(  0,   0, 255),
                      c(255,   0,   0),
                      c(102,  51, 153)) / 255

  protan_key <- rbind(c(255, 255, 255),
                      c(241, 241, 254),
                      c( 96,  96, 255),
                      c(255, 255,  21),
                      c( 21,  21, 255),
                      c( 96,  96,  28),
                      c( 62,  62, 153)) / 255

  deutan_key <- rbind(c(253, 253, 253),
                      c(217, 217, 255),
                      c(148, 148, 250),
                      c(253, 253,  44),
                      c( 44,  44, 253),
                      c(148, 148,   0),
                      c( 80,  80, 154)) / 255

  test_rgb <- test_rgb ^ 2.2
  protan <- RGB_LMS(test_rgb * 0.992052 + 0.003974)
  deutan <- RGB_LMS(test_rgb * 0.957237 + 0.0213814)
  rgb_protan <- LMS_RGB(LMS_protan(protan)) ^ (1 / 2.2)
  rgb_deutan <- LMS_RGB(LMS_deutan(deutan)) ^ (1 / 2.2)

  expect_false(any(abs(protan_key - rgb_protan) > 0.01))
  expect_false(any(abs(deutan_key - rgb_deutan) > 0.01))
})
