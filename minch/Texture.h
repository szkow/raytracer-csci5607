#ifndef TEXTURE_H
#define TEXTURE_H

#include "Vector.h"
#include <cstdio>

class Texture {
 public:
  Texture(const Texture& copy);
  Texture(const char* filename = nullptr);
  ~Texture();

  Vector ColorAt(const Vector& point) const;
  Vector ColorAt(float u, float v) const;

 protected:
  Vector** map_;

  unsigned int width_, height_;
};

#endif