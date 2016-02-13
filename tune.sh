#2d-conv benchmark
(cd perfect/pa1/kernels/2d_convolution/; make tune TUNER_ARGS="-d -snr 10.0 -c 10")
mv perfect/pa1/kernels/2d_convolution/outputs runs/pa1_2d_convolution
#DWT53 benchmark
(cd perfect/pa1/kernels/dwt53/; make tune TUNER_ARGS="-d -snr 10.0 -c 34")
mv perfect/pa1/kernels/dwt53/outputs runs/pa1_dwt53
#Histogram-eq benchmark
(cd perfect/pa1/kernels/histogram_equalization/; make tune TUNER_ARGS="-d -snr 10.0 -c 18")
mv perfect/pa1/kernels/histogram_equalization/outputs runs/pa1_histogram_equalization

#Outer-product benchmark
(cd perfect/stap/kernels/outer-product/; make tune TUNER_ARGS="-d -snr 10.0 -c 88")
mv perfect/stap/kernels/outer-product/outputs runs/stap_outer-product
#System-solve benchmark
(cd perfect/stap/kernels/system-solve/; make tune TUNER_ARGS="-d -snr 10.0 -c 63")
mv perfect/stap/kernels/system-solve/outputs runs/stap_system-solve
#Inner-product benchmark
(cd perfect/stap/kernels/inner-product/; make tune TUNER_ARGS="-d -snr 10.0 -c 28")
mv perfect/stap/kernels/inner-product/outputs runs/stap_inner-product

#PFA-interp1 benchmark
(cd perfect/sar/kernels/pfa-interp1/; make tune TUNER_ARGS="-d -snr 10.0 -c 12")
mv perfect/sar/kernels/pfa-interp1/outputs runs/sar_pfa-interp1
#PFA-interp2 benchmark
(cd perfect/sar/kernels/pfa-interp2/; make tune TUNER_ARGS="-d -snr 10.0 -c 26")
mv perfect/sar/kernels/pfa-interp2/outputs runs/sar_pfa-interp2
#BP benchmark
(cd perfect/sar/kernels/bp/; make tune TUNER_ARGS="-d -snr 10.0 -c 18")
mv perfect/sar/kernels/bp/outputs runs/sar_bp

#Debayer benchmark
(cd perfect/wami/kernels/debayer/; make tune TUNER_ARGS="-d -snr 10.0 -c 86")
mv perfect/wami/kernels/debayer/outputs runs/wami_debayer
#Lucas-Kanade benchmark
(cd perfect/wami/kernels/lucas-kanade/; make tune TUNER_ARGS="-d -snr 10.0 -c 102")
mv perfect/wami/kernels/lucas-kanade/outputs runs/wami_lucas-kanade
#Change-detection benchmark
(cd perfect/wami/kernels/change-detection/; make tune TUNER_ARGS="-d -snr 10.0 -c 38")
mv perfect/wami/kernels/change-detection/outputs runs/wami_change-detection

#FFT-1D benchmark
(cd perfect/required/kernels/fft-1d/; make tune TUNER_ARGS="-d -snr 10.0 -c 44")
mv perfect/required/kernels/fft-1d/outputs runs/required_fft-1d
#FFT-2D benchmark
(cd perfect/required/kernels/fft-2d/; make tune TUNER_ARGS="-d -snr 10.0 -c 98")
mv perfect/required/kernels/fft-2d/outputs runs/required_fft-2d
