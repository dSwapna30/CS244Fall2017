
M = csvread('Assignment3.csv',1,0);
timesecond = transpose(M(:,1));
IR = transpose(M(:,2));
RED = transpose(M(:,3));

fs = 50;
T = 1/fs;
order    = 6;
fcutlow  = 1;
fcuthigh = 1.75;


subplot(4,1,1)
plot(timesecond, IR)
title('IR Signal Vs Time')

[b,a] = butter(order,[fcutlow,fcuthigh]/(fs/2), 'bandpass');
x = filter(b,a,IR);

subplot(4,1,2)
plot(timesecond,x);
title('Filtered IR Vs Time')

subplot(4,1,3)
findpeaks(x);
title('Peaks of the filtered signal')

[pks,locs]  = findpeaks(x); 
nPeaks = numel(pks);
peaks= pks(1:nPeaks);
location = locs(1:nPeaks);

for i = 1:(nPeaks-1)
width(i) = location(i+1)- location(i);
heartratefinal(i) = 60*fs/width(i)
end


subplot(4,1,4)
plot(heartratefinal)

N = transpose(heartratefinal)
csvwrite('C:\Users\swapn\OneDrive\Documents\MATLAB\Heartratefile.csv',heartratefinal)

%width_mean= mean(width)











