function [x2,u2] = scalar_draw(x,xi,u,g)

   n = length(u);

   x2 = zeros(1,2*n);
   u2 = zeros(1,2*n);

   for i = 1:n

     x2(i*2-1) = xi(i);
     dx = xi(i)-x(i);
     u2(i*2-1) = u(i)+g(i)*dx;

     x2(i*2) = xi(i+1);
     dx = xi(i+1)-x(i);
     u2(i*2) = u(i)+g(i)*dx;

   end
