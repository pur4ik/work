#ifndef SOCKET_H
#define SOCKET_H

#include <arpa/inet.h>
#include <atomic>
#include <ifaddrs.h>
#include <iostream>
#include <linux/if_packet.h>
#include <net/if.h>
#include <netinet/ether.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

#include "layer.h"

#define CRC_POLY 0xEDB88320

enum TabloType {
  MDV1 = 0,     // MDV1
  MDV2_MDN = 1, // MDV2 & MDN
  MDV3 = 2,     // MDV3
  IDGK = 3,     //ЖК панель
};

enum work_mod_connection { wait_scenario = 1, wait_frame = 2 };

class Socket {
public:
  Socket(LayerSystem &layer_, int panel_type_, std::string iface_);
  ~Socket();

  std::vector<std::string> getNetworkInterfaces();
  void start();
  int init(std::string ifname);
  int sendPacket(unsigned char *packet, size_t length);
  void close_sock() { close(sockfd); }
  bool is_socket_open();

  std::string printAsciiText(const unsigned char *buffer);

  int listenPackets();
  std::atomic<int> network_status = 0;

  void ask_cbu_new_data();

  // Таблица CRC
  uint32_t crc32_table[256];
  void crc32_init();
  uint32_t crc32_calc(uint8_t *data, int len);
  bool compare_crc(uint32_t cbu_crc, uint32_t calculated_crc) { return cbu_crc == calculated_crc ? true : false; }
  bool check_crc(unsigned char *buffer, int numBytes);

  int count_packet_main = 0; // общее
  int count_packet_scen = 0;
  int count_packet_frame = 0;
  int count_packet_error_main = 0;
  int count_packet_error_scen = 0;
  int count_packet_error_frame = 0;
  int count_packet_carnum = 0;
  int count_packet_time = 0;
  int ping = 0;
  std::string extractedText;

  int main_counter = 0;

private:
  LayerSystem &layerSystem;
  void unpack_and_store_pixels(unsigned char *buffer, int pixel_start_in_buffer, int xfrom, int xto, int yfrom, int yto, Layer &layer);
  int panel_type;
  int sockfd;
  struct ifreq ifr;
  struct sockaddr_ll sll;

  // api CBU
  int first_identif_panel;
  int second_identif_panel;

  int first_byte_for_cbu_msg;
  int second_byte_for_cbu_msg;
  uint8_t cbu_msg[17];

  const int first_identif_panel_location = 42;
  const int second_identif_panel_location = 43;

  // where byte is placed in packets
  const int scenario_id = 0x39; // идентификатор сценария
  const int scenario_id_location = 25;

  // scenario packet
  const int scen_layout_location = 15;

  const int scen_width_1_location = 16;
  const int scen_width_2_location = 17;
  const int scen_width_3_location = 18;

  const int scen_height_location = 19;

  const int scen_count_packet_1_location = 24;
  const int scen_count_packet_2_location = 47;
  const int scen_count_packet_3_location = 48;

  const int scen_lsb_fromX_location = 26;
  const int scen_msb_fromX_location = 27;
  const int scen_byte_znak_fromX_location = 28;

  const int scen_lsb_fromY_location = 29;
  const int scen_msb_fromY_location = 30;
  const int scen_byte_znak_fromY_location = 31;

  const int scen_lsb_toX_location = 32;
  const int scen_msb_toX_location = 33;
  const int scen_byte_znak_toX_location = 34;

  const int scen_lsb_toY_location = 35;
  const int scen_msb_toY_location = 36;
  const int scen_byte_znak_toY_location = 37;

  // frame packet
  const int frame_data_index = 48; // откуда начинаются данные (фрейм)

  // time and carnum packet
  const int from_data_index_date_and_carnum = 45; // откуда начинаются данные (фрейм)
  const int time_and_carnum_id_location = 14;     // где лежит индентификатор пакета
  const int time_id_packet = 5;                   // идентификатор пакета со временем
  const int carnum_id_packet = 8;                 // идентификатор пакета с номером вагона

  int received_partnum = 0;
  int received_partnumCarTime = 0;
};

#endif