test_that("list_palettes()", {
  expect_type(list_palettes(), "list")
  expect_type(list_palettes()[["ColorBrewer"]], "character")
})
