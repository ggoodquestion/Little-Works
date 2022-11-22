clear
fn = sprintf('%d%s', 64, '_filter.txt');
data = load(fn);
x = linspace(-8000, 8000, 2*64);
title = sprintf("M=%d", 64);
figure(1);
subplot(2,2,1);
plot(x, data);
xlabel("h[n]");

interval = 1/64;
magnitude = zeros(1,128);
ARG = zeros(1,128);
arg = zeros(1,128);
count = 1;
for w=-1:interval:1
    z = exp(1i*w*pi);
    h = 0;
    for j=1:128
        h = h + data(j)*z^(1-j);
    end
    
    magnitude(count) = abs(h);
    ARG(count) = angle(h);
    arg(count) = ARG(count);
    count = count + 1;
    if(count > 128) 
        break;
    end
end

num = 128;
a = 0;
thres =round(pi,4);
pre_slope = 0;
for i=2:num
    arg(1,i)=arg(1,i)+2*a*pi;
    x1=arg(1,i-1);
    x2=arg(1,i);
    slope=round((x2-x1),4);
    while abs(slope)>=thres
        if abs(slope) > thres
            if slope>0
                a=a-1;
                arg(1,i)=arg(1,i)-2*pi;
            else
                a=a+1;
                arg(1,i)=arg(1,i)+2*pi;
            end
            x2=arg(1,i);
            slope=round((x2-x1),4);
        else
            if i < 3
                break;
            end
            if (arg(1,i)-arg(1,i-1))*(arg(1,i-1)-arg(1,i-2)) < 0
                if arg(1,i-1)-arg(1,i-2) < 0
                    a=a-1;
                    arg(1,i)=arg(1,i)-2*pi;
                else
                    a=a+1;
                    arg(1,i)=arg(1,i)+2*pi;
                end
            end
            break;
        end
    end
end

grd = round(-diff(arg),4);
xg = x(1,1:num-1);

subplot(2,2,2);
plot(x, magnitude);
xlabel("|H_M(e^{jw})|");

subplot(2,2,3);
plot(x, arg);
xlabel("arg")

subplot(2,2,4);
plot(xg, grd);
xlabel("grd");

