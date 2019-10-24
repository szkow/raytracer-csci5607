#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif

#define SELF_INTERSECT_EPSILON 0.0001f
//#define USE_SOFT_SHADOWS
#define SOFT_SHADOW_SAMPLES 75
#define SOFT_SHADOW_PERTURB 0.4f
#define RECURSIVE_RAY_DEPTH 100l

#include "ImageWriter.h"
#include "SceneReader.h"
#include "Sphere.h"
#include "Vector.h"

#include <math.h>
#include <cstdio>
#include <cstring>
#include <stack>
#include <vector>

typedef struct {
  Vector origin;
  Vector direction;
  float t;
  SceneObject* object;
} Ray;

// Shoots a ray into the scene, fills the given ray if hits something
bool CastRay(Ray* ray, const SceneReader::SceneParameters& params);

// Shoots a ray to determine shadows
float ShadowRay(Ray* ray, Light* light,
                const SceneReader::SceneParameters& params);

// Does Blinn-Phong and stuff and returns the final pixel color
Vector ShadeRay(const Ray& incoming_ray,
                const SceneReader::SceneParameters& params);

// Recursively calculates global lighting
Vector RecursiveRay(const Ray& incident_ray, std::stack<float>& eta_stack,
                    const SceneReader::SceneParameters& params, long depth);

// Does the raytracing using the above methods
int TraceRay(float* pixels, const SceneReader::SceneParameters& params);

int main(int argc, char** argv) {
  if (argc < 2) {
    fprintf(stderr, "Please provide the path to a .txt file\n");
    return 1;
  }

  // Parse stuff from input file
  char* filename = argv[1];
  SceneReader::SceneParameters params;
  if (SceneReader::Read(filename, &params) != 0) {
    fprintf(stderr, "Failed to read from %s\n", filename);
    return 1;
  }

  // Make an array to hold pixels
  float* pixels = new float[3 * params.image_width * params.image_height];

  // Define viewing window and begin raycasting
  TraceRay(pixels, params);

  // Edite input filename to .ppm extension
  char* name_end = strstr(
      filename, ".txt");  // Lop off filename's extension so we can add our own
  if (name_end == NULL) {
    fprintf(stderr, "File does not have .txt extension!\n");
    return 1;
  }
  strcpy(name_end, ".ppm");  // Replace with .ppm

  // Write pixels
  int retval = ImageWriter::PpmWrite(filename, pixels, params.image_width,
                                     params.image_height);

  // Clean up and return
  SceneReader::ParameterDestructor(&params);
  delete[] pixels;
  return retval;
}

bool CastRay(Ray* ray, const SceneReader::SceneParameters& params) {
  // Iterate over objects, depth testing
  SceneObject* min_hit = nullptr;
  float min_distance = INFINITY;  // Sentinel value
  for (std::vector<SceneObject*>::const_iterator i = params.objects.cbegin();
       i < params.objects.cend(); ++i) {
    // Determine intersection point(s)
    float t1 = -1.0f, t2 = -1.0f;
    SceneObject* hit =
        (*i)->RayIntersects(ray->direction, ray->origin, &t1, &t2);

    // Check if self-intersection due to numerical error or we didn't intersect
    // with anything
    if (t1 < SELF_INTERSECT_EPSILON || hit == nullptr) {
      continue;
    }
    // Update minimum distance
    else if (t1 < min_distance) {
      min_hit = hit;
      min_distance = t1;
    }
  }

  // If we missed everything, set object ref and hit time to sentinel values.
  // Return false
  if (min_hit == nullptr) {
    ray->object = nullptr;
    ray->t = INFINITY;
    return false;
  }

  // If we hit something, store hit scalar and object pointer. Return true
  ray->object = min_hit;
  ray->t = min_distance;
  return true;
}

Vector ShadeRay(const Ray& incoming_ray,
                const SceneReader::SceneParameters& params) {
  SceneObject* object = incoming_ray.object;
  Vector intersection =
      incoming_ray.t * incoming_ray.direction + incoming_ray.origin;
  Vector normal = object->Normal(intersection);
  Vector view = -1.0f * incoming_ray.direction;

  Vector color = Vector::Zero();
  Ray ray;

  // Sum diffuse and specular contributions from each light source in the scene
  for (std::vector<Light*>::const_iterator i = params.lights.cbegin();
       i < params.lights.cend(); ++i) {
    Light* light = *i;

    // Cast a ray to determine if pixel is in shadow
    ray.origin = intersection;
    ray.direction = light->GetLightDirection(intersection);
    float shadowed = ShadowRay(&ray, light, params);

    if (shadowed == 0.0f) {
      continue;
    }
    // If not in shadow, calculate Phong-Blinn lighting diffuse and specular
    // terms
    Vector phong_component = light->PhongDiffuseSpecular(
        intersection, normal, view,
        object);  // Note: already factors in light tint
    color = color + shadowed * phong_component;
  }

  // Recursively add in reflective and refracted raytracing
  std::stack<float> eta_stack = std::stack<float>();
  eta_stack.push(1.0f); // Push air's eta
  color = color + RecursiveRay(incoming_ray, eta_stack, params, 1);  // RECURSE!! 

  // Add in the ambient contribution
  Vector k = object->KScalars();
  color = color + k[0] * object->DiffuseColor(intersection);
  return color.Clamped(1.0f);
}

float ShadowRay(Ray* ray, Light* light,
                const SceneReader::SceneParameters& params) {
  // If we don't hit anything, we're fully exposed
  if (CastRay(ray, params) == false) {
    return 1.0f;
  }
  // If the light source is not a directional light, we need to test distance to
  // light against collision
  else if (light->DistanceTo(ray->origin) < ray->t) {
    return 1.0f;
  }
#ifndef USE_SOFT_SHADOWS
  return 0.0f;
#else
  // Shoot a bunch of randomly-perturbed rays out and use the average hit
  // percentage as an exposure percent
  Vector origin = ray->origin;
  Vector direction = ray->direction;
  float sum = 0.0f;
  for (int i = 0; i < SOFT_SHADOW_SAMPLES; ++i) {
    Vector perturbation = (Vector::Random() * SOFT_SHADOW_PERTURB);
    Ray soft = {origin, (direction + perturbation).Normed(), -1.0f, nullptr};
    sum += (CastRay(&soft, params)) ? 0.0f : 1.0f;
  }

  return sum / (float)SOFT_SHADOW_SAMPLES;
#endif
}

Vector RecursiveRay(const Ray& incident_ray, std::stack<float>& eta_stack,
                    const SceneReader::SceneParameters& params, long depth) {
  // Base cases: if we've reached our depth or if the incident ray never hit
  // anything
  if (depth > RECURSIVE_RAY_DEPTH || incident_ray.t < 0.0f) {
    return Vector::Zero();  // Identity of addition
  } else if (isinf(incident_ray.t)) {
    return params.background; // TODO: add support for skybox
  }

  Vector reflection_component = Vector::Zero();
  Vector refraction_component = Vector::Zero();

  // Get some data from the object
  Vector intersection =
      incident_ray.origin + incident_ray.t * incident_ray.direction;
  Vector incident_direction = -incident_ray.direction.Normed(); // Flip the direction
  Vector normal = incident_ray.object->Normal(intersection);
  if (incident_direction * normal < 0.0f) {
    normal = -normal;
  }
  float i_dot_n = incident_direction * normal;
  float object_eta = incident_ray.object->IndexOfRefraction();
  float incident_eta = eta_stack.top();  // Just peek at the index. We'll remove
                                         // it when we refract a ray


  // Calculate reflected direction
  Vector reflected_direction =
      2.0f * (normal * incident_direction) * normal - incident_direction;
  reflected_direction = reflected_direction.Normed();

  // Apply the Shlick Approximation to calculate Fresnel effect
  float f_0 = ((incident_eta - object_eta) / (incident_eta + object_eta)) *
              ((incident_eta - object_eta) / (incident_eta + object_eta));
  float f_r = f_0 + (1.0f - f_0) * std::powf(1.0f - i_dot_n, 5.0f);

  // Cast the reflected ray
  Ray reflected_ray = {intersection, reflected_direction, -1.0f, nullptr};
  CastRay(&reflected_ray, params);

  // Recurse with reflected ray
  reflection_component =
      f_r * RecursiveRay(reflected_ray, eta_stack, params, depth + 1l);

  /*
  // We only calculate the transmitted ray if the object is transparent
  if (incident_ray.object->IsTransparent()) {
    // Calculate a refracted direction
    float opacity = incident_ray.object->Opacity();
    float index_ratios = incident_eta / object_eta;
    Vector refracted_direction =
        -normal *
            std::sqrtf(1.0f - (index_ratios * index_ratios) *
                                  (1.0f - i_dot_n * i_dot_n)) +
        index_ratios * i_dot_n * (normal - incident_direction);
    refracted_direction = refracted_direction.Normed();

    // Cast the refracted ray
    Ray refracted_ray = {intersection, refracted_direction, -1.0f, nullptr};
    CastRay(&refracted_ray, params);

    // Recurse with refracted ray (applying Beer attenuation)
    eta_stack.pop();  // Pop the incident index
    eta_stack.push(object_eta); // And push on the new medium's
    refraction_component =
        RecursiveRay(refracted_ray, eta_stack, params, depth + 1l);
    refraction_component =
        std::expf(-opacity * incident_ray.t) * refraction_component;
  }
  */
  // Return the sum of the reflection and refraction
  return reflection_component + refraction_component;
}

int TraceRay(float* pixels, const SceneReader::SceneParameters& params) {
  // Define camera coordinates
  Vector origin = Vector(params.camera_origin);
  float view_distance = 1.0f;  // Fix view distance
  Vector forward = Vector(params.camera_forward).Normed();
  Vector u = forward.Cross(Vector(params.camera_up)).Normed();
  Vector v = u.Cross(forward);

  // Define view window in world space
  float height = 2.0f * tanf(0.5f * params.fov * (float)(M_PI / 180.0)) /
                 view_distance;  // View distance should just be 1
  float width =
      height * ((float)params.image_width /
                (float)params.image_height);  // Get width from aspect ratio
  Vector view_upper_left =
      origin + forward * view_distance - u * 0.5f * width + v * 0.5f * height;

  // Find width of one pixel in world space
  float pixel_size = width / params.image_width;

  Ray ray;
  ray.origin = origin;

  int index = 0;
  // Iterate over pixels, raycasting
  for (int y = 0; y < params.image_height; ++y) {
    Vector v_component = view_upper_left - ((float)y + 0.5f) * pixel_size * v;
    for (int x = 0; x < params.image_width; ++x) {
      // Calculate ray through current pixel
      Vector u_component = ((float)x + 0.5f) * pixel_size * u;

      // Shoot the ray
      ray.direction = (u_component + v_component - origin).Normed();
      bool hit = CastRay(&ray, params);

      // Calculate final color
      Vector color;
      if (hit) {
        color = ShadeRay(ray, params);
      } else {
        color = params.background;
      }

      // Push calculated color to the pixels array
      pixels[index] = color[0];
      pixels[index + 1] = color[1];
      pixels[index + 2] = color[2];

      index += 3;
    }
  }

  return 0;
}
