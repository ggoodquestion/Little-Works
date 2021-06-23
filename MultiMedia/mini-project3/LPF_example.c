#include<stdio.h>
#include<stdlib.h>
#include<math.h>

struct header{
	char ChunkID[4];			
	int ChunkSize;				 
	char Format[4]; 			 
	char SubChunk1ID[4];		
	int SubChunk1Size;			 
	short AudioFormat;			
	short NumChannels;			
	int SampleRate;				
	int ByteRate;				
	short BlockAlign;			
	short BitsPerSample;		
	char SubChunk2ID[4];		
	int SubChunk2Size;		
}Wav_Header;


int main(int argc, char *argv[]){ 
	char* WavIn_Name = argv[1];//input.wav 
	char* WavOut_Name = argv[2];//output.wav
								
	FILE* fp_WavIn;
	FILE* fp_WavOut; 
	
	int N = 0;
	int i,k;
	int N1, N2;
	long long int LPF1,LPF2;
	short Out;

	fp_WavIn = fopen(WavIn_Name,"rb");
	fp_WavOut = fopen(WavOut_Name,"wb");
	
	fread(&Wav_Header, 44, 1, fp_WavIn);
    
    N = Wav_Header.SubChunk2Size/Wav_Header.BlockAlign;//計算樣本數  N=105000
    
    short *Sample = malloc(sizeof(short)*N); 
    float *Xre = malloc(sizeof(float)*N);
    float *Xim = malloc(sizeof(float)*N);
    float *Sample_Un7000Hz = malloc(sizeof(float)*N);
   
    for(i=0; i<N; i++){
    	fread(&Sample[i],2,1,fp_WavIn);
	}
	
	//DFT 
	for(k=0; k<N; k++){	//X[K]
		Xre[k] = 0.0;//real part of X[k]
		Xim[k] = 0.0;//imaginary part of X[k]
		
		for(i=0; i<N; i++){
			Xre[k] += Sample[i] * cos(i * k * M_PI * 2 / N);//real part of X[k]
		}
		for(i=0; i<N; i++){
			Xim[k] -= Sample[i] * sin(i* k * M_PI * 2 / N);//imaginary part of X[k]
		} 
	}

    //LPF
	LPF1 = 7000;
    N1 = ((LPF1*N/Wav_Header.SampleRate)-1); //N1=15311 
    N2 = N - N1; //N2=89686
    
	//LPF
    for(k=0; k<N; k++){
    	Xre[k] = Xre[k]*0.5;
		Xim[k] = Xim[k]*0.5;
    	if(k>=N1 && k<=N2){ 
    		Xre[k] = 0.0;
    		Xim[k] = 0.0;
		}
	} 
	
	
	//IDFT 
	for(k=0; k<N; k++){
		Sample_Un7000Hz[k] = 0.0; 
		for(i=0; i<N; i++){
			Sample_Un7000Hz[k] += (Xre[i]*cos(i * k * M_PI * 2 / N) - Xim[i]*sin(i * k * M_PI * 2 / N));
		}
		Sample_Un7000Hz[k] = Sample_Un7000Hz[k]/N;
	}
	
	fwrite(&Wav_Header,44,1,fp_WavOut); 
    for(i=0;i<N;i++){
        Out = (short)Sample_Un7000Hz[i];
        fwrite(&Out,2,1,fp_WavOut);
    }
	
    fclose(fp_WavIn);
    fclose(fp_WavOut);

	printf("Program Ended");//程式結束 
}
