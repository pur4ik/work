// to do:
//  0 - убедиться в корректности работы мм
//  0 - проверить корректность взаимодействия бп и мм
//  2 - проверить исправность монитора и спо
//  3 - оценить частоту обновления данных спо и монитора
//  4 - проверить те ли параметры (аб)

#include <GLFW/glfw3.h>
#include <modbus/modbus.h>

#include <atomic>
#include <cmath>
#include <condition_variable>
#include <csignal>
#include <fstream>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
// #include "buev_uart.h"
#include "bp/bp.h"
#include "freq/freq_converter.h"
#include "load/socket.h"
#include "modbus.h"
#include "tests/psi_results.h"
#include "utils.h"
// #include "bp/uart.h"

#include "buev/buev_tcp.h"
#include "buev/buev_udp.h"
// #include "buev/buev_uart.h"
#include "buev/data.h"
#include "buev/led.h"
#include "buev/triggers.h"
// #include "mm/mm.h"
#include "tests/psi.h"
#include "upvs/netcat.h"
#include "upvs/upvs.h"
// #include "upvs/upvs_data.h"

upvs_data data;  // структура для упвс
psi_results results;
std::string status = " ";
bool on = true;
static int selectedIndex = -1;
std::vector<std::string> keys;
std::vector<std::string> file_names;
std::string SCENARIO;
bool run = false;
bool zeroing = true;
bool analog = true;
bool debug = false;
bool buev7 = false;
bool buev8 = false;
int selected_buevg_model = -1;
// int count = 0;
float km;
float hz;
float floatValue = 0;
bool stop = false;
float num = 0;
int work_time;
std::string filename = "../../work_time.txt";

int work_time_mins;
int work_time_hours;
int work_time_secs;

// IN WORK
int bm_ping = -1;

bool showGreenLedPopup = false;
bool showYellowLedPopup = false;
std::string green_250;
std::string yellow_250;

float build;

bool fail_224 = false;
bool exit_button = false;
bool start_250;

bool ready = false;
bool power = false;
const char* AKK_TYPES[] = {"90KL-300P-0", "56PzV-385P-0", "56PzS-350P-0", "90KGL-300/375P-0", "90KL-300P-1", "56PzV-385P-1", "56PzS-350P-1", "90KGL-300/375P-1", "НЕИЗВЕСТНОmak"};

const char* GEN_TYPES[] = {"ГИВ-25.У1", "ЭГВ-08.У1", "ЭГВ-08.1.У1", "2ГВ13У1", "НЕИЗВЕСТНО"};

const ImU32 LED_GREEN = IM_COL32(0, 255, 0, 255);     // Зеленый
const ImU32 LED_RED = IM_COL32(255, 0, 0, 255);       // Красный
const ImU32 LED_YELLOW = IM_COL32(255, 255, 0, 255);  // Желтый
const ImU32 LED_GRAY = IM_COL32(128, 128, 128, 255);  // Серый

#define BM_BUFFER_SIZE 65536
uint8_t bm_buffer[BM_BUFFER_SIZE];
size_t bm_bytes_read;
bool bm_connection = true;
std::atomic<bool> exit_flag(false);

// const Data::buev_data_t* bd = nullptr;
const Data::buev_data_t* maximums = nullptr;
std::mutex bm_data_mutex;
bool should_update_data = true;

// Глобальные переменные для накопления данных
float sum_Agen = 0.0f;     // Сумма тока генератора
float sum_Acharge = 0.0f;  // Сумма тока аккумулятора
int count_Agen = 0;        // Количество записей для Agen
int count_Acharge = 0;     // Количество записей для Acharge

bool global_init = false;

const char* buevg_models[] = {"10", "20"};

// Переменные для хранения средних значений
float average_Agen = 0.0f;
float average_Acharge = 0.0f;

double result_260_1 = 0.0;
float result_260_2 = 0.0;
float result_260_3 = 0.0;
float result_225_1 = 0.0;

// IN WORK

void overwriteFileWithInt(std::string filename, int work_time) {
    // Открываем файл в режиме записи (очистка содержимого)
    std::ofstream file(filename, std::ios::trunc);
    if (!file.is_open()) {
        std::cerr << "Ошибка: не удалось открыть файл " << filename << std::endl;
        return;
    }

    // Записываем новое значение
    file << work_time;

    if (file.fail()) {
        std::cerr << "Ошибка: не удалось записать значение в файл" << std::endl;
    } else {
        // std::cout << "Значение " << work_time << " успешно записано в файл " << filename << std::endl;
    }

    file.close();
}

void glfw_window_close_callback(GLFWwindow* window) {
    test2.unprepare();
    glfwSetWindowShouldClose(window, GLFW_TRUE);  // Устанавливаем флаг закрытия
}

void signal_handler(int signal) {
    if (signal == SIGINT) {
        on = false;
        test2.unprepare();
    }
}

void update_timer() {
    // std::cout << "7"<<std::endl;
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Ошибка: не удалось открыть файл " << filename << std::endl;
    }

    file >> work_time;

    if (file.fail()) {  // Проверяем, успешно ли считано значение
        std::cerr << "Ошибка: не удалось считать значение из файла" << std::endl;
    }
    // std::cout << "8"<<std::endl;
}
/*
int gui() {
    if (!glfwInit()) {
        std::cerr << "Не удалось инициализировать GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Создаем окно с помощью GLFW
    GLFWwindow* window = glfwCreateWindow(1000, 560, "Регулировка Нагрузки", NULL, NULL);
    if (window == nullptr) {
        std::cerr << "Не удалось создать окно GLFW" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwSetWindowCloseCallback(window, glfw_window_close_callback);

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);  // Включить вертикальную синхронизацию

    // Инициализация ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    // Настройка бекендов
    if (!ImGui_ImplGlfw_InitForOpenGL(window, true)) {
        std::cerr << "Не удалось инициализировать ImGui для GLFW" << std::endl;
        glfwTerminate();
        return -1;
    }
    if (!ImGui_ImplOpenGL3_Init("#version 130")) {
        std::cerr << "Не удалось инициализировать ImGui для OpenGL" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Загрузка шрифта
    ImGuiIO& io = ImGui::GetIO();
    ImFont* font = io.Fonts->AddFontFromFileTTF("fonts/tilda-sans_medium.ttf", 19.0f, NULL, io.Fonts->GetGlyphRangesCyrillic());
    if (!font) {
        std::cerr << "Не удалось загрузить шрифт" << std::endl;
        glfwTerminate();
        return -1;
    }
    io.Fonts->Build();  // Построение шрифтов

    // Основной цикл GUI
    while (!glfwWindowShouldClose(window) && on) {
        glfwPollEvents();

        // Начало нового кадра ImGui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(1000, 560));

        // Логика интерфейса
        ImGui::Begin("Test", &on,
                     ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

        std::string log = util.get_log();
        static float lastStatusTime = 0.0f;
        static float work_timer = 0.0f;
        float currentTime = ImGui::GetTime();

        // логика внесения в файл каждую секунду
        if (((currentTime - work_timer) >= 1.0f) && test2.timer) {
            // std::cout << "таймер = " << work_timer << "текущее = " << currentTime << "\n";
            work_timer = currentTime;
            work_time += 1;
            overwriteFileWithInt(filename, work_time);
        }

        if ((currentTime - lastStatusTime) >= 1.0f) {
            // int params = test2.get_freq(false);
            // std::cout << "частота = " << params << std::endl;
            // km = params / 125;
            // hz = params / 100;
            lastStatusTime = currentTime;
            if (analog) {
                while (test2.ready()) {
                    test2.unprepare();
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }
                // util.take_logs("ЧП в аналоговом режиме");
            }
        }

        if (_socket.getState() == true) {
            ImGui::SetCursorPos(ImVec2(85, 45));
            ImGui::SetNextItemWidth(76);
            ImGui::InputFloat("##MyFloat", &num);
            ImGui::SameLine();
            if (ImGui::Button("+", ImVec2(38, 25))) {
                num += 1.0;
            }
            ImGui::SameLine();
            if (ImGui::Button("-", ImVec2(38, 25))) {
                num -= 1.0;
            }
            if (num > 100.0) {
                num = 100.0;
            } else if (num < 0) {
                num = 0.0;
            }
            ImGui::SameLine();
            if (ImGui::Button("Отправить", ImVec2(100, 25))) {
                char str[12] = {0};
                util.float_to_char_array(str, num);
                // printf("str=%s\n", str);
                if (_socket.send(str) == 0) {
                    _socket.listen_reply();
                }
                _socket.close_socket();
                _socket.init_socket();
            }
        } else {
            ImGui::SetCursorPos(ImVec2(155, 50));
            if (ImGui::Button("Переподключение", ImVec2(140, 30))) {
                _socket.close_socket();
                _socket.init_socket();
            }
        }

        ImGui::SetCursorPosY(100);
        ImGui::Separator();

        ImGui::SetCursorPos(ImVec2(0, 470));
        ImGui::BeginChild("LogWindow", ImVec2(1000, 90), true, ImGuiWindowFlags_HorizontalScrollbar);

        std::istringstream log_stream(log);
        std::string line;
        std::string error = "Ошибка";
        std::string success = "успешно";
        std::string interrupt = "прервано";

        // Получаем текущую вертикальную позицию скролла и максимальное значение скролла
        float scroll_y = ImGui::GetScrollY();
        float scroll_max_y = ImGui::GetScrollMaxY();

        bool should_scroll = scroll_y >= scroll_max_y;

        while (std::getline(log_stream, line)) {
            if ((line.find(error) != std::string::npos) || (line.find(interrupt) != std::string::npos)) {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
            } else if (line.find(success) != std::string::npos) {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
            }

            ImGui::TextUnformatted(line.c_str());

            if (line.find(error) != std::string::npos || line.find(success) != std::string::npos) {
                ImGui::PopStyleColor();
            }
        }

        if (should_scroll) {
            ImGui::SetScrollHereY(1.0f);
        }

        ImGui::EndChild();

        if (modbus) {
            ImGui::SetCursorPos(ImVec2(30, 130));
            if (ImGui::Checkbox("Аналоговый режим управления", &analog)) {
                if (analog) {
                    while (test2.unprepare() != 0) {
                        std::this_thread::sleep_for(std::chrono::milliseconds(100));  // хз насоклько хорошая практика так писать
                    }
                    util.take_logs("ЧП в аналоговом режиме");
                } else {
                    while (test2.prepare() != 0) {
                        std::this_thread::sleep_for(std::chrono::milliseconds(100));  // хз насоклько хорошая практика так писать
                    }
                    util.take_logs("ЧП в программном режиме");
                }
            }

            ImGui::BeginDisabled(analog);

            ImGui::SetCursorPos(ImVec2(250, 165));
            ImGui::TextUnformatted("Режим сценария");
            ImGui::SetNextItemWidth(175);
            ImGui::SetCursorPos(ImVec2(250, 200));
            if (ImGui::BeginCombo("##", selectedIndex == -1 ? "Выберите сценарий" : file_names[selectedIndex].c_str())) {
                for (int i = 0; i < file_names.size(); i++) {
                    if (ImGui::Selectable(file_names[i].c_str(), selectedIndex == i)) {
                        selectedIndex = i;
                        SCENARIO = keys[i];
                    }
                }
                ImGui::EndCombo();
            }

            ImGui::SetCursorPos(ImVec2(250, 240));
            if (ImGui::Checkbox("Старт с 0 ГЦ", &zeroing)) {
                // std::cout << "zeroing = " << zeroing << std::endl;
            }

            ImGui::BeginDisabled(selectedIndex == -1);
            if (run) {
                ImGui::SetCursorPos(ImVec2(250, 290));
                if (ImGui::Button("Прервать", ImVec2(175, 40))) {
                    run = false;
                    analog = true;
                    test2.alarm = true;
                }
            } else {
                ImGui::SetCursorPos(ImVec2(250, 290));
                if (ImGui::Button("Начать выполнение", ImVec2(175, 40))) {
                    run = true;
                }
            }
            ImGui::EndDisabled();

            // Ручной режим
            ImGui::SetCursorPos(ImVec2(30, 165));
            ImGui::TextUnformatted("Ручной режим");
            ImGui::BeginDisabled(run);
            ImGui::SetCursorPos(ImVec2(30, 200));
            ImGui::SetNextItemWidth(125);
            ImGui::InputFloat("ГЦ", &floatValue, 0.01f, 150.0f, "%.2f");  // шаг 0.01, максимальное значение 1.0f, отображение 3 знаков после запятой
            ImGui::SetCursorPos(ImVec2(30, 240));
            if (ImGui::Button("Выставить частоту", ImVec2(150, 35))) {
                modbus.write_param("3001", floatValue * 100, false);
                util.take_logs("Выставлена частота:" + std::to_string(floatValue) + " герц");
            }

            ImGui::SetCursorPos(ImVec2(30, 290));
            if (ImGui::Button("ПУСК", ImVec2(60, 40))) {
                if (test2.start() == 0) {
                    util.take_logs("ЧП запущен!");
                }
            }

            ImGui::EndDisabled();

            ImGui::SetCursorPos(ImVec2(120, 290));
            if (ImGui::Button("СТОП", ImVec2(60, 40))) {
                if (test2.stop() == 0) {
                    util.take_logs("ЧП остановлен!");
                }
                modbus.write_param("3001", 0);
            }
            ImGui::EndDisabled();

            ImGui::SetCursorPos(ImVec2(30, 330));
            ImGui::TextUnformatted("Состояние преобразователя:");
            ImGui::SameLine();
            ImGui::TextUnformatted(status.c_str());

            ImGui::SetCursorPos(ImVec2(26, 350));
            ImGui::Text(" %.2f", km);
            ImGui::SameLine();
            ImGui::TextUnformatted("КМ/ЧАС");

            ImGui::SetCursorPos(ImVec2(26, 370));
            ImGui::Text(" %.2f", hz);
            ImGui::SameLine();
            ImGui::TextUnformatted("ГЕРЦ");

            if (analog) {
                ImGui::SetCursorPos(ImVec2(30, 390));
                ImGui::TextUnformatted("Источник команд для ЧП: Панель Управления");
            } else {
                ImGui::SetCursorPos(ImVec2(30, 390));
                ImGui::TextUnformatted("Источник команд для ЧП: Программа");
            }

            ImGui::SetCursorPos(ImVec2(370, 335));
            ImGui::Checkbox("Debug", &debug);
            if (debug) {
                ImGui::SetCursorPos(ImVec2(370, 360));
                ImGui::Text("ОК: ");
                ImGui::SameLine();
                ImGui::Text("%d", modbus.packets);

                ImGui::SetCursorPos(ImVec2(370, 380));
                ImGui::Text("ERR: ");
                ImGui::SameLine();
                ImGui::Text("%d", modbus.error_packets);
            }

            ImGui::SetCursorPos(ImVec2(30, 410));
            ImGui::Text("ВКЛ: ");
            ImGui::SameLine();
            ImGui::Text("%d", test2.on);

            ImGui::SetCursorPos(ImVec2(30, 430));
            ImGui::Text("РАБ: ");
            ImGui::SameLine();
            ImGui::Text("%d", test2.work);

            update_timer();
            work_time_mins = (work_time % 3600) / 60;
            work_time_hours = work_time / 3600;
            work_time_secs = work_time % 60;

            ImGui::SetCursorPos(ImVec2(130, 430));
            ImGui::Text("РАБ ИЗ ФАЙЛА: ");
            ImGui::SameLine();
            ImGui::Text("%d", work_time_hours);
            ImGui::SameLine();
            ImGui::Text("ч.");
            ImGui::SameLine();
            ImGui::Text("%d", work_time_mins);
            ImGui::SameLine();
            ImGui::Text("м.");
            ImGui::SameLine();
            ImGui::Text("%d", work_time_secs);
            ImGui::SameLine();
            ImGui::Text("c.");
        } else {
            ImGui::SetCursorPos(ImVec2(120, 300));
            ImGui::Text("Соединение ModBus не активно!");
        }

        // BUEV
        if (uart.data.time != 0) {

            ImGui::SetCursorPos(ImVec2(620, 100));
            ImGui::Text("BUEV-G");

            ImGui::SetCursorPos(ImVec2(470, 120));
            ImGui::Text("Время:");
            ImGui::SameLine();
            ImGui::Text("%d", uart.data.time);

            ImGui::SetCursorPos(ImVec2(470, 140));
            ImGui::Text("Напр.обм.возб. :");
            ImGui::SameLine();
            ImGui::Text("%d", uart.data.fwv);

            ImGui::SetCursorPos(ImVec2(470, 160));
            ImGui::Text("Треб.напр.уставки:");
            ImGui::SameLine();
            ImGui::Text("%d", uart.data.rsv);

            ImGui::SetCursorPos(ImVec2(470, 180));
            ImGui::Text("Коэф.накачки:");
            ImGui::SameLine();
            ImGui::Text("%d", uart.data.spf);

            ImGui::SetCursorPos(ImVec2(470, 200));
            ImGui::Text("Iov:");
            ImGui::SameLine();
            ImGui::Text("%d", uart.data.iov);

            ImGui::SetCursorPos(ImVec2(470, 220));
            ImGui::Text("Ur:");
            ImGui::SameLine();
            ImGui::Text("%d", uart.data.ur);

            ImGui::SetCursorPos(ImVec2(470, 240));
            ImGui::Text("Up:");
            ImGui::SameLine();
            ImGui::Text("%d", uart.data.up);

            ImGui::SetCursorPos(ImVec2(470, 260));
            ImGui::Text("Uo:");
            ImGui::SameLine();
            ImGui::Text("%d", uart.data.uo);

            ImGui::SetCursorPos(ImVec2(470, 280));
            ImGui::Text("Пар.компенсации:");
            ImGui::SameLine();
            ImGui::Text("%d", uart.data.cp);

            ImGui::SetCursorPos(ImVec2(470, 300));
            ImGui::Text("U24:");
            ImGui::SameLine();
            ImGui::Text("%d", uart.data.u24);

            ImGui::SetCursorPos(ImVec2(470, 320));
            ImGui::Text("F(частота):");
            ImGui::SameLine();
            ImGui::Text("%d", uart.data.f);

            ImGui::SetCursorPos(ImVec2(470, 340));
            ImGui::Text("R:");
            ImGui::SameLine();
            ImGui::Text("%d", uart.data.r);

            ImGui::SetCursorPos(ImVec2(470, 360));
            ImGui::Text("Перекос фаз:");
            ImGui::SameLine();
            ImGui::Text("%d", uart.data.pm);

            ImGui::SetCursorPos(ImVec2(470, 380));
            ImGui::Text("Теор.откр.кл.:");
            ImGui::SameLine();
            ImGui::Text("%d", uart.data.pt);

            ImGui::SetCursorPos(ImVec2(470, 400));
            ImGui::Text("Реал.откр.кл.");
            ImGui::SameLine();
            ImGui::Text("%d", uart.data.pr);

            ImGui::SetCursorPos(ImVec2(470, 420));
            ImGui::Text("Разница:");
            ImGui::SameLine();
            ImGui::Text("%d", uart.data.div);

            ImGui::SetCursorPos(ImVec2(470, 440));
            ImGui::Text("Spi:");
            ImGui::SameLine();
            ImGui::Text("%d", uart.data.div);

            ImGui::SetCursorPos(ImVec2(650, 120));
            ImGui::Text("Режим работы:");
            ImGui::SameLine();
            ImGui::Text("%s", uart.data.wm.c_str());  // тут стринг !!!

            ImGui::SetCursorPos(ImVec2(650, 140));
            ImGui::Text("???:");
            ImGui::SameLine();
            ImGui::Text("%d", uart.data.modbus);

            ImGui::SetCursorPos(ImVec2(650, 160));
            ImGui::Text("Ig:");
            ImGui::SameLine();
            ImGui::Text("%d", uart.data.ig);

            ImGui::SetCursorPos(ImVec2(650, 180));
            ImGui::Text("Iab");
            ImGui::SameLine();
            ImGui::Text("%d", uart.data.iab);

            ImGui::SetCursorPos(ImVec2(650, 200));
            ImGui::Text("Uab:");
            ImGui::SameLine();
            ImGui::Text("%d", uart.data.uab);

            ImGui::SetCursorPos(ImVec2(650, 220));
            ImGui::Text("FW:");
            ImGui::SameLine();
            ImGui::Text("%d", uart.data.fw);

            ImGui::SetCursorPos(ImVec2(650, 340));
            ImGui::Text("TEMP:");
            ImGui::SameLine();
            ImGui::Text("%d", uart.data.temp_value);

            ImGui::SetCursorPos(ImVec2(650, 370));
            if (ImGui::Button("Расшир.", ImVec2(100, 30))) {
                if (uart.write_data("t") != 0) {
                    util.take_logs("Ошибка конфигурации логов BUEV-G", 'b');
                };
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
                if (uart.write_data("7") != 0) {
                    util.take_logs("Ошибка конфигурации логов BUEV-G", 'b');
                };
                if (buev7) {
                    buev7 = false;
                } else {
                    buev7 = true;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
                if (uart.write_data("z") != 0) {
                    util.take_logs("Ошибка конфигурации логов BUEV-G", 'b');
                };
            }

            ImGui::SetCursorPos(ImVec2(650, 420));
            if (ImGui::Button("Темп.", ImVec2(100, 30))) {
                if (uart.write_data("t") != 0) {
                    util.take_logs("Ошибка конфигурации логов BUEV-G", 'b');
                };
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
                if (uart.write_data("8") != 0) {
                    util.take_logs("Ошибка конфигурации логов BUEV-G", 'b');
                };
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
                if (uart.write_data("z") != 0) {
                    util.take_logs("Ошибка конфигурации логов BUEV-G", 'b');
                };
            }

            if (buev7) {
                ImGui::SetCursorPos(ImVec2(650, 240));
                ImGui::Text("ZP:");
                ImGui::SameLine();
                ImGui::Text("%d", uart.data.zp);

                ImGui::SetCursorPos(ImVec2(650, 260));
                ImGui::Text("CorPH:");
                ImGui::SameLine();
                std::stringstream ss;
                ss << uart.data.corph1 << " " << uart.data.corph2 << " " << uart.data.corph3;
                ImGui::Text("%s", ss.str().c_str());

                ImGui::SetCursorPos(ImVec2(650, 280));
                ImGui::Text("PhA:");
                ImGui::SameLine();
                ImGui::Text("%d", uart.data.pha);

                ImGui::SetCursorPos(ImVec2(650, 300));
                ImGui::Text("PhB:");
                ImGui::SameLine();
                ImGui::Text("%d", uart.data.phb);

                ImGui::SetCursorPos(ImVec2(650, 320));
                ImGui::Text("PhC:");
                ImGui::SameLine();
                ImGui::Text("%d", uart.data.phc);
            }
        } else {
            ImGui::SetCursorPos(ImVec2(620, 100));
            ImGui::Text("Подключение к BUEV-G неактивно!");
        }

        ImGui::End();

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
        ImGui::EndFrame();
    }

    // Очистка ресурсов ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // Завершение работы GLFW
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
*/
void bm_draw_led(ImVec2 pos, float radius, ImU32 color) {
    // Получаем список для рисования
    auto draw_list = ImGui::GetWindowDrawList();
    // Рисуем круг
    draw_list->AddCircleFilled(pos, radius, color);
}

void _220_gui() {
    ImGui::SetCursorPos(ImVec2(285, 10));
    ImGui::Text("220");
    ImGui::SetCursorPos(ImVec2(10, 40));
    ImGui::Text("БП - выключен");
    ImGui::SetCursorPos(ImVec2(10, 60));
    ImGui::Text("АКБ - выключена");
    ImGui::SetCursorPos(ImVec2(10, 80));
    ImGui::Text("Нагрузка = 0");
    ImGui::SetCursorPos(ImVec2(10, 100));
    ImGui::Text("Автомат РН-100 - включен");
    ImGui::SetCursorPos(ImVec2(250, 120));
    if (ImGui::Button("Начать тестирование", ImVec2(160, 30))) {
        psi.flags._220_start = true;
    }

    if (psi.flags._220_start) {
        ImGui::SetCursorPos(ImVec2(10, 170));
        ImGui::Text("Дождитесь окончания разгона генератора");
        ImGui::SetCursorPos(ImVec2(10, 200));
        ImGui::Text("Нажмите кнопку Восстановление Защиты на пульте и удерживайте ее до срабатывания РН-100");
        ImGui::SetCursorPos(ImVec2(10, 230));
        ImGui::Text("Запишите значения в числовые поля и нажмите кнопку Подтвердить");
        ImGui::SetCursorPos(ImVec2(50, 280));
        ImGui::SetNextItemWidth(100);
        ImGui::InputInt("Напр.Ген./АКБ мин.", &results._220_1);

        ImGui::SetCursorPos(ImVec2(300, 280));
        ImGui::SetNextItemWidth(100);
        ImGui::InputInt("Напр.Ген./АКБ макс.", &results._220_2);
        ImGui::SetCursorPos(ImVec2(150, 320));
        if (ImGui::Button("Подтвердить", ImVec2(150, 30))) {
            psi.flags._220_1 = false;
            psi.flags._221_1 = true;
        }
    }
}

void _221_gui() {
    ImGui::SetCursorPos(ImVec2(285, 10));
    ImGui::Text("221");
    ImGui::SetCursorPos(ImVec2(10, 40));
    ImGui::Text("БП - выключен");
    ImGui::SetCursorPos(ImVec2(10, 60));
    ImGui::Text("АКБ - выключена");
    ImGui::SetCursorPos(ImVec2(10, 80));
    ImGui::Text("Нагрузка = 50");
    ImGui::SetCursorPos(ImVec2(10, 100));
    ImGui::Text("Автомат РН-100 - включен");
    ImGui::SetCursorPos(ImVec2(250, 120));
    if (ImGui::Button("Начать тестирование", ImVec2(160, 30))) {
        psi.flags._221_start = true;
    }

    if (psi.flags._221_start) {
        ImGui::SetCursorPos(ImVec2(10, 170));
        ImGui::Text("Дождитесь окончания разгона генератора");
        ImGui::SetCursorPos(ImVec2(10, 200));
        ImGui::Text("Нажмите кнопку Восстановление Защиты на пульте и удерживайте ее до срабатывания РН-100");
        ImGui::SetCursorPos(ImVec2(10, 230));
        ImGui::Text("Запишите значения в числовые поля и нажмите кнопку Подтвердить");
        ImGui::SetCursorPos(ImVec2(50, 280));
        ImGui::SetNextItemWidth(100);
        ImGui::InputInt("Напр.Ген./АКБ мин.", &results._221_1);

        ImGui::SetCursorPos(ImVec2(300, 280));
        ImGui::SetNextItemWidth(100);
        ImGui::InputInt("Напр.Ген./АКБ макс.", &results._221_2);
        ImGui::SetCursorPos(ImVec2(150, 320));
        if (ImGui::Button("Подтвердить", ImVec2(150, 30))) {
            // psi.flags._220_finish = true;
            psi.flags._221_1 = false;
            psi.flags._224 = true;
        }
    }
}

void _224_gui() {
    ImGui::SetCursorPos(ImVec2(285, 10));
    ImGui::Text("224");
    ImGui::SetCursorPos(ImVec2(300, 300));
    ImGui::Text("Проводится тестирование СПО...");
    ImGui::Text("Результат:");
    ImGui::SameLine();
    if (results._224 == 0) {
        ImGui::Text("Успех");
    } else {
        if (results._224 == -2) {
            ImGui::Text("Провал");
        }
    }

    ImGui::SetCursorPos(ImVec2(500, 300));
    if (ImGui::Button("Подтвердить", ImVec2(150, 30))) {
        psi.flags._224 = false;
        psi.flags._225 = true;
    }

}

void _225_gui () {
    ImGui::SetCursorPos(ImVec2(285, 10));
    ImGui::Text("225");
    ImGui::Text("Проконтролируйте напряжение на БП по КП 100В +/- 0.1В");
    ImGui::Text("Напряжение АКБ (СПО) = ");
    ImGui::SameLine();
    if (results._225 != -1) {
        ImGui::Text("%.2f В", results._225);
    }
    ImGui::SetCursorPos(ImVec2(500, 300));
    if (ImGui::Button("Подтвердить", ImVec2(150, 30))) {
        psi.flags._225 = false;
        psi.flags.base = true;
    }
    
}

int test_gui() {
    if (!glfwInit()) {
        std::cerr << "Не удалось инициализировать GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Создаем окно с помощью GLFW
    GLFWwindow* window = glfwCreateWindow(1280, 720, "psi", NULL, NULL);
    if (window == nullptr) {
        std::cerr << "Не удалось создать окно GLFW" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwSetWindowCloseCallback(window, glfw_window_close_callback);

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);  // Включить вертикальную синхронизацию

    // Инициализация ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    // Настройка бекендов
    if (!ImGui_ImplGlfw_InitForOpenGL(window, true)) {
        std::cerr << "Не удалось инициализировать ImGui для GLFW" << std::endl;
        glfwTerminate();
        return -1;
    }
    if (!ImGui_ImplOpenGL3_Init("#version 130")) {
        std::cerr << "Не удалось инициализировать ImGui для OpenGL" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Загрузка шрифта
    ImGuiIO& io = ImGui::GetIO();
    ImFont* font = io.Fonts->AddFontFromFileTTF("fonts/tilda-sans_medium.ttf", 19.0f, NULL, io.Fonts->GetGlyphRangesCyrillic());
    if (!font) {
        std::cerr << "Не удалось загрузить шрифт" << std::endl;
        glfwTerminate();
        return -1;
    }
    io.Fonts->Build();  // Построение шрифтов

    // Основной цикл GUI
    while (!glfwWindowShouldClose(window) && on) {
        glfwPollEvents();

        // Начало нового кадра ImGui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(1280, 720));
        ImGui::Begin("Test", &on,
                     ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

        if (psi.flags._221_1) {
            _221_gui();
        }

        if (psi.flags._220_1) {
            _220_gui();
        }

        if (psi.flags._224){
            _224_gui();
        }

        if (psi.flags._225) {
            _225_gui();
        }

        ImGui::Text("test");

        /** if (start_250) {
             // Открываем первое модальное окно (зеленый светодиод)
             if (!showGreenLedPopup && !showYellowLedPopup) {
                 ImGui::OpenPopup("Зеленый");
                 showGreenLedPopup = true;  // Активируем флаг для зеленого окна
             }

             // Модальное окно для зеленого светодиода
             if (showGreenLedPopup) {
                 if (ImGui::BeginPopupModal("Зеленый", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
                     if (ImGui::Button("Горит")) {
                         green_250 = "Горит";
                         ImGui::CloseCurrentPopup();
                         showGreenLedPopup = false;  // Закрываем зеленое окно
                         showYellowLedPopup = true;  // Переходим к желтому окну
                     }
                     ImGui::SameLine();
                     if (ImGui::Button("Промаргивает")) {
                         green_250 = "Промаргивает";
                         ImGui::CloseCurrentPopup();
                         showGreenLedPopup = false;  // Закрываем зеленое окно
                         showYellowLedPopup = true;  // Переходим к желтому окну
                     }
                     ImGui::SameLine();
                     if (ImGui::Button("Не горит")) {
                         green_250 = "Не горит";
                         ImGui::CloseCurrentPopup();
                         showGreenLedPopup = false;  // Закрываем зеленое окно
                         showYellowLedPopup = true;  // Переходим к желтому окну
                     }
                     ImGui::EndPopup();
                 }
             }

             // Модальное окно для желтого светодиода
             if (showYellowLedPopup) {
                 ImGui::OpenPopup("Желтый");  // Обязательно открываем желтое окно
                 if (ImGui::BeginPopupModal("Желтый", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
                     if (ImGui::Button("Горит")) {
                         yellow_250 = "Горит";
                         ImGui::CloseCurrentPopup();
                         showYellowLedPopup = false;  // Закрываем желтое окно
                         result_250 = true;           // Завершаем процесс
                     }
                     ImGui::SameLine();
                     if (ImGui::Button("Промаргивает")) {
                         yellow_250 = "Промаргивает";
                         ImGui::CloseCurrentPopup();
                         showYellowLedPopup = false;  // Закрываем желтое окно
                         result_250 = true;           // Завершаем процесс
                     }
                     ImGui::SameLine();
                     if (ImGui::Button("Не горит")) {
                         yellow_250 = "Не горит";
                         ImGui::CloseCurrentPopup();
                         showYellowLedPopup = false;  // Закрываем желтое окно
                         result_250 = true;           // Завершаем процесс
                     }
                     ImGui::EndPopup();
                 }
             }

             // Если оба светодиода опрошены, завершаем процесс
             if (!showGreenLedPopup && !showYellowLedPopup) {
                 start_250 = false;  // Сбрасываем флаг начала опроса
             }
         }
 */
        // if (result_260) {
        //     ImGui::SetCursorPos(ImVec2(300, 70));
        //     ImGui::Text("260:");

        //     ImGui::SetCursorPos(ImVec2(300, 90));
        //     ImGui::Text("Напряжение АКБ (СПО) = %.2f В", result_260_1);

        //     ImGui::SetCursorPos(ImVec2(300, 110));
        //     ImGui::Text("АБ (BUEV_MONITOR) = %.2f В", result_260_2);

        //     ImGui::SetCursorPos(ImVec2(300, 130));
        //     ImGui::Text("АБ (BUEV_MONITOR) = %.2f В", result_260_3);
        // }

        // if (result_224) {
        //     ImGui::SetCursorPos(ImVec2(300, 150));
        //     ImGui::Text("224:");
        //     ImGui::SameLine();
        //     if (fail_224) {
        //         ImGui::Text("Провал");
        //     } else {
        //         ImGui::Text("Успех");
        //     }
        // }

        // if (result_225) {
        //     ImGui::SetCursorPos(ImVec2(300, 170));
        //     ImGui::Text("225:");
        //     ImGui::SetCursorPos(ImVec2(300, 190));
        //     ImGui::Text("Напряжение АКБ (СПО) = %.2f В", result_225_1);
        // }

        // if (result_240) {
        //     ImGui::SetCursorPos(ImVec2(300, 210));
        //     ImGui::Text("240: Build = %.3f", build);
        // }

        // if (result_250) {
        //     ImGui::SetCursorPos(ImVec2(300, 230));
        //     ImGui::Text("250:");
        //     ImGui::SetCursorPos(ImVec2(300, 250));
        //     ImGui::Text("Зеленый:");
        //     ImGui::SameLine();
        //     ImGui::Text("%s", green_250.c_str());

        //     ImGui::SetCursorPos(ImVec2(300, 270));
        //     ImGui::Text("Желтый:");
        //     ImGui::SameLine();
        //     ImGui::Text("%s", yellow_250.c_str());
        // }

        // ImGui::SetCursorPos(ImVec2(10, 200));
        // if (ImGui::Button("power", ImVec2(50, 30))) {
        //     power = true;
        // }

        if (psi.flags.base) {
            ImGui::SetCursorPos(ImVec2(10, 100));
            if (ImGui::Button("готов", ImVec2(50, 30))) {
                psi.flags._220_1 = true;
                psi.flags.base = false;
            }

            ImGui::SetCursorPos(ImVec2(530, 40));
            ImGui::Text("link");

            if (buevg_udp.bm_led_connection) {
                bm_draw_led(ImVec2(570, 50), 10, LED_GREEN);
            } else {
                bm_draw_led(ImVec2(570, 50), 10, LED_RED);
            }

            ImGui::SetCursorPos(ImVec2(40, 40));
            ImGui::Text("АБ (BUEV-MONITOR): %.2f В", data_obj.bd.Vacc);

            ImGui::SetCursorPos(ImVec2(40, 80));
            ImGui::Text("Напряжения АКБ (СПО): %.2f В", data.BATTERY_VOLTAGE);

            ImGui::SetCursorPos(ImVec2(40, 120));
            ImGui::Text("КП: %.2f В", akip.u);

            ImGui::SetCursorPos(ImVec2(300, 10));
            ImGui::SetNextItemWidth(100);

            if (ImGui::Combo("BUEV MODEL", &selected_buevg_model, buevg_models, IM_ARRAYSIZE(buevg_models))) {
                buevg_tcp.g_opt = atoi(buevg_models[selected_buevg_model]);
                std::cout << "выбран" << buevg_tcp.g_opt << "\n";
            }

            ImGui::SetCursorPos(ImVec2(10, 250));
            if (ImGui::Button("exit", ImVec2(50, 30))) {
                exit_button = true;
            }

            ImGui::SetCursorPos(ImVec2(10, 150));
            if (ImGui::Button("init", ImVec2(50, 30))) {
                psi.init();
            }
        }

        ImGui::End();

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
        ImGui::EndFrame();
    }

    // Очистка ресурсов ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // Завершение работы GLFW
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

// void uart_thread() {
//     while (true) {
//         uart.read_data(1000);
//         uart.start_parse(uart.buev_log);
//     }
// }

void modbus_thread() {
    // std::cout << "9"<<std::endl;

    std::vector<std::string> scenario_files = util.get_files("scenarios/");

    std::map<std::string, std::string> scenarios = util.load_scenarios(scenario_files);

    for (const auto& [filename, scenario] : scenarios) {
        keys.push_back(scenario);
        file_names.push_back(filename);
    }

    test2.work_time();  // обращение с целью фиксации моточасов

    while (true) {
        // цикл ожидания
        while (/*SCENARIO.empty() ||*/ !run) {  //???
                                                // std::cout << "cycle" << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }

        if (!test2.ready()) {
            if (test2.prepare() == 0) {
                analog = false;
            }
        } else {
            util.take_logs("ЧП готов к работе");
        }

        if (zeroing) {
            modbus.write_param("3001", 0);
        }

        try {
            auto current_scenario = util.parse_scenario(SCENARIO);
            // Выполнение инструкций
            for (const auto& instruction : current_scenario) {
                instruction.func();
                if (test2.error == true) {
                    run = false;
                }
                if (!run) {
                    util.take_logs("Выполнение сценария прервано");
                    break;
                }
            }

        } catch (const std::exception& e) {
            std::cerr << "Error parsing scenario: " << e.what() << std::endl;
        }
        // SCENARIO = "";
        run = false;
        stop = false;
    }
}

void upvs_thread() {
    std::cout << "upvs wait power\n";
    while (!bp.power) {
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    }
    while (!exit_button) {
        upvs_request.recieve_data(data);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    upvs_request.stop();
}

void buev_monitor_thread() {
    while (!bp.power) {
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
        // std::cout << "жду питания\n";
    }
    while (buevg_tcp.g_opt == -1) {
        std::cout << "не выбрана модель buev-g" << "\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    }
    std::cout << "начинаю инициализацию БМ\n";
    // buevg_udp.init();
    trig.init_triggers();
    if (buevg_tcp.g_opt == 10) {
        led.LED_Gen_R = BUEV_G10_LED_Gen_R;
        led.LED_Gen_G = BUEV_G10_LED_Gen_G;
        led.LED_RPN1_G = BUEV_G10_LED_RPN1_G;
        led.LED_RPN1_R = BUEV_G10_LED_RPN1_R;
        led.LED_RPN2_G = BUEV_G10_LED_RPN2_G;
        led.LED_RPN2_R = BUEV_G10_LED_RPN2_R;
        led.LED_WORK_G = BUEV_G10_LED_WORK_G;
        led.LED_ALARM_R = BUEV_G10_LED_ALARM_R;
        led.LED_RPNM_G = BUEV_G10_LED_RPNM_G;
        led.LED_RPNM_R = BUEV_G10_LED_RPNM_R;
    } else if (buevg_tcp.g_opt == 20) {
        led.LED_Gen_R = BUEV_G20_LED_Gen_R;
        led.LED_Gen_G = BUEV_G20_LED_Gen_G;
        led.LED_RPN1_G = BUEV_G20_LED_RPN1_G;
        led.LED_RPN1_R = BUEV_G20_LED_RPN1_R;
        led.LED_RPN2_G = BUEV_G20_LED_RPN2_G;
        led.LED_RPN2_R = BUEV_G20_LED_RPN2_R;
        led.LED_WORK_G = BUEV_G20_LED_WORK_G;
        led.LED_ALARM_R = BUEV_G20_LED_ALARM_R;
        led.LED_RPNM_G = BUEV_G20_LED_RPNM_G;
        led.LED_RPNM_R = BUEV_G20_LED_RPNM_R;
    }
    buevg_udp.init();
    buevg_tcp.init();
    if (buevg_tcp.connectToServer() != -1) {
        buevg_tcp.sendMessageAndReceiveResponse(1);
    }

    while (!exit_button) {
        if (!buevg_udp.bm_connection) {
            bm_ping = 0;  // std::system("ping -c 1 10.7.8.101");
            if (bm_ping == 0) {
                buevg_tcp.closeSocket();
                buevg_udp.closeSocket();
                // sleep (1);
                buevg_tcp.init();
                if (buevg_tcp.connectToServer() != -1) {
                    buevg_tcp.sendMessageAndReceiveResponse(1);
                }
                commander.exit();
                buevg_udp.init();
                buevg_udp.bm_connection = true;
            } else {
                continue;
            }
        } else {
            buevg_udp.readData(bm_buffer, BM_BUFFER_SIZE, bm_bytes_read);
            const Data::cyclic_buev_data_t& history = data_obj.getHistory();
            maximums = &data_obj.getMaximums();

            // Очистка предыдущих данных
            std::lock_guard<std::mutex> lock(bm_data_mutex);
            if (should_update_data) {
                should_update_data = false;

                // Сброс сумм и счетчиков перед новым расчетом
                sum_Agen = 0.0f;
                sum_Acharge = 0.0f;
                count_Agen = 0;
                count_Acharge = 0;

                // Заполнение истории и накопление данных для средних значений
                for (int i = 0; i < 14 && i < data_obj.MAX_HISTORY_MASK; ++i) {
                    int idx = (history.next_idx - i - 1) & data_obj.MAX_HISTORY_MASK;
                    // bd = &history.data[idx];

                    // Накопление значений для среднего
                    sum_Agen += data_obj.bd.Agen;
                    sum_Acharge += data_obj.bd.Acharge;
                    count_Agen++;
                    count_Acharge++;

                    unsigned char akb_type = data_obj.bd.dip & DIP_ACC;
                    if (data_obj.bd.dip & 0x10) {
                        akb_type |= 0x4;
                    }
                    unsigned char gen_type = (data_obj.bd.dip & DIP_KW) >> 2;
                }

                // Рассчитываем средние значения
                if (count_Agen > 0) {
                    average_Agen = sum_Agen / count_Agen;
                } else {
                    average_Agen = 0.0f;
                }

                if (count_Acharge > 0) {
                    average_Acharge = sum_Acharge / count_Acharge;
                } else {
                    average_Acharge = 0.0f;
                }
            }
        }
    }
    buevg_udp.bm_connection = false;
    buevg_udp.bm_led_connection = false;
    buevg_tcp.closeSocket();
    buevg_udp.closeSocket();
    std::cout << "BM завершен" << "\n";
}

void psi_thread() {
    // if (bp.init() == 0) {
    //     std::this_thread::sleep_for(std::chrono::milliseconds(50));
    //     if (bp.out_on() == 0) {
    //         std::cout << "BP OK STARTING..." << "\n";
    //         while (!ready) {
    //             std::cout << "WAITING READY\n";
    //             std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    //         }

    //         // 260:
    //         while (result_260_1 == 0 || result_260_2 == 0 || result_260_3 == 0) {
    //             if (result_260_1 == 0) {
    //                 bp.set_u_kp(100.0);
    //                 if (abs(100.0 - akip.read_u()) < 1) {
    //                     result_260_1 = data.BATTERY_VOLTAGE;
    //                 } else {
    //                     std::cout << "не соответствие значений кп\n";
    //                 }
    //             }
    //             std::this_thread::sleep_for(std::chrono::milliseconds(50));
    //             if (result_260_2 == 0) {
    //                 bp.set_u_kp(90.0);
    //                 if (abs(90.0 - akip.read_u()) < 0.1) {
    //                     result_260_2 = data_obj.bd.Vacc;
    //                 } else {
    //                     std::cout << "не соответствие значений кп\n";
    //                 }
    //             }
    //             if (result_260_3 == 0) {
    //                 bp.set_u_kp(150.0);
    //                 if (abs(150.0 - akip.read_u()) < 0.1) {
    //                     result_260_3 = data_obj.bd.Vacc;
    //                 } else {
    //                     std::cout << "не соответствие значений кп\n";
    //                 }
    //             }
    //         }
    //         std::cout << "260 FINISHED\n";
    //         result_260 = true;

    //         // 224
    //         std::cout << "224 starting\n";
    //         bp.set_u(120.0);
    //         std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    //         if (data.POWER_GENERATOR == -5000.0 || data.U_GENERATOR == -5000.0 || data.U_WINDING_GENERATOR == -5000.0 || data.I_WINDING_GENERATOR == -5000.0 ||
    //             data.SPEED_BY_GENERATOR == -5000.0 || data.U_110 == -5000.0 || data.I_110 == -5000.0 || data.I_BATTERY == -5000.0 || data.BATTERY_VOLTAGE == -5000.0 ||
    //             data.TEMP_BATTERY == -5000.0) {
    //             fail_224 = true;  // Найдено недопустимое значение
    //         }

    //         if (data.STATUS == "-" || data.RCH == "-" || data.RMT_Z == "-" || data.RMT_R == "-" || data.BATTERY_CIRCUIT_BREAKAGE == "-" || data.RPN1 == "-" || data.RPN2
    //         == "-" ||
    //             data.TYPE_AKK == "-" || data.TYPE_GEN == "-") {
    //             fail_224 = true;  // Найдено недопустимое значение
    //         }
    //         result_224 = true;
    //         std::cout << "224 завершен" << "\n";

    //         // // 225
    //         std::cout << "225 starting\n";
    //         bp.set_u_kp(100.0);
    //         if (abs(100.0 - akip.read_u()) < 0.1) {
    //             result_225_1 = data.BATTERY_VOLTAGE;
    //         } else {
    //             std::cout << "не соответствие значений кп\n";
    //         }
    //         result_225 = true;
    //         std::cout << "225 завершен" << "\n";

    //         // 240
    //         std::cout << "240 starting\n";
    //         build = data_obj.header->build / 1000.0;
    //         result_240 = true;
    //         std::cout << "240 завершен" << "\n";

    //         // 250
    //         start_250 = true;
    //         while (start_250) {
    //         }

    //         while (!exit_button) {
    //             std::cout << "пси завершен" << "\n";
    //             std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    //         }
    //         bp.out_off();
    //         std::this_thread::sleep_for(std::chrono::milliseconds(500));
    //         bp.stop();
    //         ready = false;
    //         std::cout << "test завершен" << "\n";
    //     } else {
    //         std::cout << "BP ERROR" << "\n";
    //     }
    // }

    // while (!global_init) {
    // }

    // psi.init();
    // std::cout << "upvs " << data.BATTERY_VOLTAGE << " BM " << data_obj.bd.Vacc << "\n";
    // while (!exit_button) {
    // }
    // std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    // bp.stop();

    while (!psi.flags._220_1) {
    }
    psi._220(results);
    std::cout << "вышел из 220" << "\n";
    // std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    psi._221(results);
    while (!psi.flags._224) {
    }
    psi._224(results, data);
    while (!psi.flags._225) {
    }
    psi._225 (results, data);
    while (psi.flags._225){
        std::cout << data.BATTERY_VOLTAGE << "\n";
    }
    bp.out_off();
}

int main() {
    // while (true){
    // float mm = akip.read_u();
    // std::cout << "mm:" << mm << "\n";
    // }

    // std::thread uart_thread_obj(uart_thread);
    // uart_thread_obj.detach();

    std::thread upvs_thread_obj(upvs_thread);
    upvs_thread_obj.detach();

    std::thread buev_monitor_thread_obj(buev_monitor_thread);
    buev_monitor_thread_obj.detach();

    std::thread psi_thread_obj(psi_thread);
    psi_thread_obj.detach();

    // if (modbus) {
    //     update_timer();
    //     std::thread modbus_thread_obj(modbus_thread);  // Запуск потока
    //     modbus_thread_obj.detach();                    // Отделяем поток, чтобы он продолжал работать независимо от основного потока
    // }

    test_gui();
    return 0;
}

/*
sudo modprobe -r usbtmc

sudo ifconfig enp0s31f6 10.7.8.231 netmask 255.255.0.0 up
sudo ip route add 10.7.8.0/24 dev enp0s31f6

sudo ifconfig enp0s31f6 192.168.5.208 netmask 255.255.0.0 up
*/