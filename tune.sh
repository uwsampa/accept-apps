#2d-conv benchmark
(cd perfect/pa1/kernels/2d_convolution/; make tune TUNER_ARGS="-d -t 0.1 -c 8")
mv perfect/pa1/kernels/2d_convolution/outputs runs/pa1_2d_convolution
#DWT53 benchmark
(cd perfect/pa1/kernels/dwt53/; make tune TUNER_ARGS="-d -t 0.1 -c 28")
mv perfect/pa1/kernels/dwt53/outputs runs/pa1_dwt53
#Histogram-eq benchmark
(cd perfect/pa1/kernels/histogram_equalization/; make tune TUNER_ARGS="-d -t 0.1 -c 18")
mv perfect/pa1/kernels/histogram_equalization/outputs runs/pa1_histogram_equalization

#FFT-1D benchmark
(cd perfect/required/kernels/fft-1d/; make tune TUNER_ARGS="-d -snr 100.0 -c 42")
mv perfect/required/kernels/fft-1d/outputs runs/required_fft-1d
#FFT-2D benchmark
(cd perfect/required/kernels/fft-2d/; make tune TUNER_ARGS="-d -snr 100.0 -c 51")
mv perfect/required/kernels/fft-2d/outputs runs/required_fft-2d

#BP benchmark
(cd perfect/sar/kernels/bp/; make tune TUNER_ARGS="-d -snr 100.0 -c 23")
mv perfect/sar/kernels/bp/outputs runs/sar_bp
#PFA-interp1 benchmark
(cd perfect/sar/kernels/pfa-interp1/; make tune TUNER_ARGS="-d -snr 100.0 -c 23")
mv perfect/sar/kernels/pfa-interp1/outputs runs/sar_pfa-interp1
#PFA-interp2 benchmark
(cd perfect/sar/kernels/pfa-interp2/; make tune TUNER_ARGS="-d -snr 100.0 -c 31")
mv perfect/sar/kernels/pfa-interp2/outputs runs/sar_pfa-interp2

#Inner-product benchmark
(cd perfect/stap/kernels/inner-product/; make tune TUNER_ARGS="-d -snr 100.0 -c 25")
mv perfect/stap/kernels/inner-product/outputs runs/stap_inner-product
#Outer-product benchmark
(cd perfect/stap/kernels/outer-product/; make tune TUNER_ARGS="-d -snr 100.0 -c 46")
mv perfect/stap/kernels/outer-product/outputs runs/stap_outer-product
#System-solve benchmark
(cd perfect/stap/kernels/system-solve/; make tune TUNER_ARGS="-d -snr 100.0 -c 54")
mv perfect/stap/kernels/system-solve/outputs runs/stap_system-solve

#Change-detection benchmark
(cd perfect/wami/kernels/change-detection/; make tune TUNER_ARGS="-d -t 0.1 -c 29")
mv perfect/wami/kernels/change-detection/outputs runs/wami_change-detection
#Debayer benchmark
(cd perfect/wami/kernels/debayer/; make tune TUNER_ARGS="-d -t 0.1 -c 65")
mv perfect/wami/kernels/debayer/outputs runs/wami_debayer
#Lucas-Kanade benchmark
(cd perfect/wami/kernels/lucas-kanade/; make tune TUNER_ARGS="-d -snr 100.0 -c 50")
mv perfect/wami/kernels/lucas-kanade/outputs runs/wami_lucas-kanade

