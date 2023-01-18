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

// void LPF(Complex *l_X, Complex *r_X, int gain, int M, int tmpSampleRate){
//     int cutoff = (M * N / tmpSampleRate)-1;
//     for(int k = 0; k < N; k++){
//         if( k  <= cutoff || k >= N-cutoff ){
//             l_X[k].real *= gain;
//             r_X[k].real *= gain;
//             l_X[k].img *= gain;
//             r_X[k].img *= gain;
//         }else{
//             l_X[k].real = 0;
//             r_X[k].real = 0;
//             l_X[k].img = 0;
//             r_X[k].img = 0;
//         }
//     }
// }

void clearArray(Stereo* a, int size) {
    for (int i = 0; i < size; i++) {
        a[i].left = 0;
        a[i].right = 0;
    }
}

double hamming(int P, int n) {
    double res = 0.54 - 0.46 * cos((2 * PI * n) / (P - 1));
    return res;
}

void generateFilter(int P, int fc, int N, double* h, int gain) {
    // M: order, h: filter, N: sample rate, fc: cutoff frequency
    for (int n = 0; n < P; n++) {
        if(n-P/2 == 0){
            h[n] = 1 * gain;
        }else{
            h[n] = gain * sin(2 * PI * fc * (n - P / 2) / N) / (PI * (n - P / 2));
        }
        // h[n] *= hamming(P, n);
    }
}

void upSampling(Stereo *x, int size_i, int L, Stereo *y) {
    // x: input stereo data, L: Expand rate, size_i: Input data length
    for(int n = 0; n < size_i; n++){
        y[n*L].left = x[n].left * 1;
        y[n*L].right = x[n].right * 1;
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
        double y_l = 0, y_r = 0;
        for (int k = 0; k < P; k++) {
            double tmp_l, tmp_r;
            if (n - k < 0){
                // tmp_l = 0;
                // tmp_r = 0;
                continue;
            }else{
                tmp_l = x[n - k].left;
                tmp_r = x[n - k].right;
            }
            y_l += (h[k] * tmp_l);
            y_r += (h[k] * tmp_r);
        }
        y[n].left = (y_l > 32767) ? 32767 : y_l;
        y[n].right = (y_r > 32767) ? 32767 : y_r;
        // printf("%d, %d\n", y[n].left, y[n].right);
        // printf("%lf, %lf\n", y_l, y_r);
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

    /*------------ Read file start ------------*/
    printf("Read file.\n");
    // Utils varible declare
    int rawSize;  // Wav data size(byte)
    int size;     // Wav data point number
    Stereo* x;

    int currRate;
    int L, M;
    int P = 441; // Filter order

    // Read file and get basic infomation.
    Header header;
    fread(&header, 1, sizeof(Header), f_source);
    rawSize = header.subChunckSize2;
    size = rawSize / (header.bitsPerSample / 8 * header.numChannel);
    currRate = header.sampleRate;

    // Read wav data point;
    x = calloc(size, sizeof(Stereo));
    fread(x, 1, rawSize, f_source);
    fclose(f_source);

    // for(int i = 0; i < size; i++){
    //     printf("%d, %d\n", x[i].left, x[i].right);
    // }

    /*------------ Read file end ------------*/

    /*------------------ Up Sampling -------------------*/
    printf("Up sampling.\n");

    int tmpSampleRate = lcm(8000, header.sampleRate);
    L = tmpSampleRate / currRate;
    int tmpSize = L * size;
    Stereo* x_tmp = (Stereo*) calloc(tmpSize, sizeof(Stereo));

    // Up sampling
    upSampling(x, size, L, x_tmp);

    // for(int i = 0; i < tmpSize; i++){
    //     printf("%d, %d\t", x_tmp[i].left, x_tmp[i].right);
    // }
    free(x);
    /*---------------- Up Sampling End ----------------*/

    /*---------------- Filtering Start ----------------*/
    printf("Filtering.\n");

    // Delcare freq table

    Stereo* x_c = (Stereo*) calloc(tmpSize, sizeof(Stereo));  // Cutoff data
    printf("%d\n", x_c[0]);
    double *h = malloc(P * sizeof(double));
    generateFilter(P, 4000, tmpSampleRate, h, L);
    
    // for(int i = 0; i < P; i++) {
    //     printf("%lf\n", h[i]);
    // }

    conv(x_tmp, h, x_c, P, tmpSize);

    /*---------------- Filtering End ----------------*/

    /*------------------ Down Sampling -------------------*/
    /*             Origin:44.1k -> Target: 8k             */
    printf("Down sampling.\n");
    currRate = 8000;
    M = tmpSampleRate / currRate;
    int outputSize = tmpSize / M;
    printf("%d\n", outputSize);
    Stereo* x_o = calloc(outputSize, sizeof(Stereo));

    downSampling(x_tmp, outputSize, M, x_o);
    // for(int i = 0; i < outputSize; i++) {
    //     printf("%d, %d\n", x_o[i].left, x_o[i].right);
    // }
    
    /*---------------- Down Sampling End ----------------*/

    /*------------ Write file start ------------*/
    header.sampleRate = 8000;
    header.byteRate =
        header.sampleRate * header.numChannel * header.bitsPerSample / 8;
    header.subChunckSize2 =
        outputSize * header.bitsPerSample * header.numChannel / 8;

    fwrite(&header, 1, sizeof(Header), f_output);
    fwrite(x_o, 1, header.subChunckSize2, f_output);
    fclose(f_output);

    /*------------ Write file end ------------*/

    printf("finish\n");
}