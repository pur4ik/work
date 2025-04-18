#include <GLFW/glfw3.h>
#include <ft2build.h>
#include <libusb-1.0/libusb.h>
#include FT_FREETYPE_H
#include <thread>
#include <iostream>

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "image.h"
#include "imgui.h"


extern "C" {
#include "bmp.h"
}

time_t start_time_func, current_time, start_auto_mode, current_time2;

#define VENDOR_ID 0x0000
#define PRODUCT_ID 0x0001

#define CONFIGURE_HEADER 0xFE    // Пакет с конфигурацией начинается с этого хедера
#define START_FRAME_HEADER 0xFA  // Первый пакет (начала кадра) с данными начинается с этого хедера

libusb_device_handle *device_handle = nullptr;
libusb_context *ctx = nullptr;

void send_config(int WIDTH, int HEIGHT, libusb_device_handle *device_handle);

int MATRIX_PER_MODULE = 4;
int WIDTH;
static int HEIGHT = 64;

std::vector<int> choices;  // массив значений choice для авторежима
char displayed_text[128];
bool autostart = false;
int choice = 0;
bool connection = false;

Image _frame;

std::vector<uint8_t> gamma8 = {0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   1,
                               1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   2,   2,   2,   2,   2,   2,   2,   2,   3,   3,   3,   3,   3,   3,   3,   4,   4,
                               4,   4,   4,   5,   5,   5,   5,   6,   6,   6,   6,   7,   7,   7,   7,   8,   8,   8,   9,   9,   9,   10,  10,  10,  11,  11,  11,  12,  12,
                               13,  13,  13,  14,  14,  15,  15,  16,  16,  17,  17,  18,  18,  19,  19,  20,  20,  21,  21,  22,  22,  23,  24,  24,  25,  25,  26,  27,  27,
                               28,  29,  29,  30,  31,  32,  32,  33,  34,  35,  35,  36,  37,  38,  39,  39,  40,  41,  42,  43,  44,  45,  46,  47,  48,  49,  50,  50,  51,
                               52,  54,  55,  56,  57,  58,  59,  60,  61,  62,  63,  64,  66,  67,  68,  69,  70,  72,  73,  74,  75,  77,  78,  79,  81,  82,  83,  85,  86,
                               87,  89,  90,  92,  93,  95,  96,  98,  99,  101, 102, 104, 105, 107, 109, 110, 112, 114, 115, 117, 119, 120, 122, 124, 126, 127, 129, 131, 133,
                               135, 137, 138, 140, 142, 144, 146, 148, 150, 152, 154, 156, 158, 160, 162, 164, 167, 169, 171, 173, 175, 177, 180, 182, 184, 186, 189, 191, 193,
                               196, 198, 200, 203, 205, 208, 210, 213, 215, 218, 220, 223, 225, 228, 231, 233, 236, 239, 241, 244, 247, 249, 252, 255

};

std::vector<uint8_t> getFrame() {
  _frame.clear();
  switch (choice) {
    case 0:
      _frame.static_color(0, 0, 0);  // черный
      break;
    case 1:
      _frame.static_color(255, 255, 255);  // белый
      break;
    case 2:
      _frame.static_color(0, 255, 0);  // зеленый
      break;
    case 3:
      _frame.static_color(255, 0, 0);  // красный
      break;
    case 4:
      _frame.static_color(0, 0, 255);  // синий
      break;
    case 5:
      _frame.test();  // градиент
      break;
    case 6:
      _frame.displayText("white", displayed_text);
      break;
    case 7:
      _frame.position();
      break;
    case 8:
      _frame.displayText("white", "Автоматический режим");
      break;
  }

  return _frame.getBuffer();  // Возвращаем буфер изображения
}

static bool open = true;
void draw_gui() {
  ImGui::SetNextWindowPos(ImVec2(0, 0));
  ImGui::SetNextWindowSize(ImVec2(350, 270));  // основное окно
  ImGui::Begin("Диагностика платы развертки##MainWindow", &open,
               ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
  ImGui::SetCursorPosY(35);
  ImGui::SetCursorPosX(10);
  ImGui::PushItemWidth(100);
  // начало выпадающего списка
  const char *items[] = {"1", "2", "3", "4"};
  static int selectedItem = 3;  // индекс выбранного элемента
  ImGui::Text("Выберите количество матриц:");
  ImGui::SameLine();
  if (ImGui::Combo("##combo", &selectedItem, items, IM_ARRAYSIZE(items))) {
    MATRIX_PER_MODULE = selectedItem + 1;
    if (WIDTH != (MATRIX_PER_MODULE * 64)) {
      WIDTH = MATRIX_PER_MODULE * 64;
      send_config(WIDTH, HEIGHT, device_handle);
      std::cout << WIDTH << " " << HEIGHT << std::endl;
      _frame.init(WIDTH, HEIGHT);
    }
  }

  // чекбоксы
  ImGui::SetCursorPosY(70);
  static int selectedCheckbox = -1;
  const char *checkboxLabels[8] = {"Черный", "Белый", "Зеленый", "Красный", "Синий", "Градиент", "Проверка положения матриц", "Бегущая строка"};

  float leftColumnX = 10.0f;                // X-координата для левого столбца
  float rightColumnX = 110.0f;              // X-координата для правого столбца
  float initialY = ImGui::GetCursorPosY();  // Начальная Y-позиция для чекбоксов

  for (int i = 0; i < sizeof(checkboxLabels) / sizeof(checkboxLabels[0]); ++i) {
    // Определяем позиции для столбцов
    if (i < 3) {
      ImGui::SetCursorPosX(leftColumnX);
      ImGui::SetCursorPosY(initialY + i * 30);
    } else if (i < 6) {
      ImGui::SetCursorPosX(rightColumnX);
      ImGui::SetCursorPosY(initialY + (i - 3) * 30);
    } else {
      ImGui::SetCursorPosX(leftColumnX);
      ImGui::SetCursorPosY(initialY + 3 * 30 + (i - 6) * 30);
    }

    bool isChecked = (selectedCheckbox == i);

    if (ImGui::Checkbox(("##checkbox" + std::to_string(i)).c_str(), &isChecked)) {
      selectedCheckbox = i;
      choice = (i == 6) ? 7 : (i == 7) ? 6 : i;
      autostart = false;
    }

    ImGui::SameLine();  // Размещаем текст на той же линии, что и чекбокс
    ImGui::Text("%s", checkboxLabels[i]);

    // чекбокс бегущей строки
    if (i == 7) {
      ImGui::SameLine();
      ImGui::SetNextItemWidth(160);
      ImGui::InputText("##InputText", displayed_text, IM_ARRAYSIZE(displayed_text));
    }

    ImGui::Spacing();  // Отступ между строками
  }
  ImGui::Spacing();
  // авторежим
  ImGui::SetCursorPosX(40);
  if (ImGui::Button("Автоматическая смена режимов##MatrixButton", ImVec2(265, 30))) {
    choice = 8;
    autostart = true;
    choices.clear();
    time(&start_auto_mode);
    while (difftime(time(&current_time2), start_auto_mode) < 5) {
      choice = 8;
    }
    time(&start_time_func);
  }
  ImGui::SetCursorPosX(10);
  ImGui::SetCursorPosY(10);
  ImGui::Text("Статус подключения:");
  ImGui::SameLine();
  // проверка подключения
  if (connection) {
    ImVec4 color = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
    ImGui::TextColored(color, "Активно");
  } else {
    ImVec4 color = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
    ImGui::TextColored(color, "Неактивно");
  }
  ImGui::End();
}

void *gui(void *test) {
  if (!glfwInit()) {
    std::cerr << "Не удалось инициализировать GLFW" << std::endl;
    return nullptr;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Создаем окно с помощью GLFW
  GLFWwindow *window = glfwCreateWindow(350, 270, "Диагностика платы развертки", NULL, NULL);
  if (window == nullptr) {
    std::cerr << "Не удалось создать окно GLFW" << std::endl;
    glfwTerminate();
    return nullptr;
  }

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);  // Включить вертикальную синхронизацию

  // Инициализация ImGui
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui::StyleColorsDark();

  // Настройка бекендов
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 130");

  ImGuiIO &io = ImGui::GetIO();
  io.Fonts->AddFontFromFileTTF("fonts/tilda-sans_medium.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesCyrillic());
  io.Fonts->Build();  // Построение шрифтов

  // Основной цикл gui
  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
    // Начало нового кадра ImGui
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    draw_gui();  // Вызов функции отрисовки интерфейса

    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
  }

  // Очистка ресурсов
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
  glfwDestroyWindow(window);
  glfwTerminate();

  return nullptr;
}

void open_usb() {
  // установка соединения
  while (!connection) {
    if (libusb_init(&ctx) < 0) {
      std::cout << "Failed to initialize libusb." << std::endl;
      continue;
    }
    // захват устройства
    device_handle = libusb_open_device_with_vid_pid(ctx, VENDOR_ID, PRODUCT_ID);
    if (!device_handle) {
      std::cout << "Device not found." << std::endl;
      libusb_exit(ctx);
      std::this_thread::sleep_for(std::chrono::microseconds(500000));
    }

    if (device_handle) {
      std::cout << "Device found." << std::endl;
      connection = true;
    }
  }
}
void send_config(int WIDTH, int HEIGHT, libusb_device_handle *device_handle) {
  int echo_width = 0;
  int echo_height = 0;
  // цикл сверки отправленных и полученных параметров шир/выс
  while (echo_width != WIDTH || echo_height != HEIGHT) {
    std::cout << "цикл" << std::endl;
    // Формируем конфигурационный пакет
    std::vector<uint8_t> configure_packet;
    configure_packet.push_back(CONFIGURE_HEADER);  // добавление заголовка конфигурационного пакета buf[0]
    std::cout << "цикл2" << std::endl;

    configure_packet.push_back((WIDTH >> 16) & 0xFF);  // buf[1] Старший байт
    configure_packet.push_back((WIDTH >> 8) & 0xFF);   // buf[2]
    configure_packet.push_back(WIDTH & 0xFF);          // buf[3] Младший байт

    configure_packet.push_back(HEIGHT);  // buf[4]
    std::cout << "цикл3" << std::endl;

    if (device_handle) {
      libusb_bulk_transfer(device_handle, 1, configure_packet.data(), configure_packet.size(), NULL, 0);
    } else {
      std::cout << "no connection" << std::endl;
    }

    std::cout << "цикл4" << std::endl;
    uint8_t echo_response[5];
    int received_length = 0;
    int result = libusb_bulk_transfer(device_handle, 0x82, echo_response, sizeof(echo_response), &received_length,
                                      5000);  // EP2_IN_ADDR = 0x82 (2 | USB_DIR_IN)

    std::cout << "цикл5" << std::endl;
    // Проверка, что данные успешно получены и их длина правильная
    std::cout << "цикл5, received_length=" << received_length << " echo_response[0]=" << echo_response[0] << std::endl;
    if (result == LIBUSB_SUCCESS && received_length == 5 && echo_response[0] == CONFIGURE_HEADER) {
      echo_width = (echo_response[1] << 16) | (echo_response[2] << 8) | echo_response[3];
      echo_height = echo_response[4];
      std::cout << "цикл6" << std::endl;
    }

    std::cout << "цикл7" << std::endl;
    // Задержка перед повтором (если эхо-ответ не получен или неверен)
    std::this_thread::sleep_for(std::chrono::milliseconds(150));
  }

  std::cout << "[USB] Razvertka configured, send=" << WIDTH << "x" << HEIGHT << " recv=" << echo_width << "x" << echo_height << std::endl;
}

int main() {
  // создание потока для gui
  pthread_t gui_thread;
  int arg = 1;
  pthread_create(&gui_thread, NULL, gui, &arg);
  // инициализация freetype
  FT_Library ft;
  FT_Face face;

  if (FT_Init_FreeType(&ft)) {
    std::cerr << "Ошибка инициализации FreeType" << std::endl;
  }
  // открытие инттерфейса
  open_usb();
  WIDTH = MATRIX_PER_MODULE * 64;
  send_config(WIDTH, HEIGHT, device_handle);
  _frame.init(WIDTH, HEIGHT);  // инициализация кадра
  // основной цикл
  while (connection) {
    std::vector<uint8_t> frameBuffer = getFrame();
    std::vector<uint8_t> packet;
    packet.push_back(START_FRAME_HEADER);

    // Проходим по каждому пикселю, каждый пиксель 2 байта
    for (int i = 0; i < WIDTH * HEIGHT; i++) {
      // достаём пиксель из буффера
      uint16_t px16 = (frameBuffer[i * 2 + 1] << 8) | frameBuffer[i * 2];  // Старший байт + младший байт

      // восстановление RGB555
      uint8_t r = (px16 >> 10) & 0x1F;
      uint8_t g = (px16 >> 5) & 0x1F;
      uint8_t b = px16 & 0x1F;

      // Корректируем пиксель для RGB555
      uint16_t corrected_px16 = (((gamma8[(r << 3) | (r >> 2)] >> 3) << 10) | ((gamma8[(g << 3) | (g >> 2)] >> 3) << 5) | (gamma8[(b << 3) | (b >> 2)] >> 3));

      packet.push_back(corrected_px16 & 0xff);
      packet.push_back(corrected_px16 >> 8);
    }
    int status = libusb_bulk_transfer(device_handle, 1, packet.data(), packet.size(), NULL, 0);
    // проверка для авторежима
    if (choices.size() > 7) {
      if (difftime(time(&current_time), start_time_func) >= 1) {
        choice = 0;
        autostart = false;
        choices.clear();
      }
    }
    // еще одна проверка для авторежима
    if (autostart) {
      if (choices.size() == 0) {
        choices.push_back(0);
      }
      if (difftime(time(&current_time), start_time_func) >= 1) {
        int choice_temp = rand() % 8;
        bool is_duplicate = false;

        for (int j = 0; j < choices.size(); j++) {
          if (choices[j] == choice_temp) {
            is_duplicate = true;
            break;
          }
        }
        // цикл перебора режимов для автосмены
        while (is_duplicate) {
          choice_temp = rand() % 8;
          is_duplicate = false;
          for (int j = 0; j < choices.size(); j++) {
            if (choices[j] == choice_temp) {
              is_duplicate = true;
              break;
            }
          }
        }
        choice = choice_temp;
        std::cout << "choice:" << choice << std::endl;
        if (choice != 9) {
          choices.push_back(choice_temp);
        }
        time(&start_time_func);
      }
    }
    // в случае потери соединения
    if (status != LIBUSB_SUCCESS && status != LIBUSB_ERROR_TIMEOUT) {
      connection = false;
      std::cout << "sttaus:" << status << std::endl;
      // отсоединение
      libusb_close(device_handle);
      device_handle = nullptr;
      libusb_exit(ctx);
      // повторное соединение
      open_usb();

      WIDTH = MATRIX_PER_MODULE * 64;
      send_config(WIDTH, HEIGHT, device_handle);

      _frame.init(WIDTH, HEIGHT);
    }
  }

  return 0;
}
