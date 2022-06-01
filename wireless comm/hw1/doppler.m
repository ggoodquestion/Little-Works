clear all;
N =1024;
M = 511500;
x=1:N;

R = [1 0; 0 1];
L = chol(R);
M1 = randn(N/2,2);
M1 = M1*L;
M1 = M1.';

M2 = randn(N/2,2);
M2 = M2*L;
M2 = M2.';

X1(:,1:N/2) = M1(1,:)+j*M1(2,:);
X1(:,N/2+1:N) = fliplr(conj(M1(1,:)+j*M1(2,:)));
X2(:,1:N/2) = M2(1,:)+j*M2(2,:);
X2(:,N/2+1:N) = fliplr(conj(M2(1,:)+j*M2(2,:)));


corr(M1(1,:).', M1(2,:).')

%plot pdf
figure(1);
xlabel('db');
ylabel('Density');
title('Rayleigh Fading PDF Approximation')
hold on
edge=[-80:0.2:20];
R=db2mag(edge);
sigma = 1;
ray = raylpdf(R, sigma);
plot(edge, ray);

C=1;
f = [0.1 0.01 0.001];
for fm=f
    S=zeros(1,N);
    i = 1;
    for fd=-fm:2*fm/(N-1):fm
        S(i)=C/(2*pi*fm*sqrt(1-(fd/fm)^2));
        if S(i) > 1000
            S(i) = 1000;
        end
        i=i+1;
    end
    S = sqrt(S);
    %plot(x,S);
    %fgn1=fft(gn1);
    
    f1=X1.*S;
    f2=X2.*S;
    
    fft1=abs(ifft(f1));
    fft2=abs(ifft(f2));
    
    fr1=fft1.^2;
    fr2=fft2.^2;
    r = sqrt(fr1+fr2);
    
    edge=[-80:1:20];
    data = mag2db(abs(r));
    h=histogram(data, edge, 'Normalization','pdf');
    value = h.Values*7;
    bin = h.BinEdges;
    bin=bin(1:length(bin)-1);
    plot(bin, value);

    
end
legend("Theoretical Ray. PDF","hist fm=0.1","fm=0.1","hist fm=0.01", "fm=0.01", "hist fm=0.001", "fm=0.001");

hold off

% plot amp response
figure(2);
plot(x, data);
title('Correlated Rayleigh magnitude response');
xlabel('sample');
ylabel('db');

%plot phase response
figure(3);
plot(angle(fft(r)));
title('Correlated Rayleigh phase response');
xlabel('db');
ylabel('degree');
