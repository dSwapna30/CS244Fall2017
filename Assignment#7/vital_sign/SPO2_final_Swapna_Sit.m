clc;
close all;
clear all;


data_swapnasit = csvread('data_sit_swapna_sample.csv', 0, 1,[0,1,5099,3]);

csvwrite('input_Swapnasit.csv',data_swapnasit)

%data = csvread('givendata.csv', 1, 0);
% disp('data:');
% display (data);

%AC IR & DC IR:

T = data_swapnasit(:, 1);

IR = data_swapnasit(:, 2);

subplot (2,1,1)
plot(IR)
xlabel('time')
ylabel('IR Data')
title ('Unfiltered IR Data')


fs=50;
order = 6;
fcutlow = 1;
fcuthigh = 1.75;
[b,a]=butter(order, [fcutlow,fcuthigh]/(fs/2),'bandpass');
x=filter (b,a,IR);
subplot (2,1,2)
plot(x)
xlabel('time')
ylabel('IR Data')
title ('Filtered IR Data')

% To calculate R,Q,S Using findpeaks

%disp(x');
[ir_peak_r, ir_local_r] = findpeaks(x,T, 'MinPeakDistance', 0.6);
%findpeaks(x, 'MinPeakDistance', 0.6);
IR_inverted = -x;
 
[ir_peak_s1,ir_local_s1] = findpeaks(IR_inverted, T, 'MinPeakDistance', 0.6);
ir_peak_s1 = - ir_peak_s1;

%AC Red & DC Red:
 
Red = data_swapnasit(:, 3);
figure
subplot (2,1,1)
plot(T, Red)
xlabel('time')
ylabel('Red Data')
title ('Unfiltered Red Data')

fs=50;
order = 6;
fcutlow = 1;
fcuthigh = 1.75;
[d,c]=butter(order, [fcutlow,fcuthigh]/(fs/2),'bandpass');
y=filter (d,c,Red);
subplot (2,1,2)
plot(y)
xlabel('time')
ylabel('Red Data')
title ('Filtered Red Data')



%To calculate R,Q,S Using findpeaks

[red_peak_r,red_local_r] = findpeaks(y, T, 'MinPeakDistance', 0.6);
Red_inverted = -y;
[red_peak_s1,red_local_s1] = findpeaks(Red_inverted, T, 'MinPeakDistance', 0.6);
red_peak_s1 = red_peak_s1;

%IR / Red Q(x1,y1), R(x2, y2), S(x3, y3) Coordinate allocation

%for n = 1:130
for n = 1:(length(red_local_r)-1)
    Rx_red = red_local_r(n);
    Ry_red = red_peak_r(n);
    Sx_red = red_local_s1(n+1);
    Sy_red = red_peak_s1(n+1);
    Qx_red = red_local_s1(n);
    Qy_red = red_peak_s1(n);

    %To find DC - point of intersection of R with Q & S and AC
    DC_Red(n) = Qy_red + (((Sy_red-Qy_red)/(Sx_red-Qx_red))*(Rx_red-Qx_red));

    AC_Red(n) = Ry_red-DC_Red(n);
    
    Rx_ir = ir_local_r(n);
    Ry_ir = ir_peak_r(n);
    Sx_ir = ir_local_s1(n+1);
    Sy_ir = ir_peak_s1(n+1);
    Qx_ir = ir_local_s1(n);
    Qy_ir = ir_peak_s1(n);

    %To find DC - point of intersection of R with Q & S and AC
    DC_IR(n) = Qy_ir + (((Sy_ir-Qy_ir)/(Sx_ir-Qx_ir))*(Rx_ir-Qx_ir));

    AC_IR(n) = Ry_ir-DC_IR(n);


    %To calculate SPO2
    R = ((AC_Red .* DC_IR)./(AC_IR .* DC_Red));


    SPO_2 = (-45.060*R.*R) + 30.354*R + 94.845;


    
    
end
display('R:');
display (R);
display('SPO_2:');
display (SPO_2);

%csvwrite('SPO2file_Swapnasit.csv',SPO_2)