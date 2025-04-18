#include "socket.h"
#include "panels.h"
#include <cstring>
#include <ifaddrs.h>
#include <iostream>
#include <net/if.h>
#include <thread>

Socket::Socket(LayerSystem &layer_, int panel_type_, std::string ifname) : layerSystem(layer_), panel_type(panel_type_) {
  std::cout << "[Socket] Start init, panel_type_=" << panel_type_ << std::endl;

  do {
    network_status = init(ifname);
  } while (network_status < 0);

  switch (panel_type) {
  case MDV1:
    first_identif_panel = 0x50;
    second_identif_panel = 0x13;

    first_byte_for_cbu_msg = 0xfa;
    second_byte_for_cbu_msg = 0xfc;
    break;
  case MDV2_MDN:
    first_identif_panel = 0x50;
    second_identif_panel = 0x20;

    first_byte_for_cbu_msg = 0xfe;
    second_byte_for_cbu_msg = 0xfe;
    break;
  case MDV3:
    first_identif_panel = 0x50;
    second_identif_panel = 0x12;

    first_byte_for_cbu_msg = 0xfa;
    second_byte_for_cbu_msg = 0xfa;
    break;

  case IDGK:
    first_identif_panel = 0x40;
    second_identif_panel = 0x12;
    break;

  default: // если всё плохо
    first_identif_panel = 0x50;
    second_identif_panel = 0x13;

    first_byte_for_cbu_msg = 0xfa;
    second_byte_for_cbu_msg = 0xfc;
    break;
  }

  uint8_t base_cbu_msg[14] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x80, 0xc8, 0xaa, 0xbb, 0xcc, 0x08, 0x03};
  memcpy(cbu_msg, base_cbu_msg, sizeof(base_cbu_msg));

  cbu_msg[14] = first_byte_for_cbu_msg;
  cbu_msg[15] = 1;
  cbu_msg[16] = second_byte_for_cbu_msg;

  crc32_init();

  std::cout << "[Socket] Done init" << std::endl;
}

void Socket::start() {
  std::thread network_thr(&Socket::listenPackets, this);
  network_thr.detach();
}

Socket::~Socket() { close(sockfd); }

int Socket::init(std::string ifname) {
  if ((sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) == -1) {
    return -1;
  }

  memset(&ifr, 0, sizeof(ifr));
  memset(&sll, 0, sizeof(sll));

  const char *ifname_char = ifname.c_str();

  strncpy(ifr.ifr_name, ifname_char, IFNAMSIZ);

  if (ioctl(sockfd, SIOCGIFFLAGS, &ifr) == -1) {
    perror("ioctl");
    close(sockfd);
    return -1;
  }

  ifr.ifr_flags |= IFF_PROMISC;

  if (ioctl(sockfd, SIOCSIFFLAGS, &ifr) == -1) {
    perror("ioctl SIOCSIFFLAGS");
    close(sockfd);
    return -1;
  }

  if ((ioctl(sockfd, SIOCGIFINDEX, &ifr)) == -1) {
    std::cout << "Error getting interface index" << std::endl;
    close(sockfd);
    return -1;
  }

  sll.sll_family = AF_PACKET;
  sll.sll_ifindex = ifr.ifr_ifindex;
  sll.sll_protocol = htons(ETH_P_ALL);

  if ((bind(sockfd, (struct sockaddr *)&sll, sizeof(sll))) == -1) {
    std::cout << "bind error" << std::endl;
    close(sockfd);
    return -2;
  }
  std::cout << "Выбрано iface:" << " " << ifname << std::endl;
  return 0;
}

int Socket::listenPackets() {
  unsigned char buffer[1500]; // Буфер для получаемых пакетов, 1500 - ограничение в RAW пакетах
  ssize_t numBytes;

  uint8_t layout = -1;
  int mode = wait_scenario;

  Layer layer;

  Layer carnumLayer;
  Layer timeLayer;

  bool status_eth_linkup = false;
  bool need_ask_cbu = true;

  std::chrono::steady_clock::time_point prevCountUpdateTime = std::chrono::steady_clock::now();
  std::chrono::steady_clock::time_point currCountUpdateTime;

  while (true) {
    status_eth_linkup = is_socket_open();

    if (status_eth_linkup == false) { // link down
      need_ask_cbu = true;
      std::this_thread::sleep_for(std::chrono::milliseconds(400));
      continue;
    }

    if (status_eth_linkup == true && need_ask_cbu == true) { // link up, ask cbu new data
      ask_cbu_new_data();
      need_ask_cbu = false;
    }

    numBytes = recvfrom(sockfd, buffer, sizeof(buffer), 0, NULL, NULL);
    if (numBytes < 0) {
      std::cout << "[listenPackets] recvfrom err" << std::endl;
      continue;
    }

    switch (mode) {
    case wait_scenario:
      if ((buffer[first_identif_panel_location] == first_identif_panel && buffer[second_identif_panel_location] == second_identif_panel) &&
          buffer[scenario_id_location] == scenario_id) {

        if (!check_crc(buffer, numBytes)) {
          count_packet_error_main++;
          continue;
        }

        count_packet_scen++;

        // Извлечение и вывод layout
        layout = buffer[scen_layout_location];
        layer.set_layout(layout);

        // Извлечение и вывод ширины (widths)
        uint32_t width = (buffer[scen_width_1_location] << 16) | (buffer[scen_width_2_location] << 8) | buffer[scen_width_3_location];
        layer.set_width(width);

        // Извлечение и вывод высоты (heaght)
        uint8_t height = buffer[scen_height_location];
        layer.set_height(height);

        // Извлечение и вывод количества пакетов (count_packet)
        uint32_t count_packet = (buffer[scen_count_packet_1_location] << 16) | (buffer[scen_count_packet_2_location] << 8) | buffer[scen_count_packet_3_location];
        layer.set_count_packets(count_packet);

        // Извлечение и вывод fromX
        int16_t fromX = (buffer[scen_msb_fromX_location] << 8) | buffer[scen_lsb_fromX_location];
        if (buffer[scen_byte_znak_fromX_location] == 1) {
          fromX = -fromX;
        }

        layer.set_fromX(fromX);

        // Извлечение и вывод fromY
        int16_t fromY = (buffer[scen_msb_fromY_location] << 8) | buffer[scen_lsb_fromY_location];
        if (buffer[scen_byte_znak_fromY_location] == 1) {
          fromY = -fromY;
        }

        layer.set_fromY(fromY);

        // Извлечение и вывод toX
        int16_t toX = (buffer[scen_msb_toX_location] << 8) | buffer[scen_lsb_toX_location];
        if (buffer[scen_byte_znak_toX_location] == 1) {
          toX = -toX;
        }

        layer.set_toX(toX);

        // Извлечение и вывод toY
        int16_t toY = (buffer[scen_msb_toY_location] << 8) | buffer[scen_lsb_toY_location];
        if (buffer[scen_byte_znak_toY_location] == 1) {
          toY = -toY;
        }

        layer.set_toY(toY);

        printf("[SCENARIO for frame=%d] Layout=%d Width=%d Height=%d CountPackets=%d fromX=%d fromY=%d toX=%d toY=%d\n", layout, layout, width, height, count_packet, fromX, fromY,
               toX, toY);

        // момент, когда отрыгивает отрисовка
        //[SCENARIO for frame=2] Layout=2 Width=4 Height=2 CountPackets=1 fromX=0 fromY=0 toX=0 toY=0
        //[SCENARIO for frame=2] Layout=2 Width=4 Height=2 CountPackets=1 fromX=0 fromY=0 toX=0 toY=0

        if (width == 4 && height == 2) {
          // std::cout<<"ACHTUNG!!!";
          continue;
        }

        layer.init_buffer();

        mode = wait_frame;
      }
      break;

    case wait_frame:
      if ((buffer[first_identif_panel_location] == first_identif_panel && buffer[second_identif_panel_location] == second_identif_panel) && buffer[25] != scenario_id) {

        // counting_packets(panel_type, mode);

        if (!check_crc(buffer, numBytes)) {
          continue;
          count_packet_error_main++;
        }

        count_packet_frame++;

        int16_t partnums = (buffer[45] << 8) | buffer[39];

        if (received_partnum == partnums) {
          printf("[Frame %d] Received bytes: %zd partnums=%d received_partnum=%d\n", layer.layout, numBytes, partnums, received_partnum);

          int xfrom = 4 * received_partnum;
          int xto = 4 * (received_partnum + 1);

          unpack_and_store_pixels(buffer, frame_data_index, xfrom, xto, 0, layer.height, layer);

          received_partnum++;

          if (received_partnum >= layer.count_packets) {
            auto existLayout = layerSystem.getLayer(layer.layout);

            if (existLayout != nullptr) {
              if (existLayout->buffer != layer.buffer) { // буферы не совпадают, пересохраняем слой
                layerSystem.replaceLayer(layer, *existLayout);
                printf("[Frame %d] resaved with new data\n", layer.layout);
              } else {
                printf("[Frame %d] buffer equivalent\n", layer.layout);
              }
            } else {
              layerSystem.addLayer(layer);
              printf("[Frame %d] first add to layer\n", layer.layout);
            }

            received_partnum = 0;
            mode = wait_scenario;
          }
        }
      }
      break;
    }

    if ((buffer[first_identif_panel_location] == first_identif_panel && buffer[second_identif_panel_location] == second_identif_panel) // check panel
        && ((buffer[time_and_carnum_id_location] == time_id_packet) || buffer[time_and_carnum_id_location] == carnum_id_packet)) {     // check carnum or time

      if (!check_crc(buffer, numBytes)) {
        count_packet_error_main++;
        continue;
      }
      Layer *timeCarnumLayer = nullptr;

      // переключение между layout номера вагона и времени
      if (buffer[time_and_carnum_id_location] == time_id_packet) {
        timeCarnumLayer = &timeLayer;
        timeCarnumLayer->set_layout(layerSystem.timeLayerID); // layout для time_id_packet, 10 - чтобы рисовался выше всех слоёв
        count_packet_carnum++;
      } else if (buffer[time_and_carnum_id_location] == carnum_id_packet) {
        timeCarnumLayer = &carnumLayer;
        timeCarnumLayer->set_layout(layerSystem.carNumLayerID); // layout для carnum_id_packet, 20 - чтобы рисовался выше всех слоёв
        count_packet_time++;
      }

      timeCarnumLayer->set_width(buffer[time_and_carnum_id_location + 1]);
      timeCarnumLayer->set_height(buffer[time_and_carnum_id_location + 2]);
      timeCarnumLayer->set_fromX(buffer[time_and_carnum_id_location + 3]);
      timeCarnumLayer->set_fromY(buffer[time_and_carnum_id_location + 4]);
      timeCarnumLayer->set_toX(buffer[time_and_carnum_id_location + 5]);
      timeCarnumLayer->set_toY(buffer[time_and_carnum_id_location + 6]);
      timeCarnumLayer->set_count_packets(buffer[time_and_carnum_id_location + 7]);

      timeCarnumLayer->init_buffer();

      if (timeCarnumLayer->count_packets > 0) { // Несколько пакетов
        int16_t partnums = buffer[time_and_carnum_id_location + 8];

        if (received_partnumCarTime == partnums) {
          // printf("[Time & Carnum %d] Received bytes: %zd partnums=%d received_partnumCarTime=%d\n", timeCarnumLayer->layout, numBytes, partnums, received_partnumCarTime);

          int yfrom = buffer[time_and_carnum_id_location + 10] * received_partnumCarTime;
          int yto = buffer[time_and_carnum_id_location + 10] * (received_partnumCarTime + 1);

          unpack_and_store_pixels(buffer, from_data_index_date_and_carnum, 0, timeCarnumLayer->width, yfrom, yto, *timeCarnumLayer);

          received_partnumCarTime++;

          if (received_partnumCarTime >= timeCarnumLayer->count_packets) {
            auto existLayout = layerSystem.getLayer(timeCarnumLayer->layout);

            if (existLayout != nullptr) {
              if (existLayout->buffer != timeCarnumLayer->buffer) { // буферы не совпадают, пересохраняем слой
                layerSystem.replaceLayer(*timeCarnumLayer, *existLayout);
                // printf("[Time & Carnum %d] resaved with new data\n", timeCarnumLayer->layout);
              }
              // else {
              //  printf("[Time & Carnum %d] buffer equivalent\n", timeCarnumLayer->layout);
              //}
            } else {
              layerSystem.addLayer(*timeCarnumLayer);
              // printf("[Time & Carnum %d] first add to layer\n", timeCarnumLayer->layout);
            }

            received_partnumCarTime = 0;
          }
        }
      } else { // 1 пакет
        unpack_and_store_pixels(buffer, from_data_index_date_and_carnum, 0, timeCarnumLayer->width, 0, timeCarnumLayer->height, *timeCarnumLayer);

        auto existLayout = layerSystem.getLayer(timeCarnumLayer->layout);

        if (existLayout != nullptr) {
          if (existLayout->buffer != layer.buffer) { // буферы не совпадают, пересохраняем слой
            layerSystem.replaceLayer(*timeCarnumLayer, *existLayout);
            // printf("[Time & Carnum %d] resaved with new data\n", timeCarnumLayer.layout);
          }
        } else {
          layerSystem.addLayer(*timeCarnumLayer);
          // printf("[Time & Carnum %d] first add to layer\n", timeCarnumLayer.layout);
        }
      }
    }

    if (panel_type == IDGK && (buffer[first_identif_panel_location] == first_identif_panel && buffer[second_identif_panel_location] == second_identif_panel) // check panel
    ) { 
      printAsciiText(buffer); // Действия, если условия истинны
    }

    int prev_count_packet_main = count_packet_main;
    count_packet_main = count_packet_frame + count_packet_scen + count_packet_carnum + count_packet_time;
    int totalPackets;
    totalPackets = count_packet_main;

    if (count_packet_main > prev_count_packet_main) { // Изменился ли счетчик
      currCountUpdateTime = std::chrono::steady_clock::now();
      std::chrono::duration<double> packetInterval = currCountUpdateTime - prevCountUpdateTime;
      prevCountUpdateTime = currCountUpdateTime;
      ping = packetInterval.count();
    }
  }

  return 0;
}

std::string Socket::printAsciiText(const unsigned char *buffer) {
  size_t size = 1500;
  for (size_t i = 0; i < size; ++i) {
    unsigned char byte = buffer[i];

    // Если это ASCII символ (простой текст), добавляем его в строку
    if (byte >= 32 && byte <= 126) {
      extractedText += byte;
    }
    // Если встречаем символ новой строки (например, для конца строки)
    else if (byte == ',') {
      extractedText += '\n';
    }
  }

  return extractedText;
}

void Socket::unpack_and_store_pixels(unsigned char *buffer, int pixel_start_in_buffer, int xfrom, int xto, int yfrom, int yto, Layer &layer) {
  int data_index = pixel_start_in_buffer; // Начало данных в пакете (первый байт с данными)

  for (int row = yfrom; row < yto; row++) {        // Проходим по диапазону строк
    for (int col = xfrom; col < xto; col++) {      // Проходим по диапазону столбцов
      uint8_t packed_pixel = buffer[data_index++]; // Получаем запакованный пиксель
      uint16_t rgb555 = 0x0;

      // костыль для ESP32 тогда был =(
      // про распаковку см. в readme пункт #1
      // цветность у нас BGR (с ЦБУ)
      if (packed_pixel != 1) {
        // Извлекаем синий (3 старших бита)
        uint8_t b = (packed_pixel & 0b11100000) >> 5;
        b = (b << 2);

        // Извлекаем зеленый (3 средних бита)
        uint8_t g = (packed_pixel & 0b00011100) >> 2;
        g = (g << 3);

        // Извлекаем красный (2 младших бита)
        uint8_t r = (packed_pixel & 0b00000011);
        r = (r << 3);

        rgb555 = (r << 11) | (g << 5) | b;
      }

      // Проверка на выход за границы буфера
      if (row < layer.height && col < layer.width) {
        int bufferIndex = (row * layer.width + col) * 2;
        layer.buffer[bufferIndex] = rgb555 & 0xFF;            // Младший байт
        layer.buffer[bufferIndex + 1] = (rgb555 >> 8) & 0xFF; // Старший байт
      }
    }
  }
}

void Socket::ask_cbu_new_data() {
  std::cout << "[Socket] Start ask CBU new data" << std::endl;
  static ssize_t sender_cbu_bytes = -1;
  while (sender_cbu_bytes <= 0) {
    // if (is_socket_open()) {
    sender_cbu_bytes = sendPacket(cbu_msg, sizeof(cbu_msg));
    // }

    std::this_thread::sleep_for(std::chrono::milliseconds(300));
  }
  std::cout << "[Socket] Done ask CBU new data" << std::endl;
}

int Socket::sendPacket(unsigned char *packet, size_t length) {
  ssize_t numBytes = sendto(sockfd, packet, length, 0, (struct sockaddr *)&sll, sizeof(sll));
  if (numBytes < 0) {
    std::cout << "[Socket::sendPacket] sendto error, numBytes=" << numBytes << std::endl;
    return -1;
  }

  return numBytes;
}

bool Socket::is_socket_open() {
  // std::cout<<"4"<<std::endl;
  if (ioctl(sockfd, SIOCGIFFLAGS, &ifr) < 0) {
    std::cout << "[Socket::is_socket_open] SIOCGIFFLAGS" << std::endl;
    return false;
  }

  return !!(ifr.ifr_flags & IFF_UP);
}

void Socket::crc32_init() {
  for (uint32_t i = 0; i < 256; i++) {
    uint32_t crc = i;
    for (int j = 0; j < 8; j++) {
      crc = (crc & 1) ? (crc >> 1) ^ CRC_POLY : (crc >> 1);
    }
    crc32_table[i] = crc;
  }
}

uint32_t Socket::crc32_calc(uint8_t *data, int len) {
  if (!data || len < 1)
    return 0;

  uint32_t crc = 0xFFFFFFFF;

  for (int j = 0; j < len; j++) {
    uint8_t byte = data[j];
    crc = (crc >> 8) ^ crc32_table[(crc ^ byte) & 0xFF];
  }

  return (crc ^ 0xFFFFFFFF);
}

bool Socket::check_crc(unsigned char *buffer, int numBytes) {
  if (!buffer || numBytes < 4) {
    // std::cout << "[CRC32] Invalid buffer or length" << std::endl;
    return false; // Неверный буфер или недостаточная длина
  }

  uint32_t calculated_crc = crc32_calc(buffer, numBytes - 4);
  uint32_t packet_crc = (buffer[numBytes - 4] << 24) | (buffer[numBytes - 3] << 16) | (buffer[numBytes - 2] << 8) | (buffer[numBytes - 1]);

  if (!compare_crc(calculated_crc, packet_crc)) {
    // std::cout << "[CRC32] Bad CRC" << std::endl;
    return false;
  }

  return true;
}
