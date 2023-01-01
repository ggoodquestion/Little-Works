s1_1 = load('cos-8k.{Set1}.txt');
s1_2 = load('cos-8k.{Set2}.txt');
s1_3 = load('cos-8k.{Set3}.txt');
s1_4 = load('cos-8k.{Set4}.txt');

s8_1 = load('cos-16k.{Set1}.txt');
s8_2 = load('cos-16k.{Set2}.txt');
s8_3 = load('cos-16k.{Set3}.txt');
s8_4 = load('cos-16k.{Set4}.txt');

s9_1 = load('aeueo-8kHz.{Set1}.txt');
s9_2 = load('aeueo-8kHz.{Set2}.txt');
s9_3 = load('aeueo-8kHz.{Set3}.txt');
s9_4 = load('aeueo-8kHz.{Set4}.txt');

s10_1 = load('aeueo-16kHz.{Set1}.txt');
s10_2 = load('aeueo-16kHz.{Set2}.txt');
s10_3 = load('aeueo-16kHz.{Set3}.txt');
s10_4 = load('aeueo-16kHz.{Set4}.txt');

figure(1)
s1_11 = s1_1';
s1_21 = s1_2';
s1_31 = s1_3';
s1_41 = s1_4';
subplot(2,2,1)
imagesc(s1_11);
colorbar
subplot(2,2,2)
imagesc(s1_21);
colorbar
subplot(2,2,3)
imagesc(s1_31);
colorbar
subplot(2,2,4)
imagesc(s1_41);
colorbar

figure(8)
s8_11 = s8_1';
s8_21 = s8_2';
s8_31 = s8_3';
s8_41 = s8_4';
subplot(2,2,1)
imagesc(s8_11);
colorbar
subplot(2,2,2)
imagesc(s8_21);
colorbar
subplot(2,2,3)
imagesc(s8_31);
colorbar
subplot(2,2,4)
imagesc(s8_41);
colorbar

figure(9)
s9_11 = s9_1';
s9_21 = s9_2';
s9_31 = s9_3';
s9_41 = s9_4';
subplot(2,2,1)
imagesc(s9_11);
colorbar
subplot(2,2,2)
imagesc(s9_21);
colorbar
subplot(2,2,3)
imagesc(s9_31);
colorbar
subplot(2,2,4)
imagesc(s9_41);
colorbar

figure(10)
s10_11 = s10_1';
s10_21 = s10_2';
s10_31 = s10_3';
s10_41 = s10_4';
subplot(2,2,1)
imagesc(s10_11);
colorbar
subplot(2,2,2)
imagesc(s10_21);
colorbar
subplot(2,2,3)
imagesc(s10_31);
colorbar
subplot(2,2,4)
imagesc(s10_41);
colorbar