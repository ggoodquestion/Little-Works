#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>

#define PI 3.14159265
#define BUFFER_SIZE 1000

#define P 360 // Frame size
#define Q 1024 // Low-pass filter size
#define N 2048 // FFT size

typedef struct HEADERE{
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

}Header;

typedef struct COMPLEX{ // A struct to store the real and imagine part of imagine number
    double real;
    double img;
}Complex;

typedef struct STEREO{ 
    // Two record Stereo.
    // Because use 16bits depth so use short
    short left;
    short right;
}Stereo;

int gcd(int m, int n) {
    while(n != 0) { 
        int r = m % n; 
        m = n; 
        n = r; 
    } 
    return m;
}

int lcm(int m, int n) {
    return m * n / gcd(m, n);
}

void sswap(short *a, short *b){
    short tmp = *b;
    *b = *a;
    *a = tmp;
}

void dswap(double *a, double *b){
    double tmp = *b;
    *b = *a;
    *a = tmp;
}

void RaderReverse(Stereo *x){
    int j = 0;
    // printf("0 ");
    for(int i = 0; i < N-1; i++){ // Do N-1 times, compute pos  i's next number
        if(i < j){ // When j bigger than i to avoid repeating exchange
            sswap(&x[i].left, &x[j].left);
            sswap(&x[i].right, &x[j].right);
        }
        int k = N / 2;
        while(k <= j){ // If current pos is not 0 then minus the bits by 1
            j -= k;
            k /= 2; // To compare next position
        }
        j += k;
        // printf("%d ", j);
    }
}

void RaderReverseComplex(Complex *x){
    int j = 0;
    // printf("0 ");
    for(int i = 0; i < N-1; i++){ // Do N-1 times, compute pos  i's next number
        if(i < j){ // When j bigger than i to avoid repeating exchange
            dswap(&x[i].real, &x[j].real);
            dswap(&x[i].img, &x[j].img);
        }
        int k = N / 2;
        while(k <= j){ // If current pos is not 0 then minus the bits by 1
            j -= k;
            k /= 2; // To compare next position
        }
        j += k;
        // printf("%d ", j);
    }
}

Complex complex_product(Complex x, Complex y){
    Complex res;
    res.real = x.real*y.real - x.img*y.img;
    res.img = x.real*y.img + x.img*y.real;
    return res;
}

Complex complex_sum(Complex x, Complex y){
    Complex res;
    res.real = x.real + y.real;
    res.img = x.img + y.img;
    return res;
}

Complex complex_sub(Complex x, Complex y){
    Complex res;
    res.real = x.real - y.real;
    res.img = x.img - y.img;
    return res;
}

Complex complex_conj(Complex x){
    Complex res;
    res.real = x.real;
    res.img = -x.img;
    return res;
}

Complex w[N]; // Coeffieicnt of FFT

void FFT(Stereo *frame, Complex *l_X, Complex *r_X){

    Complex up_l, up_r, down_l, down_r, prod_l, prod_r;

    // Rader reverse
    RaderReverse(frame);

    // Convert real to complex
    for(int i = 0; i < N; i++){
        l_X[i].real = (double) frame[i].left;
        r_X[i].real = (double) frame[i].right;
    }

    int M = log2(N);
    int node = 1;
    int group;

    for(int m = 0; m < M; m++){
        node = pow(2, m);
        group = N / node;
        for(int j = 0; j < N; j+=2*node){ // How many group is divided
            for(int k = 0; k < node; k++){ // How many nodes in a group
                int pos = j + k + node;

                prod_l = complex_product(l_X[pos], w[N*k/2/node]);
                prod_r = complex_product(r_X[pos], w[N*k/2/node]);

                up_l = complex_sum(l_X[j+k], prod_l);
                up_r = complex_sum(r_X[j+k], prod_r);

                down_l = complex_sub(l_X[j+k], prod_l);
                down_r = complex_sub(r_X[j+k], prod_r);

                l_X[j+k] = up_l;
                r_X[j+k] = up_r;

                l_X[j+k+node] = down_l;
                r_X[j+k+node] = down_r;
            }
        }
    }

    for(int i = 0; i < N; i++){
        l_X[i].real /= sqrt(N);
        l_X[i].img /= sqrt(N);
        r_X[i].real /= sqrt(N);
        r_X[i].img /= sqrt(N);
    }
}

void LPF(Complex *l_X, Complex *r_X, int gain, int M, int tmpSampleRate){
    int cutoff = (M * N / tmpSampleRate)-1;
    for(int k = 0; k < N; k++){
        if( k  <= cutoff || k >= N-cutoff ){
            l_X[k].real *= gain;
            r_X[k].real *= gain;
            l_X[k].img *= gain;
            r_X[k].img *= gain;
        }else{
            l_X[k].real = 0;
            r_X[k].real = 0;
            l_X[k].img = 0;
            r_X[k].img = 0;
        }
    }
}

void IFFT(Stereo *frame, Complex *l_X, Complex *r_X){
    Complex up_l, up_r, down_l, down_r, prod_l, prod_r;

     // Rader reverse
    RaderReverseComplex(l_X);
    RaderReverseComplex(r_X);

    int M = log2(N);
    int node = 1;
    int group;

    for(int m = 0; m < M; m++){
        node = pow(2, m);
        group = N / node;
        for(int j = 0; j < N; j+=2*node){ // How many group is divided
            for(int k = 0; k < node; k++){ // How many nodes in a group
                int pos = j + k + node;

                Complex factor = complex_conj(w[N*k/2/node]);
                prod_l = complex_product(l_X[pos], factor);
                prod_r = complex_product(r_X[pos], factor);

                up_l = complex_sum(l_X[j+k], prod_l);
                up_r = complex_sum(r_X[j+k], prod_r);

                down_l = complex_sub(l_X[j+k], prod_l);
                down_r = complex_sub(r_X[j+k], prod_r);

                l_X[j+k] = up_l;
                r_X[j+k] = up_r;

                l_X[j+k+node] = down_l;
                r_X[j+k+node] = down_r;
            }
        }
    }

    // Convert to real number
    for(int i = 0; i < N; i++){
        frame[i].left = l_X[i].real / sqrt(N);
        frame[i].right = r_X[i].real / sqrt(N);
    }
}

void clearArray(Stereo *a, int size){
    for(int i = 0; i < size; i++){
        a[i].left = 0;
        a[i].right = 0;
    }
}

void clearArrayComplex(Complex *a, int size){
    for(int i = 0; i < size; i++){
        a[i].real = 0;
        a[i].img = 0;
    }
}

int main(int argc, char *argv[]){

    /*------------ Initialize file ------------*/
    //Varialbe declare.
    char *inputName = argv[1];
    char *outputName = argv[2];
    FILE *f_source, *f_output;

    //bits depth: 16, sample rate: 44100
    f_source = fopen(inputName, "rb"); 
    f_output = fopen(outputName, "wb");
    /*------------ Initialize file ------------*/


    /*------------ Read file start ------------*/
    printf("Read file.\n");
    // Utils varible declare
    int rawSize; // Wav data size(byte)
    int size; // Wav data point number
    Stereo *x;

    // Read file and get basic infomation.
    Header header;
    fread(&header, 1, sizeof(Header), f_source);
    rawSize = header.subChunckSize2;
    size = rawSize / (header.bitsPerSample / 8 * header.numChannel);

    // Read wav data point;
    x = calloc(size, sizeof(Stereo));
    fread(x, 1, rawSize, f_source);
    fclose(f_source);

    /*------------ Read file end ------------*/

    
    /*------------------ Up Sampling -------------------*/
    printf("Up sampling.\n");

    int tmpSampleRate = lcm(8000, header.sampleRate);
    int tmpSize = (tmpSampleRate / header.sampleRate) * size;
    Stereo *x_tmp = calloc(tmpSize, sizeof(Stereo));

    // Up sampling
    int diff = tmpSampleRate / header.sampleRate;
    for(int i = 0; i < size; i++){
        x_tmp[diff * i].left = x[i].left;
        x_tmp[diff * i].right = x[i].right;
    }
    // for(int i = 0; i < tmpSize; i++){
    //     printf("%d, %d\t", x_tmp[i].left, x_tmp[i].right);
    // }
    free(x);
    /*---------------- Up Sampling End ----------------*/


    /*---------------- Filtering Start ----------------*/
    printf("Filtering.\n");

    // Delcare freq table
    int frameNum = tmpSize / P;
    if(tmpSize % P != 0) frameNum++;

    // Make coefficient table
    for(int k = 0; k < N; k++){ 
        w[k].real = cos(2 * PI * k / N); 
        w[k].img = -sin(2 * PI * k / N); 
    }

    Stereo *x_afterFFt = calloc(tmpSize, sizeof(Stereo));
    Complex *freq_l;
    Complex *freq_r;
    freq_l = (Complex *) malloc(N * sizeof(Complex));
    freq_r = (Complex *) malloc(N * sizeof(Complex));
    clearArrayComplex(freq_l, N);
    clearArrayComplex(freq_r, N);

    FILE *ffp = fopen("freq.txt", "w");
    // Divid in frame
    Stereo *x_p = malloc(N * sizeof(Stereo));
    clearArray(x_p, N);
    for(int i = 0; i < frameNum; i++){
        // Zeropadding
        printf("frame%d\n", i);
        for(int j = 0; j < P; j++){
            if((i*P + j) >= tmpSize) break;
            x_p[j] = x_tmp[i*P+j];
        }

        FFT(x_p, freq_l, freq_r);
        LPF(freq_l, freq_r, 80, 8000, tmpSampleRate);
        IFFT(x_p, freq_l, freq_r);
        
        // Copy frame into tmp
        for(int a = 0; a < P+Q; a++){
            if(i*P+a >= tmpSize) break;
            x_afterFFt[i*P+a].left += x_p[a].left;
            x_afterFFt[i*P+a].right += x_p[a].right;
        }
        clearArray(x_p, N);
        clearArrayComplex(freq_l, N);
        clearArrayComplex(freq_r, N);
    }

    fclose(ffp);


    /*---------------- Filtering End ----------------*/


    /*------------------ Down Sampling -------------------*/
    /*             Origin:44.1k -> Target: 8k             */
    printf("Down sampling.\n");
    float freqDiffRate = (float) size / header.sampleRate;
    int outputSize = floor(freqDiffRate * 8000);
    printf("%d\n", outputSize);
    Stereo *x_o = calloc(outputSize, sizeof(Stereo));
    diff = tmpSampleRate / 8000;

    for(int i = 0; i < outputSize; i++){
        x_o[i].left = x_afterFFt[diff*i].left;
        x_o[i].right = x_afterFFt[diff*i].right;
    }
    /*---------------- Down Sampling End ----------------*/


    /*------------ Write file start ------------*/
    header.sampleRate = 8000;
    header.byteRate = header.sampleRate * header.numChannel * header.bitsPerSample / 8;
    header.subChunckSize2 = outputSize * header.bitsPerSample * header.numChannel / 8;

    fwrite(&header, 1, sizeof(Header), f_output);
    fwrite(x_o, 1, header.subChunckSize2, f_output);
    fclose(f_output);

    /*------------ Write file end ------------*/

    printf("finish\n");

}