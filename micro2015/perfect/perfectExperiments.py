def getKernels():
    kernels = [
               ('pa1','2d_convolution',None,'2dconv_output.0.mat', True),
               ('pa1','dwt53',None,'dwt53_output.0.mat', True),
               ('pa1','histogram_equalization',None,'histeq_output.0.mat', True),
               ('required','fft-1d','random_input.mat','fft_output.mat', True),
               ('required','fft-2d','random_input.mat','fft_output.mat', True),
               ('stap','inner-product',None,'snr.txt', False),
               ('stap','outer-product',None,'snr.txt', False),
               ('stap','system-solve',None,'snr.txt', False),
               ('sar','bp',None,'snr.txt', False),
               ('sar','pfa-interp1',None,'snr.txt', False),
               ('sar','pfa-interp2',None,'snr.txt', False),
               ('wami','change-detection',None,'err.txt', False),
               ('wami','debayer',None,'err.txt', False),
               ('wami','lucas-kanade',None,'output.mat', True)
               ]
    return kernels;

"""
               ('pa1','2d_convolution',None,'2dconv_output.0.mat', True),
               ('pa1','dwt53',None,'dwt53_output.0.mat', True),
               ('pa1','histogram_equalization',None,'histeq_output.0.mat', True),
               ('required','fft-1d','random_input.mat','fft_output.mat', True),
               ('required','fft-2d','random_input.mat','fft_output.mat', True),
               ('stap','inner-product',None,'snr.txt', False),
               ('stap','outer-product',None,'snr.txt', False),
               ('stap','system-solve',None,'snr.txt', False),
               ('sar','bp',None,'snr.txt', False),
               ('sar','pfa-interp1',None,'snr.txt', False),
               ('sar','pfa-interp2',None,'snr.txt', False),
               ('wami','change-detection',None,'err.txt', False),
               ('wami','debayer',None,'err.txt', False),
               ('wami','lucas-kanade',None,'output.mat', True)
"""
