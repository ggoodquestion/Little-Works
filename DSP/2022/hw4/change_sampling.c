#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PI 3.14159265
#define BUFFER_SIZE 1000

typedef struct HEADERE {
    // RIFF header
    char chunckId[4];
    int chunckSize;
    char fileFormat[4];

    // Wave file header
    char subChunckId1[4];
    int subChunckSize1;
    short int waveFmt;
    short int numChannel;
    int sampleRate;
    int byteRate;
    short int blockAlign;
    short int bitsPerSample;

    // Date file header
    char subChunckId2[4];
    int subChunckSize2;

} Header;

typedef struct STEREO {
    // Two record Stereo.
    // Because use 16bits depth so use short
    short left;
    short right;
} Stereo;

int gcd(int m, int n) {
    while (n != 0) {
        int r = m % n;
        m = n;
        n = r;
    }
    return m;
}

int lcm(int m, int n) {
    return m * n / gcd(m, n);
}

Header readHeader(FILE *fp){
    Header header;

    fread(header.chunckId, 4, sizeof(char), fp);
    fread(&header.chunckSize, 1, sizeof(int), fp);
    fread(header.fileFormat, 4, sizeof(char), fp);
    fread(header.subChunckId1, 4, sizeof(char), fp);
    fread(&header.subChunckSize1, 1, sizeof(int), fp);
    fread(&header.waveFmt, 1, sizeof(short), fp);
    fread(&header.numChannel, 1, sizeof(short), fp);
    fread(&header.sampleRate, 1, sizeof(int), fp);
    fread(&header.byteRate, 1, sizeof(int), fp);
    fread(&header.blockAlign, 1, sizeof(short), fp);
    fread(&header.bitsPerSample, 1, sizeof(short), fp);
    fread(header.subChunckId2, 4, sizeof(char), fp);
    fread(&header.subChunckSize2, 1, sizeof(int), fp);

    return header;
}

void writeHeader(FILE *fp, Header header){\
    fwrite(header.chunckId, 4, sizeof(char), fp);
    fwrite(&header.chunckSize, 1, sizeof(int), fp);
    fwrite(header.fileFormat, 4, sizeof(char), fp);
    fwrite(header.subChunckId1, 4, sizeof(char), fp);
    fwrite(&header.subChunckSize1, 1, sizeof(int), fp);
    fwrite(&header.waveFmt, 1, sizeof(short), fp);
    fwrite(&header.numChannel, 1, sizeof(short), fp);
    fwrite(&header.sampleRate, 1, sizeof(int), fp);
    fwrite(&header.byteRate, 1, sizeof(int), fp);
    fwrite(&header.blockAlign, 1, sizeof(short), fp);
    fwrite(&header.bitsPerSample, 1, sizeof(short), fp);
    fwrite(header.subChunckId2, 4, sizeof(char), fp);
    fwrite(&header.subChunckSize2, 1, sizeof(int), fp);
}

void clearArray(Stereo* a, int size) {
    for (int i = 0; i < size; i++) {
        a[i].left = 0;
        a[i].right = 0;
    }
}

void copyArray(Stereo* a, Stereo* b, int size) {
    for (int i = 0; i < size; i++) {
        a[i].left = b[i].left;
        a[i].right = b[i].right;
    }
}

double hamming(int P, int n) {
    double res = 0.54 - 0.46 * cos((2 * PI * n) / (P - 1));
    return res;
}

void generateFilter(int P, int fc, int N, double* h) {
    // M: order, h: filter, N: sample rate, fc: cutoff frequency
    for (int n = 0; n < P; n++) {
        h[n] = sin(2 * PI * fc * (n - P / 2)) / (PI * (n - P / 2));
        h[n] *= hamming(P, n);
    }
}

void upSampling(Stereo *x, int size_i, int L, Stereo *y) {
    // x: input stereo data, L: Expand rate, size_i: Input data length
    for(int n = 0; n < size_i; n++){
        y[n*L].left = x[n].left * L;
        y[n*L].right = x[n].right * L;
    }
}

void downSampling(Stereo* x, int size_o, int M, Stereo* y) {
    // x: input stereo data, L: Compress rate, size_o: Output data length
    for(int n = 0; n < size_o; n++){
        y[n] = x[n*M];
    }
}
void conv(Stereo* x, double* h, Stereo* y, int P, int size) {
    for (int n = 0; n < size; n++) {
        y[n].left = 0;
        y[n].right = 0;
        for (int k = 0; k < P; k++) {
            double tmp_l, tmp_r;
            if (n - k < 0){
                tmp_l = 0;
                tmp_r = 0;
            }else{
                tmp_l = x[n - k].left;
                tmp_r = x[n - k].right;
            }
            y[n].left += (short)(h[k] * tmp_l);
            y[n].right += (short)(h[k] * tmp_r);
        }
    }
}



int main(int argc, char* argv[]) {
    /*------------ Initialize file ------------*/
    // Varialbe declare.
    char* inputName = argv[1];
    char* outputName = argv[2];
    FILE *f_source, *f_output;

    // bits depth: 16, sample rate: 44100
    f_source = fopen(inputName, "rb");
    f_output = fopen(outputName, "wb");
    /*------------ Initialize file ------------*/

    int inputSize, outputSize, tmpSize; // File size in bytes
    int size_i, size_tmp, size_o; // Number of points
    int srcFs, tarFs, tmpFs;
    int L, M; // L: expand rate, M: compress rate
    int P = 960; // Filter order

    /* Prepare headers and Parameters */
    // Read and make a new header
    printf("Read and prepare.\n");
    Header header_i = readHeader(f_source);
    inputSize = header_i.subChunckSize2;
    srcFs = header_i.sampleRate;
    size_i = inputSize / (header_i.bitsPerSample / 8 * header_i.numChannel);
    
    tarFs = 8000;
    tmpFs = lcm(srcFs, tarFs);
    L = tmpFs / srcFs;
    M = tmpFs / tarFs;
    tmpSize = inputSize * L;
    outputSize = outputSize / M;
    size_tmp = L * size_i;
    size_o = size_tmp / M;

    Header header_o = header_i;
    header_o.subChunckSize2 = outputSize;
    header_o.chunckSize = outputSize + 44;
    header_o.sampleRate = tarFs;
    header_o.byteRate = tarFs * header_o.bitsPerSample * header_o.numChannel / 8;
    writeHeader(f_output, header_o); // Write into file

    double *h = (double *) malloc(sizeof(double)*P);
    generateFilter(P, tarFs/2, tmpFs, h);
 
    /*     Header End     */

    /*    Do the work buffer by buffer    */
    int bufSize_i = P / L;
    int bufSize_o = P / M;
    int frameNum = floor(size_i / bufSize_i); // To avoid memory full, process in frame by frame
    Stereo *buf_i = calloc(bufSize_i, sizeof(Stereo));
    Stereo *buf_p = calloc(P, sizeof(Stereo)); // Previous buffer for tmp
    Stereo *buf_c = calloc(P, sizeof(Stereo)); // Current buffer for tmp
    Stereo *buf_cc = calloc(2*P, sizeof(Stereo)); // concate buffer for tmp
    Stereo *buf_f = calloc(2*P, sizeof(Stereo)); // flitered buffer for tmp
    Stereo *buf_s = calloc(P, sizeof(Stereo)); // sperate from buf_cc
    Stereo *buf_r = calloc(bufSize_o, sizeof(Stereo)); // result of flitered buffer in tar fs
    int currReadNum, remain;
    
    printf("Do it buffer by buffer.\n");
    for(int n = 0; n < frameNum; n++){
        printf("%d\n", n);
        if(n == frameNum - 1){
            remain = size_i - n * bufSize_i;
            currReadNum = (remain % bufSize_i == 0) ? bufSize_i : remain % bufSize_i;
        }else{
            currReadNum = bufSize_i; 
        }
        fread(buf_i, currReadNum, sizeof(Stereo), f_source);
        for(int i = 0; i < bufSize_i; i++){
            // printf("%d, %d\n", buf_i[i].left, buf_i[i].right);
        }
        upSampling(buf_i, bufSize_i, L, buf_c);
        for(int i = 0; i < P; i++){
            buf_cc[i] = buf_p[i];
            buf_cc[P+i] = buf_c[i];
            // printf("%d, %d\n", buf_c[i].left, buf_c[i].right);
        }
        conv(buf_cc, h, buf_f, P, 2*P);
        for(int i = 0; i < P; i++){
            buf_s[i] = buf_f[P+i];
            // printf("%d, %d\n", buf_s[i].left, buf_s[i].right);
        }
        downSampling(buf_s, bufSize_o, M, buf_r);
        // printf("%d, %d\n", buf_r->left, buf_r->right);
        fwrite(buf_r, bufSize_o, sizeof(Stereo), f_output);

        copyArray(buf_p, buf_c, P);
        clearArray(buf_c, P);
    }


    fclose(f_source);
    fclose(f_output);
    printf("finish\n");
}