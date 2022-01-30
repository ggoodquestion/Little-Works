#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define PI 3.14159265
#define fc 4000 //Cut off frequency

int getFileSize(char fileName[])
{ // To calculate the number of bytes of file, below will use this function
    FILE *file;
    file = fopen(fileName, "rb");
    // Use fseek to point out the end of file so that can tell how many bytes that the file has
    fseek(file, 0, SEEK_END);
    int size = ftell(file);
    return size;
}

void generateFilter(int M, double *h, int N)
{
    //M: order, h: filter, N: sample period

    int n;
    for (int i = 0; i < 2 * M; i++)
    {

        n = i - M;
        if (n == 0)
        {
            h[i] = 1;
        }
        else
        {
            h[i] = sin(2 * PI * fc * n / N) / (n * PI);
        }
    }
}

void exportFilter(int M, double *H)
{
    char fn[10];
    sprintf(fn, "%d_filter.txt", M);
    FILE *fp = fopen(fn, "w");
    for (int i = 0; i < 2 * M; i++)
    {
        fprintf(fp, "%lf ", H[i]);
    }
    fclose(fp);
}

void DFT(double *h, int N, int M)
{
    //N: sample rate, M: order
    double *H = malloc(2 * M * sizeof(double));
    for (int k = 0; k < 2 * M; k++)
    {
        double real = 0, imagine = 0;
        for (int n = 0; n < 2 * M; n++)
        {
            real += h[n] * cos(2 * PI * k * n / (2 * M));
            imagine -= h[n] * sin(2 * PI * k * n / (2 * M));
        }
        H[k] = (sqrt(pow(real, 2) + pow(imagine, 2)));
    }
    exportFilter(M, H);

    free(H);
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

int main(int argc, char *argv[])
{
    int sampleRate;
    int sampleSize;
    int f;
    double A;
    double second;
    long long int sampleNum;

    int order = 64;

    double *h = (double *)malloc(2 * order * sizeof(double));
    generateFilter(order, h, 16000);
    exportFilter(order, h);

    printf("finish");
}