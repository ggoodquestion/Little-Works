s410786004_1
r = roots(data);
vpa(r,4);

for i = 1:length(r)
    z = r(i);
    if abs(z) > 1
        zc = 1/conj(z);
        r(i) = zc;
    end
end

syms c;
p = 1;
for i = 1:length(r)
    p = p * (c - r(i));
end


p = expand(p);
p = sym2poly(p);

data2 = zeros(1,num);
data2(1) = 0;
data2(2:128) = double(p(1,:));

mag2 = zeros(1,128);
ARG2 = zeros(1,128);
arg2 = zeros(1,128);

count = 1;
for w=-1:interval:1
    z = exp(1i*w*pi);
    h = 0;
    for j=1:128
        h = h + data2(j)*z^(1-j);
    end
    
    mag2(count) = abs(h);
    ARG2(count) = angle(h);
    arg2(count) = ARG2(count);
    count = count + 1;
    if(count > 128) 
        break;
    end
end

a = 0;
thres =round(pi,4);
for i=2:num
    arg2(1,i)=arg2(1,i)+2*a*pi;
    x1=arg2(1,i-1);
    x2=arg2(1,i);
    slope=round((x2-x1),4);
    while abs(slope)>=thres
        if abs(slope) > thres
            if slope>0
                a=a-1;
                arg2(1,i)=arg2(1,i)-2*pi;
            else
                a=a+1;
                arg2(1,i)=arg2(1,i)+2*pi;
            end
            x2=arg2(1,i);
            slope=round((x2-x1),4);
        else
            if i < 3
                break;
            end
            if (arg2(1,i)-arg2(1,i-1))*(arg2(1,i-1)-arg2(1,i-2)) < 0
                if arg2(1,i-1)-arg2(1,i-2) < 0
                    a=a-1;
                    arg2(1,i)=arg2(1,i)-2*pi;
                else
                    a=a+1;
                    arg2(1,i)=arg2(1,i)+2*pi;
                end
            end
            break;
        end
    end
end

grd2 = vpa(-diff(arg2),4);
xg = x(1,1:num-1);

figure(2);

subplot(2,2,1);
plot(x, data2);
xlabel("h_{min}[n]");

subplot(2,2,2);
plot(x, mag2);
xlabel("|H_{min}[n]|");

subplot(2,2,3);
plot(x, arg2);
xlabel("arg(H_{min}(z))");

subplot(2,2,4);
plot(xg,grd2);
xlabel("grd(H_{min}(z))");

fp = fopen("64_filter_min.txt", 'w');
fprintf(fp,"%f ", data2);
fclose(fp);