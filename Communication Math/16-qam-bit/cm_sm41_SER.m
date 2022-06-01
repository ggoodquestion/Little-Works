function [p]=cm_sm41_SER(snr_in_dB)
% [p]=cm_sm41(snr_in_dB)
%		CM_SM41  finds the probability of error for the given
%   		value of snr_in_dB, SNR in dB.
N=50000;
d=1;				  	% min. distance between symbols
Eav=10*d^2;		 	  	% energy per symbol
snr=10^(snr_in_dB/10);	 	  	% SNR per bit (given)
sgma=sqrt(Eav/(8*snr));	  	  	% noise variance
M=16;
% Generation of the data source follows.
for i=1:N
  temp=rand;		        	  	% a uniform R.V. between 0 and 1
  dsource(i)=1+floor(M*temp);	  	% a number between 1 and 16, uniform 
end
% Mapping to the signal constellation follows.
mapping=[-3*d 3*d;
	      -d  3*d;
           d  3*d;
	     3*d  3*d;
	      -3*d  d;
	         -d  d;
             d  d;
	      3*d  d;
     	 -3*d  -d; 
 	   -d  -d; 
	    d  -d;
      3*d  -d;
	 -3*d  -3*d;
	   -d  -3*d;
	    d  -3*d;
	  3*d  -3*d];
for i=1:N
  qam_sig(i,:)=mapping(dsource(i),:);
end
% received signal
for i=1:N
  n  = [sgma*randn sgma*randn];
  %[n(1) n(2)]=gngauss(sgma);
  r(i,:) = qam_sig(i,:) + n;
end
% detection and error probability calculation
numoferr=0;
for i=1:N
 
    % write your decoder here
    % The decision is named decis
    % Hint.
    % Given received signal vector r(i,:),
    % find the minimum distance among r(i,:) and the 16 signal points in
    % mapping(j,:), j=1,2,... 16. If the distance to mapping(k,:) is
    % minimum, decis=k
    distance=zeros(16);
    for j=1:16
        a=(mapping(j,1)-r(i,1))^2;
        b=(mapping(j,2)-r(i,2))^2;
        distance(j)=sqrt(a+b);
    end
    mindis=min(distance);
    pos=find(distance==mindis);
    decis=pos(1);
       
    
  if (decis~=dsource(i))
    numoferr=numoferr+1;
  end
end
p=numoferr/(N);		  