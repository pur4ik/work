#include "data.h"

#include "triggers.h"

Data data_obj;

bool Data::parsePacket(uint8_t* buffer, size_t length) {
    // Проверяем минимальную длину пакета
    if (length < sizeof(buev_packet_header_t)) {
        std::cerr << "ERROR: Received packet is too short!" << std::endl;
        return false;
    }

    // Извлекаем заголовок пакета
    header = reinterpret_cast<const buev_packet_header_t*>(buffer);

    // Проверяем корректность заголовка
    if (header->id != BUEVG_PACKET_ID || header->version != BUEV_PACKET_VERSION) {  // Предполагаемые значения ID и версии
        std::cerr << "ERROR: Invalid packet header!" << std::endl;
        return false;
    }

    // Вычисляем количество записей
    size_t records_count = header->records_count;
    size_t expected_size = sizeof(buev_packet_header_t) + records_count * sizeof(buevg_record_t);
    if (length < expected_size) {
        std::cerr << "ERROR: Received packet is incomplete!" << std::endl;
        return false;
    }

    // Извлекаем записи
    const buevg_record_t* records = reinterpret_cast<const buevg_record_t*>(buffer + sizeof(buev_packet_header_t));
    records_.clear();
    for (size_t i = 0; i < records_count; ++i) {
        records_.push_back(records[i]);
    }

    processRecords(records_);

    return true;
}

void Data::processRecords(const std::vector<buevg_record_t>& records) {
    for (const auto& rec : records) {
        // Получаем указатель на следующую свободную ячейку в циклическом буфере
        // std::cout << "time =" << rec.time << "\n";
        // Преобразуем данные из buevg_record_t в buev_data_t
        bd.tick = rec.time;
        bd.Agen = rec.amp_gen / 10.0;
        bd.Agen_mV = rec.amp_gen_mV / 100.0;
        bd.Acharge = rec.amp_charge / 10.0;
        bd.Acharge_mV = rec.amp_charge_mV / 100.0;
        bd.Awinding = rec.amp_winding / 1000.0;
        bd.Vwinding = rec.nap_winding / 100.0;
        bd.Vph1 = rec.nap_faza1 / 100.0;
        bd.Vph2 = rec.nap_faza2 / 100.0;
        bd.Vph3 = rec.nap_faza3 / 100.0;
        bd.Vacc = rec.nap_akk / 100.0;
        bd.VGen = rec.nap_gen / 100.0;
        bd.V100V = rec.nap_100v / 100.0;
        bd.key = ((rec.status & 0x0080) ? 1 : 0) * 90.0;  // STATUS_KEY = 0x0080
        bd.temperature = rec.temp_akk / 10.0;
        bd.speed = rec.speed;
        bd.perekos = rec.perekos;
        bd.oborot = rec.oborot;
        bd.dip = rec.dip;
        bd.led = rec.led;
        bd.status = rec.status;
        bd.alarm = rec.alarm;
        bd.charge_state = rec.charge_stage;

        bd.Vrpn1 = rec.nap_rpn1 / 100.0;
        bd.Vrpn2 = rec.nap_rpn2 / 100.0;
        bd.Vrpnm = rec.nap_rpnm / 100.0;

        // Обновляем индекс следующей записи
        history_.next_idx = (history_.next_idx + 1) & MAX_HISTORY_MASK;

        // Обновляем максимальные значения
        if (bd.Agen > maximums_.Agen) maximums_.Agen = bd.Agen;
        if (bd.Acharge > maximums_.Acharge) maximums_.Acharge = bd.Acharge;
        if (bd.Awinding > maximums_.Awinding) maximums_.Awinding = bd.Awinding;
        if (bd.Vwinding > maximums_.Vwinding) maximums_.Vwinding = bd.Vwinding;
        if (bd.Vph1 > maximums_.Vph1) maximums_.Vph1 = bd.Vph1;
        if (bd.Vph2 > maximums_.Vph2) maximums_.Vph2 = bd.Vph2;
        if (bd.Vph3 > maximums_.Vph3) maximums_.Vph3 = bd.Vph3;
        if (bd.Vacc > maximums_.Vacc) maximums_.Vacc = bd.Vacc;
        if (bd.V100V > maximums_.V100V) maximums_.V100V = bd.V100V;

        trig.check_triggers(bd);
    }
}

void Data::rst_maximums() {
    maximums_.Agen = 0;
    maximums_.Acharge = 0;
    maximums_.Awinding = 0;
    maximums_.Vwinding = 0;
    maximums_.Vph1 = 0;
    maximums_.Vph2 = 0;
    maximums_.Vph3 = 0;
    maximums_.Vacc = 0;
    maximums_.V100V = 0;
}

std::string Data::read_file(const std::string& filename) {
    std::ifstream infile(filename);
    if (!infile.is_open()) {
        std::cerr << "Не удалось открыть файл: " << filename << std::endl;
        return "";
    }

    std::string firstLine;
    std::getline(infile, firstLine);  // Читаем первую строку

    // Копируем оставшийся контент во временный файл
    std::ostringstream tempContent;
    std::string line;
    while (std::getline(infile, line)) {
        tempContent << line << "\n";
    }
    infile.close();

    // Записываем временный контент обратно в исходный файл
    std::ofstream outfile(filename, std::ios::trunc);
    if (outfile.is_open()) {
        outfile << tempContent.str();
        outfile.close();
    } else {
        std::cerr << "Не удалось открыть файл для записи: " << filename << std::endl;
    }

    // std::cout<<"считано" << firstLine << "\n";

    return firstLine;
}

void Data::parse_response(const std::string& response, upvs_data& data) {
    std::istringstream stream(response);  // Создаём поток из строки
    std::string line;

    // Вспомогательная функция для извлечения значения между <value>...</value>
    auto extractValue = [&stream]() -> std::string {
        std::string buffer;
        size_t start = buffer.find("<value>");
        size_t end = buffer.find("</value>");
        if (start != std::string::npos && end != std::string::npos) {
            return buffer.substr(start + 7, end - start - 7);  // 7 = длина "<value>"
        }
        return "";
    };

    // Вспомогательная функция для безопасного преобразования строк в числа
    auto safe_stod = [](const std::string& value, const std::string& fieldName) -> double {
        try {
            return std::stod(value);
        } catch (const std::invalid_argument& e) {
            std::cerr << "Ошибка преобразования '" << value << "' для поля " << fieldName << ": " << e.what() << std::endl;
            return 0.0;
        } catch (const std::out_of_range& e) {
            std::cerr << "Значение '" << value << "' для поля " << fieldName << " вне допустимого диапазона: " << e.what() << std::endl;
            return 0.0;
        }
    };
    auto safe_stoi = [](const std::string& value, const std::string& fieldName) -> int {
        try {
            return std::stoi(value);
        } catch (const std::invalid_argument& e) {
            std::cerr << "Ошибка преобразования '" << value << "' для поля " << fieldName << ": " << e.what() << std::endl;
            return 0;
        } catch (const std::out_of_range& e) {
            std::cerr << "Значение '" << value << "' для поля " << fieldName << " вне допустимого диапазона: " << e.what() << std::endl;
            return 0;
        }
    };
    try {
        std::string buffer = response;  // Копируем всю строку для обработки
        data.POWER_GENERATOR = safe_stod(util.extract_tag(buffer, "value"), "POWER_GENERATOR");
        data.U_GENERATOR = safe_stod(util.extract_tag(buffer, "value"), "U_GENERATOR");
        data.U_WINDING_GENERATOR = safe_stod(util.extract_tag(buffer, "value"), "U_WINDING_GENERATOR");
        data.I_WINDING_GENERATOR = safe_stod(util.extract_tag(buffer, "value"), "I_WINDING_GENERATOR");
        data.SPEED_BY_GENERATOR = safe_stod(util.extract_tag(buffer, "value"), "SPEED_BY_GENERATOR");
        data.GENERATOR_SHAFT_ROTATION_SPEED = safe_stoi(util.extract_tag(buffer, "value"), "GENERATOR_SHAFT_ROTATION_SPEED");
        data.U_110 = safe_stod(util.extract_tag(buffer, "value"), "U_110");
        data.I_110 = safe_stod(util.extract_tag(buffer, "value"), "I_110");
        data.I_BATTERY = safe_stod(util.extract_tag(buffer, "value"), "I_BATTERY");
        data.BATTERY_CHARGE_STAGE = safe_stoi(util.extract_tag(buffer, "value"), "BATTERY_CHARGE_STAGE");
        data.BATTERY_VOLTAGE = safe_stod(util.extract_tag(buffer, "value"), "BATTERY_VOLTAGE");
        data.TEMP_BATTERY = safe_stod(util.extract_tag(buffer, "value"), "TEMP_BATTERY");

        data.STATUS = util.extract_tag(buffer, "value");
        data.RCH = util.extract_tag(buffer, "value");
        data.RMT_Z = util.extract_tag(buffer, "value");
        data.RMT_R = util.extract_tag(buffer, "value");
        data.BATTERY_CIRCUIT_BREAKAGE = util.extract_tag(buffer, "value");
        data.RPN1 = util.extract_tag(buffer, "value");
        data.RPN2 = util.extract_tag(buffer, "value");

        data.TYPE_AKK = util.extract_tag(buffer, "value");
        data.TYPE_GEN = util.extract_tag(buffer, "value");
    } catch (const std::exception& e) {
        std::cerr << "Ошибка при парсинге ответа: " << e.what() << std::endl;
    }
    
}
