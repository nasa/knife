function grad = scalar_grad(x,u)

  n = length(u);
  grad = zeros(size(u));

  grad(1) =  (u(1  )-u(2)) / (x(1  )-x(2));
  grad(n) =  (u(n-1)-u(n)) / (x(n-1)-x(n));

  for i = 2:n-1
    grad(i) = grad(i) + 0.5*(u(i-1)-u(i)) / (x(i-1)-x(i));
    grad(i) = grad(i) + 0.5*(u(i)-u(i+1)) / (x(i)-x(i+1));
  end


