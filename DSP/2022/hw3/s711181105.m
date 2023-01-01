clear;
close all;
N = 2000;
w = linspace(-1*pi, pi, N);

%% Implement function
z = exp(1i*w);
zp = z.^-1;

qa = 0.98*exp(1i*0.8*pi);
qb = 0.98*exp(-1i*0.8*pi);
pa = 0.8*exp(1i*0.4*pi);
pb = 0.8*exp(-1i*0.4*pi);

H1 = (1-qa*zp).*(1-qb*zp)./((1-pa*zp).*(1-pb*zp));

H2 = ones(1, N);
for k = 1:4
    ck = 0.95*exp(1i*(0.15*pi+0.02*k*pi));
    ckp = conj(ck);
    H2 = H2.*(((ckp-zp).*(ck-zp)./((1-ck*zp).*(1-ckp*zp))).^2);
end

%% ARG
H = H1.*H2;
ARG = angle(H);
figure(1);
subplot(2,1,1);
plot(w,ARG);  
set(gca,'XTick',-pi:0.2*pi:pi);
set(gca,'XTickLabel',{'-\pi','-0.8\pi','-0.6\pi','-0.4\pi','-0.2\pi', ...
                        '0','0.2\pi','0.4\pi','0.6\pi','0.8\pi','\pi'});
%% arg
arg = ARG;
step = 0;
interval = 2*pi/N;
thres = 1000;
for i=2:N
    slope = (ARG(i)-ARG(i-1))/interval;
    if(slope > thres)
        if(slope > 0)
            step = step - 1;
        elseif(slope < 0)
            step = step + 1;
        end
    end
    arg(i) = step*2*pi+arg(i);
end
subplot(2,1,2);
plot(w,arg);
ylim([-130,30]);
set(gca,'XTick',-pi:0.2*pi:pi);
set(gca,'XTickLabel',{'-\pi','-0.8\pi','-0.6\pi','-0.4\pi','-0.2\pi', ...
                        '0','0.2\pi','0.4\pi','0.6\pi','0.8\pi','\pi'});
%% grd
grd = -diff(arg);
figure(2);
subplot(2,1,1);
plot(w(1:N-1),grd);
set(gca,'XTick',-pi:0.2*pi:pi);
set(gca,'XTickLabel',{'-\pi','-0.8\pi','-0.6\pi','-0.4\pi','-0.2\pi', ...
                        '0','0.2\pi','0.4\pi','0.6\pi','0.8\pi','\pi'});

%% Magintude
magR = abs(H);
subplot(2,1,2);
plot(w,magR);
set(gca,'XTick',-pi:0.2*pi:pi);
set(gca,'XTickLabel',{'-\pi','-0.8\pi','-0.6\pi','-0.4\pi','-0.2\pi', ...
                        '0','0.2\pi','0.4\pi','0.6\pi','0.8\pi','\pi'});
%% Make wave
n = 1:60;
hamm = hanning(60)';
x1 = hamm.*cos(0.8*pi*n);
x2 = hamm.*cos(0.2*pi*n);
x3 = hamm.*cos(0.4*pi*n);
x4 = zeros(1,120);
x = cat(2,x1,x2,x3,x4);

figure(3);
subplot(2,1,1);
plot(x);
%% DTFT
K =300; % For total 300 point
F = zeros(1,K);

for n=1:K
    tmp = 0;
    for k=1:K
        tmp = tmp + x(k) * exp(-1i*2*pi*n*k/K);
    end
    F(n) = 20*log10(abs(tmp));
end

% Flip spectrogram
F_tmp(1:150) = F(151:300);
F_tmp(151:300) = F(1:150);
F = F_tmp;
w = linspace(-1*pi, pi, K);

subplot(2,1,2);
plot(w,F);
set(gca,'XTick',-pi:0.2*pi:pi);
set(gca,'XTickLabel',{'-\pi','-0.8\pi','-0.6\pi','-0.4\pi','-0.2\pi', ...
                        '0','0.2\pi','0.4\pi','0.6\pi','0.8\pi','\pi'});
%% Implement LCCDE
a1 = [1 -0.98*exp(1i*0.8*pi)];
a2 = [1 -0.98*exp(-1i*0.8*pi)];

b1 = [1 -0.8*(1i*0.4*pi)];
b2 = [1 -0.8*(-1i*0.4*pi)];

a = conv(a1,a2);
b = conv(b1,b2);

for k=1:4
    ck = 0.95*exp(1i*(0.15*pi+0.02*pi*k));
    ckp = ck';

    c1 = [ckp -1];
    c2 = [ck -1];
    at = conv(c1, c2);
    at = conv(at, at);
    a = conv(a, at);

    d1 = [1 -ck];
    d2 = [1 -ckp];
    bt = conv(d1, d2);
    bt = conv(bt, bt);
    b = conv(b, bt);
end

% Set y[0] at left of equation
a = a/b(1);
b = -b/b(1);
b(1) = 0;

%% Utilize LCCDE
y = zeros(1,300);
for i=1:300
    % Input part
    for j=1:length(a)
        if i<j
            continue
        end
        y(i) = a(j)*x(i-j+1) + y(i);
    end

    % Output part
    for j=1:length(b)
        if i<j
            continue
        end
        y(i) = b(j)*y(i-j+1) + y(i);
    end
end

figure(4);
subplot(2,1,1);
plot(y);