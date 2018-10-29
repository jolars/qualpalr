library(qualpalr)
context("color formula tests")

test_that("color formula conversions work properly", {
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
