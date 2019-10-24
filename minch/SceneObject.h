#ifndef SCENE_OBJECT_H
#define SCENE_OBJECT_H

#include <cstdio>
#include "ShadingData.h"
#include "Texture.h"
#include "Vector.h"

/**
 * The interface for all objects in the scene
 */
class SceneObject {
 public:
  // Constructor
  SceneObject(Vector position = Vector::Zero());
  virtual ~SceneObject();

  // Pure virtuals
  virtual SceneObject* RayIntersects(const Vector& dir, const Vector& origin,
                                     float* t1, float* t2) = 0;
  virtual Vector Normal(const Vector& point) = 0;
  virtual Vector DiffuseColor(const Vector& point) = 0; // Depends on texture, uvs, etc. so we make it pure virtual
  virtual float IndexOfRefraction() = 0;
  virtual float Specularity() = 0;
  virtual const Vector& KScalars() = 0;
  virtual const Vector& SpecularColor() = 0;
  virtual const Vector& Opacity() = 0;
  virtual bool IsTransparent() = 0;

  // Virtuals
  virtual const Vector& Position();

 protected:
  Vector position_;
  Vector diffuse_color;
};

/**
 * A SceneObject with lighting data
 */
class LitSceneObject : public SceneObject {
 public:
  LitSceneObject(const Vector& position = Vector::Zero(),
                 const Vector& k_scalars = Vector::Zero(),
                 const Vector& diffuse_color = Vector::Zero(),
                 const Vector& specular_color = Vector::Zero(),
                 float specularity = 0.0f, float opacity = 1.0f,
                 float eta = 0.0f);

  LitSceneObject(const Vector& position, const Vector& k_scalars,
                 const Vector& specular_color, float specularity,
                 const char* texture_map, float opacity = 1.0f,
                 float eta = 0.0f);
  virtual ~LitSceneObject();
  virtual SceneObject* RayIntersects(const Vector& dir, const Vector& origin,
                                     float* t1, float* t2) = 0;
  virtual Vector Normal(const Vector& point) = 0;

  virtual Vector DiffuseColor(const Vector& point);
  virtual float Specularity();
  virtual const Vector& KScalars();
  virtual const Vector& SpecularColor();
  virtual const Vector& Opacity();
  virtual float IndexOfRefraction();
  virtual bool IsTransparent();

 protected:
   /// <summary> If the object is textured, have this function return the
   /// texture coordinates at the point. Otherwise, the return value is
   /// not used. </summary>
  virtual Vector TextureCoordinatesAt(const Vector& point) = 0;
  ShadingData light_data_;
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
  SimpleSceneObject(ShadingData* daddy,
                    Vector position = Vector::Zero());

  virtual SceneObject* RayIntersects(const Vector& dir, const Vector& origin,
                                     float* t1, float* t2) = 0;
  virtual Vector Normal(const Vector& point) = 0;

  virtual Vector DiffuseColor(const Vector& point);
  virtual float Specularity();
  virtual const Vector& KScalars();
  virtual const Vector& SpecularColor();
  virtual const Vector& Opacity();
  virtual float IndexOfRefraction();
  virtual bool IsTransparent();

 protected:
  virtual Vector TextureCoordinatesAt(const Vector& point) = 0;
  ShadingData* shader_;
};

#endif
