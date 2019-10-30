#include "SceneReader.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "PolyObject.h"
#include "Sphere.h"

#include <cctype>
#include <cstdio>
#include <cstring>
#include <sstream>

int SceneReader::Read(const char* filename, SceneParameters* params) {
  FILE* input = fopen(filename, "r");
  if (input == NULL) {  // Check file open
    READ_ERROR(input, "Invalid filename provided!");
    fclose(input);
    return 1;
  }

  float x, y, z, radius, A, B, C;   // General position and constant values
  float light_r, light_g, light_b;  // Light colors
  float w;                          // Light type
  float diffuse_r, diffuse_g, diffuse_b, spec_r, spec_g, spec_b, const_a,
      const_d, const_s;  // Material colors
  float specularity;     // Material specularity
  float opacity, eta;     // Transparent material stuffs
  unsigned int v_1, v_2, v_3, t_1, t_2, t_3, n_1, n_2,
      n_3;  // Indices into verts list
  std::vector<Vector> verts;
  std::vector<Vector> normals;
  std::vector<Vector> uvs;
  std::vector<unsigned int> vert_indices;
  std::vector<unsigned int> normal_indices;
  std::vector<unsigned int> uv_indices;
  std::string texture_map;
  bool using_texture;

  int scan_return;                   // Holds return value of fscanf calls
  char word[100];                    // Buffer to read into
  char line[1000];                   // Buffer to read full lines into
  int necessary_keywords_count = 0;  // Count of necessary keywords provided

  while (fscanf(input, "%99s", word) != EOF) {
    if (word[0] == '#') {  // Ignore comments
      fgets(line, 1000, input);
      continue;
    }
    /***************************************************
     * Necessary keywords
     **************************************************/
    else if (strcmp(word, "eye") == 0) {  // eye
      scan_return =
          fscanf(input, "%f %f %f\n", &params->camera_origin[0],
                 &params->camera_origin[1], &params->camera_origin[2]);
      if (scan_return < 3) {
        READ_ERROR(input, "Invalid eye arguments provided");
      }
      ++necessary_keywords_count;
    } else if (strcmp(word, "viewdir") == 0) {  // viewdir
      scan_return =
          fscanf(input, "%f %f %f\n", &params->camera_forward[0],
                 &params->camera_forward[1], &params->camera_forward[2]);
      if (scan_return < 3) {
        READ_ERROR(input, "Invalid viewdir arguments provided");
      }
      if (params->camera_forward[0] == 0.0f &&
          params->camera_forward[1] == 0.0f &&
          params->camera_forward[2] == 0.0f) {
        READ_ERROR(input, "Invalid view direction provided!");
      }
      ++necessary_keywords_count;
    } else if (strcmp(word, "updir") == 0) {  // updir
      scan_return = fscanf(input, "%f %f %f\n", &params->camera_up[0],
                           &params->camera_up[1], &params->camera_up[2]);
      if (scan_return < 3) {
        READ_ERROR(input, "Invalid updir arguments provided");
      }
      if (params->camera_up[0] == 0.0f && params->camera_up[1] == 0.0f &&
          params->camera_up[2] == 0.0f) {
        READ_ERROR(input, "Invalid view up provided!");
      }
      ++necessary_keywords_count;
    } else if (strcmp(word, "vfov") == 0) {  // vfov
      scan_return = fscanf(input, "%f\n", &params->fov);
      if (scan_return < 1) {
        READ_ERROR(input, "Invalid vfov arguments provided");
      }
      if (params->fov >= 180.0f || params->fov <= 0.0f) {
        READ_ERROR(input, "Invalid FOV provided!");
      }
      ++necessary_keywords_count;
    } else if (strcmp(word, "imsize") == 0) {  // imsize
      scan_return =
          fscanf(input, "%i %i\n", &params->image_width, &params->image_height);
      if (scan_return < 2) {
        READ_ERROR(input, "Invalid imsize arguments provided");
      }
      if (params->image_height < 0 || params->image_width < 0) {
        READ_ERROR(input, "Invalid dimensions provided!");
      }
      ++necessary_keywords_count;
    } else if (strcmp(word, "bkgcolor") == 0) {  // bkgcolor
      scan_return = fscanf(input, "%f %f %f\n", &params->background[0],
                           &params->background[1], &params->background[2]);
      if (scan_return < 3) {
        READ_ERROR(input, "Invalid bkgcolor arguments provided");
      } else if (params->background[0] < 0.0f || params->background[1] < 0.0f ||
                 params->background[2] < 0.0f || params->background[0] > 1.0f ||
                 params->background[1] > 1.0f || params->background[2] > 1.0f) {
        READ_ERROR(input, "Invalid color value provided!");
      }
      ++necessary_keywords_count;
    }
    /***************************************************
     * Object and material keywords
     **************************************************/
    else if (strcmp(word, "mtlcolor") == 0) {  // mtlcolor
      if (verts.size() >
          0) {  // If we had verts in the buffer, make an object out of them
        if (using_texture) {
          params->objects.push_back(new PolyObject(
              verts, vert_indices, normals, normal_indices, uvs, uv_indices,
              Vector(const_a, const_d, const_s), Vector(spec_r, spec_g, spec_b),
              specularity, opacity, eta, texture_map.c_str()));
        } else {
          params->objects.push_back(
              new PolyObject(verts, vert_indices, normals, normal_indices, uvs,
                             uv_indices, Vector(const_a, const_d, const_s),
                             Vector(diffuse_r, diffuse_g, diffuse_b),
                             Vector(spec_r, spec_g, spec_b), specularity, opacity, eta));
        }

        // Reset the vectors so we don't double up verts, etc.
        verts.clear();
        vert_indices.clear();
        uvs.clear();
        uv_indices.clear();
        normals.clear();
        normal_indices.clear();
      }

      scan_return = fscanf(input, "%f %f %f %f %f %f %f %f %f %f %f %f\n", &diffuse_r,
                           &diffuse_g, &diffuse_b, &spec_r, &spec_g, &spec_b,
                           &const_a, &const_d, &const_s, &specularity, &opacity, &eta);
      if (scan_return < 12) {
        READ_ERROR(input, "Invalid mtlcolor arguments provided");
      }
      if (diffuse_r < 0.0f || diffuse_g < 0.0f || diffuse_b < 0.0f ||
          spec_r < 0.0f || spec_g < 0.0f || spec_b < 0.0f || const_a < 0.0f ||
          const_d < 0.0f || const_s < 0.0f || specularity < 0 ||
          diffuse_r > 1.0f || diffuse_g > 1.0f || diffuse_b > 1.0f ||
          spec_r > 1.0f || spec_g > 1.0f || spec_b > 1.0f || const_a > 1.0f ||
          const_d > 1.0f || const_s > 1.0f || eta < 0.0f || opacity < 0.0f || opacity > 1.0f) {
        READ_ERROR(input, "Invalid mtlcolor value provided!");
      }

      using_texture = false;                    // Change the texture flag
    } else if (strcmp(word, "texture") == 0) {  // texture
      if (verts.size() >
          0) {  // If we had verts in the buffer, make an object out of them
        if (using_texture) {
          params->objects.push_back(new PolyObject(
              verts, vert_indices, normals, normal_indices, uvs, uv_indices,
              Vector(const_a, const_d, const_s), Vector(spec_r, spec_g, spec_b),
              specularity, opacity, eta, texture_map.c_str()));
        } else {
          params->objects.push_back(
              new PolyObject(verts, vert_indices, normals, normal_indices, uvs,
                             uv_indices, Vector(const_a, const_d, const_s),
                             Vector(diffuse_r, diffuse_g, diffuse_b),
                             Vector(spec_r, spec_g, spec_b), specularity, opacity, eta));
        }

        // Reset the vectors so we don't double up verts, etc.
        verts.clear();
        vert_indices.clear();
        uvs.clear();
        uv_indices.clear();
        normals.clear();
        normal_indices.clear();
      }
      
      char buffer[1000];
      char* buffer_ptr = buffer;
      scan_return = fscanf(input, "%s\n", buffer_ptr);
      texture_map = std::string(buffer);

      if (scan_return < 1 ||
          texture_map.substr(texture_map.length() - 4, 4).compare(".ppm") !=
              0) {
        READ_ERROR(input, "Invalid texture map provided!");
      }
      using_texture = true;                    // Change the texture flag
    } else if (strcmp(word, "sphere") == 0) {  // sphere
      scan_return = fscanf(input, "%f %f %f %f\n", &x, &y, &z, &radius);
      if (scan_return < 4) {
        READ_ERROR(input, "Invalid sphere arguments provided");
      } else if (radius <= 0.0f) {
        READ_ERROR(input, "Invalid sphere radius provided!");
      } else {
        if (using_texture) {
          params->objects.push_back(new Sphere(
              Vector(x, y, z), radius, Vector(const_a, const_d, const_s),
              Vector(spec_r, spec_g, spec_b), specularity, opacity, eta,
              texture_map.c_str()));
        } else {
          params->objects.push_back(new Sphere(
              Vector(x, y, z), radius, Vector(const_a, const_d, const_s),
              Vector(diffuse_r, diffuse_g, diffuse_b),
              Vector(spec_r, spec_g, spec_b), specularity, opacity, eta));
        }
      }

    } /* else if (strcmp(word, "ellipsoid") == 0) {  // ellipsoid
       scan_return =
           fscanf(input, "%f %f %f %f %f %f\n", &x, &y, &z, &A, &B, &C);
       if (scan_return < 6) {
         READ_ERROR(input, "Invalid ellipsoid arguments provided");
       } else if (A <= 0.0f || B <= 0.0f || C <= 0.0f) {
         READ_ERROR(input, "Invalid ellipsoid radius provided!");
       } else {
         params->objects.push_back(new Ellipsoid(
             Vector(x, y, z), Vector(A, B, C), Vector(const_a, const_d,
     const_s), Vector(diffuse_r, diffuse_g, diffuse_b), Vector(spec_r, spec_g,
     spec_b), specularity));
       }
     } */
    /***************************************************
     * .obj Format keywords
     **************************************************/
    else if (strcmp(word, "v") == 0) {  // v
      scan_return = fscanf(input, "%f %f %f\n", &x, &y, &z);
      if (scan_return < 3) {
        READ_ERROR(input, "Invalid vertex provided!");
      } else {
        verts.push_back(Vector(x, y, z));
      }
    } else if (strcmp(word, "vn") == 0) {  // vn
      scan_return = fscanf(input, "%f %f %f\n", &x, &y, &z);
      if (scan_return < 3) {
        READ_ERROR(input, "Invalid vn provided!");
      } else {
        normals.push_back(Vector(x, y, z));
      }
    } else if (strcmp(word, "vt") == 0) {  // vt
      scan_return = fscanf(input, "%f %f %*f\n", &x, &y);
      if (scan_return < 2) {
        READ_ERROR(input, "Invalid vt provided!");
      } else {
        uvs.push_back(Vector(x, y, 0.0f));
      }
    } else if (strcmp(word, "f") == 0) {  // f
      // Read line into buffer
      fscanf(input, " ");  // Read in spaces if they exist
      if (fgets(line, 1000, input) == nullptr) {
        READ_ERROR(input, "Error reading f line!");
      }

      unsigned int v_4, t_4, n_4;
      unsigned int* v[] = {&v_1, &v_2, &v_3,
                           &v_4};  // Made indices into an array
      unsigned int* t[] = {&t_1, &t_2, &t_3,
                           &t_4};  //  so we can use arithmetic to index
      unsigned int* n[] = {&n_1, &n_2, &n_3, &n_4};

      // Define some variables we'll need in the parsing loop
      const char* delims =
          "/\n";  // Just a constant representing the delimiters we use
      std::string line_str =
          std::string(line);  // The line we read above, but as a string
      std::string
          token;  // A token in the line (aka, an unsigned in in string form)
      size_t first_delim;  // Index into line_str to the first delimiter we
                        // encounter from the last token
      int end_of_token =
          0;  // Index into line_str to the end of the last token processed
      bool quad_flag = false;  // Flag for if the line defines a quad, not a tri
      bool using_n = false, using_t = false;  // Flags for if non-flat shading
                                              // and texturing, respectively

      /*
      if (line_str.find_first_of('/') == std::string::npos) { // If file has no
      vn or vt
        // Easy: just scan in the vertex indices
        scan_return = sscanf(line_str.c_str(), "%u %u %u\n", &v_1, &v_2, &v_3);
        if (scan_return < 3) {
          READ_ERROR(input, "Failed to read f indices!")
        }
      } else { }
      */

      // Count how many slashes are in one 'term' of the line
      size_t end_of_term = line_str.find_first_of(' ');
      if (end_of_term == std::string::npos) {
        READ_ERROR(input, "Invalid f values provided!");
      }
      size_t last_slash = 0;
      int slash_count = 0;
      while (last_slash < end_of_term && last_slash != std::string::npos) {
        last_slash = line_str.find_first_of('/', last_slash + 1);
        ++slash_count;
      }

      // Preprocess the line to replace space between tokens with a slash
      for (unsigned int j = 0; j < line_str.length(); ++j) {
        char c = line_str[j];
        if (c == ' ') {
          if (j == 0) {
            continue;
          }
          while (line_str[j + 1] == ' ') {
            line_str.erase(j, 1);
          }
          if (std::isdigit(line_str[j - 1]) &&
              (std::isdigit(line_str[j + 1]) || line_str[j + 1] == '-')) {
            // If we found a space where the its neighbors are digits, replace
            // it
            line_str.replace(j, 1, "/");
          }
        }
      }

      int count = 0;  // Counts how many tokens we've gone through
      while ((first_delim = line_str.find_first_of(delims, end_of_token)) !=
             std::string::npos) {
        token = line_str.substr(end_of_token,
                                first_delim - end_of_token);  // Define token
        end_of_token = first_delim + 1;

        if (token.length() != 0 && token.compare("/") != 0) {
          switch (count % slash_count) {  // Switch on number of numbers read
            case 0:
              scan_return =
                  sscanf(token.c_str(), "%u",
                         v[count / slash_count]);  // Read the vertex index
              break;
            case 1:
              scan_return =
                  sscanf(token.c_str(), "%u",
                         t[count / slash_count]);  // Read the texture index
              using_t = true;
              break;
            case 2:
              scan_return =
                  sscanf(token.c_str(), "%u",
                         n[count / slash_count]);  // Read the normal index
              using_n = true;
              break;
          }
          if (scan_return < 1) {
            READ_ERROR(input, "Failed to read f indices!");
          }
          if (count > 9) {
            quad_flag = true;
          }
        }

        ++count;
      }

      // Go through and push all the parsed indices on to their respective
      // vectors
      vert_indices.push_back(v_1 - 1u);
      vert_indices.push_back(v_2 - 1u);
      vert_indices.push_back(v_3 - 1u);
      if (quad_flag) {
        vert_indices.push_back(v_3 - 1u);
        vert_indices.push_back(v_4 - 1u);
        vert_indices.push_back(v_1 - 1u);
      }
      if (using_t) {
        uv_indices.push_back(t_1 - 1u);
        uv_indices.push_back(t_2 - 1u);
        uv_indices.push_back(t_3 - 1u);
        if (quad_flag) {
          uv_indices.push_back(t_3 - 1u);
          uv_indices.push_back(t_4 - 1u);
          uv_indices.push_back(t_1 - 1u);
        }
      }
      if (using_n) {
        normal_indices.push_back(n_1 - 1u);
        normal_indices.push_back(n_2 - 1u);
        normal_indices.push_back(n_3 - 1u);
        if (quad_flag) {
          normal_indices.push_back(n_3 - 1u);
          normal_indices.push_back(n_4 - 1u);
          normal_indices.push_back(n_1 - 1u);
        }
      }
    }
    /***************************************************
     * Light keywords
     **************************************************/
    else if (strcmp(word, "light") == 0) {  // light
      scan_return = fscanf(input, "%f %f %f %f %f %f %f\n", &x, &y, &z, &w,
                           &light_r, &light_g, &light_b);
      if (scan_return < 7) {
        READ_ERROR(input, "Invalid light arguments provided");
      } else if (light_r < 0.0f || light_g < 0.0f || light_b < 0.0f ||
                 light_r > 1.0f || light_g > 1.0f || light_b > 1.0f) {
        READ_ERROR(input, "Invalid light color provided");
      }
      if (w == 0) {
        params->lights.push_back(new DirectionalLight(
            Vector(x, y, z), Vector(light_r, light_g, light_b)));
      } else if (w == 1) {
        params->lights.push_back(
            new PointLight(Vector(x, y, z), Vector(light_r, light_g, light_b)));
      } else {
        READ_ERROR(input, "Invalid light w provided!");
      }
    } else if (strcmp(word, "attlight") == 0) {  // attlight
      scan_return = fscanf(input, "%f %f %f %f %f %f %f %f %f %f\n", &x, &y, &z,
                           &w, &light_r, &light_g, &light_b, &A, &B, &C);
      if (scan_return < 10) {
        READ_ERROR(input, "Invalid attlight arguments provided");
      } else if (light_r < 0.0f || light_g < 0.0f || light_b < 0.0f ||
                 light_r > 1.0f || light_g > 1.0f || light_b > 1.0f) {
        READ_ERROR(input, "Invalid light color provided");
      }
      if (w == 1) {
        params->lights.push_back(
            new PointLight(Vector(x, y, z), Vector(light_r, light_g, light_b),
                           Vector(A, B, C)));
      } else {
        READ_ERROR(input, "Invalid light w provided!");
      }
    } else {
      READ_ERROR(input, "Unrecognized keyword provided!");
    }
  }

  // Check if all the necessary keywords (e.g. imsize, vfov) have been provided
  if (necessary_keywords_count != 6) {
    READ_ERROR(input, "Invalid scene descriptor file");
  }

  // Make a polygonal object using verts if one has been provided
  if (verts.size() > 0) {
    // TODO: handle case where material properties are redefined after .obj
    // specs
    if (using_texture) {
      params->objects.push_back(new PolyObject(
          verts, vert_indices, normals, normal_indices, uvs, uv_indices,
          Vector(const_a, const_d, const_s), Vector(spec_r, spec_g, spec_b),
          specularity, opacity, eta, texture_map.c_str()));
    } else {
      params->objects.push_back(
          new PolyObject(verts, vert_indices, normals, normal_indices, uvs,
                         uv_indices, Vector(const_a, const_d, const_s),
                         Vector(diffuse_r, diffuse_g, diffuse_b),
                         Vector(spec_r, spec_g, spec_b), specularity, opacity, eta));
    }
  }
  fclose(input);
  return 0;
}

void SceneReader::ParameterDestructor(SceneParameters* params) {
  for (std::vector<SceneObject*>::iterator i = params->objects.begin();
       i < params->objects.end(); ++i) {
    delete *i;
  }
}
