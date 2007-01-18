function res = scalar_res(x,xi,h,u,g)

  uinf = 1;

  n = length(u);
  res = zeros(size(u));

  res(1) =  h(1)*uinf;
  dx = xi(n+1)-x(n);
  res(n) = -h(n+1)*(u(n)+g(n)*dx);

  for i = 1:n-1
    dx = xi(i+1)-x(i);
    left = u(i)+g(i)*dx;
    dx = xi(i+1)-x(i+1);
    right = u(i+1)+g(i+1)*dx;
    uavg = sqrt(left+right);
    flux = 0.5*(left+right-(right-left));
    res(i)   = res(i)   - h(i)*flux;
    res(i+1) = res(i+1) + h(i)*flux;
  end


