clear all;
P = 441;

[x, fs] = audioread("input2.wav");
fi = fs;
fo = 8000;
L = lcm(fi, fo) / fi;
M = lcm(fi, fo) / fo;
f = sinc_filter(P, 4000, fs*L)';
y = up_sampling(x, L);
a = zeros(P+length(y)-1, 2);
a(:,1) = conv(y(:,1), f);
a(:,2) = conv(y(:,2), f);
r = down_sampling(a, M);
audiowrite("output.wav", r, 8000);

F = fft(f);
plot(abs(F));

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