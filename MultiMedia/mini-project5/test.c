#include<stdio.h>
#include<stdlib.h>

int main(){
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
    int **before = (int**) malloc(sizeof(void*) * 8);
    int *after = malloc(sizeof(int) * 64);
    for(int i = 0; i < 8; i++){
        before[i] = malloc(sizeof(int) * 8);
    }
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            before[i][j] = j;
        }
    }
    for(int i = 0; i < 64; i++){
        after[i] = before[z_y[i]][z_x[i]];
    }
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            printf("%d\t", before[i][j]);
        }
        printf("\n");
    }
    printf("\n");
    for(int i = 0; i < 64; i++){
        printf("%d ", after[i]);
    }
    printf("\n");
    for(int i = 0; i < 64; i++){
        before[z_y[i]][z_x[i]] = after[i];
    }
    printf("\n");

    int count = 2;
    int value = after[0];
    for(int i = 1; i < 64; i++){
        if(after[i] != value){
            count++;
            value = after[i];
        }
    }
    int size = count*2;
    char *rle = malloc(sizeof(char) * size);

    count = 0;
    value = after[0];
    int num = 1;
    for(int i = 1; i < 64; i++){
        if(after[i] != value){
            rle[count] = num;
            rle[count+1] = value;
            value = after[i];
            num = 1;
            count += 2;
        }else{
            num++;
        }
    }
    rle[count] = num;
    rle[count+1] = value;
    rle[count+2] = 0;
    rle[count+3] = 0;
    for(int i = 0; i < size; i+=2){
        printf("(%d,%d) ", rle[i], rle[i+1]);
    }

    int ai = 0;
    for(int i = 0; i < rle[i] != 0; i+=2){
        for(int j = 0; j < rle[i]; j++, ai++){
            after[ai] = rle[i+1];
        }
    }
    
    printf("\n");
    for(int i = 0; i < 64; i++){
        printf("%d ", after[i]);
    }
    printf("fini");
}