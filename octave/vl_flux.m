function f = vl_flux(left,right)

  xnorm = 1;

  gamma = 1.4
  gm1 = 1.0 - gamma;

  rho = left(1);
  u = left(2);
  p = left(3);
  u2 = u*u;
  e = p/gm1 + 0.5*rho*u2;
  c = sqrt(gamma*p/rho);

  ubar = u;
  mach = ubar/c
  ubp2a = -ubar + 2.0*c

  if (abs(fmach) < my_1) then
    fluxp1 = 0.25*rho*c*(fmach + 1.0)**2
    fluxp2 = fluxp1*(ubp2a/gamma + u)
    fluxp3 = fluxp1*(ynorm*ubp2a/gamma + v)
    fluxp4 = fluxp1*(znorm*ubp2a/gamma + w)
    fluxp5 = fluxp1*((-gm1*ubar*ubar                                     &
                      + my_2*gm1*ubar*c + my_2*c*c)/(gamma*gamma - my_1)      &
                     + my_haf*q2 + face_speed*(-ubar + my_2*c)/gamma)
  else if (fmach >= my_1) then
      fluxp1 = area*rho*ubar
      fluxp2 = area*(rho*u*ubar + xnorm*press)
      fluxp3 = area*(rho*v*ubar + ynorm*press)
      fluxp4 = area*(rho*w*ubar + znorm*press)
      fluxp5 = area*((enrgy + press)*ubar + face_speed*press)
    else
      fluxp1 = my_0
      fluxp2 = my_0
      fluxp3 = my_0
      fluxp4 = my_0
      fluxp5 = my_0
    end if


