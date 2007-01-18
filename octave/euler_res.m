function res = euler_res(x,xi,h,cu)

  gamma = 1.4;
  gm1  = gamma - 1.0;
  rho0 = 1.0;
  c0 = 1.0;
  u0 = 0.3;
  p0 = rho0*c0*c0 / gamma

  n = length(cu);
  res = zeros(size(cu));

  up = c2p(cu);
  
  f = [ rho0*u0 (rho0*u0*u0+up(1,3)) u0*(uc(1,3)+up(1,3))];
  res(1,:) = res(1,:) - h(1).*f;

  rho = up(n,1)
  u   = up(n,2)
  f = [ rho*u (rho*u*u+p0) u*(p0+p0/gm1+0.5*rho*u*u)];
  res(n,:) = res(n,:) + h(n+1).*f;

  for i = 1:n-1
    left = u(i,:)
    right = u(i+1,:)
    flux = roe_flux(left,right);
    res(i  ,:) = res(i  ,:) - h(i).*flux;
    res(i+1,:) = res(i+1,:) + h(i).*flux;
  end


