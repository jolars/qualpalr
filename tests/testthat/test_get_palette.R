test_that("get_palette())", {
  expect_error(get_palette("NonExistentPalette"))
  expect_error(get_palette("ColorBrewer,Accent"))
  expect_silent(get_palette("ColorBrewer:Accent"))
})
