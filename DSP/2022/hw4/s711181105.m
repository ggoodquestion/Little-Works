clear all;
P = 440;

[x, fs] = audioread("input3.wav");
fi = 44100;
fo = 8000;
L = lcm(fi, fo) / fi;
M = lcm(fi, fo) / fo;
fc = fs*L;
f = sinc_filter(P, 4000, fc)';

f1 = sinc_filter(P, fc/3/2, fc)';
fc = fc / 3;
f2 = sinc_filter(P, fc/3/2, fc)';
fc = fc / 3;
f3 = sinc_filter(P, fc/7/2, fc)';
fc = fc / 7;
f4 = sinc_filter(P, fc/7/2, fc)';
y = up_sampling(x, L);

% down 3
a = zeros(P+length(y)-1, 2);
a(:,1) = conv(y(:,1), f1);
a(:,2) = conv(y(:,2), f1);
a = down_sampling(a, 3);

% down 3
y = zeros(P+length(a)-1, 2);
y(:,1) = conv(a(:,1), f2);
y(:,2) = conv(a(:,2), f2);
y = down_sampling(y, 3);

% down 7
a = zeros(P+length(y)-1, 2);
a(:,1) = conv(y(:,1), f3);
a(:,2) = conv(y(:,2), f3);
a = down_sampling(a, 7);

% down 7
y = zeros(P+length(a)-1, 2);
y(:,1) = conv(a(:,1), f4);
y(:,2) = conv(a(:,2), f4);
r = down_sampling(y, 7);

audiowrite("output.wav", r, 8000);

F = fft(f);
%plot(abs(F));

function [p]=sinc_filter(M, fc, N)
    h = zeros(M, 1);
    
    for n=1:M
        if n-M/2 == 0
            h(n) = 1;
        else
            h(n)=sin(2*pi*fc*(n-M/2)/N)/((n-M/2)*pi);
        end
    end

    hamm = hamming(M);
    h = h.*hamm;
    p=h;
end

function [y]=up_sampling(x, L)
    len_i = length(x);
    len_o = len_i * L;
    y = zeros(len_o, 2);
    for i=1:len_i
        y(i*L,:) = x(i, :)*L;
    end
end

function [y]=down_sampling(x, M)
    len_i = length(x);
    len_o = ceil(len_i / M);
    y = zeros(len_o, 2);
    for i=1:len_i
        if i*M > len_i
            break;
        end
        y(i,:) = x(i*M, :);
    end
end