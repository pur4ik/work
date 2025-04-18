#include <GLFW/glfw3.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <iostream>
#include <thread>

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "image.h"
#include "imgui.h"
#include "panels.h"
#include "utils.h"
#include <unistd.h>

extern "C" {
#include "bmp.h"
}

int current_iface_id;
bool all_ifaces = false;

std::vector<std::string> ifaces = Utils::getNetworkInterfaces(all_ifaces);
std::string current_iface = ifaces[0];

std::vector<std::unique_ptr<Panels>> allPanel;
Image _frame;

bool isComboActive = true;
static bool open = true;
void draw_gui() {
  // Увеличиваем окно
  ImGui::SetNextWindowPos(ImVec2(0, 0));
  ImGui::SetNextWindowSize(ImVec2(1920, 1080)); // Увеличенное окно
  ImGui::Begin("Диагностика платы развертки##MainWindow", &open,
               ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

  ImGui::SetCursorPos(ImVec2(1300, 50));
  if (ImGui::Checkbox("Отображать все интерфейсы", &all_ifaces)) {
    ifaces = Utils::getNetworkInterfaces(all_ifaces);
  }

  ImGui::SetCursorPos(ImVec2(1300, 25));
  if (ifaces.empty()) {
    ImGui::Text("Нет доступных сетевых интерфейсов");
  } else {

    ImGui::Text("Выберите сетевой интерфейс:");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(150);
    ImGui::BeginDisabled(!isComboActive);
    if (ImGui::BeginCombo("##combo", current_iface.c_str())) { // Изначально выбран элемент current_iface
      for (size_t i = 0; i < ifaces.size(); ++i) {
        // Устанавливаем флаг is_selected на true для current_iface
        bool is_selected = (current_iface_id == i);
        // Создаем пункт выпадающего списка
        if (ImGui::Selectable(ifaces[i].c_str(), is_selected)) {
          if (current_iface_id != i) {
            current_iface_id = i;
            // all_ifaces = true;
            current_iface = ifaces[current_iface_id];          // Устанавливаем выбранный элемент
            std::cout << "выбран: " << ifaces[i] << std::endl; // Выводим выбранный элемент в консоль
            isComboActive = false;
            allPanel.emplace_back(std::make_unique<Panels>(MDV1, "МДВ-1", 20, 25, 128, 64, ifaces[i]));
            allPanel.emplace_back(std::make_unique<Panels>(MDV2_MDN, "МДВ-2", 20, 270, 320, 64, ifaces[i]));
            allPanel.emplace_back(std::make_unique<Panels>(MDV2_MDN, "МДН", 20, 500, 320, 64, ifaces[i]));
            allPanel.emplace_back(std::make_unique<Panels>(MDV3, "МДВ-3", 650, 25, 128, 64, ifaces[i]));
            allPanel.emplace_back(std::make_unique<Panels>(IDGK, "ЖК", 0, 0, 0, 0, ifaces[i]));
          }
        }

        if (is_selected) {
          ImGui::SetItemDefaultFocus();
        }
      }
      ImGui::EndCombo(); // Закрываем комбинированный список
    }
    ImGui::EndDisabled();
  }

  int all_packet = 0;
  int all_error_packet = 0;

  for (auto &single_panel : allPanel) {
    all_packet += single_panel.get()->get_count_packet_main();
    all_error_packet += single_panel.get()->get_count_packet_error_main();
    single_panel.get()->drawPanel();
  }

  ImGui::SetCursorPos(ImVec2(1650, 950));

  ImGui::Text("Общее количество всех пакетов: %d", all_packet);
  ImGui::SetCursorPos(ImVec2(1650, 975));
  ImGui::Text("Общее количество битых пакетов:");
  if (all_error_packet <= 0) {
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
  } else {
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
  }
  ImGui::SameLine();
  ImGui::TextUnformatted(std::to_string(all_error_packet).c_str());
  ImGui::PopStyleColor();
  ImGui::End();
}

int main() {

  if (!glfwInit()) {
    std::cerr << "Не удалось инициализировать GLFW" << std::endl;
    return -1;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Создаем окно с помощью GLFW
  GLFWwindow *window = glfwCreateWindow(1920, 1080, "Диагностика платы развертки", NULL, NULL);
  if (window == nullptr) {
    std::cerr << "Не удалось создать окно GLFW" << std::endl;
    glfwTerminate();
    return -1;
  }

  if (!glfwInit()) {
    const char *description;
    int code = glfwGetError(&description);
    std::cerr << "Ошибка GLFW: " << description << " (код " << code << ")" << std::endl;
    return -1;
  }

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1); // Включить вертикальную синхронизацию

  // Инициализация ImGui
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui::StyleColorsDark();

  // Настройка бекендов
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 130");

  ImGuiIO &io = ImGui::GetIO();
  io.Fonts->AddFontFromFileTTF("fonts/tilda-sans_medium.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesCyrillic());
  io.Fonts->Build(); // Построение шрифтов

  // Основной цикл gui
  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
    // Начало нового кадра ImGui
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    draw_gui(); // Вызов функции отрисовки
                // интерфейса

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

  // Очистка ресурсов
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}
