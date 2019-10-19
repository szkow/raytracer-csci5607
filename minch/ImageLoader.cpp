#include <stdio.h>
#include <string.h>
#define _USE_MATH_DEFINES
#include <math.h>

int main(int argc, char** argv) {
    if (argc != 2) {	// Make sure we're provided enough arguments
        fprintf(stderr, "Insufficient arguments provided. Exiting...\n");
        return 1;
    }

    char* filename = argv[1];   // Parse filename

    int width, height;
    FILE* input = fopen(filename, "r");
    if (fscanf(input, "%*s %i %i", &width, &height) != 2) {  // Parse width and height of image
        fprintf(stderr, "Failed to read from file!\n");
        fclose(input);
        return 1;
    }
	if (width < 0 || height < 0) {	// Check validity of dimensions
		fprintf(stderr, "Invalid dimensions provided!\n");
		fclose(input);
		return 1;
	}
    fclose(input);   // Close input file


    char* name_end = strstr(filename, ".txt");   // Lop off filename's extension so we can add our own
    if (name_end == NULL) {
        fprintf(stderr, "File does not have .txt extension!\n");
        return 1;
    }
    strcpy(name_end, ".ppm");   // Replace with .ppm

    FILE* output = fopen(filename, "w");    // Get stream for output file
	if (output == NULL) {	// Check if we opened the file correctly
		fprintf(stderr, "Unable to write to %s!\n", filename);
		return 1;
	}
    fprintf(output, "P3\n%i %i\n255\n", width, height); // Write the header to output
    
    const char* format = "%i %i %i\n";	// Define string format
	for (int i = 0; i < height; ++i) {	// Set color for each pixel in the image
		for (int j = 0; j < width; ++j) {

			// Calculate current progress in row/column and transform the axes so we're on a cartesian plane
			float i_percent = (float)i / (float)height;
			float j_percent = (float)j / (float)width;
			float i_norm = -i_percent + 0.5f;
			float j_norm = j_percent - 0.5f;
			
			// Calculate intensity of pixel
			float percent_intensity = (cosf(powf(j_norm * (float)M_PI * 4.0f, i % 10)) + sinf(powf(i_norm * (float)M_PI * 4.0f, j % 3))) / 2.0f + 1.0f;
			
			// Use intensity for RGB values
			int r = (int)(percent_intensity * (120.0f * i_percent));
			int g = (int)(percent_intensity * (100.0f * j_percent));
			int b = (int)(percent_intensity * (180.0f * i_percent * j_percent));

			// Print pixel to output
			fprintf(output, format, r, g, b);
		}
    }


    fclose(output); // Close output file
	return 0;
}