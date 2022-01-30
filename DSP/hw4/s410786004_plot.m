intval=0.001;
num = int64(2/intval);
thres=3000;
magnR=zeros(1, num);
ARG=zeros(1, num);
arg=zeros(1, num);
x=zeros(1, num);
count=0;
for w=-1:intval:1
    count=count+1;
    r=1;
    z=r*exp(1i*w*pi);
    zp=z^-1;

    qa=0.98*exp(1i*0.8*pi);
    qb=0.98*exp(-1i*0.8*pi);

    pa=0.8*exp(1i*0.4*pi);
    pb=0.8*exp(-1i*0.4*pi);

    k=1;

    H1=((1-qa*zp)*(1-qb*zp))/((1-pa*zp)*(1-pb*zp));
    H2=1;
    for k=1:4
        ck=0.95*exp(1i*(0.15*pi+0.02*pi*k));
        ckp=conj(ck);
        H2=H2*(((ckp-zp)*(ck-zp))/((1-ck*zp)*(1-ckp*zp)))^2;
    end
    H=H1*H2;

    x(1,count)=w;
    magnR(1,count)=abs(H);
    ARG(1,count)=angle(H);
    arg(1,count)=ARG(1,count);
end

figure(1);
subplot(2,1,1);
plot(x, ARG);

a=0;
r=0;
for i=2:num
    arg(1,i)=arg(1,i)+2*a*pi;
    x1=arg(1,i-1);
    x2=arg(1,i);
    slope=(x2-x1)/intval;
    while abs(slope)>thres
        if slope>0
            a=a-1;
            arg(1,i)=arg(1,i)-2*pi;
        else
            a=a+1;
            arg(1,i)=arg(1,i)+2*pi;
        end
        x2=arg(1,i);
        slope=(x2-x1)/intval;
    end
end

subplot(2,1,2);
plot(x, arg);

grd=-diff(arg);
grd=grd(1,1:num-1);
xg=x(1,1:num-1);

figure(2);
subplot(2,1,1);
plot(x, magnR);
subplot(2,1,2);
plot(xg, grd);

x=1:300;
y=zeros(1,300);
count=1;

w1=0.8*pi;
w2=0.2*pi;
w3=0.4*pi;

n=1:60;
h=hann(60);
w1=w1*n;
w2=w2*n;
w3=w3*n;
c1=cos(w1);
c2=cos(w2);
c3=cos(w3);
for i=1:60
    c1(1,i)=c1(1,i)*h(i,1);
    c2(1,i)=c2(1,i)*h(i,1);
    c3(1,i)=c3(1,i)*h(i,1);
end
for i=1:60
    y(1,count)=c1(1,i);
    y(1,count+60)=c2(1,i);
    y(1,count+120)=c3(1,i);
    count=count+1;
end
%plot(x,y);

sum=zeros(1,300);

for k=1:300
    sr=0;
    si=0; 
    for n=1:300
        sr=sr+y(1,n)*cos(2*pi*k*n/300);
        si=si-y(1,n)*sin(2*pi*k*n/300);
    end
    res=sqrt(sr^2+si^2);
    sum(1,k)=20*log10(res);
end
figure(3);
subplot(2,1,1);
plot(x, y);
subplot(2,1,2);
plot(2*x/300, sum);

filter=zeros(1,300);
s410786004_lccde;

filter(1,1)=y(1,1);
for i=2:300
    for j=1:19
        if i<=j
            break;
        else
            filter(1,i)=filter(1,i)-a(1,j)*filter(1,i-j+1)+b(1,j)*y(1,i-j+1);
        end

    end
end

x=1:300;

figure(4);
plot(x, filter);
