clear all;
N = 1024;
x=1:N;

R = [1 0.75; 0.75 1];
L = chol(R);
M1 = randn(N,2);
M1 = M1*L;
M1 = M1.';

M2 = randn(N,2);
M2 = M2*L;
M2 = M2.';

x1 = abs(M1(1,:)+j*M1(2,:));
x2 = abs(M2(1,:)+j*M2(2,:));

plot(x, x1);