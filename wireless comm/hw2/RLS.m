function [p] = RLS(lambda,delta,N,M,noise)
% Parameters
W       = 3.1;
%M       = 11;   % size of equalizer
%N       = 500;  % training step, size of signal
sigma   = sqrt(0.001);

% doppler channel
%C=1;
%fm=0.001;
%i = 1;
%for fd=-fm:2*fm/(3-1):fm
%    S(i)=C/(2*pi*fm*sqrt(1-(fd/fm)^2));
%    if S(i) > 100
%        S(i) = 100;
%    end
%    i=i+1;
%end
%h       = ifft(S);
%a       = abs(randn(N,1)+j*randn(N,1));

% Signal and Channel
a       = randn(N,1); % source signal Tx1 matrix
h       = 1/2*(1+cos(2*pi/W*((1:3)-2))); % Channel
v       = sigma * randn(N+3-1,1); % noise
d       = zeros(N,1); % desired signal    
e       = zeros(N,1); % error, MSE

if(noise==true)
    u       = conv(a,h) + v; % received signal
else
    u       = conv(a,h); % received signal
end

d(8:N)= a(1:N-7); % delay 7 unit

% RLS
% Initialize
w       = zeros(M,1); % equalizer
P       = delta * eye(M,M); 

for n=M:N
    uu      = u(n-M+1:n);   

    % Calculate error
    e(n) = abs(d(n) - w'*uu);
    
    PI      = uu'*P;
    gamma   = lambda + PI*uu;
    k       = PI'/gamma;
    alpha   = d(n) - w'*uu;
    w       = w + k*alpha;
    pp      = k*PI;
    P       =(P-pp)/lambda;
end
p=e;