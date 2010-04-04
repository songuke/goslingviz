%--------------------------------------------------------------------------
% Son Hua, NUS
% 2010-03-07
%--------------------------------------------------------------------------
close all;
clear all;

Fs = 44100;
%
% body impulse response
%
bodyIR = wavread('../output/gtrbody.wav');
bodyIR = 2 * (bodyIR - min(bodyIR)) / (max(bodyIR) - min(bodyIR)) - 1;

%
% body frequency response
%
bodyFR = fft(bodyIR); % windows?
bodyFR = bodyFR(1 : length(bodyFR) / 2); % only need to use half of the 
% frequency domain due to symmetric 
% (Nyquist sampling theorem: highest frequency <= half of sampling frequency)

%
% freq and bw varies for each body impulse response.
%
freq = 77;
bw = 6;
% [74, 79]

W       = linspace(0, pi, length(bodyFR))';
[B, A]  = invfreqz(bodyFR(74:79), W(74:79), 2, 2); % BiQuad using invfreqz

[estBodyFR, U]  = freqz(B, A, length(bodyFR));
residualFR      = bodyFR ./ estBodyFR;

% in order to get residual (time domain) from residualFR(frequency domain),
% we need to recover the second part of residualFR, which is the flip of
% the conjugate of the first part. If we plot the magnitude, we see they
% are symmetric over the center of the frequency.
residualFR2 = zeros(length(bodyFR) * 2, 1);
residualFR2(1 : length(bodyFR)) = residualFR;
residualFR2(length(bodyFR) + 1 : end) = flipdim(conj(residualFR), 1);
residual = real(ifft(residualFR2));
residual = 2 * (residual - min(residual)) / (max(residual) - min(residual)) - 1;
residual = residual - mean(residual);

%--------------------------------------------------------------------------
range = 1 : length(bodyIR);

figIR = figure;
subplot(211);
plot(bodyIR(range), 'b');
subplot(212);
plot(residual(range), 'r');
title('Body Impulse Response');
drawnow;

figFR = figure;
subplot(311);
plot(abs(bodyFR(1 : 500)), 'b'); hold on; axis([0 500 0 150]);
subplot(312);
plot(abs(estBodyFR(1 : 500)), 'r'); hold on; axis([0 500 0 150]);
plot(abs(residualFR(1 : 500)), 'g');
title('Body Frequency Response');
legend('Parametric damped mode', 'Residual');
subplot(313);
plot(abs(bodyFR(1 : 500)), 'b'); hold on;
plot(abs(estBodyFR(1 : 500)), 'r');
plot(abs(residualFR(1 : 500)), 'g'); axis([0 500 0 150]);
legend('Body Frequency Response', 'Parametric damped mode', 'Residual');
%title('Frequency response');
pause;



disp('Press any key to listen to raw body impulse response...'); pause;
soundsc(bodyIR, Fs);
disp('Press any key to listen to residual body impulse response...');pause;
soundsc(residual, Fs);

% save residual 
wavwrite(residual, 44100, 16, '../output/gtrbody_residual.wav');
% save BiQuad coefficients
fd = fopen('../output/gtrbody_BA.txt', 'wt');
fprintf(fd, '%f %f %f %f %f %f\n', B(1), B(2), B(3), A(1), A(2), A(3));
fclose(fd);

% generate white noise
whitenoise = randn(1, 4096);
whitenoise = 2 * (whitenoise - min(whitenoise)) / (max(whitenoise) - min(whitenoise)) - 1;
kernel = residual(1 : 1500);
kernel = kernel / sum(kernel);
excitation = conv(whitenoise, kernel);
%excitation = filter(B, A, whitenoise); % prefilter white noise with the second order filter found above
excitation = 2 * (excitation - min(excitation)) / (max(excitation) - min(excitation)) - 1;
%excitation = excitation - mean(excitation);

%pause;
%soundsc(whitenoise, 44100);
%pause;
%soundsc(excitation, 44100);

figExt = figure; 
subplot(211);
plot(whitenoise, 'b'); 
subplot(212);
plot(excitation(1 : length(whitenoise)), 'r');
title('Excitation');

% save excitation
wavwrite(excitation, Fs, 16, '../output/gtrbody_excitation.wav');

% save figures
mkdir('results');
saveas(figIR, sprintf('results/bodyIR.png'));
saveas(figFR, sprintf('results/bodyFR.png'));
saveas(figExt, sprintf('results/excitation.png'));

