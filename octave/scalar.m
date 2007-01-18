
n = 10

xi = linspace(0,1,n+1);
x = 0.5*(xi(1:n)+xi(2:n+1));

dh = 0.05
h = 1.0-dh+dh*cos(2*pi*xi);

u = ones(1,n);

exact = 1.0+dh-dh*cos(2*pi*x);

for iter = 1:10
  for subiter = 1:n
    g = scalar_grad(x,u);
    #g = zeros(1,n);
    res = scalar_res(x,xi,h,u,g);
    u = u+res/n;
  end

  [x2, u2] = scalar_draw(x,xi,u,g);

  plot(x2,u2,xi,h);axis([0 1 0 2])

end
res_l2 = norm(res)/n
error_l2 = norm(u-exact)

