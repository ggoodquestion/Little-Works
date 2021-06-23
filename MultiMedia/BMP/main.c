#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#define PI 3.1415926

typedef struct BMP_HEADER{
    /* Bitmap file header */
    char ID[2];
    int fileSize;
    int reserved;
    int bitmap_data_offset;

    /* Bitmap info header */
    int bitmap_header_size;
    int width;
    int height;
    short plane;
    short bit_per_pixel;
    int compression;
    int bitmap_data_size;
    int H_resolution;
    int V_resolution;
    int used_color;
    int important_color;
}Header;
double beta(int k){
    if(k == 0) return sqrt(0.5);
    else return 1;
}
int main(int argv, char *argc[]){
    char *input = argc[1];
    char *output = argc[2];
    FILE *f_in = fopen(input, "rb");

    /* Read file */
    Header header;
    fread(header.ID, 2, 1, f_in);
    fread(&header.fileSize, 4, 1, f_in);
    fread(&header.reserved, 4, 1, f_in);
    fread(&header.bitmap_data_offset, 4, 1, f_in);
    fread(&header.bitmap_header_size, 4, 1, f_in);
    fread(&header.width, 4, 1, f_in);
    fread(&header.height, 4, 1, f_in);
    fread(&header.plane, 2, 1, f_in);
    fread(&header.bit_per_pixel, 2, 1, f_in);
    fread(&header.compression, 4, 1, f_in);
    fread(&header.bitmap_data_size, 4, 1, f_in);
    fread(&header.H_resolution, 4, 1, f_in);
    fread(&header.V_resolution, 4, 1, f_in);
    fread(&header.used_color, 4, 1, f_in);
    fread(&header.important_color, 4, 1, f_in);

    // RGB data
    int sample = header.bitmap_data_size / 3;
    unsigned char *R = malloc(sample);
    unsigned char *G = malloc(sample);
    unsigned char *B = malloc(sample);
    for(int i = 0; i < sample; i++){
        fread(&B[i], 1, 1, f_in);
        fread(&G[i], 1, 1, f_in);
        fread(&R[i], 1, 1, f_in);
    }

    unsigned int *Y = malloc(sample * sizeof(int));
    unsigned int *Cb = malloc(sample * sizeof(int));
    unsigned int *Cr = malloc(sample * sizeof(int));
    for(int i = 0; i < sample; i++){
        int r = R[i], g = G[i], b = B[i];

        Y[i] = 0.299*r + 0.587*g + 0.114*b;
        Cb[i] = -0.169*r - 0.331*g + 0.5*b + 128;
        Cr[i] = 0.5*r - 0.419*g -0.081*b + 128;
    }
    
    /* DCT */
    // Truth table
    double **cos_t = (double**) malloc(sizeof(void*) * 8);
    for(int i = 0; i < 8; i++){
        cos_t[i] = (double*) malloc(sizeof(double) * 8);
    }
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            cos_t[i][j] = cos(PI*i*(2*j+1)/(2*8));
        }
    }

    int H = header.height / 8;
    int W = header.width / 8;
    double **f_y = (double**) malloc(sizeof(void*) * header.height);
    double **f_cb = (double**) malloc(sizeof(void*) * header.height);
    double **f_cr = (double**) malloc(sizeof(void*) * header.height);
    double **F_y = (double**) malloc(sizeof(void*) * header.height);
    double **F_cb = (double**) malloc(sizeof(void*) * header.height);
    double **F_cr = (double**) malloc(sizeof(void*) * header.height);
    for(int i = 0; i < header.height; i++){
        f_y[i] = (double*) malloc(sizeof(double) * header.width);
        f_cb[i] = (double*) malloc(sizeof(double) * header.width);
        f_cr[i] = (double*) malloc(sizeof(double) * header.width);
        F_y[i] = (double*) malloc(sizeof(double) * header.width);
        F_cb[i] = (double*) malloc(sizeof(double) * header.width);
        F_cr[i] = (double*) malloc(sizeof(double) * header.width);
    }
    for(int i = 0; i < header.height; i++){
        for(int j = 0; j < header.width; j++){
            f_y[i][j] = Y[j + i * header.width];
            f_cb[i][j] = Cb[j + i * header.width];
            f_cr[i][j] = Cr[j + i * header.width];
        }
    }
    
    for(int i = 0; i < H; i++){
        for(int j = 0; j < W; j++){

            for(int u = 0; u < 8; u++){
                for(int v = 0; v < 8; v++){
                    double s_y = 0, s_cb = 0, s_cr = 0;
                    for(int r = 0; r < 8; r++){
                        for(int c = 0; c < 8; c++){
                            int a = c + j * 8;
                            int b = r + i * 8;
                            s_y += beta(u) * beta(v) * f_y[b][a] * cos_t[u][r] * cos_t[v][c];
                            s_cb += beta(u) * beta(v) * f_cb[b][a] * cos_t[u][r] * cos_t[v][c];
                            s_cr += beta(u) * beta(v) * f_cr[b][a] * cos_t[u][r] * cos_t[v][c];
                        }
                        int x = v + j * 8;
                        int y = u + i * 8;
                        F_y[y][x] = 0.25 * s_y;
                        F_cb[y][x] = 0.25 * s_cb;
                        F_cr[y][x] = 0.25 * s_cr;
                    }
                }
            }
        }
    }
    /* Quantization */
    double table[8][8] = {{16, 11, 10, 16, 24, 40, 51, 61},
                         {12, 12, 14, 29, 26, 58, 60, 55},
                         {14, 13, 16, 24, 40, 57, 69, 56},
                         {14, 17, 22, 29, 51, 87, 80, 62},
                         {18, 22, 37, 56, 68, 109, 103, 77},
                         {24, 35, 55, 64, 81, 104, 113, 92},
                         {49, 64, 78, 87, 103, 121, 120, 101},
                         {72, 92, 95, 98, 112, 100, 103, 99}};

    for(int i = 0; i < H; i++){
        for(int j = 0; j < W; j++){
            for(int v = 0; v < 8; v++){
                for(int u = 0; u < 8; u++){
                    int x = u + j * 8;
                    int y = v + i * 8;
                    int by = F_y[y][x] / table[v][u];
                    int bcb = F_y[y][x] / table[v][u];
                    int bcr = F_y[y][x] / table[v][u];
                    F_y[y][x] = by;
                    F_cb[y][x] = bcb;
                    F_cr[y][x] = bcr;
                }
            }
        }
    }

    /* Inverse Quantization */
    for(int i = 0; i < H; i++){
        for(int j = 0; j < W; j++){
            for(int v = 0; v < 8; v++){
                for(int u = 0; u < 8; u++){
                    int x = u + j * 8;
                    int y = v + i * 8;
                    int by = F_y[y][x] * table[v][u];
                    int bcb = F_y[y][x] * table[v][u];
                    int bcr = F_y[y][x] * table[v][u];
                    F_y[y][x] = by;
                    F_cb[y][x] = bcb;
                    F_cr[y][x] = bcr;
                }
            }
        }
    }

    /* IDCT */
    for(int i = 0; i < H; i++){
        for(int j = 0; j < W; j++){

            for(int r = 0; r < 8; r++){
                for(int c = 0; c < 8; c++){
                    int s_y = 0, s_cb = 0, s_cr = 0;
                    for(int u = 0; u < 8; u++){
                        for(int v = 0; v < 8; v++){
                            int a = v + j * 8;
                            int b = u + i * 8;
                            s_y += beta(u) * beta(v) * F_y[b][a] * cos_t[u][r] * cos_t[v][c];
                            s_cb += beta(u) * beta(v) * F_cb[b][a] * cos_t[u][r] * cos_t[v][c];
                            s_cr += beta(u) * beta(v) * F_cr[b][a] * cos_t[u][r] * cos_t[v][c];
                        }
                    }
                    int x = c + j * 8;
                    int y = r + i * 8;
                    f_y[y][x] = (int)(0.25 * s_y);
                    f_cb[y][x] = (int)(0.25 * s_cb);
                    f_cr[y][x] = (int)(0.25 * s_cr);
                }
            }
        }
    }

    /* Convert YCbCr to RGB */
    for(int i = 0; i < header.height; i++){
        for(int j = 0; j < header.width; j++){
            int index = j + i * header.width;
            Y[index] = f_y[i][j];
            Cb[index] = f_cb[i][j];
            Cr[index] = f_cr[i][j];
        }
    }

    for(int i = 0; i < sample; i++){
        
        int y = Y[i];
        int cb = Cb[i] - 128;
        int cr = Cr[i] - 128;

        int r = y + 1.403*cr;
        int g = y - 0.344*cb - 0.714*cr;
        int b = y + 1.773*cb;
        if(r >= 255){
            r = 255;
        }else if(r < 0){
            r = 0;
        }
        if(g >= 255){
            g = 255;
        }else if(g < 0){
            g = 0;
        }
        if(b >= 255){
            b = 255;
        }else if(b < 0){
            b = 0;
        }
        R[i] = r;
        G[i] = g;
        B[i] = b;    
    }
    
    /* Write file */
    FILE *f_out = fopen(output, "wb");
    fwrite(header.ID, 2, 1, f_out);
    fwrite(&header.fileSize, 4, 1, f_out);
    fwrite(&header.reserved, 4, 1, f_out);
    fwrite(&header.bitmap_data_offset, 4, 1, f_out);
    fwrite(&header.bitmap_header_size, 4, 1, f_out);
    fwrite(&header.width, 4, 1, f_out);
    fwrite(&header.height, 4, 1, f_out);
    fwrite(&header.plane, 2, 1, f_out);
    fwrite(&header.bit_per_pixel, 2, 1, f_out);
    fwrite(&header.compression, 4, 1, f_out);
    fwrite(&header.bitmap_data_size, 4, 1, f_out);
    fwrite(&header.H_resolution, 4, 1, f_out);
    fwrite(&header.V_resolution, 4, 1, f_out);
    fwrite(&header.used_color, 4, 1, f_out);
    fwrite(&header.important_color, 4, 1, f_out);

    for(int i = 0; i < sample; i++){
        fwrite(&B[i], 1, 1, f_out);
        fwrite(&G[i], 1, 1, f_out);
        fwrite(&R[i], 1, 1, f_out);
    }
    
    fclose(f_in);
    fclose(f_out);
    printf("Great!!!");
}