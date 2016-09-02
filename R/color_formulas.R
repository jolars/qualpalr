RGB_HSL <- function(RGB) {
  R <- RGB[, 1]
  G <- RGB[, 2]
  B <- RGB[, 3]

  M <- pmax(R, G, B)
  m <- pmin(R, G, B)
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

Lab_DIN99d <- function(Lab) {
  L <- Lab[, 1]
  a <- Lab[, 2]
  b <- Lab[, 3]

  L99d <- 325.22 * log(1 + 0.0036 * L)
  e    <- a * cos(50 * pi / 180) + b * sin(50 * pi / 180)
  f    <- 1.14 * (- a * sin(50 * pi / 180) + b * cos(50 * pi / 180))
  C99d <- 22.5 * log(1 + 0.06 * sqrt(e ^ 2 + f ^ 2))
  h99d <- (atan2(f, e) * 180 / pi) + 50
  a99d <- C99d * cos(h99d * pi / 180)
  b99d <- C99d * sin(h99d * pi / 180)

  cbind(L99d, a99d, b99d)
}

XYZ_LMS <- function(XYZ) {
  t(rbind(c(  0.15514, 0.54312, - 0.03286),
          c(- 0.15514, 0.45684,   0.03286),
          c(        0,       0,   0.01608)) %*% t(XYZ))
}

LMS_XYZ <- function(LMS) {
  t(solve(rbind(c(  0.15514, 0.54312, - 0.03286),
                c(- 0.15514, 0.45684,   0.03286),
                c(        0,       0,   0.01608))) %*% t(LMS))
}

LMS_protan <- function(LMS) {
  t(rbind(c(0, 2.02344, - 2.52581),
          c(0,       1,         0),
          c(0,       0,         1)) %*% t(LMS))
}

LMS_deutan <- function(LMS) {
  t(rbind(c(1,        0,       0),
          c(0.494207, 0, 1.24827),
          c(0,        0,       1)) %*% t(LMS))
}

LMS_tritan <- function(LMS) {
  t(rbind(c(         1,        0, 0),
          c(         0,        1, 0),
          c(- 0.395913, 0.801109, 0)) %*% t(LMS))
}

RGB_LMS <- function(RGB) {
  t(rbind(c(  17.8824, 43.5161,  4.11935),
          c(  3.45565, 27.1554,  3.86714),
          c(0.0299566, 0.184309, 1.46709)) %*% t(RGB))
}

LMS_RGB <- function(LMS) {
  t(solve(rbind(c(  17.8824, 43.5161,  4.11935),
                c(  3.45565, 27.1554,  3.86714),
                c(0.0299566, 0.184309, 1.46709))) %*% t(LMS))
}

daltonize <- function(err) {
  t(rbind(c(  0, 0, 0),
          c(0.7, 1, 0),
          c(0.7, 0, 1)) %*% t(err))
}
