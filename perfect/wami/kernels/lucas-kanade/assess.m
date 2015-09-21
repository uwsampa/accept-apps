
arg_list = argv();

if (length(arg_list)==1)
  golden_fn = arg_list{1};
  relax_fn = arg_list{2};

  load(golden_fn);
  load(relax_fn);

  snr = 20 * log10( norm(golden) / norm(golden-output) );

  fprintf(stdout, '%.2f\n', snr);
else
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
endif
