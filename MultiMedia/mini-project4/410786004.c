#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#define PI 3.1415926

/* Quantization table */
double q_table[8][8] = {{16, 11, 10, 16, 24, 40, 51, 61},
                         {12, 12, 14, 29, 26, 58, 60, 55},
                         {14, 13, 16, 24, 40, 57, 69, 56},
                         {14, 17, 22, 29, 51, 87, 80, 62},
                         {18, 22, 37, 56, 68, 109, 103, 77},
                         {24, 35, 55, 64, 81, 104, 113, 92},
                         {49, 64, 78, 87, 103, 121, 120, 101},
                         {72, 92, 95, 98, 112, 100, 103, 99}};

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
typedef struct RGB{
    /* B G R is the order of store */
    /* Using Unsigned char to store RGB is important */
    unsigned char B;
    unsigned char G;
    unsigned char R;
}RGB;
typedef struct YCbCr{
    unsigned int Y;
    unsigned int Cb;
    unsigned int Cr;
}YCbCr;
typedef struct BMP{
    Header header;
    int *palette;
    RGB *bitmap_data;
}Bmp;
typedef struct f_YCbCr{
    double Y;
    double Cb;
    double Cr;
}f_YCbCr;

int H, W;
f_YCbCr **F;
YCbCr **f;
double **cos_r, **cos_c;

int getFileSize(char fileName[]){ // To calculate the number of bytes of file, below will use this function
    FILE *file;
    file = fopen(fileName, "rb");
    // Use fseek to point out the end of file so that can tell how many bytes that the file has
    fseek(file, 0, SEEK_END); 
    int size = ftell(file);
    fclose(file);
    return size;
}
RGB YCbCr2RGB(YCbCr yrb){
    RGB res;
    int y = yrb.Y, r = yrb.Cr - 128, b = yrb.Cb - 128; // Minus 128 to recover origin value
    /* Below is to avoid overflow (0-255) */
    res.R = ((y + 1.403*r) >= 255) ? 255 : (y + 1.403*r);
    res.G = ((y - 0.344*b - 0.714*r) >= 255) ? 255 : (y - 0.344*b - 0.714*r);
    res.B = ((y + 1.773*b) >= 255) ? 255 : (y + 1.773*b);

    res.R = ((y + 1.403*r) < 0) ? 0 : res.R;
    res.G = ((y - 0.344*b - 0.714*r) < 0) ? 0 : res.G;
    res.B = ((y + 1.773*b) < 0) ? 0 : res.B;
    return res;
}
YCbCr RGB2YCbCr(RGB rgb){ // Convert RGB to YCbCr
    YCbCr res;
    int r = rgb.R, g = rgb.G, b = rgb.B;
    res.Y = 0.299*r + 0.587*g + 0.114*b;
    res.Cb = -0.169*r - 0.331*g + 0.5*b + 128;
    res.Cr = 0.5*r - 0.419*g -0.081*b + 128;
    return res;
}
Bmp readFile(char *input){
    FILE *f_input = fopen(input, "rb");
    Header header;
    Bmp bmp;
    int size = getFileSize(input);
    fread(header.ID, 2, 1, f_input); //Important
    fread(&header.fileSize, 4, 1, f_input);
    fread(&header.reserved, 4, 1, f_input);
    fread(&header.bitmap_data_offset, 4, 1, f_input);
    fread(&header.bitmap_header_size, 4, 1, f_input);
    fread(&header.width, 4, 1, f_input);
    fread(&header.height, 4, 1, f_input);
    fread(&header.plane, 2, 1, f_input);
    fread(&header.bit_per_pixel, 2, 1, f_input);
    fread(&header.compression, 4, 1, f_input);
    fread(&header.bitmap_data_size, 4, 1, f_input);
    fread(&header.H_resolution, 4, 1, f_input);
    fread(&header.V_resolution, 4, 1, f_input);
    fread(&header.used_color, 4, 1, f_input);
    fread(&header.important_color, 4, 1, f_input);

    bmp.header = header;
    bmp.bitmap_data = (RGB*) malloc(header.bitmap_data_size);
    fread(bmp.bitmap_data, header.bitmap_data_size, 1, f_input);

    fclose(f_input);
    return bmp;
}
void writeFile(char *output, Bmp bmp){
    FILE *f_output = fopen(output, "wb");

    Header header;
    header = bmp.header;
    fwrite(header.ID, 2, 1, f_output); //Important
    fwrite(&header.fileSize, 4, 1, f_output);
    fwrite(&header.reserved, 4, 1, f_output);
    fwrite(&header.bitmap_data_offset, 4, 1, f_output);
    fwrite(&header.bitmap_header_size, 4, 1, f_output);
    fwrite(&header.width, 4, 1, f_output);
    fwrite(&header.height, 4, 1, f_output);
    fwrite(&header.plane, 2, 1, f_output);
    fwrite(&header.bit_per_pixel, 2, 1, f_output);
    fwrite(&header.compression, 4, 1, f_output);
    fwrite(&header.bitmap_data_size, 4, 1, f_output);
    fwrite(&header.H_resolution, 4, 1, f_output);
    fwrite(&header.V_resolution, 4, 1, f_output);
    fwrite(&header.used_color, 4, 1, f_output);
    fwrite(&header.important_color, 4, 1, f_output);
    
    for(int i = 0; i < H * 8; i++){
        if(i >= bmp.header.height) continue;
        for(int j = 0; j < W * 8; j++){
            if(j >= bmp.header.width) continue;
            
            RGB rgb = YCbCr2RGB(f[i][j]);
            fwrite(&rgb, sizeof(rgb), 1, f_output);
        }
    }
    fclose(f_output);
}
void initialize(Bmp bmp){
    int h = bmp.header.height;
    int w = bmp.header.width;
    int h_, w_;
    // Edge length of 8x8 block
    H = h / 8;
    W = w / 8;
    /* Deal with the bmp can't divided by 8 */
    if(h % 8 != 0) H++;
    if(w % 8 != 0) W++;
    h_ = H * 8;
    w_ = W * 8;
    f = (YCbCr**) malloc(sizeof(void*) * h_);
    F = (f_YCbCr**) malloc(sizeof(void*) * h_);
    for(int i = 0; i < h_; i++){
        f[i] = (YCbCr*) malloc(sizeof(YCbCr) * w_);
        F[i] = (f_YCbCr*) malloc(sizeof(f_YCbCr) * w_);
    }
    for(int i = 0; i < h_; i++){
        for(int j = 0; j < w_; j++){
            if((i >= h) || (j >= w)){
                /* Fill 0 in the outside pixels */
                RGB tmp = {0, 0, 0};
                f[i][j] = RGB2YCbCr(tmp);
            }else{
                f[i][j] = RGB2YCbCr(bmp.bitmap_data[j + i * w]);
            }
        }
    }
    /* Truth table for the cos of DCT */
    cos_c = (double**) malloc(sizeof(void*) * 8);
    cos_r = (double**) malloc(sizeof(void*) * 8);
    for(int i = 0; i < 8; i++){
        cos_c[i] = (double*) malloc(sizeof(double) * 8);
        cos_r[i] = (double*) malloc(sizeof(double) * 8);
    }
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            cos_c[i][j] = cos(PI*i*(2*j+1)/(2*8));
            cos_r[i][j] = cos(PI*i*(2*j+1)/(2*8));
        }
    }
}
double beta(int k){
    if(k == 0) return sqrt(0.5);
    else return 1;
}
void convertToFreq(){ // Use DCT to transform bmp to  freqency domain
    printf("DTC...\n");
    for(int i = 0; i < H; i++){
        for(int j = 0; j < W; j++){
            // Below 4 for loops is used to blocking
            // Then convert u,v,r,c that used to blocking to real position on 2D graph
            for(int u = 0; u < 8; u++){
                for(int v = 0; v < 8; v++){
                    f_YCbCr sum = {0, 0, 0};
                    int x = v + j * 8, y = u + i * 8;
                    for(int r = 0; r < 8; r++){
                        for(int c = 0; c < 8; c++){
                            int a = c + j * 8, b = r + i * 8;
                            double y = (double)(f[b][a].Y);
                            double cb = (double)(f[b][a].Cb);
                            double cr = (double)(f[b][a].Cr);
                            sum.Y += beta(u) * beta(v) * y * cos_c[u][r] * cos_r[v][c];
                            sum.Cb += beta(u) * beta(v) * cb * cos_c[u][r] * cos_r[v][c];
                            sum.Cr += beta(u) * beta(v) * cr * cos_c[u][r] * cos_r[v][c];
                        }
                    }
                    F[y][x].Y = 0.25 * sum.Y;
                    F[y][x].Cb = 0.25 * sum.Cb;
                    F[y][x].Cr = 0.25 * sum.Cr;
                }
            }
        }
    }
}
void quantization(){ 
    /* Quantize the value of Freq */
    /* Quantizing the freqency by dividing F and q_table */
    printf("Quantization...\n");
    for(int h = 0; h < H; h++){
        for(int w = 0; w < W; w++){
            for(int c = 0; c < 8; c++){
                for(int r = 0; r < 8; r++){
                    int x = r + w * 8, y = c + h * 8;
                    int y_ = (F[y][x].Y)/(q_table[c][r]);
                    int cb = (F[y][x].Cb)/(q_table[c][r]);
                    int cr = (F[y][x].Cr)/(q_table[c][r]);
                    F[y][x].Y = y_;
                    F[y][x].Cb = cb;
                    F[y][x].Cr = cr;
                }
            }
        }
    }
}
void i_quantization(){
    /* Inverse quantize the value of Freq */
    /* Inverse quantizing the freqency by multiplying F* and q_table */
    printf("Inverse Quantization...\n");
    for(int h = 0; h < H; h++){
        for(int w = 0; w < W; w++){
            for(int c = 0; c < 8; c++){
                for(int r = 0; r < 8; r++){
                    int x = r + w * 8, y = c + h * 8;
                    int y_ = (F[y][x].Y)*(q_table[c][r]);
                    int cb = (F[y][x].Cb)*(q_table[c][r]);
                    int cr = (F[y][x].Cr)*(q_table[c][r]);
                    F[y][x].Y = y_;
                    F[y][x].Cb = cb;
                    F[y][x].Cr = cr;
                }
            }
        }
    }
}
void convertToBmp(){
    /* IDCT */
    printf("IDTC...\n");
    for(int i = 0; i < H; i++){
        for(int j = 0; j < W; j++){
            for(int r = 0; r < 8; r++){
                for(int c = 0; c < 8; c++){
                    int Y = 0, Cb = 0, Cr = 0;
                    int x = c + j * 8, y = r + i * 8;
                    for(int u = 0; u < 8; u++){
                        for(int v = 0; v < 8; v++){
                            int a = v + j * 8, b = u + i * 8;
                            int y = (int)(F[b][a].Y);
                            int cb = (int)(F[b][a].Cb);
                            int cr = (int)(F[b][a].Cr);
                            Y += beta(u) * beta(v) * y * cos_c[u][r] * cos_r[v][c];
                            Cb += beta(u) * beta(v) * cb * cos_c[u][r] * cos_r[v][c];
                            Cr += beta(u) * beta(v) * cr * cos_c[u][r] * cos_r[v][c];
                        }
                    }
                    f[y][x].Y = (int)(0.25 * Y);
                    f[y][x].Cb = (int)(0.25 * Cb);
                    f[y][x].Cr = (int)(0.25 * Cr);
                    RGB rgb = YCbCr2RGB(f[y][x]);
                }
            }
        }
    }
}
int main(int argc, char *argv[]){
    char *input = argv[1], *output = argv[2];
    Bmp bmp = readFile(input);
    initialize(bmp);
    convertToFreq();
    quantization();
    i_quantization();
    convertToBmp();
    writeFile(output, bmp);
    printf("finish...");
}