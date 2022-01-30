set = [1, 4, 16, 64, 256, 512, 1024, 2048];
for i=1:8
    fn = sprintf('%d%s', set(i), '_filter.txt');
    data = load(fn);
    x = linspace(0, 16000, 2*set(i));
    title = sprintf("M=%d", set(i));
    subplot(4, 2, i);
    plot(x, data);
    xlabel(title);
end


