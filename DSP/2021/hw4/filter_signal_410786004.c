#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define PI 3.14159265
#define fc 4000 //Cut off frequency

typedef struct WAVE_HEADERE
{
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

} WaveHeader;

typedef struct WAVE
{
    WaveHeader header;
    char *data;
    long long int index;     //Record the index to store when adding sample
    long long int size;      //Data size
    long long int sampleNum; //Number of samples

} Wave;

WaveHeader makeHeader(int sampleRate, int sampleSize)
{ //Sign the needed information
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

Wave makeWave(int sampleRate, int sampleSize)
{
    Wave wave;
    wave.header = makeHeader(sampleRate, sampleSize);
    return wave;
}

void giveWaveDuration(Wave *wave, double second)
{
    long long int totalBytes = (long long int)(second * wave->header.byteRate);

    //initialize
    wave->data = (char *)malloc(totalBytes);
    wave->index = 0;
    wave->size = totalBytes;
    wave->sampleNum = wave->header.sampleRate * second;
    wave->header.subChunckSize2 = totalBytes;
    wave->header.chunckSize = totalBytes + 36;
}

void waveAddASample(Wave *wave, double x)
{ //Add a sample by bits respectively
    //Do pcm quantization
    char *sample;
    //Use char as a byte storage to store data of short int(2 bytes), int(2 bytes), long int(4 bytes)
    if (wave->header.bitsPerSample == 8)
    {
        short int sample8bits = (short int)(127 + x);
        //Allocate memory address
        sample = (char *)&sample8bits;
        (wave->data)[wave->index] = sample[0];
        wave->index += 1;
    }
    else if (wave->header.bitsPerSample == 16)
    {
        int sample16bits = (int)(x);
        //Allocate memory address
        sample = (char *)&sample16bits;
        (wave->data)[wave->index] = sample[0];
        (wave->data)[wave->index + 1] = sample[1];
        wave->index += 2;
    }
    else if (wave->header.bitsPerSample == 32)
    {
        long int sample32bits = (long int)(x);
        //Allocate memory address
        sample = (char *)&sample32bits;
        (wave->data)[wave->index] = sample[0];
        (wave->data)[wave->index + 1] = sample[1];
        (wave->data)[wave->index + 2] = sample[2];
        (wave->data)[wave->index + 3] = sample[3];
        wave->index += 4;
    }
}

int getFileSize(char fileName[])
{ // To calculate the number of bytes of file, below will use this function
    FILE *file;
    file = fopen(fileName, "rb");
    // Use fseek to point out the end of file so that can tell how many bytes that the file has
    fseek(file, 0, SEEK_END);
    int size = ftell(file);
    return size;
}

void convolution(double *x, double *h, double *y, int M, int size)
{
    for (int n = 0; n < size; n++)
    {
        y[n] = 0;
        for (int k = 0; k < 2 * M; k++)
        {
            double tmp;
            if (n - k < 0)
                tmp = 0;
            else
                tmp = x[n - k];
            y[n] += (h[k] * tmp);
        }
    }
}

void readWaveFile(char *fn, double *x)
{
    Wave wave;
    FILE *fp = fopen(fn, "rb");
    //Read wave data to fn.wav by stdout
    fread(&wave.header, sizeof(WaveHeader), 1, fp);

    int size = getFileSize(fn) / 2 - 22;
    short *tmp = malloc(size * 2);
    fread(tmp, size, 2, fp);

    for (int i = 0; i < size; i++)
    {
        x[i] = tmp[i];
    }

    fclose(fp);
}

void loadFilter(double *h, double *hm){
    FILE *fp1 = fopen("64_filter.txt", "r");
    FILE *fp2 = fopen("64_filter_min.txt", "r");

    for(int i = 0; i < 128; i++){
        fscanf(fp1, "%lf", &h[i]);
        fscanf(fp2, "%lf", &hm[i]);
    }
}

int main(int argc, char *argv[])
{
    int sampleRate;
    int sampleSize;
    int f;
    double A;
    double second;
    long long int sampleNum;
    //k for subject needed
    int k[8] = {0, 1, 2, 4, 8, 16, 32, 64};

    //Load parameter
    sampleRate = 16000;
    sampleSize = 16;
    f = 110;
    A = 0.1;
    second = 0.5;
    sampleNum = second * sampleRate;

    // For filter
    double h[128];
    double hm[128];

    loadFilter(h, hm); 

    //Generate wave data
    double x[sampleNum];
    double max = pow(2, 15);
    for (int j = 0; j < 8; j++)
    {
        //Initial wave
        Wave wave = makeWave(sampleRate, sampleSize);
        giveWaveDuration(&wave, second);

        for (long long int i = 0; i < sampleNum; i++)
        {
            x[i] = A * cos((double)(2 * PI * f * k[j] * i / sampleRate)) * max; //this is source signal, A = 0.1
            int quantization = floor(x[i] + 0.5);
            waveAddASample(&wave, (double)quantization);
        }
        char fn[16];
        sprintf(fn, "wave_%dHz.wav", f * k[j]);
        FILE *fp = fopen(fn, "wb");

        //Write wave data to fn.wav by stdout
        fwrite(&wave.header, sizeof(WaveHeader), 1, fp);
        fwrite(wave.data, wave.size, 1, fp);

        fclose(fp);
        free(wave.data);
        // printf("wave %d\n", k[j]);
    }

    int M = 64;
    for (int b = 0; b < 8; b++)
        {
            char fn[16];
            sprintf(fn, "wave_%dHz.wav", f * k[b]);

            int size = getFileSize(fn) / 2 - 22;
            double *x1 = malloc(sizeof(double) * size);
            double *x2 = malloc(sizeof(double) * size);
            double *y1 = calloc(size, sizeof(double));
            double *y2 = calloc(size, sizeof(double));

            readWaveFile(fn, x1);
            readWaveFile(fn, x2);

            convolution(x1, h, y1, M, size);
            convolution(x2, hm, y2, M, size);

            Wave wave1 = makeWave(sampleRate, sampleSize);
            giveWaveDuration(&wave1, second);
            Wave wave2 = makeWave(sampleRate, sampleSize);
            giveWaveDuration(&wave2, second);

            for (int a = 0; a < size; a++)
            {
                int quantization = floor(y1[a] + 0.5);
                waveAddASample(&wave1, (double)quantization);
                quantization = floor(y2[a] + 0.5);
                waveAddASample(&wave2, (double)quantization);
            }
            //Write wave data to fn.wav by stdout
            char fn1[30];
            char fn2[30];
            
            sprintf(fn1, "filter_%dHz_%d.wav", f * k[b], M);
            sprintf(fn2, "filter_min_%dHz_%d.wav", f * k[b], M);
            FILE *fp1 = fopen(fn1, "wb");
            fwrite(&wave1.header, sizeof(WaveHeader), 1, fp1);
            fwrite(wave1.data, wave1.size, 1, fp1);
             FILE *fp2 = fopen(fn2, "wb");
            fwrite(&wave2.header, sizeof(WaveHeader), 1, fp2);
            fwrite(wave2.data, wave2.size, 1, fp2);
        }

    printf("finish");
}