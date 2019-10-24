#include "SceneObject.h"

LitSceneObject::LitSceneObject(const Vector& position, const Vector& k_scalars,
                               const Vector& diffuse_color,
                               const Vector& specular_color, float specularity,
                               float opacity, float eta)
    : SceneObject(position),
      light_data_(ShadingData(k_scalars, diffuse_color, specular_color,
                              specularity, opacity, eta)) {}

LitSceneObject::LitSceneObject(const Vector& position, const Vector& k_scalars,
                               const Vector& specular_color, float specularity,
                               const char* texture_map, float opacity,
                               float eta)
    : SceneObject(position),
      light_data_(ShadingData(k_scalars, texture_map, specular_color,
                              specularity, opacity, eta)) {}

LitSceneObject::~LitSceneObject() { }

SceneObject::SceneObject(Vector position) : position_(position) {}

SceneObject::~SceneObject() {}

const Vector& SceneObject::Position() { return position_; }

Vector LitSceneObject::DiffuseColor(const Vector& point) { 
  if (light_data_.is_textured_) {
    return light_data_.GetDiffuseColor(TextureCoordinatesAt(point));
  } else {
    return light_data_.GetDiffuseColor(point);
  }
}

float LitSceneObject::Specularity() { return light_data_.specularity_; }

const Vector& LitSceneObject::KScalars() { return light_data_.k_; }

const Vector& LitSceneObject::SpecularColor() { return light_data_.specular_; }

float LitSceneObject::Opacity() { return light_data_.opacity_; }

float LitSceneObject::IndexOfRefraction() {
  return light_data_.index_of_refraction_;
}

bool LitSceneObject::IsTransparent() { return light_data_.is_transparent_; }

SimpleSceneObject::SimpleSceneObject(ShadingData* daddy, Vector position)
    : SceneObject(position) {
  shader_ = daddy;
}

Vector SimpleSceneObject::DiffuseColor(const Vector& point) {
  if (shader_->is_textured_) {
    return shader_->GetDiffuseColor(TextureCoordinatesAt(point));
  } else {
    return shader_->GetDiffuseColor(point);
  }
}

float SimpleSceneObject::Specularity() { return shader_->specularity_; }

const Vector& SimpleSceneObject::KScalars() { return shader_->k_; }

const Vector& SimpleSceneObject::SpecularColor() {
  return shader_->specular_;
}

float SimpleSceneObject::Opacity() { return shader_->opacity_; }

float SimpleSceneObject::IndexOfRefraction() {
  return shader_->index_of_refraction_;
}

bool SimpleSceneObject::IsTransparent() { return shader_->is_transparent_; }
