#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<io.h>
#include<fcntl.h>
#include<string.h>

#define PI 3.14159265
#define BUFFER_SIZE 1000

double **cosTable;
double **sinTable;
double *window;

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

typedef struct WAVE_INFO{ // Used to record the sample, frequency, file name of that 8 cos waves
    int sampleRate;
    int freq;
    char fn[3];
}WaveInfo;

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

WaveHeader makeHeader(int sampleRate, int sampleSize){ //Sign the needed information
    WaveHeader wh;

    wh.chunckId[0] = 'R';
    wh.chunckId[1] = 'I';
    wh.chunckId[2] = 'F';
    wh.chunckId[3] = 'F';

    wh.fileFormat[0] = 'W';
    wh.fileFormat[1] = 'A';
    wh.fileFormat[2] = 'V';
    wh.fileFormat[3] = 'E';

    wh.subChunckId1[0] = 'f';
    wh.subChunckId1[1] = 'm';
    wh.subChunckId1[2] = 't';
    wh.subChunckId1[3] = ' ';

    wh.subChunckSize1 = 16;
    wh.waveFmt = 1;
    wh.numChannel = 1;
    wh.sampleRate = sampleRate;
    wh.bitsPerSample = sampleSize;
    wh.byteRate = (int)sampleRate * sampleSize / 8;
    wh.blockAlign = (int)sampleSize / 8;

    wh.subChunckId2[0] = 'd';
    wh.subChunckId2[1] = 'a';
    wh.subChunckId2[2] = 't';
    wh.subChunckId2[3] = 'a';

    wh.chunckSize = 36;
    wh.subChunckSize2 = 0;

    return wh;
}

Wave makeWave(int sampleRate, int sampleSize){
    Wave wave;
    wave.header = makeHeader(sampleRate, sampleSize);
    return wave;
}

void giveWaveDuration(Wave *wave, double second){
    long long int totalBytes = (long long int) (second * wave->header.byteRate);
    
    //initialize
    wave->data = (char*)malloc(totalBytes);
    wave->index = 0;
    wave->size = totalBytes;
    wave->sampleNum = (long long int)wave->header.sampleRate * second;
    wave->header.subChunckSize2 = totalBytes;
    wave->header.chunckSize = totalBytes + 36;
}

void waveAddASample(Wave *wave, double x){ //Add a sample by bits respectively
    //Do pcm quantization
    char *sample;
    //Use char as a byte storage to store data of short int(2 bytes), int(2 bytes), long int(4 bytes)
    if(wave->header.bitsPerSample == 8){
        short int sample8bits = (short int) (127+x);
        //Allocate memory address
        sample = (char*)&sample8bits;
        (wave->data)[wave->index] = sample[0];
        (wave->data)[wave->index] = sample[1];
        wave->index += 1;
    }else if(wave->header.bitsPerSample == 16){
        int sample16bits = (int) (x);
        //Allocate memory address
        sample = (char*)&sample16bits;
        (wave->data)[wave->index] = sample[0];
        (wave->data)[wave->index + 1] = sample[1];
        wave->index += 2;
    }else if(wave->header.bitsPerSample == 32){
        long int sample32bits = (long int) (x);
        //Allocate memory address
        sample = (char*)&sample32bits;
        (wave->data)[wave->index] = sample[0];
        (wave->data)[wave->index + 1] = sample[1];
        (wave->data)[wave->index + 2] = sample[2];
        (wave->data)[wave->index + 3] = sample[3];
        wave->index += 4;
    }
}

void initCosNSinTable(int K, int N){
    // Because call cos, sin function any interate is too slow, 
    // so I make two Table to record correspond cos and sin value by k and n
    // It make the proformance better
    cosTable = (double **) malloc(K * sizeof(void *));
    sinTable = (double **) malloc(K * sizeof(void *));
    for(int i = 0; i < N; i++){
        cosTable[i] = (double *) malloc(N * sizeof(double));
        sinTable[i] = (double *) malloc(N * sizeof(double));
    }
    for(int k = 0; k < K; k++){
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

void makeFileName(char tmp[20], char fn[3], int sr){ // To easily make file name of the 8 cos waves
    if(sr == 16000){
        sprintf(tmp, "cos_%sHz-16k.wav", fn);
    }else{
        sprintf(tmp, "cos_%sHz-8k.wav", fn);
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

void DFT(Setting setting, int sampleRate, short int x[], int size, int setIndex, char fileName[]){
    int N, M, P,frameNum;
    //Transform below window or interval from ms to point
    N = setting.DFTWindowSize * sampleRate / 1000;
    M = setting.frameInterval * sampleRate / 1000;
    P = setting.anaWindowSize * sampleRate / 1000;

    //Calculate how many frame need to transform
    if(size % M == 0){
        frameNum = size / M;
    }else{
        frameNum = size / M + 1;
    }
    printf("%d ", frameNum);
    double res = 0;

    //Generate spectrogram file(.txt) name.
    char buf[30] = {'\0'};
    strncpy(buf, fileName, strlen(fileName)-3);
    sprintf(buf, "%s{Set%d}.txt", buf, setIndex);

    //Open file.
    FILE *fp;
    fp = fopen(buf, "w");

    int remain = 0;

    printf("Transforming %s.\n", buf);

    // DFT, first, need to decide which frame to transform
    // Second, give which frequency bin to calculate its db
    // Third, implement the Fourier transform formula
    ImgNum sum = {0, 0};
    for(int m = 0; m < frameNum; m++){
        
        for(int k = 0; k < N; k++){
            sum.real = 0;
            sum.imagine = 0;
            for(int n = 0; n < N; n++){
                // Transform exponential to Euler form
                sum.real += x[m*M+n] * window[n] * cosTable[k][n];
                sum.imagine += x[m*M+n] * window[n] * sinTable[k][n] * -1;
            }
            res = sqrt(pow(sum.real, 2) + pow(sum.imagine, 2)); // Get absolute value of imagine number
            res = 20 * log10(res); // Calculate db value
            fprintf(fp,"%lf ", res); // Write in txt file
        }
        fprintf(fp, "\n");
    }
    fclose(fp);
}

void makeSpectrogram(char fileName[], Setting setting, int sampleRate, int setIndex){ //Combine read file and DFT
    FILE *file;
    file = fopen(fileName, "rb");  
    fseek(file, 44 , SEEK_CUR); // Move file reading index to 44 aka begin of wave data

    int size = getFileSize(fileName) / 2 - 22; // Calculate total file points
    short int data[size];
    fread(data, sizeof(short int), size, file); // Read data and store them in short int type array ''
    fclose(file);

    DFT(setting, sampleRate, data, size, setIndex, fileName);
}

int main(int argc, char *argv[]){
    //_setmode( _fileno( stdout ), _O_BINARY );
    //_setmode( _fileno( stderr ), _O_TEXT );

    // Store infomation of 8 cos waves
    WaveInfo cos8Info[8] = {
        {16000, 50, "050"},
        {16000, 200, "200"},
        {16000, 55, "055"},
        {16000, 220, "220"},
        {8000, 50, "050"},
        {8000, 200, "200"},
        {8000, 55, "055"},
        {8000, 220, "220"}
    };

    //Generate 8 waves 
    Wave wave;
    double x, sec = 1.0;
    int A = 10000;
    for(int i = 0; i < 8; i++){
        int f = cos8Info[i].freq;
        int sr = cos8Info[i].sampleRate;
        wave = makeWave(cos8Info[i].sampleRate, 16);
        int sampleNum = sec * sr;
        giveWaveDuration(&wave, sec);
        for(int j = 0; j < sampleNum; j++){
            x = A * cos(2*PI*f*j/sampleNum);
            waveAddASample(&wave, x);
        }
        // Store them by frequency and smaple rate
        char fName[20] = {};
        makeFileName(fName, cos8Info[i].fn, sr);
        FILE *file;
        file = fopen(fName, "wb");
        fwrite(&(wave.header), sizeof(WaveHeader), 1, file);
        fwrite((void*)(wave.data),sizeof(char), wave.size, file);
        fclose(file);
        free(wave.data);
    }
    
    // Setting for this project
    Setting setting[4] = {
        {5, 'r', 8, 5},
        {5, 'm', 8, 5},
        {20, 'r', 32, 10},  
        {20, 'm', 32, 10}
    };

    //Make spectrogram
    int N, P;
    for(int i = 0; i < 4; i++){
        N = setting[i].DFTWindowSize * 8000 / 1000;
        P = setting[i].anaWindowSize * 8000 / 1000;
        initCosNSinTable(N, N);
        windowFunction(setting[i].windowType, 0, P, N);
        makeSpectrogram("cos_050Hz-8k.wav", setting[i], 8000, i+1);
        makeSpectrogram("cos_055Hz-8k.wav", setting[i], 8000, i+1);
        makeSpectrogram("cos_200Hz-8k.wav", setting[i], 8000, i+1);
        makeSpectrogram("cos_220Hz-8k.wav", setting[i], 8000, i+1);
        makeSpectrogram("vowel-8k.wav", setting[i], 8000, i+1);
        free(cosTable);
        free(sinTable);
        free(window);

        N = setting[i].DFTWindowSize * 16000 / 1000;
        P = setting[i].anaWindowSize * 16000 / 1000;
        initCosNSinTable(N, N);
        windowFunction(setting[i].windowType, 0, P, N);
        makeSpectrogram("cos_200Hz-16k.wav", setting[i], 16000, i+1);
        makeSpectrogram("cos_050Hz-16k.wav", setting[i], 16000, i+1);
        makeSpectrogram("cos_055Hz-16k.wav", setting[i], 16000, i+1);
        makeSpectrogram("cos_220Hz-16k.wav", setting[i], 16000, i+1);
        makeSpectrogram("vowel-16k.wav", setting[i], 16000, i+1);
        free(cosTable);
        free(sinTable);
        free(window);
    }

    printf("finish");
}