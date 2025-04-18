#ifndef IMAGE_H
#define IMAGE_H

#include <libusb-1.0/libusb.h>

#include <algorithm>
#include <cstdint>
#include <vector>

class Image {
 public:
  Image() = default;
  ~Image() = default;

  void init(int WIDTH, int HEIGHT);
  void clear() { std::fill(buffer.begin(), buffer.end(), 0x0); };
  void fill() { std::fill(buffer.begin(), buffer.end(), 0xff); };

  // мои функции
  void static_color(uint8_t r, uint8_t g, uint8_t b);
  uint32_t rgb(float ratio);
  void test();
  void displayText(const char *color, const char *text);
  void position();

  void setPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b);

  std::vector<uint8_t> reverseBuffer();

  std::vector<uint8_t> getBuffer() { return reverse_image == false ? buffer : reverseBuffer(); }

  bool reverse_image = false;

 private:
  int width_image = 0;
  int height_image = 0;
  std::vector<uint8_t> buffer;
};

#endif