arg_list = argv();

if (length(arg_list)==2)
  golden_fn = arg_list{1};
  relax_fn = arg_list{2};

  load(golden_fn);
  load(relax_fn);

  if (norm(golden-output_small)==0)
    fprintf('1.0e+9\n');
  else
    snr = 20 * log10( norm(golden) / norm(golden-output_small) );
    fprintf(stdout, '%.2f\n', snr);
  endif
else
  fprintf('1.0\n');
endif
