#ifndef LAYER_H
#define LAYER_H

#include <stdint.h>
#include <vector>
#include <iostream>
#include <algorithm>

struct Layer {
    int layout;
    uint32_t width;
    uint32_t height;
    int16_t fromX;        // Текущая позиция X
    int16_t initialFromX; // Начальная позиция X
    int16_t fromY;        // Текущая позиция Y
    int16_t initialFromY; // Начальная позиция Y
    int16_t toX;
    int16_t toY;
    uint32_t count_packets;
    std::vector<uint8_t> buffer;

    // костыли которые тянутся с ESP32... 
    int fixWidth = -1;
    int fixHeight = -1;

    void set_layout(int layout_) { layout = layout_; };
    void set_width(uint32_t width_) { width = width_; };
    void set_height(uint32_t height_) { height = height_; };
    void set_fromX(uint32_t fromX_) { fromX = fromX_; initialFromX = fromX_; };
    void set_fromY(uint32_t fromY_) { fromY = fromY_; initialFromY = fromY_; };
    void set_toX(uint32_t toX_) { toX = toX_; };
    void set_toY(uint32_t toY_) { toY = toY_; };
    void set_count_packets(uint32_t count_packets_) { count_packets = count_packets_; };
    void init_buffer() { buffer.resize(width * height * 2, 0); };
};

class LayerSystem {
public:
    LayerSystem(){};
    ~LayerSystem(){};

    void addLayer(const Layer layer) { 
        layers.emplace_back(layer); 
        sortSignalsByLayout();
    }

    Layer* getLayer(int layout) {
        for (auto& layer : layers) {
            if (layer.layout == layout) {
                return &layer;
            }
        }
        return nullptr;
    }

    void sortSignalsByLayout() {
        std::sort(layers.begin(), layers.end(), [](const Layer& a, const Layer& b) {
            return a.layout < b.layout; // Чем меньше значение, тем выше приоритет
        });
    }

    std::vector<Layer> &getAllLayer() { return layers; };
    void clearLayers() { layers.clear(); }

    void replaceLayer(const Layer& from, Layer& to) {
        to.layout = from.layout;
        to.width = from.width;
        to.height = from.height;
        to.fromX = from.fromX;
        to.initialFromX = from.initialFromX;
        to.fromY = from.fromY;
        to.initialFromY = from.initialFromY;
        to.toX = from.toX;
        to.toY = from.toY;
        to.count_packets = from.count_packets;
        to.buffer = from.buffer;
    }

    uint16_t pixel(const Layer &layer, int x, int y) const {
        if (x >= 0 && x < layer.width && y >= 0 && y < layer.height) {
            int bufferIndex = (y * layer.width + x) * 2;

            uint16_t lowByte = layer.buffer[bufferIndex];      // Младший байт
            uint16_t highByte = layer.buffer[bufferIndex + 1]; // Старший байт

            return (highByte << 8) | lowByte;
        }

        return 0;
    }


    void dump(const Layer &layer) const {
        std::cout << "[layout " << layer.layout << "] SIZE=" << layer.width << "x" << layer.height << " allLayserCount=" << layers.size() << std::endl;
        std::cout << "-----------------" << std::endl;
        std::cout << "+";
        for (int i = 0; i < layer.width; i++) {
            std::cout << "-";
        }
        std::cout << "+" << std::endl;

        for (int j = 0; j < layer.height; j++) {
            std::cout << "|";
            for (int i = 0; i < layer.width; i++) {
                uint16_t pixelValue = pixel(layer, i, j); 
                
                if (pixelValue == 0x0000) {
                    std::cout << ".";
                } else {
                    std::cout << "#"; 
                }
            }
            std::cout << "|";
            std::cout << std::endl;
        }
        std::cout << "+";
        for (int i = 0; i < layer.width; i++) {
            std::cout << "-";
        }
        std::cout << "+" << std::endl;
    }

    static const int carNumLayerID = 20;
    static const int timeLayerID = 10;

private:
    std::vector<Layer> layers;
};


#endif
