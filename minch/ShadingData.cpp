#include "ShadingData.h"
#include "Texture.h"
#include "Vector.h"

ShadingData::ShadingData()
    : is_transparent_(false),
      is_textured_(false),
      specularity_(0.0f),
      k_(Vector::Zero()),
      diffuse_(Vector::Zero()),
      specular_(Vector::Zero()),
      opacity_(1.0f),
      index_of_refraction_(0.0f),
      texture_(nullptr) {}

ShadingData::ShadingData(const ShadingData& other) { 
  k_ = other.k_;
  diffuse_ = other.diffuse_;
  specular_ = other.specular_;
  specularity_ = other.specularity_;
  opacity_ = other.opacity_;
  index_of_refraction_ = other.index_of_refraction_;
  is_textured_ = other.is_textured_;
  is_transparent_ = other.is_transparent_;
  texture_ = new Texture(*other.texture_);
}

ShadingData::ShadingData(const Vector& k, const Vector& diffuse,
                         const Vector& specular, float specularity, float opacity, float eta)
    : ShadingData() {
  k_ = k;
  diffuse_ = diffuse;
  specular_ = specular;
  specularity_ = specularity;
  opacity_ = opacity;
  index_of_refraction_ = eta;
  is_transparent_ = opacity_ < 1.0f;
}

ShadingData::ShadingData(const Vector& k,
                         const char* texture,
                         const Vector& specular, float specularity, float opacity, float eta)
  : ShadingData(k, Vector::Zero(), specular, specularity, opacity, eta) {
  texture_ = new Texture(texture);
  is_textured_ = true;
}

ShadingData::~ShadingData() { delete texture_; }

Vector ShadingData::GetDiffuseColor(const Vector& point) {
  if (is_textured_) {
    return texture_->ColorAt(point);
  } else {
    return diffuse_;
  }
}
