#include "panels.h"
#include "utils.h"
#include <algorithm>
#include <cstdint>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "socket.h"

Panels::Panels(int panel_type, std::string name, int startPosX, int startPosY, int width, int height, std::string iface)
    : _socket(_layer, panel_type, iface), _name(std::move(name)), _startPosX(startPosX), _startPosY(startPosY), _width(width), _height(height),
      _panel_type(panel_type) { // Начало тела конструктора
  _frame.init(_width, _height); // Инициализация объекта _frame
  _socket.start();              // Запуск сокета
}

std::vector<uint8_t> Panels::getFrame() {
  _frame.clear();

  auto &allLayers = _layer.getAllLayer();

  if (!allLayers.empty()) {
    for (auto &solo_layer : allLayers) {
      _frame.drawLayer(solo_layer);
    }
  }

  return _frame.getBuffer();
}

void Panels::drawPanel() {
  if (_panel_type != IDGK) {
    std::vector<uint8_t> frameBuffer = getFrame();
    std::vector<uint8_t> rgbaBuffer = convertBufferToRGBA(frameBuffer, _width, _height);

    float pixelSize = 2.0;
    float gapSize = 1.0;

    ImDrawList *drawList = ImGui::GetWindowDrawList();
    ImGui::SetCursorPos(ImVec2(_startPosX, _startPosY - 25));
    ImGui::TextUnformatted(_name.c_str());

    ImGui::SetCursorPos(ImVec2(_startPosX, _startPosY));
    ImVec2 panelPos = ImGui::GetCursorScreenPos(); // Верхний левый угол панели

    for (int y = 0; y < _height; ++y) {
      for (int x = 0; x < _width; ++x) {
        int index = (y * _width + x) * 4;
        uint8_t r = rgbaBuffer[index];
        uint8_t g = rgbaBuffer[index + 1];
        uint8_t b = rgbaBuffer[index + 2];
        uint8_t a = rgbaBuffer[index + 3];

        float startX = panelPos.x + x * (pixelSize + gapSize);
        float startY = panelPos.y + y * (pixelSize + gapSize);
        float endX = startX + pixelSize;
        float endY = startY + pixelSize;
        ImU32 color = IM_COL32(r, g, b, a);
        drawList->AddRectFilled(ImVec2(startX, startY), ImVec2(endX, endY), color, pixelSize * 0.2f);
      }
    }
    // clang-format off
drawList->AddRect(
    ImVec2(_startPosX, _startPosY),
    ImVec2(_startPosX + _width * (pixelSize + gapSize) + 2, 
           _startPosY + _height * (pixelSize + gapSize) + 2),
    IM_COL32(0, 0, 0, 255), 0.0f, 0, 4.0f);

drawList->AddRect(
    ImVec2(_startPosX, _startPosY),
    ImVec2(_startPosX + _width * (pixelSize + gapSize) + 2, 
           _startPosY + _height * (pixelSize + gapSize) + 2),
    IM_COL32(255, 255, 255, 255), 0.0f, 1, 3.0f);
    // clang-format on

    float textStartX = panelPos.x + _width * (pixelSize + gapSize) + 25;
    float textStartY = panelPos.y - 5;

    ImGui::SetCursorScreenPos(ImVec2(textStartX, textStartY));
    ImGui::Text("Общее количество пакетов:");
    ImGui::SameLine();
    ImGui::TextUnformatted(std::to_string(_socket.count_packet_main).c_str());

    ImGui::SetCursorScreenPos(ImVec2(textStartX, textStartY + 15));
    ImGui::Text("Пакеты сценария:");
    ImGui::SameLine();
    ImGui::TextUnformatted(std::to_string(_socket.count_packet_scen).c_str());

    ImGui::SetCursorScreenPos(ImVec2(textStartX, textStartY + 30));
    ImGui::Text("Пакеты кадра:");
    ImGui::SameLine();
    ImGui::TextUnformatted(std::to_string(_socket.count_packet_frame).c_str());

    ImGui::SetCursorScreenPos(ImVec2(textStartX, textStartY + 45));
    ImGui::Text("Битые пакеты:");
    ImGui::SameLine();
    if (_socket.count_packet_error_main <= 0) {
      ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
    } else {
      ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
    }

    ImGui::TextUnformatted(std::to_string(_socket.count_packet_error_main).c_str());
    ImGui::PopStyleColor();

    ImGui::SetCursorScreenPos(ImVec2(textStartX, textStartY + 60));
    ImGui::Text("Пакеты номера вагона:");
    ImGui::SameLine();
    ImGui::TextUnformatted(std::to_string(_socket.count_packet_carnum).c_str());

    ImGui::SetCursorScreenPos(ImVec2(textStartX, textStartY + 75));
    ImGui::Text("Пакеты времени:");
    ImGui::SameLine();
    ImGui::TextUnformatted(std::to_string(_socket.count_packet_time).c_str());

    ImGui::SetCursorScreenPos(ImVec2(textStartX, textStartY + 90));
    ImGui::Text("Задержка:");
    ImGui::SameLine();
    ImGui::TextUnformatted(std::to_string(_socket.ping).c_str());
    ImGui::SameLine();
    ImGui::Text("мс");
    frameBuffer.clear();
    rgbaBuffer.clear();
  }
  // ЖК ПАНЕЛЬ
  Utils::ParsedData data = Utils::parseAndDisplay(_socket.extractedText);
  ImGui::SetCursorPos(ImVec2(1600, 130));
  ImGui::Text("ЖК Панель:");
  ImGui::SetCursorPos(ImVec2(1600, 150));
  ImGui::Text("Номер Вагона: %s", data.carNum.c_str());
  ImGui::SetCursorPos(ImVec2(1600, 170));
  ImGui::Text("Язык: %s", data.lang.c_str());
  ImGui::SetCursorPos(ImVec2(1600, 190));
  ImGui::Text("IDGK0: %s", data.idGK0.c_str());
  ImGui::SetCursorPos(ImVec2(1600, 210));
  ImGui::Text("IDGK1: %s", data.idGK1.c_str());
  ImGui::SetCursorPos(ImVec2(1600, 230));
  ImGui::Text("Темепература внтури: %s", data.tempIn.c_str());
  ImGui::SetCursorPos(ImVec2(1600, 250));
  ImGui::Text("Температура снаружи: %s", data.tempOut.c_str());
  ImGui::SetCursorPos(ImVec2(1600, 270));
  ImGui::Text("Статус первого туалета : %s", data.wc1.c_str());
  ImGui::SetCursorPos(ImVec2(1600, 290));
  ImGui::Text("Статус второго туалета: %s", data.wc2.c_str());
  ImGui::SetCursorPos(ImVec2(1600, 310));
  ImGui::Text("Тревога: %s", data.alarm.c_str());
  ImGui::SetCursorPos(ImVec2(1600, 330));
  ImGui::Text("Задержка: %s", data.delay.c_str());
  ImGui::SetCursorPos(ImVec2(1600, 350));
  ImGui::Text("Дата: %s", data.dateTime.c_str());
  ImGui::SetCursorPos(ImVec2(1600, 370));
  ImGui::Text("Вызов: %s", data.call.c_str());
  ImGui::SetCursorPos(ImVec2(1600, 390));
  ImGui::Text("Отдых: %s", data.rest.c_str());
  ImGui::SetCursorPos(ImVec2(1600, 410));
  ImGui::Text("Скорость: %s", data.speed.c_str());
  ImGui::SetCursorPos(ImVec2(1600, 430));
  ImGui::Text("Позиция: %s", data.position.c_str());
  ImGui::SetCursorPos(ImVec2(1600, 450));
  ImGui::Text("Станции: %s", data.stations.c_str());
}

std::vector<uint8_t> Panels::convertBufferToRGBA(const std::vector<uint8_t> &buffer, int width, int height) {
  std::vector<uint8_t> rgbaBuffer(width * height * 4, 0); // 4 байта на пиксель для RGBA

  for (int i = 0; i < width * height; ++i) {
    // Считываем пиксель из RGB555
    uint16_t rgb555 = (buffer[i * 2 + 1] << 8) | buffer[i * 2];
    uint8_t r = (rgb555 >> 10) & 0x1F; // 5 бит красного
    uint8_t g = (rgb555 >> 5) & 0x1F;  // 5 бит зеленого
    uint8_t b = rgb555 & 0x1F;         // 5 бит синего

    // Преобразуем в RGB888 (8 бит на компоненту) и добавляем альфа-канал
    rgbaBuffer[i * 4 + 0] = (r << 3) | (r >> 2); // Красный
    rgbaBuffer[i * 4 + 1] = (g << 3) | (g >> 2); // Зеленый
    rgbaBuffer[i * 4 + 2] = (b << 3) | (b >> 2); // Синий
    rgbaBuffer[i * 4 + 3] = 255;                 // Альфа (прозрачность)
  }

  return rgbaBuffer;
}

