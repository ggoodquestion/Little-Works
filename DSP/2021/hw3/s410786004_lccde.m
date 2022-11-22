a1=[1 -0.8*exp(1i*0.4*pi)];
a2=[1 -0.8*exp(-1i*0.4*pi)];

b1=[1 -0.98*exp(1i*0.8*pi)];
b2=[1 -0.98*exp(-1i*0.8*pi)];

a=conv(a1,a2);
b=conv(b1,b2);

for k=1:4
    ck=0.95*exp(1i*(0.15*pi+0.02*pi*k));
    ckp=ck';
    ak1=[1 -ck];
    ak2=[1 -ckp];

    bk1=[ckp -1];
    bk2=[ck -1];

    at=conv(ak1,ak2);
    at=conv(at,at);
    a=conv(a,at);

    bt=conv(bk1, bk2);
    bt=conv(bt,bt);
    b=conv(b,bt);
end

a=a/a(1,1);
b=b/a(1,1);
a(1,1)=0;

