#include "SceneObject.h"

LitSceneObject::LitSceneObject(const Vector& position,
                               const Vector& material_scalars,
                               const Vector& diffuse_color,
                               const Vector& specular_color, float specularity)
    : SceneObject(position),
      light_data_{material_scalars, specular_color, specularity, diffuse_color},
      texture_map_(nullptr), 
      textured_(false) {}

LitSceneObject::LitSceneObject(const Vector& position,
                               const Vector& material_scalars,
                               const Vector& specular_color, float specularity,
                               const char* texture_map)
    : SceneObject(position),
      light_data_{material_scalars, specular_color, specularity,
                  -1.0f * Vector::One()},
      texture_map_(Texture(texture_map)),
      textured_(true) {}

Texture* LitSceneObject::TextureMap() { 
  if (textured_) {
    return &texture_map_; 
  }
  return nullptr;
}

SceneObject::SceneObject(Vector position) : position_(position) {}

SceneObject::~SceneObject() {}

const Vector& SceneObject::Position() { return position_; }

LitSceneObject::LightingParcel LitSceneObject::LightData(const Vector& point) {
  if (textured_) {
    light_data_.diffuse_color = texture_map_.ColorAt(point);  
  }
  return light_data_;
}

SimpleSceneObject::SimpleSceneObject(LitSceneObject* daddy, Vector position)
    : SceneObject(position), daddy_(daddy) {}

SceneObject::LightingParcel SimpleSceneObject::LightData(const Vector& point) {
  return daddy_->LightData(point);
}
