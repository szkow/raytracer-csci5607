#ifndef SCENE_OBJECT_H
#define SCENE_OBJECT_H

#include "Vector.h"
#include "Texture.h"
#include <cstdio>

/**
 * The interface for all objects in the scene
 */
class SceneObject {
 public:
  typedef struct {
    const Vector material_scalars;
    const Vector specular_color;
    const float specularity;
    Vector diffuse_color; // This value is modified in LightData()
  } LightingParcel;

  // Constructor
  SceneObject(Vector position = Vector::Zero());

  virtual ~SceneObject();
  // Pure virtuals

  virtual SceneObject* RayIntersects(const Vector& dir, const Vector& origin,
                                     float* t1, float* t2) = 0;
  virtual Vector Normal(const Vector& point) = 0;
  virtual LightingParcel LightData(const Vector& point) = 0;

  // Virtuals

  virtual const Vector& Position();

 protected:
  Vector position_;
};

/**
 * A SceneObject with lighting data
 */
class LitSceneObject : public SceneObject {
 public:
  LitSceneObject(const Vector& position = Vector::Zero(),
                 const Vector& material_scalars = Vector::Zero(),
                 const Vector& diffuse_color = Vector::Zero(),
                 const Vector& specular_color = Vector::Zero(), float specularity = 0.0f);

  LitSceneObject(const Vector& position,
                const Vector& material_scalars,
                const Vector& specular_color,
                float specularity, const char* texture_map);

  Texture* TextureMap();

  virtual SceneObject* RayIntersects(const Vector& dir, const Vector& origin,
                                     float* t1, float* t2) = 0;
  virtual Vector Normal(const Vector& point) = 0;

  virtual LightingParcel LightData(const Vector& point);

 protected:
  LightingParcel light_data_;
  Texture texture_map_;
  bool textured_;
};

/**
 * A SceneObject with no lighting data. If it needs to be rendered,
 * it has a pointer to LitSceneObject which it can steal light and
 * color data from.
 *
 * This is primarily used to represent polygons in a PolyObject.
 */
class SimpleSceneObject : public SceneObject {
 public:
  SimpleSceneObject(LitSceneObject* daddy = nullptr,
                    Vector position = Vector::Zero());

  virtual SceneObject* RayIntersects(const Vector& dir, const Vector& origin,
                                     float* t1, float* t2) = 0;
  virtual Vector Normal(const Vector& point) = 0;

  virtual LightingParcel LightData(const Vector& point);

 protected:
  LitSceneObject* daddy_;
};

#endif
