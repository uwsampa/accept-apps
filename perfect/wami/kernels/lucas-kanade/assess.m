load output.mat;

if (exist("output_small", "var"))
  load inout/small_golden.mat;
  snr = 20 * log10( norm(golden) / norm(golden-output_small) );
elseif (exist("output_medium", "var"))
  load inout/medium_golden.mat;
  snr = 20 * log10( norm(golden) / norm(golden-output_medium) );
else
  load inout/large_golden.mat;
  snr = 20 * log10( norm(golden) / norm(golden-output_large) );
endif

fprintf(stdout, '%.2f\n', snr);
