#pragma once

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>
#include <iostream>
#include <vector>
#include <sstream>
#include <fstream> 
#include "../utils.h"

#include "../upvs/upvs_data.h"

#define BUEVG_PACKET_ID 0xb00eff01
#define BUEV_PACKET_VERSION 1

class Data {
   private:
   public:
    typedef struct {
        uint32_t id;
        uint32_t version;
        uint32_t build;
        uint32_t records_count;
        uint32_t serial_number;
    } __attribute__((packed)) buev_packet_header_t;

    typedef struct {
        uint32_t tick;
        float Vph1;
        float Vph2;
        float Vph3;
        float Agen;
        float Agen_mV;
        float Acharge;
        float Acharge_mV;
        float Awinding;
        float Vwinding;
        float Vacc;
        float VGen;
        float V100V;
        float key;
        float temperature;
        uint32_t speed;
        uint32_t perekos;
        uint32_t oborot;
        uint32_t dip;
        uint32_t led;
        uint32_t status;
        uint32_t alarm;
        uint32_t charge_state;
        float Vrpn1;
        float Vrpn2;
        float Vrpnm;
    } buev_data_t;

    typedef struct param_t {
        uint32_t number;
        uint32_t time;
        int16_t amp_gen;
        int16_t amp_gen_mV;
        int16_t amp_winding;
        int16_t amp_charge;
        int16_t amp_charge_mV;
        uint16_t nap_winding;
        int16_t nap_faza1;
        int16_t nap_faza2;
        int16_t nap_faza3;
        uint16_t nap_akk;
        uint16_t nap_gen;
        uint16_t nap_100v;
        uint16_t nap_rpn1;
        uint16_t nap_rpn2;
        uint16_t nap_rpnm;
        int16_t temp_akk;
        int16_t add_temp_akk;
        uint16_t oborot;
        uint8_t speed;
        uint8_t dip;
        uint8_t charge_stage;
        uint32_t led;
        uint32_t status;
        uint16_t alarm;
        uint16_t perekos;
        uint32_t buevTimeWork;
        uint32_t genTimeWork;
    } __attribute__((packed)) buevg_record_t;

    std::vector<buevg_record_t> records_;
    static constexpr uint32_t MAX_HISTORY_MASK = 0xffffff;

    typedef struct {
        buev_data_t data[MAX_HISTORY_MASK + 1];
        uint32_t next_idx;
    } cyclic_buev_data_t;

    cyclic_buev_data_t history_;  // Циклический буфер для хранения данных
    buev_data_t maximums_;
    buev_data_t bd;
    const buev_packet_header_t *header;

    bool parsePacket(uint8_t *buffer, size_t length);
    void processRecords(const std::vector<buevg_record_t> &records);
    void rst_maximums();

    const std::vector<buevg_record_t> &getRecords() const {
        return records_;
    }
    const cyclic_buev_data_t &getHistory() const {
        return history_;
    }
    const buev_data_t &getMaximums() const {
        return maximums_;
    }

    std::string read_file(const std::string &filename);
    void parse_response(const std::string &response, upvs_data &data);
    float get_u(float u);
};
extern Data data_obj;