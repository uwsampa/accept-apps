load random_input.mat
load fft_output.mat
snr = 20*log10(norm(fft(input)) / norm(fft(input)-output));
fprintf(stdout, '%.2f\n', snr);
