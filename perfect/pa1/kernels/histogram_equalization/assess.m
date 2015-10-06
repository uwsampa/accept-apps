arg_list = argv();

if (length(arg_list)==2)
  orig_fn = arg_list{1};
  relax_fn = arg_list{2};

  load(orig_fn);
  load(relax_fn);

  if (norm(output_reference-output_small)==0)
    fprintf('1.0e+9\n');
  else
    snr = 20 * log10( norm(output_reference) / norm(output_reference-output_small) );
    fprintf(stdout, '%.2f\n', snr);
  endif
else
  fprintf('1.0\n');
endif
