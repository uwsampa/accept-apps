#include "datatype.h"
#include "config.h"
#include "prototype.h"

#include "rgbimage.h"


UINT8	Lqt [BLOCK_SIZE];
UINT8	Cqt [BLOCK_SIZE];
UINT16	ILqt [BLOCK_SIZE];
UINT16	ICqt [BLOCK_SIZE];

INT16	Y1 [BLOCK_SIZE];
INT16	Y2 [BLOCK_SIZE];
INT16	Y3 [BLOCK_SIZE];
INT16	Y4 [BLOCK_SIZE];
INT16	CB [BLOCK_SIZE];
INT16	CR [BLOCK_SIZE];
INT16	Temp [BLOCK_SIZE];
UINT32 lcode = 0;
UINT16 bitindex = 0;

INT16 global_ldc1;
INT16 global_ldc2;
INT16 global_ldc3;

void ACCEPTRegion(unsigned char* image, int image_size) {
}

UINT8* encodeImage(
	RgbImage* srcImage,
	UINT8 *outputBuffer,
	UINT32 qualityFactor,
	UINT32 imageFormat
) {
	UINT16 i, j;

	global_ldc1 = 0;
	global_ldc2 = 0;
	global_ldc3 = 0;

	/** Quantization Table Initialization */
	initQuantizationTables(qualityFactor);

	/* Writing Marker Data */
	outputBuffer = writeMarkers(outputBuffer, imageFormat, srcImage->w, srcImage->h);

	for (i = 0; i < srcImage->h; i += 8) {
		printf("Row %d\n", i);
		for (j = 0; j < srcImage->w; j += 8) {
			readMcuFromRgbImage(srcImage, j, i, Y1);

			/* Encode the data in MCU */
			outputBuffer = encodeMcu(imageFormat, outputBuffer);
		}
	}

	/* Close Routine */
	closeBitstream(outputBuffer);

	return outputBuffer;
}

UINT8* encodeMcu(
	UINT32 imageFormat,
	UINT8 *outputBuffer
) {

	levelShift(Y1);
	dct(Y1);
	quantization(Y1, ILqt);

	// Inject the error
	ACCEPTRegion((char*) Temp, BLOCK_SIZE);

	outputBuffer = huffman(1, outputBuffer);


	return outputBuffer;
}
