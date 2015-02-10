load random_input.mat;
load fft_output.mat;
snr = 20*log10(norm(fft2(input)) / norm(fft2(input)-output));
fprintf(stdout, '%.2f\n', snr);
