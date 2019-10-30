#ifndef SHADING_DATA_H
#define SHADING_DATA_H

#include "Texture.h"
#include "Vector.h"

class ShadingData {
 public:
  friend class LitSceneObject;
  friend class SimpleSceneObject;

  ShadingData();
  ShadingData(const ShadingData& other);
  ShadingData(const Vector& k, const Vector& diffuse,
                                 const Vector& specular, float specularity,
                                 float opacity, float eta);
  ShadingData(const Vector& k, const char* texture, const Vector& specular,
              float specularity, float opacity, float eta);
  ~ShadingData();

  Vector GetDiffuseColor(const Vector& point);

 private:
  bool is_transparent_, is_textured_;
  float specularity_, opacity_, index_of_refraction_;
  Vector k_, diffuse_, specular_;
  Texture* texture_;
};

#endif