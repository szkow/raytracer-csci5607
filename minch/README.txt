This program takes a path to a .txt file which contains "imsize [width] [height]" only.
It outputs a .ppm image of the dimensions given by the input file.

The calculation of the pixels' RGB values is a sort of amalgam of different functions.
I started by transforming the regular row-major pixel-matrix representation axes of 
the image to a cartesian plane where we can define functions on the interval
[-1/2, 1/2]. Then, I defined a basic surface based on trig functions, 
z = cos(x*2pi)+sin(y*2pi). To give it more intrigue, I decided to take the arguments of
the trig functions to different powers and made them dependant on x and y. I used 
the modulo operator to ensure the power would exceed binary integer representation.
Lastly, I normalized the function so that it would output a number in [0, 1]. I used
this intensity with x and y to color the image.
