#ifndef SPHERE_H
#define SPHERE_H

#include "SceneObject.h"
#include "Vector.h"
#include "math.h"

#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif

class Sphere : public LitSceneObject {
 private:
  float radius_;

 protected:
  virtual Vector TextureCoordinatesAt(const Vector& point);

 public:
  Sphere(const Vector& position, float radius, const Vector& k_scalars,
         const Vector& diffuse_color, const Vector& specular_color,
         float specularity, float opacity, float eta);
  Sphere(const Vector& position, float radius, const Vector& k_scalars,
         const Vector& specular_color, float specularity, float opacity, float eta,
         const char* texture_map);

  ~Sphere();

  virtual SceneObject* RayIntersects(const Vector& dir, const Vector& origin,
                                     float* t1, float* t2);
  virtual Vector Normal(const Vector& point);

};

#endif