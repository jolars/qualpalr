RGB_HSL <- function(RGB) {
  R <- RGB[, 1]
  G <- RGB[, 2]
  B <- RGB[, 3]

  M <- pmax.int(R, G, B)
  m <- pmin.int(R, G, B)
  C <- M - m

  i1 <- C == 0
  i2 <- M == R & C > 0
  i3 <- M == G & C > 0
  i4 <- M == B & C > 0

  Hprime <- double(length(R))
  Hprime[i1] <- 0
  Hprime[i2] <- ((G[i2] - B[i2]) / C[i2]) %% 6
  Hprime[i3] <- (B[i3] - R[i3]) / C[i3] + 2
  Hprime[i4] <- (R[i4] - G[i4]) / C[i4] + 4

  H <- Hprime * 60
  L <- (M + m) / 2

  S <- double(length(R))
  S[!i1] <- C[!i1] / (1 - abs(2 * L[!i1] - 1))
  S[i1] <- 0

  cbind(H, S, L)
}

HSL_RGB <- function(HSL) {
  H <- HSL[, 1]
  S <- HSL[, 2]
  L <- HSL[, 3]

  H[H < 0] <- H[H < 0] + 360

  C <- (1 - abs(2 * L - 1)) * S
  Hprime <- H / 60
  X <- C * (1 - abs(Hprime %% 2 - 1))
  m <- L - C / 2

  R <- G <- B <- double(length(H))

  i1 <- Hprime >= 0 & Hprime < 1
  i2 <- Hprime >= 1 & Hprime < 2
  i3 <- Hprime >= 2 & Hprime < 3
  i4 <- Hprime >= 3 & Hprime < 4
  i5 <- Hprime >= 4 & Hprime < 5
  i6 <- Hprime >= 5 & Hprime < 6

  R[i1] <- C[i1]
  R[i2] <- X[i2]
  R[i3] <- 0
  R[i4] <- 0
  R[i5] <- X[i5]
  R[i6] <- C[i6]

  G[i1] <- X[i1]
  G[i2] <- C[i2]
  G[i3] <- C[i3]
  G[i4] <- X[i4]
  G[i5] <- 0
  G[i6] <- 0

  B[i1] <- 0
  B[i2] <- 0
  B[i3] <- X[i3]
  B[i4] <- C[i4]
  B[i5] <- C[i5]
  B[i6] <- X[i6]

  cbind(R, G, B) + m
}

XYZ_DIN99d <- function(XYZ, Xr = 0.95047, Yr = 1, Zr = 1.08883) {
  XYZ[, 1] <- 1.12 * XYZ[, 1] - 0.12 * XYZ[, 3]

  Lab <- XYZ_Lab(XYZ, Xr = Xr, Yr = Yr, Zr = Zr)

  L <- Lab[, 1]
  a <- Lab[, 2]
  b <- Lab[, 3]

  u <- 50 * pi / 180

  L99d <- 325.22 * log(1 + 0.0036 * L)
  e    <- a * cos(u) + b * sin(u)
  f    <- 1.14 * (b * cos(u) - a * sin(u))
  G    <- sqrt(e ^ 2 + f ^ 2)
  C99d <- 22.5 * log(1 + 0.06 * G)
  h99d <- atan2(f, e) + 50 * pi / 180
  a99d <- C99d * cos(h99d)
  b99d <- C99d * sin(h99d)

  cbind(L99d, a99d, b99d)
}

sRGB_XYZ <- function(sRGB) {
  ind <- sRGB > 0.04045

  sRGB[!ind] <- sRGB[!ind] / 12.92
  sRGB[ind]  <- ((sRGB[ind] + 0.055) / (1.055)) ^ 2.4
  sRGB       <- matrix(sRGB, ncol = 3)

  t(tcrossprod(rbind(c(0.4124564, 0.3575761, 0.1804375),
                     c(0.2126729, 0.7151522, 0.0721750),
                     c(0.0193339, 0.1191920, 0.9503041)), sRGB))
}

XYZ_sRGB <- function(XYZ) {
  sRGB <- t(tcrossprod(solve(rbind(c(0.4124564, 0.3575761, 0.1804375),
                                   c(0.2126729, 0.7151522, 0.0721750),
                                   c(0.0193339, 0.1191920, 0.9503041))), XYZ))

  ind <- sRGB > 0.0031308

  sRGB[!ind] <- 12.92 * sRGB[!ind]
  sRGB[ind]  <- 1.055 * sRGB[ind] ^ (1 / 2.4) - 0.055
  matrix(sRGB, ncol = 3)
}

XYZ_Lab <- function(XYZ, Xr = 0.95047, Yr = 1, Zr = 1.08883) {
  X <- XYZ[, 1]
  Y <- XYZ[, 2]
  Z <- XYZ[, 3]

  epsilon <- 216 / 24389
  kelvin  <- 24389 / 27

  xr <- X / Xr
  yr <- Y / Yr
  zr <- Z / Zr

  ix <- xr > epsilon
  iy <- yr > epsilon
  iz <- zr > epsilon

  fx <- fy <- fz <- double(length(xr))
  fx[ix]  <- xr[ix] ^ (1 / 3)
  fx[!ix] <- (xr[!ix] * kelvin + 16) / 116

  fy[iy]  <- yr[iy] ^ (1 / 3)
  fy[!iy] <- (yr[!iy] * kelvin + 16) / 116

  fz[iz]  <- zr[iz] ^ (1 / 3)
  fz[!iz] <- (zr[!iz] * kelvin + 16) / 116

  L <- 116 *  fy - 16
  a <- 500 * (fx - fy)
  b <- 200 * (fy - fz)

  cbind(L, a, b)
}

Lab_XYZ <- function(Lab, Xr = 0.95047, Yr = 1, Zr = 1.08883) {
  L <- Lab[, 1]
  a <- Lab[, 2]
  b <- Lab[, 3]

  epsilon <- 216 / 24389
  kelvin  <- 24389 / 27

  fy <- (L + 16) / 116
  fx <- a / 500 + fy
  fz <- fy - b / 200

  ix <- fx ^ 3 > epsilon
  iy <-      L > kelvin * epsilon
  iz <- fz ^ 3 > epsilon

  xr <- yr <- zr <- double(length(L))

  xr[ix]  <- fx[ix] ^ 3
  xr[!ix] <- (116 * fx[!ix] - 16) / kelvin

  yr[iy]  <- ((L[iy] + 16) / 116) ^ 3
  yr[!iy] <- L[!iy] / kelvin

  zr[iz]  <- fz[iz] ^ 3
  zr[!iz] <- (116 * fz[!iz] - 16) / kelvin

  X <- Xr * xr
  Y <- Yr * yr
  Z <- Zr * zr

  cbind(X, Y, Z)
}
