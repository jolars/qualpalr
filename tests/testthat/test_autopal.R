library(qualpalr)

test_that("autopal() works as expected", {
  expect_warning(autopal(3))
  expect_warning(autopal(3, target = 14))
})
