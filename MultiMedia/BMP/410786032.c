#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <io.h>
#include <malloc.h>
#define PI 3.1415926535897932384626433832795

double **cos_Table;

typedef struct header
{
    short ID;
    int Filesize;
    int Reserved;
    int Bitmap_Data_Offset;
    int Bitmap_Header_Size;
    int Width;
    int Height;
    short Planes;
    short Bits_Per_Pixel;
    int Compression;
    int Bitmap_Data_Size;
    int H_Resolution;
    int V_Resolution;
    int Used_Colors;
    int Important_Colors;
    int Palette;

}header;

typedef struct RGB 
{
	unsigned char R;
	unsigned char G;
	unsigned char B;
	float Y;
	float Cb;
	float Cr;

} ImgRGB;

void read_header(FILE *openptr,header *input_picture){

    fread(&input_picture->ID,sizeof(short),1,openptr);
    fread(&input_picture->Filesize,1,sizeof(int),openptr);
    fread(&input_picture->Reserved,sizeof(int),1,openptr);
    fread(&input_picture->Bitmap_Data_Offset,1,sizeof(int),openptr);
    fread(&input_picture->Bitmap_Header_Size,1,sizeof(int),openptr);
    fread(&input_picture->Width,1,sizeof(int),openptr);
    fread(&input_picture->Height,1,sizeof(int),openptr);
    fread(&input_picture->Planes,sizeof(short),1,openptr);
    fread(&input_picture->Bits_Per_Pixel,1,sizeof(short),openptr);
    fread(&input_picture->Compression,1,sizeof(int),openptr);
    fread(&input_picture->Bitmap_Data_Size,1,sizeof(int),openptr);
    fread(&input_picture->H_Resolution,1,sizeof(int),openptr);
    fread(&input_picture->V_Resolution,1,sizeof(int),openptr);
    fread(&input_picture->Used_Colors,1,sizeof(int),openptr);
    fread(&input_picture->Important_Colors,1,sizeof(int),openptr);
    //fread(&input_picture->Palette,1,sizeof(double),openptr);
}

ImgRGB **F;

ImgRGB** malloc_2D(int row, int col){/*make two order array,and the datatype is IMRGB*/
	ImgRGB **Array, *Data;
	int i;
	Array = (ImgRGB**)malloc(row*sizeof(ImgRGB *));
	Data = (ImgRGB*)malloc(row*col*sizeof(ImgRGB));
	for (i = 0; i<row; i++, Data += col){
		Array[i] = Data;
	}
	return Array;
}

void InputData(FILE* fp, ImgRGB **array, int H, int W, int skip){/*read input.bmp data in the order of B G R*/
	int temp;
	char skip_buf[3];
	int i, j;
	for (i = 0; i<H; i++){
		for (j = 0; j<W; j++){
			temp = fgetc(fp);
			array[i][j].B = temp;
			temp = fgetc(fp);
			array[i][j].G = temp;
			temp = fgetc(fp);
			array[i][j].R = temp;
			if (i<9 && j<8) printf("%02x%02x%02x ", array[i][j].R, array[i][j].G, array[i][j].B); // DEBUG

			if(i==j)
			printf("%f %f %f \n",array[i][j].R, array[i][j].G, array[i][j].B);
		}
		if (skip != 0) fread(skip_buf, skip, 1, fp);
		if (i<9) printf("\n"); // DEBUG
	}
	printf("\n"); // DEBUG
}

void generate_RGB_to_YCbCr(ImgRGB **RGB ,int u ,int v){/*generate_RGB_to_YCbCr*/
	
	unsigned int x,y,z;
	for(x=0;x<8;x++){
		for(y=0;y<8;y++){

			RGB[x+8*u][y+8*v].Y = (int)(0.299 * RGB[x+8*u][y+8*v].R + 0.587 * RGB[x+8*u][y+8*v].G + 0.114 * RGB[x+8*u][y+8*v].B);
			RGB[x+8*u][y+8*v].Cr = (int)(0.5 * RGB[x+8*u][y+8*v].R - 0.419 * RGB[x+8*u][y+8*v].G + 0.081 * RGB[x+8*u][y+8*v].B + 128);
			RGB[x+8*u][y+8*v].Cb = (int)(-0.169 * RGB[x+8*u][y+8*v].R - 0.331 * RGB[x+8*u][y+8*v].G + 0.5 * RGB[x+8*u][y+8*v].B + 128);
			//printf("|%d %d %d| ",RGB[x+8*u][y+8*v].R,RGB[x+8*u][y+8*v].G,RGB[x+8*u][y+8*v].B);
		}
		//printf("\n");
	}
	printf("Finish generate_RGB_to_YCbCr\n");
}

void COS_Table(){/*make cosine table for accelerating the code*/
    int i,x,y;

    cos_Table = (double **) malloc(8 * sizeof(void *));
    
    for(x = 0; x < 8; x++)
    {
		cos_Table[x] = (double *) malloc(8 * sizeof(double));
        for(y=0;y<8;y++){
			cos_Table[x][y]=cos(PI*x*(2*y+1)/16);
		}
    }
}

void DCT(ImgRGB **RGB,int u,int v){/*DCT transform*/
	double belta_u,belta_v;
	int i,j,r,c;
	
	for(r=0;r<8;r++){
		for(c=0;c<8;c++){
			if(r==0)
				belta_u=(sqrt(0.5));
			else
				belta_u=1;
			if(c==0)
				belta_v=(sqrt(0.5));
			else
				belta_v=1;
			F[r+8*u][c+8*v].Y = 0;
			F[r+8*u][c+8*v].Cr = 0;
			F[r+8*u][c+8*v].Cb = 0;
			for(i=0;i<8;i++){
				for(j=0;j<8;j++){
					double Y = RGB[i+8*u][j+8*v].Y;
					double Cr = RGB[i+8*u][j+8*v].Cr;
					double Cb = RGB[i+8*u][j+8*v].Cb;
					
					F[r+8*u][c+8*v].Y += (0.25 * belta_u * belta_v * Y * cos_Table[r][i] * cos_Table[c][j]);
					F[r+8*u][c+8*v].Cr += (0.25 * belta_u * belta_v * Cr * cos_Table[r][i] * cos_Table[c][j]);
					F[r+8*u][c+8*v].Cb += (0.25 * belta_u * belta_v * Cb * cos_Table[r][i] * cos_Table[c][j]);

				}
			}
		}
	}
	
	printf("Finish DCT\n");
}

void Quantization(ImgRGB **RGB,int u,int v){/*Quantization*/

	int i,j;
	double QUN_TABLE_Y[8][8] = { 16 , 11 , 10 , 16 , 24 , 40 , 51 , 61 ,
								12 , 12 , 14 , 19 , 26 , 58 , 60 , 55 ,
								14 , 13 , 16 , 24 , 40 , 57 , 69 , 56 ,
								14 , 17 , 22 , 29 , 51 , 87 , 80 , 62 ,
								18 , 22 , 37 , 56 , 68 , 109 , 103 , 77 ,
								24 , 35 , 55 , 64 , 81 , 104 , 113 , 92 ,
								49 , 64 , 78 , 87 , 103 , 121 , 120 , 101 ,
								72 , 92 , 95 , 98 , 112 , 100 , 103 , 99 };

	double QUN_TABLE_CbCr[8][8]={17 , 18 , 24 , 47 , 99 , 99 , 99 , 99 ,
								18 , 21 , 26 , 66 , 99 , 99 , 99 , 99 ,
								24 , 26 , 56 , 99 , 99 , 99 , 99 , 99 ,
								47 , 66 , 99 , 99 , 99 , 99 , 99 , 99 ,
								99 , 99 , 99 , 99 , 99 , 99 , 99 , 99 ,
								99 , 99 , 99 , 99 , 99 , 99 , 99 , 99 ,
								99 , 99 , 99 , 99 , 99 , 99 , 99 , 99 ,
								99 , 99 , 99 , 99 , 99 , 99 , 99 , 99 };

	for(i=0;i<8;i++){
		for(j=0;j<8;j++){
			F[i+8*u][j+8*v].Y=round( F[i+8*u][j+8*v].Y/QUN_TABLE_Y[i][j] );
			F[i+8*u][j+8*v].Cb=round( F[i+8*u][j+8*v].Cb/QUN_TABLE_Y[i][j] );
			F[i+8*u][j+8*v].Cr=round( F[i+8*u][j+8*v].Cr/QUN_TABLE_Y[i][j] );

		}
	}
	printf("Finish Quantization\n");
}

void INQuantization(ImgRGB **RGB,int u,int v){/*INQuantization*/

	int i,j;
	double QUN_TABLE_Y[8][8] = { 16 , 11 , 10 , 16 , 24 , 40 , 51 , 61 ,
								12 , 12 , 14 , 19 , 26 , 58 , 60 , 55 ,
								14 , 13 , 16 , 24 , 40 , 57 , 69 , 56 ,
								14 , 17 , 22 , 29 , 51 , 87 , 80 , 62 ,
								18 , 22 , 37 , 56 , 68 , 109 , 103 , 77 ,
								24 , 35 , 55 , 64 , 81 , 104 , 113 , 92 ,
								49 , 64 , 78 , 87 , 103 , 121 , 120 , 101 ,
								72 , 92 , 95 , 98 , 112 , 100 , 103 , 99};

	double QUN_TABLE_CbCr[8][8]={17 , 18 , 24 , 47 , 99 , 99 , 99 , 99 ,
								18 , 21 , 26 , 66 , 99 , 99 , 99 , 99 ,
								24 , 26 , 56 , 99 , 99 , 99 , 99 , 99 ,
								47 , 66 , 99 , 99 , 99 , 99 , 99 , 99 ,
								99 , 99 , 99 , 99 , 99 , 99 , 99 , 99 ,
								99 , 99 , 99 , 99 , 99 , 99 , 99 , 99 ,
								99 , 99 , 99 , 99 , 99 , 99 , 99 , 99 ,
								99 , 99 , 99 , 99 , 99 , 99 , 99 , 99 };

	for(i=0;i<8;i++){
		for(j=0;j<8;j++){
			F[i+8*u][j+8*v].Y= F[i+8*u][j+8*v].Y * QUN_TABLE_Y[i][j] ;
			F[i+8*u][j+8*v].Cb= F[i+8*u][j+8*v].Cb * QUN_TABLE_Y[i][j] ;
			F[i+8*u][j+8*v].Cr= F[i+8*u][j+8*v].Cr * QUN_TABLE_Y[i][j] ;
		}
	}
	printf("Finish INQuantization\n");
}

void IDCT(ImgRGB **RGB,int u,int v){/*Inverse DCT*/
	double belta_u,belta_v;
	int i,j,r,c;

	for(r=0;r<8;r++){
		for(c=0;c<8;c++){
			RGB[r+8*u][c+8*v].Y = 0;
			RGB[r+8*u][c+8*v].Cr = 0;
			RGB[r+8*u][c+8*v].Cb = 0;
			for(i=0;i<8;i++){
				for(j=0;j<8;j++){

					if(i==0)
						belta_u=1/(sqrt(2));
					else
						belta_u=1;
					if(j==0)
						belta_v=1/(sqrt(2));
					else
						belta_v=1;
					
					RGB[r+8*u][c+8*v].Y += (0.25 * belta_u * belta_v * F[i+8*u][j+8*v].Y * cos_Table[i][r] * cos_Table[j][c]);
					RGB[r+8*u][c+8*v].Cr += (0.25 * belta_u * belta_v * F[i+8*u][j+8*v].Cr * cos_Table[i][r] * cos_Table[j][c]);
					RGB[r+8*u][c+8*v].Cb += (0.25 * belta_u * belta_v * F[i+8*u][j+8*v].Cb * cos_Table[i][r] * cos_Table[j][c]);

				}
			}
		}
	}
	printf("Finish DCT\n");
}

void generate_YCbCr_to_RGB(ImgRGB **RGB ,int u ,int v){/*generate YCbCr to RGB*/
	int x,y;
	int r,g,b;
	for(x=0;x<8;x++){
		for(y=0;y<8;y++){

			r = RGB[x+8*u][y+8*v].Y + 1.403 * (RGB[x+8*u][y+8*v].Cr - 128);
			g = RGB[x+8*u][y+8*v].Y - 0.714 * (RGB[x+8*u][y+8*v].Cr - 128) - 0.344 * (RGB[x+8*u][y+8*v].Cb - 128);
			b = RGB[x+8*u][y+8*v].Y + 1.773 * (RGB[x+8*u][y+8*v].Cb - 128);
			//printf("|%d %d %d| ", r,g,b);
			if(r>255)
			r=255;

			if(r<0)
			r=0;

			if(g>255)
			g=255;

			if(g<0)
			g=0;

			if(b>255)
			b=255;

			if(b<0)
			b=0;
			
			RGB[x+8*u][y+8*v].R=r;
			RGB[x+8*u][y+8*v].G=g;
			RGB[x+8*u][y+8*v].B=b;
			//printf("|%d %d %d| ", RGB[x+8*u][y+8*v].R,RGB[x+8*u][y+8*v].G,RGB[x+8*u][y+8*v].B);
		}
		//printf("\n");
	}
	printf("Finish generate_YCbCr_to_RGB\n");
}

/*output header and data*/
void output_bmp(ImgRGB **RGB, FILE* outfile, header input_picture, int skip){
	char skip_buf[3] = { 0, 0, 0 };
	int x, y;
    
	fwrite(&input_picture.ID, sizeof(short), 1, outfile);
	fwrite(&input_picture.Filesize, sizeof(int), 1, outfile);
	fwrite(&input_picture.Reserved, sizeof(int), 1, outfile);
	fwrite(&input_picture.Bitmap_Data_Offset, sizeof(int), 1, outfile);
	fwrite(&input_picture.Bitmap_Header_Size, sizeof(int), 1, outfile);
	fwrite(&input_picture.Width, sizeof(int), 1, outfile);
	fwrite(&input_picture.Height, sizeof(int), 1, outfile);
	fwrite(&input_picture.Planes, sizeof(short), 1, outfile);
	fwrite(&input_picture.Bits_Per_Pixel, sizeof(short), 1, outfile);
	fwrite(&input_picture.Compression, sizeof(int), 1, outfile);
	fwrite(&input_picture.Bitmap_Data_Size, sizeof(int), 1, outfile);
	fwrite(&input_picture.H_Resolution, sizeof(int), 1, outfile);
	fwrite(&input_picture.V_Resolution, sizeof(int), 1, outfile);
	fwrite(&input_picture.Used_Colors, sizeof(int), 1, outfile);
	fwrite(&input_picture.Important_Colors, sizeof(int), 1, outfile);

	for (x = 0; x<input_picture.Height; x++){
		for (y = 0; y<input_picture.Width; y++){
		
			fwrite(&RGB[x][y].B, sizeof(char), 1, outfile);
			fwrite(&RGB[x][y].G, sizeof(char), 1, outfile);
			fwrite(&RGB[x][y].R, sizeof(char), 1, outfile);
			if (x<9 && y<8) printf("%02x%02x%02x ", RGB[x][y].R, RGB[x][y].G, RGB[x][y].B); // DEBUG
		}
		if (skip != 0) { fwrite(skip_buf, skip, 1, outfile); }
		if (x<9) printf("\n"); // DEBUG
	}
}

int main (int argc,char *argv[])
{
    FILE *inptr;
    FILE *outptr;
    header input_picture;
	char* Out_Name = argv[2];
    char* In_Name = argv[1];

    inptr=fopen(In_Name,"rb");
	if (inptr) printf("Open %s as input file!\n", In_Name);
	else printf("Fail to open %s as input file!\n", In_Name);

	outptr = fopen(Out_Name, "wb");

	if (outptr) printf("Open %s as output file!\n", Out_Name);
	else printf("Fail to open %s as output file!\n", Out_Name);

    read_header(inptr,&input_picture);

    int H = input_picture.Height;
	int W = input_picture.Width;

// skip paddings at the end of each image line
	int skip = (4 - (input_picture.Width * 3) % 4);
	if (skip == 4) skip = 0;
	char skip_buf[3] = { 0, 0, 0 };

// 8x8, if not multiples of 8, then bitmap padded, i.e. one more block
	int block_H = H / 8;
	if (H % 8 != 0) block_H ++;
	int block_W = W / 8;
	if (W % 8 != 0) block_W ++;

    // printf("Image size: Width: %d x Height=%d pixels, total %d bytes\n", W, H, W*H * 3);
	// printf("Image line skip=%d bytes\n", skip);
	// printf("# of 8x8 blocks: W=%d x H=%d blocks, total %d blocks\n", block_W, block_H, block_W*block_H);
	// printf("\nLoading BMP data: RRGGBB RRGGBB ... (BGR format in BMP file)\n");

	ImgRGB **Data_RGB = malloc_2D(input_picture.Height, input_picture.Width);
	InputData(inptr, Data_RGB, input_picture.Height, input_picture.Width, skip);
    fclose(inptr);
	//printf("Data size loaded: %d bytes\n", H*W * 3);
	COS_Table();
	F=malloc_2D(H, W);

	int u,v;
	for(u=0;u<block_W;u++){
		for(v=0;v<block_H;v++){
			
			generate_RGB_to_YCbCr(Data_RGB,u,v);
			DCT( Data_RGB, u, v);
			Quantization(Data_RGB,u,v);
			INQuantization(Data_RGB,u,v);
			IDCT( Data_RGB, u, v);
			generate_YCbCr_to_RGB(Data_RGB,u ,v);
		}
	}

    output_bmp(Data_RGB, outptr, input_picture, skip);
	printf("\n"); // DEBUG
	printf("Output decompressed image to: %s\n\n", Out_Name);
	printf("Free memory and close files!\n");

	free(Data_RGB[0]);
	free(Data_RGB);
	
	fclose(outptr);

	printf("Job done!\n");
	free(Data_RGB);
	free(cos_Table);
	free(F);
	
	return 0;
}