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

typedef struct TMP_STEREO {
    Stereo *data;
    double sr_times; // Sampling times
    int isClean; // If data being processed
} TmpStereo;

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

void generateFilter(int P, double wc, int N, double* h) {
    // M: order, h: filter, N: sample rate, fc: cutoff frequency
    for (int n = 0; n < P; n++) {
        if(n-P/2 == 0){
            h[n] = h[n-1];
            continue;
        }
        h[n] = sin(wc * (n - P / 2)) / (PI * (n - P / 2));
        h[n] *= hamming(P, n);
        // printf("%lf\n", h[n]);
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
        y[n].left = 0;
        y[n].right = 0;
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
            y[n].left += (short)(h[k] * tmp_l);
            y[n].right += (short)(h[k] * tmp_r);
        }
    }
}

int main(int argc, char* argv[]) {
    /* Target: 44.1k -> 8k */

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
    int P = 256; // Filter order

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

    /*------------ Read file end ------------*/

    /*------------------ Up Sampling -------------------*/
    printf("Up sampling L=80.\n");

    int srcFs = 44100, tarFs = 8000;
    int isClean = 1;
    int dec = 441;
    int tmpSampleRate = lcm(srcFs, tarFs);
    L = 80; // Upsampling 80 times
    int tmpSize = L * size / dec;
    Stereo* x_tmp = (Stereo*) calloc(tmpSize, sizeof(Stereo));
    double *h = malloc(P * sizeof(double));
    generateFilter(P, PI/80, tmpSampleRate, h);

    double LM_tmp_r, LM_tmp_l;
    int x_id;
    for(int i = 0; i < tmpSize; i++){
        LM_tmp_r = 0;        
        LM_tmp_l = 0;
        for(int j = 0; j < P; j++){
            x_id = i * dec - j;
            if(x_id < 0) break;
            
            // printf("%lf\n", h[j]);

            if(x_id % 80 == 0){
                x_id /= 80;
                LM_tmp_r += (double) x[x_id].right * h[j] * L;
                LM_tmp_l += (double) x[x_id].left * h[j] * L;
            }
        }
        // printf("%lf, %lf\n", LM_tmp_r, LM_tmp_l);
        
        x_tmp[i].left = (short) LM_tmp_l;
        x_tmp[i].right = (short) LM_tmp_r;
    }


    /*---------------- Filtering End ----------------*/

    /*------------------ Down Sampling -------------------*/
    /*             Origin:44.1k -> Target: 8k             */
    // printf("Down sampling.\n");
    // currRate = 8000;
    // M = tmpSampleRate / currRate;
    // int outputSize = tmpSize / M;
    // printf("%d\n", outputSize);
    // Stereo* x_o = calloc(outputSize, sizeof(Stereo));

    // downSampling(x_tmp, outputSize, M, x_o);
    
    /*---------------- Down Sampling End ----------------*/

    /*------------ Write file start ------------*/
    header.sampleRate = 8000;
    header.byteRate =
        header.sampleRate * header.numChannel * header.bitsPerSample / 8;
    header.subChunckSize2 =
        tmpSize * header.bitsPerSample * header.numChannel / 8;

    fwrite(&header, 1, sizeof(Header), f_output);
    fwrite(x_tmp, 1, header.subChunckSize2, f_output);
    fclose(f_output);

    /*------------ Write file end ------------*/

    printf("finish\n");
}