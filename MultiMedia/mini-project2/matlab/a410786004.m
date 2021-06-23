allTxtFile = dir('*.txt');
%fileNames = allTxtFile(:).name;
i = 1;
for n = 1:10
    figure(n)
    for k = 1:4
        subplot(2,2,k)
        imagesc(load(allTxtFile(i).name)');
        colorbar
        i = i+1;
    end
end