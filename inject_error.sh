# Added for safety
git pull

# 2d-conv benchmark
(cd perfect/pa1/kernels/2d_convolution/; make tune TUNER_ARGS="-d -run 1000 -c 100 -target conv2d")
#DWT53 benchmark
(cd perfect/pa1/kernels/dwt53/; make tune TUNER_ARGS="-d -run 1000 -c 100 -target dwt53_row_transpose")
#Histogram-eq benchmark
(cd perfect/pa1/kernels/histogram_equalization/; make tune TUNER_ARGS="-d -run 1000 -c 100 -target histEq")

#Outer-product benchmark
(cd perfect/stap/kernels/outer-product/; make tune TUNER_ARGS="-d -run 1000 -c 100 -target stap_compute_covariance_estimate")
#System-solve benchmark
(cd perfect/stap/kernels/system-solve/; make tune TUNER_ARGS="-d -run 1000 -c 100 -target stap_system_solver")
#Inner-product benchmark
(cd perfect/stap/kernels/inner-product/; make tune TUNER_ARGS="-d -run 1000 -c 100 -target stap_apply_weighting")

#PFA-interp1 benchmark
(cd perfect/sar/kernels/pfa-interp1/; make tune TUNER_ARGS="-d -run 1000 -c 100 -target sar_interp1")
#PFA-interp2 benchmark
(cd perfect/sar/kernels/pfa-interp2/; make tune TUNER_ARGS="-d -run 1000 -c 100 -target sar_interp2")
#BP benchmark
(cd perfect/sar/kernels/bp/; make tune TUNER_ARGS="-d -run 1000 -c 100 -target sar_backprojection")

#Debayer benchmark
(cd perfect/wami/kernels/debayer/; make tune TUNER_ARGS="-d -run 1000 -c 100 -target wami_debayer")
#Lucas-Kanade benchmark
(cd perfect/wami/kernels/lucas-kanade/; make tune TUNER_ARGS="-d -run 1000 -c 100 -target lucas_kanade")
#Change-detection benchmark
(cd perfect/wami/kernels/change-detection/; make tune TUNER_ARGS="-d -run 1000 -c 100 -target wami_gmm")

#FFT-1D benchmark
(cd perfect/required/kernels/fft-1d/; make tune TUNER_ARGS="-d -run 1000 -c 100 -target fft")
#FFT-2D benchmark
(cd perfect/required/kernels/fft-2d/; make tune TUNER_ARGS="-d -run 1000 -c 100 -target fft")


# Copy everything
mkdir errors
cp errors/pa1_2d_convolution/error_stats.txt errors/pa1_2d_convolution.txt
cp errors/pa1_dwt53/error_stats.txt errors/pa1_dwt53.txt
cp errors/pa1_histogram_equalization/error_stats.txt errors/pa1_histogram_equalization.txt
cp errors/sar_pfa-interp1/error_stats.txt errors/sar_pfa-interp1.txt
cp errors/sar_pfa-interp2/error_stats.txt errors/sar_pfa-interp2.txt
cp errors/sar_bp/error_stats.txt errors/sar_bp.txt
cp errors/stap_outer-product/error_stats.txt errors/stap_outer-product.txt
cp errors/stap_system-solve/error_stats.txt errors/stap_system-solve.txt
cp errors/stap_inner-product/error_stats.txt errors/stap_inner-product.txt
cp errors/wami_debayer/error_stats.txt errors/wami_debayer.txt
cp errors/wami_lucas-kanade/error_stats.txt errors/wami_lucas-kanade.txt
cp errors/wami_change-detection/error_stats.txt errors/wami_change-detection.txt
cp errors/required_fft-1d/error_stats.txt errors/required_fft-1d.txt
cp errors/required_fft-2d/error_stats.txt errors/required_fft-2d.txt


