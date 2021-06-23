# 多媒體與訊號處理
## 作業三
>### 410786004 通訊三 蘇家駒

---


## 簡介
本次作業首先使用argv獨進兩個參數，分別為輸入檔名與輸出檔名
，並宣告struct Stereo紀錄雙聲道的值，使用fread將資料讀進Stereo陣
列，使用DFT函示轉為頻域，在此之前我需要初始化cos、sin和window
table以加快程式速度，DFT我將算出來的複數存在我寫的struct ImgNum
裡，在進行LPF，將欲濾除的頻率算成frequency bin再去刪除超過該範圍
的k值，最後做IDFT，將DFT算得之複數去與IDFT之尤拉表示式相乘，由於
轉換後是實數訊號因此不用考慮imagine part，因此只靠綠相乘後的real
part，最後轉出的資料即為濾波後的音檔。至於題目要求的兩個雜訊，我
使用的方法為，先將前0.1秒的片段複製下來，接著每0.1秒便減去該片段
，即可去除雜訊。

## 製作想法與要點
### 1. DFT：
- 宣告兩動態陣列儲存spectrum
``` 
ImgNum **freqField_l;
ImgNum **freqField_r;
```
- 進行SDFT並將複數結果存入
- SDFT：analize size=2ms、frame size=3ms、frame interval=1ms
、window type=hanning
### 2. LPF：
- 將20k算成對應的將兩個frequency bin
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
- 將該範圍內(也就是高頻部分)的頻率設為0
### 3. IDFT：
- 將DFT結果進行轉換，應為兩複數相乘- 由於轉出為實數訊號因此相乘後只靠濾實數部分，因此是實部相乘
減去虛部相乘，最後除K是因為加了K次要取平均的能量
```
x[m*M+n].left += (short) ((l_sum.real - l_sum.imagine) / K);
x[m*M+n].right += (short) ((r_sum.real - r_sum.imagine) / K);
```
- 轉換後的data即是過濾完的訊號
### 4. 消除2個tones；
- 觀察兩個雜訊的頻率約為60Hz與16000Hz
- 因此我觀察前0.1秒並擷取，接著每0.1秒減去該片段即消除兩個雜
訊
- 選0.1秒的原因是我假設0.1秒時兩雜訊皆為周期的整數倍(經過驗證也是如此且誤差很小)

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
