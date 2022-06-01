function y = mydft(x)
nx=length(x);
y=zeros(size(x));
%% Write your DFT here
for k=1:nx
  y(k)=0;
  for n=1:nx
    y(k)=y(k)+x(n).*exp(-1j.*2.*pi.*(n-1).*(k-1)./nx);
  end
end
