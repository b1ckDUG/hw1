#include "rasterizer.h"

using namespace std;

namespace CGL {

  RasterizerImp::RasterizerImp(PixelSampleMethod psm, LevelSampleMethod lsm,
    size_t width, size_t height,
    unsigned int sample_rate) {
    this->psm = psm;
    this->lsm = lsm;
    this->width = width;
    this->height = height;
    this->sample_rate = sample_rate;

    sample_buffer.resize(width * height * sample_rate, Color::White);
  }

  // Used by rasterize_point and rasterize_line
  void RasterizerImp::fill_pixel(size_t x, size_t y, Color c) {
    // TODO: Task 2: You might need to this function to fix points and lines (such as the black rectangle border in test4.svg)
    // NOTE: You are not required to implement proper supersampling for points and lines
    // It is sufficient to use the same color for all supersamples of a pixel for points and lines (not triangles)


      size_t scale = sqrt(sample_rate);
      sample_buffer[y * (scale * width) + x] = c;
  }

  // Rasterize a point: simple example to help you start familiarizing
  // yourself with the starter code.
  //
  void RasterizerImp::rasterize_point(float x, float y, Color color) {
    // fill in the nearest pixel
    int sx = (int)floor(x);
    int sy = (int)floor(y);

    // check bounds
    if (sx < 0 || sx >= width) return;
    if (sy < 0 || sy >= height) return;

    size_t scale = sqrt(sample_rate);
      for (int dx = 0; dx < scale; dx++) {
          for (int dy = 0; dy < scale; dy++) {
              fill_pixel(sx * scale + dx, sy * scale + dy, color);
          }
      }
    return;
  }

  // Rasterize a line.
  void RasterizerImp::rasterize_line(float x0, float y0,
    float x1, float y1,
    Color color) {
    if (x0 > x1) {
      swap(x0, x1); swap(y0, y1);
    }

    float pt[] = { x0,y0 };
    float m = (y1 - y0) / (x1 - x0);
    float dpt[] = { 1,m };
    int steep = abs(m) > 1;
    if (steep) {
      dpt[0] = x1 == x0 ? 0 : 1 / abs(m);
      dpt[1] = x1 == x0 ? (y1 - y0) / abs(y1 - y0) : m / abs(m);
    }
    while (floor(pt[0]) <= floor(x1) && abs(pt[1] - y0) <= abs(y1 - y0)) {
      rasterize_point(pt[0], pt[1], color);
        for (int dx = 0; dx < scale; dx++) {
          for (int dy = 0; dy < scale; dy++) {
            Vector2D subpixel{x + (dx + 0.5f) / scale,y + (dy + 0.5f) / scale};
              if (is_inside_tri(subpixel, A, B, C)) {
                fill_pixel(subpixel.x * scale, subpixel.y * scale, color);
            }
          }
        }
      pt[0] += dpt[0]; pt[1] += dpt[1];
    }
  }

  // Rasterize a triangle.
  void RasterizerImp::rasterize_triangle(float x0, float y0,
    float x1, float y1,
    float x2, float y2,
    Color color) {
    // TODO: Task 1: Impleme nt basic triangle rasterization here, no supersampling
    size_t xmin = (x0 <= x1) ? ((x0 <= x2) ? x0 : x2) : ((x1 <= x2) ? x1 : x2);
    size_t xmax = (x0 >= x1) ? ((x0 >= x2) ? x0 : x2) : ((x1 >= x2) ? x1 : x2);
    size_t ymin = (y0 <= y1) ? ((y0 <= y2) ? y0 : y2) : ((y1 <= y2) ? y1 : y2);
    size_t ymax = (y0 >= y1) ? ((y0 >= y2) ? y0 : y2) : ((y1 >= y2) ? y1 : y2);
    size_t scale = sqrt(sample_rate);

    Vector2D A(x0, y0);
    Vector2D B(x1, y1);
    Vector2D C(x2, y2);
    for (int x = xmin; x <= xmax; x++) {
      for (int y = ymin; y <= ymax; y++) {
        for (int dx = 0; dx < scale; dx++) {
          for (int dy = 0; dy < scale; dy++) {
            Vector2D subpixel{x + (dx + 0.5f) / scale,y + (dy + 0.5f) / scale};
            if (is_inside_tri(subpixel, A, B, C)) {
              fill_pixel(subpixel.x * scale, subpixel.y * scale, color);
            }
          }
        }
      }
    }

    // TODO: Task 2: Update to implement super-sampled rasterization

  }


  void RasterizerImp::rasterize_interpolated_color_triangle(float x0, float y0, Color c0,
    float x1, float y1, Color c1,
    float x2, float y2, Color c2)
  {
    // TODO: Task 4: Rasterize the triangle, calculating barycentric coordinates and using them to interpolate vertex colors across the triangle
    // Hint: You can reuse code from rasterize_triangle



  }


  void RasterizerImp::rasterize_textured_triangle(float x0, float y0, float u0, float v0,
    float x1, float y1, float u1, float v1,
    float x2, float y2, float u2, float v2,
    Texture& tex)
  {
    // TODO: Task 5: Fill in the SampleParams struct and pass it to the tex.sample function.
    // TODO: Task 6: Set the correct barycentric differentials in the SampleParams struct.
    // Hint: You can reuse code from rasterize_triangle/rasterize_interpolated_color_triangle




  }

  void RasterizerImp::set_sample_rate(unsigned int rate) {
    // TODO: Task 2: You may want to update this function for supersampling support

    this->sample_rate = rate;


    this->sample_buffer.resize(width * height * sample_rate, Color::White);
  }


  void RasterizerImp::set_framebuffer_target(unsigned char* rgb_framebuffer,
    size_t width, size_t height)
  {
    // TODO: Task 2: You may want to update this function for supersampling support

    this->width = width;
    this->height = height;
    this->rgb_framebuffer_target = rgb_framebuffer;


    this->sample_buffer.resize(width * height * sample_rate, Color::White);
  }


  void RasterizerImp::clear_buffers() {
    std::fill(rgb_framebuffer_target, rgb_framebuffer_target + 3 * width * height, 255);
    std::fill(sample_buffer.begin(), sample_buffer.end(), Color::White);
  }


  // This function is called at the end of rasterizing all elements of the
  // SVG file.  If you use a supersample buffer to rasterize SVG elements
  // for antialising, you could use this call to fill the target framebuffer
  // pixels from the supersample buffer data.
  //
  void RasterizerImp::resolve_to_framebuffer() {
    // TODO: Task 2: You will likely want to update this function for supersampling support


    for (int x = 0; x < width; ++x) {
      for (int y = 0; y < height; ++y) {
        //Color col = sample_buffer[y * width + x];
        size_t scale = sqrt(sample_rate);
        Color color(0, 0, 0);
        for (int dx = 0; dx < scale; dx++) {
            for (int dy = 0; dy < scale; dy++) {
                size_t sx = x * scale + dx;
                size_t sy = y * scale + dy;
                color += sample_buffer[sy * (scale * width) + sx];
            }
        }
        color.r /= float(sample_rate);
        color.g /= float(sample_rate);
        color.b /= float(sample_rate);
        for (int k = 0; k < 3; ++k) {
          this->rgb_framebuffer_target[3 * (y * width + x) + k] = (&color.r)[k] * 255;
        }
      }

    }

  }

    bool RasterizerImp::is_inside_tri(Vector2D point, Vector2D A, Vector2D B, Vector2D C) {
      Vector2D AB{B.x - A.x, B.y - A.y};
      Vector2D BC{C.x - B.x, C.y - B.y};
      Vector2D CA{A.x - C.x, A.y - C.y};
      auto sign1 = cross(Vector2D{point.x - A.x, point.y - A.y}, AB);
      auto sign2 = cross(Vector2D{point.x - B.x, point.y - B.y}, BC);
      auto sign3 = cross(Vector2D{point.x - C.x, point.y - C.y}, CA);
      if (sign1 > 0 && sign2 > 0 && sign3 > 0) {
          return true;
      }
      if (sign1 < 0 && sign2 < 0 && sign3 < 0) {
          return true;
      }
      if (sign1 == 0 || sign2 == 0 && sign3 == 0) {
          return true;
      }
      return false;
    }

    Rasterizer::~Rasterizer() { }


}// CGL
