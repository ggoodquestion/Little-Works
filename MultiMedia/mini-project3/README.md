# �h�C��P�T���B�z
## �@�~�T
>### 410786004 �q�T�T Ĭ�a�s

---


## ²��
�����@�~�����ϥ�argv�W�i��ӰѼơA���O����J�ɦW�P��X�ɦW
�A�ëŧistruct Stereo�������n�D���ȡA�ϥ�fread�N���Ū�iStereo�}
�C�A�ϥ�DFT����ର�W��A�b�����e�ڻݭn��l��cos�Bsin�Mwindow
table�H�[�ֵ{���t�סADFT�ڱN��X�Ӫ��ƼƦs�b�ڼg��struct ImgNum
�̡A�b�i��LPF�A�N���o�����W�v�⦨frequency bin�A�h�R���W�L�ӽd��
��k�ȡA�̫ᰵIDFT�A�NDFT��o���Ƽƥh�PIDFT���שԪ�ܦ��ۭ��A�ѩ�
�ഫ��O��ưT���]�����ΦҼ{imagine part�A�]���u�a��ۭ��᪺real
part�A�̫���X����ƧY���o�i�᪺���ɡC�ܩ��D�حn�D��������T�A��
�ϥΪ���k���A���N�e0.1�����q�ƻs�U�ӡA���ۨC0.1��K��h�Ӥ��q
�A�Y�i�h�����T�C

## �s�@�Q�k�P�n�I
### 1. DFT�G
- �ŧi��ʺA�}�C�x�sspectrum
``` 
ImgNum **freqField_l;
ImgNum **freqField_r;
```
- �i��SDFT�ñN�ƼƵ��G�s�J
- SDFT�Ganalize size=2ms�Bframe size=3ms�Bframe interval=1ms
�Bwindow type=hanning
### 2. LPF�G
- �N20k�⦨�������N���frequency bin
```
r1 = ((range * K / sampleRate) - 1);
r2 = K - r1;
for(int i = 0; i < fNum; i++){
    for(int k = 0; k < K; k++){
        if(k > r1 && k < r2){
            ImgNum tmp = {0, 0};
            freqField_l[i][k] = tmp;
            freqField_r[i][k] = tmp;
        }
    }
}
```
- �N�ӽd��(�]�N�O���W����)���W�v�]��0
### 3. IDFT�G
- �NDFT���G�i���ഫ�A������ƼƬۭ�- �ѩ���X����ưT���]���ۭ���u�a�o��Ƴ����A�]���O�곡�ۭ�
��h�곡�ۭ��A�̫ᰣK�O�]���[�FK���n����������q
```
x[m*M+n].left += (short) ((l_sum.real - l_sum.imagine) / K);
x[m*M+n].right += (short) ((r_sum.real - r_sum.imagine) / K);
```
- �ഫ�᪺data�Y�O�L�o�����T��
### 4. ����2��tones�F
- �[�������T���W�v����60Hz�P16000Hz
- �]�����[��e0.1����^���A���ۨC0.1���h�Ӥ��q�Y���������
�T
- ��0.1����]�O�ڰ��]0.1��ɨ����T�Ҭ��P������ƭ�(�g�L���Ҥ]�O�p���B�~�t�ܤp)

```
int sgp = 0.1 * sampleRate;
Stereo *tones = (Stereo *) malloc(sizeof(Stereo) * sgp);
for(int i = 0; i < sgp; i++){
    tones[i] = x[i];
}
for(int i = 0; i < size; i++){
    x[i].left -= tones[i % sgp].left;
    x[i].right -= tones[i % sgp].right;
}
```
