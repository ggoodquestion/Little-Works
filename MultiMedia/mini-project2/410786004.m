allTxtFile = dir('*.txt');
i=5;
q = 1;
for k = 1:40
    if i == 5
        figure((k+3)/4);
        i = 1;
        q = mod(q+1, 2);
    end
    if(q == 1)
        f = 4000;
        q = 1;
    else
        f = 8000;
        q = 0;
    end
    a = load(allTxtFile(k).name)';
    s = size(a);        
    subplot(2,2,i);
    imagesc([1 s(2)],[0  f],a);
    xlabel('Frame Index (m)')
    ylabel('Frequency (Hz)')
    cb = colorbar;
    cb.Title.String = 'Frequnecy Amplitude (db)';
    title(allTxtFile(k).name);
    i = i+1;
end