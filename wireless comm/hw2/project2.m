clear;
close all;

% Parameters
lambda  = [0.2 1 1.5];
delta   = [100 250 400];
mu      = [0.005 0.025 0.075];
N       = 500;
M       = 11;

% Store the value for compare
% error, MSE
e_RLS   = RLS(1,250,N,M,true); 
e_LMS   = LMS(0.05,N,M,true);

e_RLS_no_noise   = RLS(1,250,N,M,false); 
e_LMS_no_noise   = LMS(0.05,N,M,false);

% result
e_RLS_delta1 = RLS(1,delta(1),N,M,true); 
e_RLS_delta2 = RLS(1,delta(2),N,M,true); 
e_RLS_delta3 = RLS(1,delta(3),N,M,true); 

e_RLS_lambda1 = RLS(lambda(1),250,N,M,true); 
e_RLS_lambda2 = RLS(lambda(2),250,N,M,true); 
e_RLS_lambda3 = RLS(lambda(3),250,N,M,true);  

e_LMS_mu1 = LMS(mu(1),N,M,true); 
e_LMS_mu2 = LMS(mu(2),N,M,true); 
e_LMS_mu3 = LMS(mu(3),N,M,true); 


x = 1:N; % xaxis

% Compare RLS & LMS
figure(1);
semilogy(x,e_RLS);
hold on;
semilogy(x,e_LMS);
legend('RLS','LMS');

title("Learning curve E for LMS and RLS algorithm");
subtitle("\lambda=1 \delta=250 \mu=0.05")
xlabel("time step n", "FontSize", 12);
ylabel("Output error E in db", "FontSize", 12);

% Compare RLS & LMS without noise
figure(2);
semilogy(x,e_RLS_no_noise);
hold on;
semilogy(x,e_LMS_no_noise);
legend('RLS','LMS');

title("Learning curve E for LMS and RLS algorithm without noise");
subtitle("\lambda=1 \delta=250 \mu=0.05")
xlabel("time step n", "FontSize", 12);
ylabel("Output error E in db", "FontSize", 12);

% Compare RLS in delta
figure(3);
semilogy(x,e_RLS_delta1);
hold on;
semilogy(x,e_RLS_delta2);
hold on;
semilogy(x,e_RLS_delta3);
hold off

legend('\delta =100','\delta =250',"\delta =400");
title("Learning curve E for RLS algorithm in different \delta");
xlabel("time step n", "FontSize", 12);
ylabel("Output error E in db", "FontSize", 12);

% Compare RLS in lambda
figure(4);
semilogy(x,e_RLS_lambda1);
hold on;
semilogy(x,e_RLS_lambda2);
hold on;
semilogy(x,e_RLS_lambda3);
hold off

legend('\lambda =0.2','\lambda =0.5',"\lambda =1");
title("Learning curve E for RLS algorithm in different \lambda");
xlabel("time step n", "FontSize", 12);
ylabel("Output error E in db", "FontSize", 12);

% Compare LMS in mu
figure(5);
semilogy(x,e_LMS_mu1);
hold on;
semilogy(x,e_LMS_mu2);
hold on;
semilogy(x,e_LMS_mu3);
hold off

legend('\mu =0.005','\mu =0.025',"\mu =0.075");
title("Learning curve E for LMS algorithm in different \mu");
xlabel("time step n", "FontSize", 12);
ylabel("Output error E in db", "FontSize", 12);

%k       = (lambda^-1 * P*uu)/(1 + lambda^-1 * uu'*P*uu);
%alpha   = d(n) - uu'*w;
%w       = w + k*alpha;
%P       = lambda^-1 * P - lambda^-1 * k*uu'*P;
%P       = 1/(lambda + uu'*P*uu) * (P-P*uu*uu'*P);
%w       = w + P*uu*(d(n) - w'*uu);