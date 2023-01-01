function [p]=gene_filter(M, fc, N)

h = zeros(1, M);

fn = sprintf("./filter/filter_%d.txt", M);
fp = fopen(fn, "w");

for i=0:2*M-1
    n = i-M;
    if n == 0
        h(i+1) = 1;
    else
        h(i+1)=sin(2*pi*fc*n/N)/(n*pi);
    end
    fprintf(fp, "%f ", h(i+1));
end

fclose(fp);

p=h;