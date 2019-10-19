#ifndef SCENE_READER_H
#define SCENE_READER_H

#include <vector>
#include "SceneObject.h"
#include "Light.h"

#define READ_ERROR(out, str)    \
  fprintf(stderr, "%s\n", str); \
  fclose(input);                \
  return 1;

class SceneReader {
 public:
  typedef struct {
    float camera_origin[3];
    float camera_forward[3];
    float camera_up[3];
    float fov;
    int image_width;
    int image_height;
    float background[3];
    std::vector<SceneObject*> objects;
    std::vector<Light*> lights;
  } SceneParameters;

  static int Read(const char* filename, SceneParameters* params);
  static void ParameterDestructor(SceneParameters* params);
};

#endif  // !SCENE_READER_H
