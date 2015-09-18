arg_list = argv();

input_fn = arg_list{1};
output_fn = arg_list{2};

load(input_fn)
load(output_fn)
snr = 20*log10(norm(fft2(input)) / norm(fft2(input)-output));
fprintf(stdout, '%.2f\n', snr);
