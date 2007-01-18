#!/usr/bin/env octave

a = [
  0  1
  1  1
  1  0
  1  0
]

[q, r] = qr(a)

## a*x = b
## q*r*x = b
## r*x = q'*b
## x = inv(r)*q'*b

invr = inv(r(1:2,1:2));

invr = [invr [0 0;0 0];[0 0 0 0];[0 0 0 0]]

Ainv = invr*q'

##inv(a'*a)*a'

m = [ 0 1
      1 1]

m*Ainv(1:2,:)
