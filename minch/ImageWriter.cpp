#include "ImageWriter.h"
#include <cstdio>

int ImageWriter::PpmWrite(const char* filename, const float* pixels, int width, int height) {
	FILE* output = fopen(filename, "w");    // Get stream for output file
	if (output == NULL) {	// Check if we opened the file correctly
		fprintf(stderr, "Unable to write to %s!\n", filename);
		fclose(output);
		return 1;
	}
	fprintf(output, "P3\n%i %i\n255\n", width, height); // Write the header to output

	const char* format = "%i %i %i\n";	// Define string format
	for (int i = 0; i < 3 * width * height; i += 3) {	// Set color for each pixel in the image
			// Print pixel to output
		fprintf(output, format, (int)(255 * pixels[i]), (int)(255 * pixels[i + 1]), (int)(255 * pixels[i+2]));
	}


	fclose(output); // Close output file
	return 0;
}
