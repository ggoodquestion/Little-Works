clear all;
M = 120;
f = sinc_filter(M, 4000, 16000);

[x, fs] = audioread("input.wav");
fi = fs;
fo = 8000;
L = lcm(fi, fo) / fi;
M = lcm(fi, fo) / fo;
y = up_sampling(x, L);
r = down_sampling(y, M);

function [p]=sinc_filter(M, fc, N)
    h = zeros(1, M);
    
    for i=0:2*M-1
        n = i-M;
        if n == 0
            h(i+1) = 1;
        else
            h(i+1)=sin(2*pi*fc*n/N)/(n*pi);
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
    len_o = len_i / M
    y = zeros(len_o, 2);
    for i=1:len_i
        y(i,:) = x(i*M, :)*L;
    end
end