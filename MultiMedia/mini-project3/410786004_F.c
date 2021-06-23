#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<io.h>
#include<fcntl.h>
#include<string.h>

#define PI 3.14159265
#define BUFFER_SIZE 1000

typedef struct ImagineNumber ImgNum;

double **cosTable;
double **sinTable;
double *window;
//Used to record spectrogram.
ImgNum **freqField_l;
ImgNum **freqField_r;
//Global values for frame number & K(i.e N).
int fNum, K;

typedef struct WAVE_HEADERE{
    //RIFF header
    char chunckId[4];
    int chunckSize;
    char fileFormat[4];

    //Wave file header
    char subChunckId1[4];
    int subChunckSize1;
    short int waveFmt;
    short int numChannel;
    int sampleRate;
    int byteRate;
    short int blockAlign;
    short int bitsPerSample;

    //Date file header
    char subChunckId2[4];
    int subChunckSize2;

}WaveHeader;

typedef struct WAVE{
    WaveHeader header;
    char *data;
    long long int index; //Record the index to store when adding sample
    long long int size; //Data size
    long long int sampleNum; //Number of samples

}Wave;

typedef struct spectroSetting{ // Used to record analize window size, window type, DFT window size and frame interval
    int anaWindowSize;
    char windowType;
    int DFTWindowSize; //N
    int frameInterval; //M
}Setting;

typedef struct ImagineNumber{ // A struct to store the real and imagine part of imagine number
    double real;
    double imagine;
}ImgNum;

typedef struct STEREO{ // Two record Stereo.
    short left;
    short right;
}Stereo;

void initCosNSinTable(int N){
    // Because call cos, sin function any interate is too slow, 
    // so I make two Table to record correspond cos and sin value by k and n
    // It make the proformance better
    cosTable = (double **) malloc(N * sizeof(void *));
    sinTable = (double **) malloc(N * sizeof(void *));
    for(int i = 0; i < N; i++){
        cosTable[i] = (double *) malloc(N * sizeof(double));
        sinTable[i] = (double *) malloc(N * sizeof(double));
    }
    for(int k = 0; k < N; k++){
        for(int n = 0; n < N; n++){
            cosTable[k][n] = cos((2*PI*k*n)/N);
            sinTable[k][n] = sin((2*PI*k*n)/N);
        }
    }
}

void windowFunction(char type, int lBound, int hBound, int N){
    //'r' for rectangular, 'm' for Hamming, 'n' for Hanning
    // Make window be a table to reduce the times to execute cos function
    window = (double *) malloc(N * sizeof(double));
    for(int  i = 0; i < hBound; i++){
        if((i >= lBound) && (i < hBound)){
            if(type == 'r'){
                window[i] = 1.0;
            }else if(type == 'm'){
                window[i] = 0.54 - 0.46*cos((2*PI*i)/(hBound-1));
            }else if(type == 'n'){
                window[i] = 0.5-0.5*cos((2*PI*i)/(hBound-1));
            }
        }else{
            window[i] = 0.0;
        }
    }
}

int getFileSize(char fileName[]){ // To calculate the number of bytes of file, below will use this function
    FILE *file;
    file = fopen(fileName, "rb");
    // Use fseek to point out the end of file so that can tell how many bytes that the file has
    fseek(file, 0, SEEK_END); 
    int size = ftell(file);
    return size;
}

void DFT(Setting setting, int sampleRate, Stereo x[], int size){
    printf("DFT\n");
    int N, M, P,frameNum;
    //Transform below window or interval from ms to point
    N = setting.DFTWindowSize * sampleRate / 1000;
    M = setting.frameInterval * sampleRate / 1000;

    //Calculate how many frame need to transform
    frameNum = size / M;
    fNum = frameNum;
    K = N;

    freqField_l = (ImgNum**) malloc(sizeof(void *) * frameNum);
    freqField_r = (ImgNum**) malloc(sizeof(void *) * frameNum);
    for(int i = 0; i < frameNum; i++){
        freqField_l[i] = (ImgNum *) malloc(sizeof(ImgNum) * N);
        freqField_r[i] = (ImgNum *) malloc(sizeof(ImgNum) * N);
    }

    int remain = 0;

    // DFT, first, need to decide which frame to transform
    // Second, give which frequency bin to calculate its db
    // Third, implement the Fourier transform formula
    for(int m = 0; m < frameNum; m++){
        for(int k = 0; k < N; k++){
            for(int n = 0; n < N && (m*M+n < size ); n++){
                // Transform exponential to Euler form
                freqField_l[m][k].real += x[m*M+n].left * window[n] * cosTable[k][n];
                freqField_l[m][k].imagine -= x[m*M+n].left * window[n] * sinTable[k][n];

                freqField_r[m][k].real += x[m*M+n].right * window[n] * cosTable[k][n];
                freqField_r[m][k].imagine -= x[m*M+n].right * window[n] * sinTable[k][n];
            }
        }
    }
}

void LPF(int range, int size, int sampleRate){
    printf("LPF\n");
    int r1, r2;
    //Calculate the freq indexs represent the up bound frequency.
    r1 = ((range * K / sampleRate) - 1);
    r2 = K - r1;

    // Let the high frequency equals to 0;
    for(int i = 0; i < fNum; i++){
        for(int k = 0; k < K; k++){
            if(k > r1 && k < r2){
                ImgNum tmp = {0, 0};
                freqField_l[i][k] = tmp;
                freqField_r[i][k] = tmp;
            }
        }
    }
}

void IDFT(int size, FILE *fp, int f, int sampleRate){
    printf("IDFT\n");
    Stereo *x;
    x = (Stereo*) calloc(size, sizeof(Stereo));
    int M = f * sampleRate / 1000;
    double res_l, res_r;

    ImgNum l_sum = {0, 0}, r_sum = {0, 0};
    for(int m = 0; m < fNum; m++){
        for(int n = 0; n < K; n++){
            l_sum.real = 0;
            l_sum.imagine = 0;
            r_sum.real = 0;
            r_sum.imagine = 0;
            for(int k = 0; k < K; k++){
                // Transform frequency to time.
                // The projuct of two complex number, for example,
                // (a+jb) * (c+jd) = ac -bd + j(ad + bc).
                // But we don't need to care imagine part because when transform to real signal
                // , the imagine will be 0.
                l_sum.real += freqField_l[m][k].real * cosTable[k][n];
                l_sum.imagine += freqField_l[m][k].imagine * sinTable[k][n];

                r_sum.real += freqField_r[m][k].real * cosTable[k][n];
                r_sum.imagine += freqField_r[m][k].imagine * sinTable[k][n];
            }
            x[m*M+n].left += (short) ((l_sum.real - l_sum.imagine) / K);
            x[m*M+n].right += (short) ((r_sum.real - r_sum.imagine) / K);
        }
    }
    fwrite(x, sizeof(Stereo), size, fp);
    free(x);
}

void removeTones(Stereo x[], int sampleRate, int size){
    // Observing first 0.1s of the signal, and substracts itself by this segment
    // every 0.1s to remove the tones.
    int sgp = 0.1 * sampleRate;
    Stereo *tones = (Stereo *) malloc(sizeof(Stereo) * sgp);
    for(int i = 0; i < sgp; i++){
        tones[i] = x[i];
    }

    for(int i = 0; i < size; i++){
        x[i].left -= tones[i % sgp].left;
        x[i].right -= tones[i % sgp].right;
    }
    free(tones);
}

int main(int argc, char *argv[]){
    //Varialbe declare.
    char *sourceName = argv[1];
    char *outputName = argv[2];
    Stereo *d_source;
    FILE *f_source, *f_output;

    f_source = fopen(sourceName, "rb"); //bits depth: 16, sample rate: 48000
    f_output = fopen(outputName, "wb");

    // Read file and get basic infomation.
    WaveHeader w_source;
    int n, N, P;
    n = getFileSize(sourceName) - 44;
    fread(&w_source, 44, 1, f_source);
    d_source = malloc(n);
    fseek(f_source, 44, SEEK_CUR);
    fread(d_source, sizeof(Stereo), n/4, f_source);
    fwrite(&w_source, sizeof(WaveHeader), 1, f_output);

    // Prepare to trnasform.
    Setting setting = {2, 'n', 2, 1}; //Appropriate framing.
    N = setting.DFTWindowSize * w_source.sampleRate / 1000;
    P = setting.anaWindowSize * w_source.sampleRate / 1000;
    initCosNSinTable(N);
    windowFunction(setting.windowType, 0, P, N);

    // First, transform to frequency space.
    // Second, product with LPF.
    // Third, inverse transform to time space.
    removeTones(d_source, w_source.sampleRate, n/4);

    DFT(setting, w_source.sampleRate, d_source, n/4);

    LPF(20000, n/4, w_source.sampleRate);

    IDFT(n/4, f_output, setting.frameInterval, w_source.sampleRate);

    //Free memory.
    fclose(f_output);
    fclose(f_source);
    free(d_source);
    free(freqField_l);
    free(freqField_r);
    free(cosTable);
    free(sinTable);
    free(window);

    printf("finish");
}