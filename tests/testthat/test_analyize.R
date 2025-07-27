library(qualpalr)

colors <- c("#FF5733", "#33FF57", "#3357FF", "#F0F0F0", "#000000")

rgb <- t(col2rgb(colors) / 255)

# bg <- as.vector(col2rgb("")) / 255
bg <- double(0)

opts <- list(bg = bg, cvd = list(), metric = "din99d")

analyze_palette(rgb)
