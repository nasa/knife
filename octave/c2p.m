function up = c2p(uc)
  
  gamma = 1.4;
  gm1 = gamma-1.0;
  rho = uc(:,1);
  u = uc(:,2)./rho;

  up = [ rho,  u,  gm1.*(uc(:,3)-0.5.*rho.*u.*u) ];
