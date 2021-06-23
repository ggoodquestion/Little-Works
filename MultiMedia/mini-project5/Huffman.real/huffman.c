#include<stdio.h>
#include<stdlib.h>
#include<math.h>

typedef struct codebook{
    char value;
    char code;
}codebook;

typedef struct node{
    char value;
    int num;
    int pos;
    struct node *right;
    struct node *left;
}node;

void sort(node **tar, int begin, int size){
    for(int i = begin; i < size; i++){
        int j = i;
        while(j > 0 && tar[j-1]->num > tar[j]->num){
            node *tmp = tar[j];
            tar[j] = tar[j-1];
            tar[j-1] = tmp;
            j--;
        }
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
int main(){
    printf("Huffman");
    
    /* Read file */
    char *inputData;
    int dataSize = 0;
    FILE *input = fopen("input.txt", "r");
    fwrite(inputData, );

    printf("finish...");
}