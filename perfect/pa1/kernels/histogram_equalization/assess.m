batch_size = 1;

err = 0.0;
combined_err = 0.0;

for i = 0:(batch_size - 1)
  load(["histeq_output." (int2str(i)) ".mat"])
  if (exist("output_small", "var"))
    ## disp("small")
    load histeq_output.small.mat
    diff = abs(output_small - output_reference);
    err_total = sum(sum(diff));
    err = (err_total / (255.0 * 255.0)) / (rows(output_reference) * columns(output_reference));
  elseif (exist("output_medium", "var"))
    ## disp("medium")
    load histeq_output.medium.mat
    diff = abs(output_medium - output_reference);
    err_total = sum(sum(diff));
    err = (err_total / (255.0 * 255.0)) / (rows(output_reference) * columns(output_reference));
  elseif (exist("output_large", "var"))
    ## disp("large")
    load histeq_output.large.mat
    diff = abs(output_large - output_reference);
    err_total = sum(sum(diff));
    err = (err_total / (255.0 * 255.0)) / (rows(output_reference) * columns(output_reference));
  else
    err = 1.0;
  endif
  
  combined_err = combined_err + err;
  
endfor

fprintf(stdout, 'error: %.16f\n', combined_err / batch_size);
