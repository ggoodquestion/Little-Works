clear all;
close all;

M = [1 4 16 64 256 512 1024 2048];
f = [0 110 220 440 880 1760 3520 7040];

for i=1:length(M)
    h = gene_filter(M(i), 4000, 16000);
    tmp = fft(h);

    %% Flip spectualgram
    H(1:M(i))=tmp(M(i)+1:2*M(i));
    H(M(i)+1:2*M(i))=tmp(1:M(i));
    H  = abs(H);

    %% Plot
    figure(1);
    subplot(4,2,i);
    plot(h);
    
    title("h[n]", sprintf("M=%d", M(i)));

    figure(2)
    subplot(4,2,i);
    x = linspace(-pi, pi, 2*M(i));
    plot(x,H);
    title("H(e^{j\omega})",sprintf("M=%d", M(i)));
    set(gca,'XTick',-pi:pi/2:pi);
    set(gca,'XTickLabel',{'-\pi','-\pi/2','0','\pi/2','\pi'});

    %% Filtering
    for j=1:length(f)
        wav = sprintf("./wav/wave_%dHz.wav", f(j));
        o_wav = sprintf("./export/filter_%dHz_%d.wav", f(j), M(i));
        [x, fs] = audioread(wav);
        y = conv(x, h);
        audiowrite(o_wav, y, fs);
    end
end



   