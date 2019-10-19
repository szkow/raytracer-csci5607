#include "Texture.h"

#include <cmath>
#include <cstdio>

Texture::Texture(const char* filename) {
  if (filename != nullptr) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {  // If given a null filename, exit
      fprintf(stderr, "Unable to open texture %s!\n", filename);
      throw 1;
    } else {  // Otherwise, construct
      if (fscanf(file, "P3 %u %u 255\n", &width_, &height_) < 2) {
        fprintf(stderr, "%s is not a valid .ppm file!\n", filename);
      }

      // Dynamically allocate pixels array
      map_ = new Vector*[height_];
      for (unsigned int i = 0; i < height_; ++i) {
        map_[i] = new Vector[width_];
      }

      // Fill pixels array
      unsigned int r, g, b;
      for (unsigned int i = 0; i < height_; ++i) {
        for (unsigned int j = 0; j < width_; ++j) {
          fscanf(file, "%u %u %u ", &r, &g, &b);
          map_[i][j] =
              Vector((float)r / 255.0f, (float)g / 255.0f, (float)b / 255.0f);
        }
      }
    }
  }
}

Texture::~Texture() {
  for (unsigned int j = 0; j < height_; ++j) {
    delete[] map_[j];
  }
  delete[] map_;
}

Texture::Texture(const Texture& copy) {
  width_ = copy.width_;
  height_ = copy.height_;

  // Dynamically allocate pixels array
  map_ = new Vector*[height_];
  for (unsigned int i = 0; i < height_; ++i) {
    map_[i] = new Vector[width_];
  }

  for (unsigned int i = 0; i < height_; ++i) {
    for (unsigned int j = 0; j < width_; ++j) {
      map_[i][j] = copy.map_[i][j];
    }
  }
}

Vector Texture::ColorAt(const Vector& point) {
  return ColorAt(point[0], point[1]);
}

Vector Texture::ColorAt(float u, float v) {
  if (u < 0.0f || v < 0.0f) {
    fprintf(stderr, "Invalid u (%f) and v (%f) coordinates!\n", u, v);
    throw 1;
  }
  unsigned int j = (unsigned int)roundf(u * width_);
  unsigned int i = (unsigned int)roundf(v * height_);
  j = (j - 1) % width_;
  i = (i - 1) % height_;

  //return Vector(u, 0, v);
  return map_[i][j];
}
