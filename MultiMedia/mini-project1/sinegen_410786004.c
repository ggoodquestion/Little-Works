#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<io.h>
#include<fcntl.h>

#define PI 3.14159265

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

int main(int argc, char *argv[]){
    _setmode( _fileno( stdout ), _O_BINARY );
    _setmode( _fileno( stderr ), _O_TEXT );

    int sampleRate;
    int sampleSize;
    int f;
    double A;
    double second;
    long long int sampleNum;

    //Load parameter
    sampleRate = atoi(argv[1]);
    sampleSize = atoi(argv[2]);
    f = atoi(argv[3]);
    A = atof(argv[4]);
    second = atof(argv[5]);
    sampleNum = (long long int)second * sampleRate;

    //Generate wave data
    Wave wave = makeWave(sampleRate, sampleSize);
    giveWaveDuration(&wave, second);
    
    //Generate a sin function and sample it by x, quantify by q, noise by e
    //But to reduce memiry usage, I just ues x
    double p = 0, p0 = 0, sqnr, e;
    //float e[sampleNum];
    double x[sampleNum];
    //int q[sampleNum];
    //double delta = A/pow(2,wave.header.bitsPerSample-1);
    double delta = 1.0;
    
    for(long long int i = 0; i < sampleNum; i++){
        x[i] = A * sin(2*PI*f*i/sampleRate);
        p += pow(x[i], 2);
        int tmp = (int) (x[i]/delta+0.5);
        //q[i] = delta * tmp;
        //e[i] = x[i] - tmp * delta;
        e = x[i] - tmp * delta;
        p0 += pow(e, 2);
        waveAddASample(&wave, x[i]);
    }
    p = p / sampleNum;
    p0 = p0 / sampleNum;
    sqnr = 10 * (log(p/p0) / log(10));

    //Write wave data to fn.wav by stdout
    char *tmp;
    tmp = (char*) &wave.header;
    for(int i = 0; i < sizeof(WaveHeader); i++){
        fprintf(stdout, "%c", tmp[i]);
    }
    for(int i = 0; i < wave.size; i++){
        fprintf(stdout, "%c", wave.data[i]);
    }

    //Write SQNR to squr.txt by stderr
    char s[30] = {};
    char a[5] = "testt";
    sprintf(s, "%.15lf", sqnr);
    fprintf(stderr, "%s", s);

    //printf("finish");
}