s1_1 = load('cos_050Hz-8k.{Set1}.txt');
s1_2 = load('cos_050Hz-8k.{Set2}.txt');
s1_3 = load('cos_050Hz-8k.{Set3}.txt');
s1_4 = load('cos_050Hz-8k.{Set4}.txt');

s2_1 = load('cos_050Hz-16k.{Set1}.txt');
s2_2 = load('cos_050Hz-16k.{Set2}.txt');
s2_3 = load('cos_050Hz-16k.{Set3}.txt');
s2_4 = load('cos_050Hz-16k.{Set4}.txt');

s3_1 = load('cos_055Hz-8k.{Set1}.txt');
s3_2 = load('cos_055Hz-8k.{Set2}.txt');
s3_3 = load('cos_055Hz-8k.{Set3}.txt');
s3_4 = load('cos_055Hz-8k.{Set4}.txt');

s4_1 = load('cos_055Hz-16k.{Set1}.txt');
s4_2 = load('cos_055Hz-16k.{Set2}.txt');
s4_3 = load('cos_055Hz-16k.{Set3}.txt');
s4_4 = load('cos_055Hz-16k.{Set4}.txt');

s5_1 = load('cos_200Hz-8k.{Set1}.txt');
s5_2 = load('cos_200Hz-8k.{Set2}.txt');
s5_3 = load('cos_200Hz-8k.{Set3}.txt');
s5_4 = load('cos_200Hz-8k.{Set4}.txt');

s6_1 = load('cos_200Hz-16k.{Set1}.txt');
s6_2 = load('cos_200Hz-16k.{Set2}.txt');
s6_3 = load('cos_200Hz-16k.{Set3}.txt');
s6_4 = load('cos_200Hz-16k.{Set4}.txt');

s7_1 = load('cos_220Hz-8k.{Set1}.txt');
s7_2 = load('cos_220Hz-8k.{Set2}.txt');
s7_3 = load('cos_220Hz-8k.{Set3}.txt');
s7_4 = load('cos_220Hz-8k.{Set4}.txt');

s8_1 = load('cos_220Hz-16k.{Set1}.txt');
s8_2 = load('cos_220Hz-16k.{Set2}.txt');
s8_3 = load('cos_220Hz-16k.{Set3}.txt');
s8_4 = load('cos_220Hz-16k.{Set4}.txt');

s9_1 = load('vowel-8k.{Set1}.txt');
s9_2 = load('vowel-8k.{Set2}.txt');
s9_3 = load('vowel-8k.{Set3}.txt');
s9_4 = load('vowel-8k.{Set4}.txt');

s10_1 = load('vowel-16k.{Set1}.txt');
s10_2 = load('vowel-16k.{Set2}.txt');
s10_3 = load('vowel-16k.{Set3}.txt');
s10_4 = load('vowel-16k.{Set4}.txt');

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

figure(2)
s2_11 = s2_1';
s2_21 = s2_2';
s2_31 = s2_3';
s2_41 = s2_4';
subplot(2,2,1)
imagesc(s2_11);
colorbar
subplot(2,2,2)
imagesc(s2_21);
colorbar
subplot(2,2,3)
imagesc(s2_31);
colorbar
subplot(2,2,4)
imagesc(s2_41);
colorbar

figure(3)
s3_11 = s3_1';
s3_21 = s3_2';
s3_31 = s3_3';
s3_41 = s3_4';
subplot(2,2,1)
imagesc(s3_11);
colorbar
subplot(2,2,2)
imagesc(s3_21);
colorbar
subplot(2,2,3)
imagesc(s3_31);
colorbar
subplot(2,2,4)
imagesc(s3_41);
colorbar

figure(4)
s4_11 = s4_1';
s4_21 = s4_2';
s4_31 = s4_3';
s4_41 = s4_4';
subplot(2,2,1)
imagesc(s4_11);
colorbar
subplot(2,2,2)
imagesc(s4_21);
colorbar
subplot(2,2,3)
imagesc(s4_31);
colorbar
subplot(2,2,4)
imagesc(s4_41);
colorbar

figure(5)
s5_11 = s5_1';
s5_21 = s5_2';
s5_31 = s5_3';
s5_41 = s5_4';
subplot(2,2,1)
imagesc(s2_11);
colorbar
subplot(2,2,2)
imagesc(s2_21);
colorbar
subplot(2,2,3)
imagesc(s2_31);
colorbar
subplot(2,2,4)
imagesc(s2_41);
colorbar

figure(6)
s6_11 = s6_1';
s6_21 = s6_2';
s6_31 = s6_3';
s6_41 = s6_4';
subplot(2,2,1)
imagesc(s6_11);
colorbar
subplot(2,2,2)
imagesc(s6_21);
colorbar
subplot(2,2,3)
imagesc(s6_31);
colorbar
subplot(2,2,4)
imagesc(s6_41);
colorbar

figure(7)
s7_11 = s7_1';
s7_21 = s7_2';
s7_31 = s7_3';
s7_41 = s7_4';
subplot(2,2,1)
imagesc(s7_11);
colorbar
subplot(2,2,2)
imagesc(s7_21);
colorbar
subplot(2,2,3)
imagesc(s7_31);
colorbar
subplot(2,2,4)
imagesc(s7_41);
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