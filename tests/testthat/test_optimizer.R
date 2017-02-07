library(qualpalr)
context("optimizer tests")

test_that("optimize_qualpal() works as expected", {
  pal <- qualpal(3)
  expect_s3_class(pal, "qualpal")
})