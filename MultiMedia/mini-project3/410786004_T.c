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

void LPF(int range, int size, int sampleRate){
    printf("LPF\n");
    int r1, r2;
    //Calculate the freq indexs represent the up bound frequency.
    r1 = ((range * K / sampleRate) - 1);
    r2 = K - r1;

    // Let the high frequency equals to 0;
    for(int i = 0; i < fNum; i++){
        for(int k = 0; k < K; k++){
            if(k >= r1 && k <= r2){
                ImgNum tmp = {0, 0};
                freqField_l[i][k] = tmp;
                freqField_r[i][k] = tmp;
            }
        }
    }
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
    // Setting setting = {2, 'n', 2, 1}; //Appropriate framing.
    // N = setting.DFTWindowSize * w_source.sampleRate / 1000;
    // P = setting.anaWindowSize * w_source.sampleRate / 1000;
    // initCosNSinTable(N);
    // windowFunction(setting.windowType, 0, P, N);

    // First, transform to frequency space.
    // Second, product with LPF.
    // Third, inverse transform to time space.
    removeTones(d_source, w_source.sampleRate, n/4);

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