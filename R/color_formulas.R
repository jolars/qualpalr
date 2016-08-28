# A vectorized version of the CIEDE2000 color difference formula ----------

CIEDE2000 <- function(L1, a1, b1, L2, a2, b2, kL = 1, kH = 1, kC = 1) {
  kL <- 1
  kH <- 1
  kC <- 1

  C1 <- sqrt(a1 ^ 2 + b1 ^ 2)
  C2 <- sqrt(a2 ^ 2 + b2 ^ 2)
  C <- (C1 + C2) / 2
  G <- 0.5 * (1 - sqrt(C ^ 7  / (C ^ 7 + 25 ^ 7)))

  a1prime <- (1 + G) * a1
  a2prime <- (1 + G) * a2

  C1prime <- sqrt(a1prime ^ 2 + b1 ^ 2)
  C2prime <- sqrt(a2prime ^ 2 + b2 ^ 2)

  h1prime <- ifelse(b1 == 0 & a1prime == 0, 0, (atan2(b1, a1prime) * 180) / pi)
  h2prime <- ifelse(b2 == 0 & a2prime == 0, 0, (atan2(b2, a2prime) * 180) / pi)
  h1prime <- ifelse(h1prime < 0, h1prime + 360, h1prime)
  h2prime <- ifelse(h2prime < 0, h2prime + 360, h2prime)

  dL <- L2 - L1
  dC <- C2prime - C1prime

  dh <- ifelse(C1prime * C2prime == 0,
               0,
               ifelse(abs(h2prime - h1prime) <= 180,
                      h2prime - h1prime,
                      ifelse(h2prime - h1prime > 180,
                             h2prime - h1prime - 360,
                             h2prime - h1prime + 360)))

  dH <- 2 * sqrt(C1prime * C2prime) * sinpi((dh / 2) / 180)

  LBar <- (L1 + L2) / 2
  CBar <- (C1prime + C2prime) / 2

  hBar <- ifelse(C1prime * C2prime == 0,
                 h1prime + h2prime,
                 ifelse(abs(h1prime - h2prime) <= 180,
                        (h1prime + h2prime) / 2,
                        ifelse(h1prime + h2prime < 360,
                               (h1prime + h2prime + 360) / 2,
                               (h1prime + h2prime - 360) / 2)))

  t <- 1 -
       0.17 * cospi((hBar - 30) / 180) +
       0.24 * cospi((2 * hBar) / 180) +
       0.32 * cospi((3 * hBar + 6) / 180) -
       0.20 * cospi((4 * hBar - 63) / 180)

  dtheta <- 30 * exp(-((hBar - 275) / 25) ^ 2)
  RC <- 2 * sqrt((CBar ^ 7) / ((CBar ^ 7) + 25 ^ 7))
  SL <- 1 + (0.015 * ((LBar - 50) ^ 2)) / sqrt(20 + (LBar - 50) ^ 2)
  SC <- 1 + 0.045 * CBar
  SH <- 1 + 0.015 * CBar * t
  RT <- - sinpi((2 * dtheta) / 180) * RC

  sqrt((dL / (kL * SL)) ^ 2 +
       (dC / (kC * SC)) ^ 2 +
       (dH / (kH * SH)) ^ 2 +
       (RT * (dC / (kC * SC)) * (dH / (kH * SH))))
}

hsl_rgb <- function(HSL) {
  H <- ifelse(HSL[1] < 0, HSL[1] + 360, HSL[1])
  S <- HSL[2]
  L <- HSL[3]

  C <- (1 - abs(2 * L - 1)) * S
  Hprime <- H / 60
  X <- C * (1 - abs(Hprime %% 2 - 1))
  m <- L - C / 2

  if (Hprime < 1) {
    c(C, X, 0) + m
  } else if (Hprime < 2) {
    c(X, C, 0) + m
  } else if (Hprime < 3) {
    c(0, C, X) + m
  } else if (Hprime < 4) {
    c(0, X, C) + m
  } else if (Hprime < 5) {
    c(X, 0, C) + m
  } else if (Hprime < 6) {
    c(C, 0, X) + m
  } else {
    c(0, 0, 0) + m
  }
}
