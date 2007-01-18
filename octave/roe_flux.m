function f = roe_flux(left,right)

  rhol = left(1);
  ul = left(2);
  pl = left(3);
  el = pl/gm1 + 0.5*rhol*ul*ul;
  hl = (pl+el)/rhol;

  rhor = right(1);
  ur = right(2);
  pr = right(3);
  er = pr/gm1 + 0.5*rhor*ur*ur;
  hr = (pr+er)/rhor;

  rho = sqrt(rhol*rhor);
  wl = rho/(rho+rhor);
  wr = 1.0 - wl;
  u = wl*ul + wr*ur; 
  u2 = u*u;
  h = wl*hl+wr*hr;
  c = sqrt(gm1*(h-0.5*q2));
