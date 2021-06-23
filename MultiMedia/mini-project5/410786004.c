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
double qq_table[8][8] = {{17, 18, 24, 47, 99, 99, 99, 99},
                         {18, 21, 26, 66, 99, 99, 99, 99},
                         {24, 26, 56, 99, 99, 99, 99, 99},
                         {47, 66, 99, 99, 99, 99, 99, 99},
                         {99, 99, 99, 99, 99, 99, 99, 99},
                         {99, 99, 99, 99, 99, 99, 99, 99},
                         {99, 99, 99, 99, 99, 99, 99, 99},
                         {99, 99, 99, 99, 99, 99, 99, 99}};                         
int z_x[64] = {0, 1, 0, 0, 1, 2, 3, 2,
                1, 0, 0, 1, 2, 3, 4, 5,
                4, 3, 2, 1, 0, 0, 1, 2,
                3, 4, 5, 6, 7, 6, 5, 4,
                3, 2, 1, 0, 1, 2, 3, 4,
                5, 6, 7, 7, 6, 5, 4, 3,
                2, 3, 4, 5, 6, 7, 7, 6,
                5, 4, 5, 6, 7, 7, 6, 7};
int z_y[64] = {0, 0, 1, 2, 1, 0, 0, 1,
                2, 3, 4, 3, 2, 1, 0, 0,
                1, 2, 3, 4, 5, 6, 5, 4,
                3, 2, 1, 0, 0, 1, 2, 3,
                4, 5, 6, 7, 7, 6, 5, 4,
                3, 2, 1, 2, 3, 4, 5, 6,
                7, 7, 6, 5, 4, 3, 4, 5,
                6, 7, 7, 6, 5, 6, 7, 7};
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
    RGB **bitmap_data;
}Bmp;
typedef struct f_YCbCr{
    double Y;
    double Cb;
    double Cr;
}f_YCbCr;
typedef struct codebook{
    int value;
    unsigned int code;
    int size;
}codebook;
typedef struct node{
    int value;
    int num;
    int pos;
    struct node *right;
    struct node *left;
}node;
typedef struct Dsize{
    int Y;
    int Cb;
    int Cr;
}Dsize;

int H, W;
int size_y, size_cb, size_cr;
f_YCbCr **F;
YCbCr **f;
double **cos_r, **cos_c;
int *AC_y, *AC_cb, *AC_cr;
int *DC_y, *DC_cb, *DC_cr;
int *rle_y, *rle_cb, *rle_cr;
Dsize byteDc, byteAc, DBS, ABS;

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
    bmp.bitmap_data = (RGB**) malloc(sizeof(void*) * header.height);
    for(int i = 0; i < header.height    ; i++){
        bmp.bitmap_data[i] = malloc(sizeof(RGB) * header.width);
    }
    int skip;
    for(int i = 0; i < header.height; i++){
        skip = 0;
        for(int j = 0; j < header.width; j++){
            fread(&bmp.bitmap_data[i][j], sizeof(RGB), 1, f_input);
            skip += 3;
        }
        skip %= 4;
        char buf;
        for(int j = 0; j < skip; j++) fread(&buf, 1, 1, f_input);
    }

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
    
    int skip;
    for(int i = 0; i < header.height; i++){
        skip = 0;
        for(int j = 0; j < header.width; j++){
            RGB rgb = YCbCr2RGB(f[i][j]);
            fwrite(&rgb, sizeof(rgb), 1, f_output);
            skip += 3;
        }
        skip %= 4;
        char buf = 0;
        for(int j = 0; j < skip; j++) fwrite(&buf, 1, 1, f_output);
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
    int rowSize = ((bmp.header.width+31)/32)*4*8;
    for(int i = 0; i < h_; i++){
        for(int j = 0; j < w_; j++){
            if((i >= h) || (j >= w)){
                /* Fill 0 in the outside pixels */
                RGB tmp = {0, 0, 0};
                f[i][j] = RGB2YCbCr(tmp);
            }else{
                f[i][j] = RGB2YCbCr(bmp.bitmap_data[i][j]);
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
void emptize(Bmp bmp){
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
    for(int i = 0; i < h_; i++){
        for(int j = 0; j < w_; j++){
            RGB tmp = {0, 0, 0};
            f_YCbCr ytmp = {0, 0, 0};
            f[i][j] = RGB2YCbCr(tmp);
            F[i][j] = ytmp;
        }
    }
    for(int i = 0; i < H*W*64; i++){
        AC_y[i] = 0;
        AC_cb[i]= 0;
        AC_cr[i] = 0;
    }
    for(int i = 0; i < H*W; i++){
        DC_y[i] = 0;
        DC_cb[i]= 0;
        DC_cr[i] = 0;
    }
    for(int i = 0; i < size_y; i++){
        rle_y[i] = 0;
    }
    for(int i = 0; i < size_cb; i++){
        rle_cb[i] = 0;
    }
    for(int i = 0; i < size_cr; i++){
        rle_cr[i] = 0;
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
                    double y_ = round((F[y][x].Y)/(q_table[c][r]));
                    double cb = round((F[y][x].Cb)/(qq_table[c][r]));
                    double cr = round((F[y][x].Cr)/(qq_table[c][r]));
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
                    int cb = (F[y][x].Cb)*(qq_table[c][r]);
                    int cr = (F[y][x].Cr)*(qq_table[c][r]);
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
void sort(node **tar, int begin, int size){
    for(int i = begin; i < size; i++){
        int j = i;
        while(j > begin && (tar[j-1]->num > tar[j]->num)){
            node *tmp = tar[j];
            tar[j] = tar[j-1];
            tar[j-1] = tmp;
            j--;
        }
    }
}
void copyIntArr(int *ori, int *cpy, int size){
    for(int i = 0; i < size; i++){
        cpy[i] = ori[i];
    }
}
int getCodeBookSize(int *tar, int size){
    for(int i = 0; i < size; i++){
        int j = i;
        while(j > 0 && tar[j-1] > tar[j]){
            int tmp = tar[j];
            tar[j] = tar[j-1];
            tar[j-1] = tmp;
            j--;
        }
    }
    int count = 1;
    int value = tar[0];
    for(int i = 1; i < size; i++){
        if(tar[i] != value){
            value = tar[i];
            count++;
        }
    }
    return count;
}
void initNode(int *tar, int size, node **cop, codebook *book){
    int count = 0;
    int value = tar[0];
    int num = 1;
    for(int i = 1; i < size; i++){
        if(tar[i] != value){
            cop[count]->value = value;
            cop[count]->num = num;
            cop[count]->left = NULL;
            cop[count]->right = NULL;
            cop[count]->pos = count;
            value = tar[i];
            num = 1;
            count++;
        }else{
            num++;
        }
    }
    cop[count]->value = value;
    cop[count]->num = num;
    cop[count]->left = NULL;
    cop[count]->right = NULL;
    sort(cop, 0, ++count);
    for(int i = 0; i < count; i++){
        cop[i]->pos = i;
        book[i].code = 0;
        book[i].value = cop[i]->value;
        book[i].size = 0;
    }
}
void buildTree(node **cop, int size){
    for(int i = 0; i < size-1; i++){ //size-1
        sort(cop, i, size);
        node *new = (node*) malloc(sizeof(node));
        new->num = cop[i]->num + cop[i+1]->num;
        new->left = cop[i];
        new->right = cop[i+1];
        cop[i+1] = new;
    }
}
void findCode(int buf, node *now, codebook *book, int layer){
    if(now->left == NULL || now->right == NULL){
        book[now->pos].code = buf;
        book[now->pos].size = layer;
        return;
    }
    buf = buf << 1;
    int buf1 = buf+0;
    int buf2 = buf+1;
    findCode(buf1, now->left, book, layer+1);
    findCode(buf2, now->right, book, layer+1);
    return;
}
codebook getCode(int value, codebook *book){
    // printf("origin: %d, ", value);
    codebook res;
    for(int i = 0;; i++){
        if(book[i].value == value){
            res = book[i];
            break;
        }
    }
    // printf("value: %d, code: %d, size: %d\n", res.value, res.code, res.size);
    return res;
}
int outputCode(FILE *fp, int fs, int *data, codebook *book){
    char buf = 0;
    int offset = 0;
    int flag = 1;
    for(int i = 0; i < fs; i++){
        codebook nc = getCode(data[i], book);
        for(int j = 0; j < nc.size; j++){
            unsigned int tmp = nc.code;
            tmp <<= (32 - nc.size + j);
            tmp >>= 31;
            buf += tmp;
            if(flag == 8){
                fwrite(&buf, 1, 1, fp);
                offset++;
                buf = 0;
                flag = 1;
            }else{
                buf <<= 1;
                flag++;
            }
        }
    }
    if(flag != 8){
        buf <<= 8-flag;
    }
    fwrite(&buf, 1, 1, fp);
    return offset+1;
}
void outputBook(char *fn, codebook *book, int size){
    FILE *fp = fopen(fn, "w");
    for(int i = 0; i < size; i++){
        codebook tmp = book[i];
        fprintf(fp, "%d %d ", tmp.value, tmp.size);
        unsigned int code;
        for(int j = 0; j < tmp.size; j++){
            code = tmp.code;
            code <<= (32 - tmp.size + j);
            code >>= 31;
            fprintf(fp, "%u", code);
        }
        fprintf(fp, "\n");
    }
    fclose(fp);
}
void huffman(){
    printf("Huffman...\n");

    int *tmp_dy = malloc(sizeof(int) * H*W);
    int *tmp_dcb = malloc(sizeof(int) * H*W);
    int *tmp_dcr = malloc(sizeof(int) * H*W);
    copyIntArr(DC_y, tmp_dy, H*W);
    copyIntArr(DC_cb, tmp_dcb, H*W);
    copyIntArr(DC_cr, tmp_dcr, H*W);

    DBS.Y = getCodeBookSize(tmp_dy, H*W);
    DBS.Cb = getCodeBookSize(tmp_dcb, H*W);
    DBS.Cr = getCodeBookSize(tmp_dcr, H*W);
    codebook *dy_book = (codebook*) malloc(sizeof(codebook) * DBS.Y);
    codebook *dcb_book = (codebook*) malloc(sizeof(codebook) * DBS.Cb);
    codebook *dcr_book = (codebook*) malloc(sizeof(codebook) * DBS.Cr);
    node **cop_y = (node**) malloc(sizeof(void*) * DBS.Y);
    node **cop_cb = (node**) malloc(sizeof(void*) * DBS.Cb);
    node **cop_cr = (node**) malloc(sizeof(void*) * DBS.Cr);
    for(int i = 0; i < DBS.Y; i++){
        cop_y[i] = (node*) malloc(sizeof(node));
    }
    for(int i = 0; i < DBS.Cb; i++){
        cop_cb[i] = (node*) malloc(sizeof(node));
    }
    for(int i = 0; i < DBS.Cr; i++){
        cop_cr[i] = (node*) malloc(sizeof(node));
    }
    
    initNode(tmp_dy, H*W, cop_y, dy_book);
    initNode(tmp_dcb, H*W, cop_cb, dcb_book);
    initNode(tmp_dcr, H*W, cop_cr, dcr_book);
    buildTree(cop_y, DBS.Y);
    buildTree(cop_cb, DBS.Cb);
    buildTree(cop_cr, DBS.Cr);
    findCode(0, cop_y[DBS.Y-1], dy_book, 0);
    findCode(0, cop_cb[DBS.Cb-1], dcb_book, 0);
    findCode(0, cop_cr[DBS.Cr-1], dcr_book, 0);
    //
    int *tmp_ry = malloc(sizeof(int) * size_y);
    int *tmp_rcb = malloc(sizeof(int) * size_cb);
    int *tmp_rcr = malloc(sizeof(int) * size_cr);
    copyIntArr(rle_y, tmp_ry, size_y);
    copyIntArr(rle_cb, tmp_rcb, size_cb);
    copyIntArr(rle_cr, tmp_rcr, size_cr);

    ABS.Y = getCodeBookSize(tmp_ry, size_y);
    ABS.Cb = getCodeBookSize(tmp_rcb, size_cb);
    ABS.Cr = getCodeBookSize(tmp_rcr, size_cr);
    codebook *ay_book = (codebook*) malloc(sizeof(codebook) * ABS.Y);
    codebook *acb_book = (codebook*) malloc(sizeof(codebook) * ABS.Cb);
    codebook *acr_book = (codebook*) malloc(sizeof(codebook) * ABS.Cr);
    node **r_y = (node**) malloc(sizeof(void*) * ABS.Y);
    node **r_cb = (node**) malloc(sizeof(void*) * ABS.Cb);
    node **r_cr = (node**) malloc(sizeof(void*) * ABS.Cr);
    for(int i = 0; i < ABS.Y; i++){
        r_y[i] = (node*) malloc(sizeof(node));
    }
    for(int i = 0; i < ABS.Cb; i++){
        r_cb[i] = (node*) malloc(sizeof(node));
    }
    for(int i = 0; i < ABS.Cr; i++){
        r_cr[i] = (node*) malloc(sizeof(node));
    }

    initNode(tmp_ry, size_y, r_y, ay_book);
    initNode(tmp_rcb, size_cb, r_cb, acb_book);
    initNode(tmp_rcr, size_cr, r_cr, acr_book);
    buildTree(r_y, ABS.Y);
    buildTree(r_cb, ABS.Cb);
    buildTree(r_cr, ABS.Cr);
    findCode(0, r_y[ABS.Y-1], ay_book, 0);
    findCode(0, r_cb[ABS.Cb-1], acb_book, 0);
    findCode(0, r_cr[ABS.Cr-1], acr_book, 0);

    FILE* fp = fopen("buff.txt", "wb");
    byteDc.Y = outputCode(fp, W*H, DC_y, dy_book);
    byteDc.Cb = outputCode(fp, W*H, DC_cb, dcb_book);
    byteDc.Cr = outputCode(fp, W*H, DC_cr, dcr_book);

    byteAc.Y = outputCode(fp, size_y, rle_y, ay_book);
    byteAc.Cb = outputCode(fp, size_cb, rle_cb, acb_book);
    byteAc.Cr = outputCode(fp, size_cr, rle_cr, acr_book);

    fclose(fp);

    outputBook("DC_y.txt", dy_book, DBS.Y);
    outputBook("DC_cb.txt", dcb_book, DBS.Cb);
    outputBook("DC_cr.txt", dcr_book, DBS.Cr);
    outputBook("AC_y.txt", ay_book, ABS.Y);
    outputBook("AC_cb.txt", acb_book, ABS.Cb);
    outputBook("AC_cr.txt", acr_book, ABS.Cr);
}
void loadBook( char *fn, int bs, node *root){
    root->left = NULL;
    root->right = NULL;
    FILE *fp = fopen(fn, "r");
    int value;
    int size;
    node *now;
    for(int i = 0; i < bs; i++){
        fscanf(fp, "%d %d", &value, &size);
        char c;
        now = root;
        fgetc(fp);
        while((c=fgetc(fp)) != '\n'){
            if(c == '0'){
                if(now->left == NULL){
                    node *tmp = malloc(sizeof(node));
                    tmp->left = NULL;
                    tmp->right = NULL;
                    now->left = tmp;
                }
                now = now->left;
            }else if(c == '1'){
                if(now->right == NULL){
                    node *tmp = malloc(sizeof(node));
                    tmp->left = NULL;
                    tmp->right = NULL;
                    now->right = tmp;
                }
                now = now->right;
            }
        }
        now->value = value;
    }
}
void decode(unsigned char *data, int *con, int cs, int bs, node *tree){
    int flag = 0;
    unsigned char bit;
    node *now = tree;
    for(int i = 0; i < bs; i++){
        for(int j = 0; j < 8; j++){
            bit = data[i];
            bit <<= j;
            bit >>= 7;
            if(bit == 0){
                now = now->left;
            }else if(bit == 1){
                now = now->right;
            }
            //if(now == NULL) return;
            if((now->left == NULL)&&(now->right == NULL)){
                con[flag] = now->value;
                flag++;
                if(flag == cs) return;
                now = tree;
            }
        }
    }
    return;
}
void D_huffman(){
    printf("Huffman decoding...\n");
    node *dyt = malloc(sizeof(node));
    node *dcbt = malloc(sizeof(node));
    node *dcrt = malloc(sizeof(node));
    node *ayt = malloc(sizeof(node));
    node *acbt = malloc(sizeof(node));
    node *acrt = malloc(sizeof(node));

    loadBook("DC_y.txt", DBS.Y, dyt);
    loadBook("DC_cb.txt", DBS.Cb, dcbt);
    loadBook("DC_cr.txt", DBS.Cr, dcrt);
    loadBook("AC_y.txt", ABS.Y, ayt);
    loadBook("AC_cb.txt", ABS.Cb, acbt);
    loadBook("AC_cr.txt", ABS.Cr, acrt);

    FILE *fp = fopen("buff.txt", "rb");

    unsigned char *con = malloc(byteDc.Y);
    fread(con, 1, byteDc.Y, fp);
    decode(con, DC_y, H*W, byteDc.Y, dyt);
    free(con);

    con = malloc(byteDc.Cb);
    fread(con, 1, byteDc.Cb, fp);
    decode(con, DC_cb, H*W, byteDc.Cb, dcbt);
    free(con);

    con = malloc(byteDc.Cr);
    fread(con, 1, byteDc.Cr, fp);
    decode(con, DC_cr, H*W, byteDc.Cr, dcrt);
    free(con);
    
    con = malloc(byteAc.Y);
    fread(con, 1, byteAc.Y, fp);
    decode(con, rle_y, size_y, byteAc.Y, ayt);
    free(con);

    con = malloc(byteAc.Cb);
    fread(con, 1, byteAc.Cb, fp);
    decode(con, rle_cb, size_cb, byteAc.Cb, acbt);
    free(con);

    con = malloc(byteAc.Cr);
    fread(con, 1, byteAc.Cr, fp);
    decode(con, rle_cr, size_cr, byteAc.Cr, acrt);
    free(con);
}
void DPCM(){
    /* DPCM for DC */
    printf("DCPM...\n");
    DC_y = malloc(sizeof(int) * H*W);
    DC_cb = malloc(sizeof(int) * H*W);
    DC_cr = malloc(sizeof(int) * H*W);
    int b_y = 0, b_cb = 0, b_cr = 0;
    for(int i = 0; i < H; i++){
        for(int j = 0; j < W; j++){
            int pos = i * W + j;
            int x = j * 8, y = i * 8;
            DC_y[pos] = F[y][x].Y - b_y;
            DC_cb[pos] = F[y][x].Cb - b_cb;
            DC_cr[pos] = F[y][x].Cr - b_cr;
            b_y = F[y][x].Y;
            b_cb = F[y][x].Cb;
            b_cr = F[y][x].Cr;
        }
    }
}
void zigzag(){
    printf("Zigzag...\n");
    AC_y = malloc(sizeof(int) * H*W*64);
    AC_cb = malloc(sizeof(int) * H*W*64);
    AC_cr = malloc(sizeof(int) * H*W*64);
    for(int i = 0; i < H; i++){
        for(int j = 0; j < W; j++){
            for(int m = 0; m < 64; m++){
                if(m == 0){
                    AC_y[j*64+i*64*W+m] = 0;
                    AC_cb[j*64+i*64*W+m] = 0;
                    AC_cr[j*64+i*64*W+m] = 0;
                    continue;
                }
                AC_y[j*64+i*64*W+m] = F[z_y[m] + i*8][z_x[m] + j*8].Y;
                AC_cb[j*64+i*64*W+m] = F[z_y[m] + i*8][z_x[m] + j*8].Cb;
                AC_cr[j*64+i*64*W+m] = F[z_y[m] + i*8][z_x[m] + j*8].Cr;
            }
        }
    }
}
int RLE_size(int *z){
    int count = 2;
    int value = z[1];
    for(int i = 2; i < H*W*64; i++){
        if((i%64) == 0) continue;
        if(z[i] != value){
            count++;
            value = z[i];
        }
    }
    int size = count*2;
    return size;
}
int RLE(int *z, int *res){
    printf("RLE...\n");
    int count = 0;
    int value = z[1];
    int num = 1;
    for(int i = 2; i < H*W*64; i++){
        if((i%64) == 0) continue;
        if(z[i] != value){
            res[count] = num;
            res[count+1] = value;
            value = z[i];
            num = 1;
            count += 2;
        }else{
            num++;
        }
    }
    res[count] = num;
    res[count+1] = value;
    res[count+2] = 0;
    res[count+3] = 0;
}
void I_RLE(int *z, int *src){
    printf("Inverse RLE..\n");
    int ai = 0;
    for(int i = 0; src[i] != 0; i+=2){
        for(int j = 0; j < src[i]; j++, ai++){
            if((ai%64) == 0) ai++;
            z[ai] = src[i+1];
        }
    }
}
void I_zigzag(){
    printf("Inverse zizag\n");
    for(int i = 0; i < H; i++){
        for(int j = 0; j < W; j++){
            for(int m = 0; m < 64; m++){
                F[z_y[m] + i*8][z_x[m] + j*8].Y = AC_y[j*64+i*64*W+m];
                F[z_y[m] + i*8][z_x[m] + j*8].Cb = AC_cb[j*64+i*64*W+m];
                F[z_y[m] + i*8][z_x[m] + j*8].Cr = AC_cr[j*64+i*64*W+m];
            }
        }
    }
}
void I_DPCM(){
    printf("Inverse DPCM...\n");
    int b_y = 0, b_cb = 0, b_cr = 0;
    for(int i = 0; i < H; i++){
        for(int j = 0; j < W; j++){
            int pos = i * W + j;
            int x = j * 8, y = i * 8;
            F[y][x].Y = DC_y[pos] + b_y;
            F[y][x].Cb = DC_cb[pos] + b_cb;
            F[y][x].Cr = DC_cr[pos] + b_cr;
            b_y = F[y][x].Y;
            b_cb = F[y][x].Cb;
            b_cr = F[y][x].Cr;
        }
    }
}
int main(int argc, char *argv[]){
    char *input = argv[1], *output = argv[2];
    Bmp bmp = readFile(input);
    initialize(bmp);
    convertToFreq();
    quantization();

    /* Compression */
    DPCM();
    zigzag();
    
    size_y =  RLE_size(AC_y);
    size_cb =  RLE_size(AC_cb);
    size_cr =  RLE_size(AC_cr);
    rle_y = (int*) malloc(sizeof(int) * size_y);
    rle_cb = (int*) malloc(sizeof(int) * size_cb);
    rle_cr = (int*) malloc(sizeof(int) * size_cr);
    RLE(AC_y, rle_y);
    RLE(AC_cb, rle_cb);
    RLE(AC_cr, rle_cr);
    
    //Huffman
    huffman(size_y, size_cb, size_cr);
    emptize(bmp);
    D_huffman();

    I_RLE(AC_y, rle_y);
    I_RLE(AC_cb, rle_cb);
    I_RLE(AC_cr, rle_cr);

    I_zigzag();
    I_DPCM();

    i_quantization();
    convertToBmp();
    writeFile(output, bmp);

    printf("finish...");
}