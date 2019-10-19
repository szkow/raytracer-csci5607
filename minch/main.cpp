#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif

#define RAYTRACE_DEPTH 1000l
#define SELF_INTERSECT_EPSILON 0.0001f
//#define USE_SOFT_SHADOWS
#define SOFT_SHADOW_SAMPLES 75
#define SOFT_SHADOW_PERTURB 0.4f

#include "ImageWriter.h"
#include "SceneReader.h"
#include "Sphere.h"
#include "Vector.h"

#include <math.h>
#include <cstdio>
#include <cstring>
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
Vector ShadeRay(const Vector& ray_intersection, const Vector& normal,
                const Vector& view, SceneObject::LightingParcel& light_data,
                const SceneReader::SceneParameters& params);

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
    ray->t = -1.0f;
    return false;
  }

  // If we hit something, store hit scalar and object pointer. Return true
  ray->object = min_hit;
  ray->t = min_distance;
  return true;
}

Vector ShadeRay(const Vector& ray_intersection, const Vector& normal,
                const Vector& view, SceneObject::LightingParcel& light_data,
                const SceneReader::SceneParameters& params) {
  Vector color = Vector::Zero();
  Ray ray;

  // Sum diffuse and specular contributions from each light source in the scene
  for (std::vector<Light*>::const_iterator i = params.lights.cbegin();
       i < params.lights.cend(); ++i) {
    Light* light = *i;

    // Cast a ray to determine if pixel is in shadow
    ray.origin = ray_intersection;
    ray.direction = light->GetLightDirection(ray_intersection);
    float shadowed = ShadowRay(&ray, light, params);

    if (shadowed == 0.0f) {
      continue;
    }
    // If not in shadow, calculate Phong-Blinn lighting diffuse and specular
    // terms
    Vector phong_component = light->PhongDiffuseSpecular(
        ray_intersection, normal, view,
        light_data);  // Note: already factors in light tint
    color = color + shadowed * phong_component;
  }

  // Add in the ambient contribution
  color = color + light_data.material_scalars[0] * light_data.diffuse_color;
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

Vector RecursiveRay(const Ray& incident_ray, float incident_index, float f_r, float attenuation, long depth, const SceneReader::SceneParameters& params) {
  // Base case
  if (depth == 0l) {
    return Vector::Zero();  // Identity of addition
  } 

  Vector reflection_component, refraction_component;
  if (incident_ray.t < 0.0f) {
    return Vector::Zero();  //TODO: add support for skybox
  }

  // Get some data from the object
  Vector intersection =
      incident_ray.origin + incident_ray.t * incident_ray.direction;
  SceneObject::LightingParcel light_data = incident_ray.object->LightData(intersection);  //TODO: this is so slooooowwwww
  Vector incident_direction = -1.0f * incident_ray.direction.Normed();
  Vector normal = incident_ray.object->Normal(intersection);

  // Calculate reflected direction
  Vector reflected_direction =
      2.0f * (normal * incident_direction) * normal - incident_direction;
  reflected_direction =
      reflected_direction.Normed();

  // Cast the reflected ray
  Ray reflected_ray = {intersection, reflected_direction, -1.0f, nullptr};
  CastRay(&reflected_ray, params);
  reflection_component = RecursiveRay(reflected_ray, incident_index, f_r, attenuation, depth / 2l, params);

  //TODO: add an is_transparent bool, index_of_refraction to SceneObject
  // Calculate a refracted direction
  float object_index;
  float i_dot_n = incident_direction * normal;
  float index_ratios = incident_index / object_index;
  Vector refracted_direction =
      -1.0f * normal *
          std::sqrtf(1.0f - (index_ratios * index_ratios) *
                                (1.0f - i_dot_n * i_dot_n)) +
      index_ratios * i_dot_n * (normal - incident_direction);
  refracted_direction = refracted_direction.Normed();

  // Cast the refracted ray
  Ray refracted_ray = {intersection, refracted_direction, -1.0f, nullptr};
  refraction_component =
      RecursiveRay(refracted_ray, object_index, f_r, attenuation, depth / 2l, params);

  // Return the sum of the reflection and refraction
  return f_r * reflection_component +
         std::expf(-attenuation * refracted_ray.t) * refraction_component;
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
        Vector intersection = ray.t * ray.direction + ray.origin;
        Vector normal = ray.object->Normal(intersection);
        SceneObject::LightingParcel light_data =
            ray.object->LightData(intersection);
        color = ShadeRay(intersection, normal, -1.0f * ray.direction,
                         light_data, params);
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
