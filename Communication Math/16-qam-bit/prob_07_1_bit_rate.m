% MATLAB script for Illustrative Problem 7.10.
clear all;
close all;

SNRindB1=0:2:15;
SNRindB3=0:2:15;
SNRindB2=0:0.1:15;
M=16;
k=log2(M);
for i=1:length(SNRindB1)
    smld_err_prb1(i)=cm_sm41(SNRindB1(i));	% simulated error rate
    smld_err_prb2(i)=cm_sm41_gene(SNRindB1(i));	% simulated error rate
    smld_err_prb3(i)=cm_sm41_SER(SNRindB1(i));	% simulated error rate
end

for i=1:length(SNRindB2)
    SNR=exp(SNRindB2(i)*log(10)/10);    	% signal-to-noise ratio
    % theoretical symbol error rate
    theo_err_prb1(i)=4*qfunc(sqrt(3*k*SNR/(M-1)));
    %theo_err_prb(i)=4*erfc(sqrt(3*k*SNR/(M-1)/2))/2;

    % BER of 16-QAM
    pb1 = (erfc(sqrt(2*SNR/5)) + erfc(3*sqrt(2*SNR/5)))/4;
    pb2 = (2*erfc(sqrt(2*SNR/5)) + erfc(3*sqrt(2*SNR/5)) - erfc(5*sqrt(2*SNR/5)))/4;
    theo_err_prb2(i)=(pb1 + pb2)/2;
end

% Plotting commands follow.
semilogy(SNRindB1,smld_err_prb1,"*");

hold
semilogy(SNRindB1,smld_err_prb2, "o");
semilogy(SNRindB1,smld_err_prb3, ".");
semilogy(SNRindB2,theo_err_prb1);
semilogy(SNRindB2,theo_err_prb2);
legend(['Simulated bit error rate A   ';'Simulated bit error rate B   ';'Simulated symbol error rate  ';'Theoretical symbol error rate'; 'Theoretical bit error rate   ']); % 29
xlabel('E_b/N_0 in dB','fontsize',16,'fontname','Helvetica');
ylabel('Error Probability','fontsize',16,'fontname','Helvetica');
title('Performance of a 16-QAM system from Monte Carlo simulation (410786004)','fontsize',16,'fontname','Helvetica');
fname = 'prob_07_10(410786004).png';
print (fname, '-dpng'); 