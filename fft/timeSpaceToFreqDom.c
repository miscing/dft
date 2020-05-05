#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <complex.h>
#include <fftw3.h>
#include <sndfile.h>

// Global Varibles
int samplerate, N;
double *in;
fftw_complex *out;

void complexToFile( fftw_complex *out) {

	char *fileName = "freqDomain";
	double binWidth = (((double)samplerate/2)/((double)N/2));
	/* printf("binWidth:%f\n", binWidth); */

	FILE *file = fopen(fileName, "w");
	if (file == NULL) {
		perror("Writting complex to file failed with:");
		exit(EXIT_FAILURE);
	}

	int arraySize = N/2+1;

	for (int i = 0; i< arraySize; i++) {
		/* fprintf(file, "%d %2.5f %2.5f\n", i, out[i][0], out[i][1]); */
		// now we gotta output two relevant columns. Apparently bin or log(bin) for x axis and y= log(magnitude(real + i *img))

		double mag;
		float freqBin;
		mag = cabs(out[i]);
		freqBin = i * binWidth;
		fprintf(file, "%f %f\n", freqBin, mag);
	}
	fclose(file);
}

void realToFile( double *in) {

	char *fileName = "timeDomain";

	FILE *file = fopen(fileName, "w");
	if (file == NULL) {
		perror("Writting real values to file failed with:");
		exit(EXIT_FAILURE);
	}

	for (int i = 0; i < N; i++) {
		fprintf(file, "%d %f\n", i, in[i]);
	}

	fclose(file);
}


void openReadFile(char *filePath) {
	SNDFILE *file;
	SF_INFO inFileInfo;
	int frames, channels, format, sec;

	file = sf_open(filePath, SFM_READ, &inFileInfo);
	if (file == NULL) {
		perror("Opening first argument as file failed with:");
		exit(EXIT_FAILURE);
	}

	frames = inFileInfo.frames;
	samplerate = inFileInfo.samplerate;
	channels = inFileInfo.channels;
	format = inFileInfo.format;
	sec = inFileInfo.sections;
	printf("Read file has frames: %d, samplerate: %d hz, channels: %d, format: %d, sections:%d\n", frames, samplerate, channels, format, sec);
	N = frames * channels;

	// allocate memory and read values into array, then close file
	in = malloc(sizeof(double)*N);
	sf_read_double(file, in, frames);
	sf_close(file);
}

void openAlsa(char *device) {
	puts("feature not implemented. You attempting to get device:");
	puts(device);
	exit(EXIT_SUCCESS);
}

int main( int argc, char **argv) {
	//variables used for fft conversion
	fftw_plan p;

	if (argc == 2) {
		// should fail if failed to open file
		openReadFile( argv[1]);
	} else if (argc == 3) {
		if (strcmp(argv[1],"d") == 0 || strcmp(argv[1], "device") == 0) {
			//TODO: check input argv[2] is a valid string/device name
			openAlsa(argv[2]);
		} else {
			printf("Wrong usage: testAudioFFT [d/device] file/device_name");
			exit(EXIT_FAILURE);
		}
	} else {
		printf("Wrong usage: testAudioFFT [d/device] file/device_name");
		exit(EXIT_FAILURE);
	}

	// allocate half the memory for the out array since complex numbers require half the space
	int outSize = N /2 +1;
	out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * outSize);
	// create plan for fft
	// N is size, in/out are obvious, then directions FORWARD/BACKWARDS and use estimate for now, later use MEASURE for longer startup but faster execution
	p = fftw_plan_dft_r2c_1d(N, in, out, FFTW_ESTIMATE);
	// execute
	fftw_execute(p);

	// save raw signal results to file where first column is time and second is amplitude
	realToFile(in);
	complexToFile(out);

	// save fft:d results to file where first column in frequency and second is amplitude
	fftw_destroy_plan(p);
	fftw_free(in);
	fftw_free(out);



	// plot this shizzle
	system("gnuplot -p -e \"plot 'freqDomain'\"");
	system("gnuplot -p -e \"plot 'timeDomain'\"");
	return 0;
}
