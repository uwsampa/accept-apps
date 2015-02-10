batch_size = 30;

err = 0.0;
combined_err = 0.0;

for i = 0:(batch_size - 1)
  load(["2dconv_output." (int2str(i)) ".mat"])
  if (exist("output_small", "var"))
    ## disp("small")
    load 2dconv_output.small.mat
    diff = abs(output_small - output_reference);
    err_total = sum(sum(diff));
    err = (err_total / 255.0) / (rows(output_reference) * columns(output_reference));
  elseif (exist("output_medium", "var"))
    ## disp("medium")
    load 2dconv_output.medium.mat
    diff = abs(output_medium - output_reference);
    err_total = sum(sum(diff));
    err = (err_total / 255.0) / (rows(output_reference) * columns(output_reference));
  elseif (exist("output_large", "var"))
    ## disp("large")
    load 2dconv_output.large.mat
    diff = abs(output_large - output_reference);
    err_total = sum(sum(diff));
    err = (err_total / 255.0) / (rows(output_reference) * columns(output_reference));
  else
    err = 1.0;
  endif
  
  combined_err = combined_err + err;
  
endfor

fprintf(stdout, '%.4f\n', combined_err / batch_size);
