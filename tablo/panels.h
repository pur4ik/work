#ifndef PANELS_H
#define PANELS_H

#include <cstdint>
#include <string>
#include <vector>
#include <map>

#include "image.h"
#include "imgui.h"
#include "layer.h"
#include "socket.h"

class Panels {
public:
  Panels(int panel_type, std::string name, int startPosX, int startPosY, int width, int height, std::string iface);
  ~Panels() = default;
  std::vector<uint8_t> getFrame();
  void drawPanel();
  std::vector<uint8_t> convertBufferToRGBA(const std::vector<uint8_t> &buffer, int width, int height);


  int get_count_packet_main() { return _socket.count_packet_main; };
  int get_count_packet_error_main() { return _socket.count_packet_error_main; };

private:
  LayerSystem _layer;
  std::string _name;
  int _panel_type;
  int _startPosX;
  int _startPosY;
  int _width;
  int _height;

  Socket _socket;
  Image _frame;
};
#endif