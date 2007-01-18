function f = vl_flux(left,right)

  xnorm = 1;

  gamma = 1.4;
  gm1 = gamma - 1.0;

  rho = left(1);
  u = left(2);
  p = left(3);
  u2 = u*u;
  e = p/gm1 + 0.5*rho*u2;
  c = sqrt(gamma*p/rho);

  ubar = u;
  mach = ubar/c;
  ubp2a = -ubar + 2.0*c;

  if (abs(mach) < 1.0)
    fluxp1 = 0.25*rho*c*(mach + 1.0)^2;
    fluxp2 = fluxp1*(ubp2a/gamma + u);
    fluxp3 = fluxp1*((-gm1*ubar*ubar                                     ...
                      + 2.0*gm1*ubar*c + 2.0*c*c)/(gamma*gamma - 1.0)    ...
                     + 0.5*u2 );
  else 
    if (mach >= 1.0)
      fluxp1 = rho;
      fluxp2 = rho*u*ubar + p;
      fluxp3 = (e + p)*ubar;
    else
      fluxp1 = 0.0;
      fluxp2 = 0.0;
      fluxp3 = 0.0;
    end
  end

  rho = right(1);
  u = right(2);
  p = right(3);
  u2 = u*u;
  e = p/gm1 + 0.5*rho*u2;
  c = sqrt(gamma*p/rho);

  ubar = u;
  mach = ubar/c;
  ubm2a = -ubar - 2.0*c;

  if (abs(mach) < 1.0)
    fluxm1 = -0.25*rho*c*(mach - 1.0)^2;
    fluxm2 = fluxm1*(ubm2a/gamma + u);
    fluxm3 = fluxm1*((-gm1*ubar*ubar                                     ...
                      - 2.0*gm1*ubar*c + 2.0*c*c)/(gamma*gamma - 1.0)    ...
                     + 0.5*u2 );
  else 
    if (mach >= 1.0)
      fluxm1 = rho;
      fluxm2 = rho*u*ubar + p;
      fluxm3 = (e + p)*ubar;
    else
      fluxm1 = 0.0;
      fluxm2 = 0.0;
      fluxm3 = 0.0;
    end
  end

  f = [ fluxp1+fluxm1 fluxp2+fluxm2 fluxp3+fluxm3 ];
