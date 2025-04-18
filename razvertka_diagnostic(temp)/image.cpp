#include "image.h"

#include <libusb-1.0/libusb.h>
// #include <ncurses.h>
#include <unistd.h>

#include <chrono>
#include <cmath>
#include <cstring>
#include <iostream>
#include <thread>

#include "ft2build.h"
#include "ftfont.h"
#include FT_FREETYPE_H

extern "C" {
#include "bmp.h"
}

#define CONFIGURE_HEADER 0xFE

using namespace std;

int c_cycle = 0;
float offset = 0.0f;

// Image::Image(int WIDTH, int HEIGHT) {
//   width_image = WIDTH;
//   height_image = HEIGHT;
//   buffer.resize(WIDTH * HEIGHT * 2, 0);
// }

void Image::init(int WIDTH, int HEIGHT) {
  width_image = WIDTH;
  height_image = HEIGHT;
  buffer.clear();
  buffer.resize(WIDTH * HEIGHT * 2, 0);
}

void Image::setPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b) {
  if (x < 0 || x >= width_image || y < 0 || y >= height_image) {
    return;
  }

  // Индекс в буфере для пикселя (x, y)
  int index = (y * width_image + x) * 2;  // Умножаем на 2 для RGB555

  // Преобразуем значения RGB в формат RGB555
  uint16_t rgb555 = ((r >> 3) << 10) | ((g >> 3) << 5) | (b >> 3);
  buffer[index] = rgb555 & 0xff;    // Младший байт
  buffer[index + 1] = rgb555 >> 8;  // Старший байт
}

void Image::static_color(uint8_t r, uint8_t g, uint8_t b) {
  for (int iy = 0, HEIGHT = height_image, WIDTH = width_image; iy < HEIGHT; iy++) {
    for (int ix = 0; ix < WIDTH; ix++) {
      setPixel(ix, iy, r, g, b);
    }
  }
}


int prevx, prevregion, pr, pg, pb, count1 = 0;
uint32_t Image::rgb(float ratio) {
  // Нормализуем ratio для 6 регионов по 256 единиц
  int normalized = float(ratio * 256 * 6);

  // Находим регион (участок градиента) и смещение внутри региона
  int region = normalized / 256;
  int ix = normalized % 256;
  uint8_t r = 0, g = 0, b = 0;

  switch (region) {
    case 0:  // Красный → Желтый (увеличение зеленого)
      count1++;
      if (count1 > 33) {
        r = pr, g = pg, b = pb;
      } else {
        r = 255;
        g = ix;  // Зеленый увеличивается
        b = 0;
      };
      break;
    case 1:          // Желтый → Зеленый (уменьшение красного)
      r = 255 - ix;  // Красный уменьшается
      g = 255;
      b = 0;
      break;
    case 2:  // Зеленый → Голубой (увеличение синего)
      r = 0;
      g = 255;
      b = ix;  // Синий увеличивается
      break;
    case 3:  // Голубой → Синий (уменьшение зеленого)
      count1++;
      if (count1 < 10) {
        r = 0, g = pg - 1, b = 255;
      } else {
        r = 0;
        g = 255 - ix;  // Зеленый уменьшается
        b = 255;
      }
      break;
    case 4:    // Синий → Магента (увеличение красного)
      r = ix;  // Красный увеличивается
      g = 0;
      b = 255;
      break;
    case 5:  // Магента → Красный (уменьшен= ix;я
      r = 255;
      g = 0;
      b = 255 - ix;  // Синий уменьшается
      break;
      break;
  }

  if (prevregion != region) {
    count1 = 0;
  }
  pr = r, pg = g, pb = b;
  prevregion = region;

  // Возвращаем значение цвета в формате RGB
  return r + (g << 8) + (b << 16);
}

void Image::test() {
  float gradient_value = 0.0f;

  for (int iy = 0; iy <= height_image; iy++) {
    for (int ix = 0; ix <= width_image; ix++) {
      // нормализуем значение градиента
      gradient_value = (float)(ix + offset) / width_image;

      // нормализация gradient_value в пределах [0, 1]
      gradient_value -= floor(gradient_value);

      // вызов функции rgb для получения цвета по градиенту
      uint32_t color = rgb(gradient_value);  // rgb() возвращает 32-битное значение цвета

      // Разбиваем color на компоненты r, g, b
      uint8_t r = (color & 0xFF);        // младший байт — это r
      uint8_t g = (color >> 8) & 0xFF;   // следующий байт — это g
      uint8_t b = (color >> 16) & 0xFF;  // старший байт — это b
      // std::this_thread::sleep_for(std::chrono::milliseconds(10));

      // Устанавливаем пиксель на изображении
      setPixel(ix, iy, r, g, b);
    }
  }

  // смещение для анимации
  offset += 15.0f;
  /*if (offset >= width_image) {
    offset = 0.0f;  // сброс при выходе за пределы ширины изображения
  }*/

  std::this_thread::sleep_for(std::chrono::milliseconds(20));  // задержка для плавности
}

int getWidthText(const char *text) {
  Bitmap *b = bm_create(1, 1);

  int textWidthPixels = bm_text_width(b, text);

  bm_free(b);

  return textWidthPixels;
}

int getHeightText(const char *text) {
  Bitmap *b = bm_create(1, 1);

  int textHeightPixels = bm_text_height(b, text);

  bm_free(b);

  return textHeightPixels;
}

int shrink_count = 3;
bool ft_init = false;
bool shrink = false;
BmFont *font;
void Image::displayText(const char *color, const char *text) {
  if (strlen(text) == 0) {
    text = "Тестовое Сообщение";
  }

  int w = width_image;
  int h = height_image;

  static int x_Disp = -w;
  // Создаем Bitmap
  Bitmap *b = bm_create(w, h);

  // Инициализируем шрифт FreeType и загружаем шрифт
  if (ft_init == false) {
    bmft_init();
    font = bmft_load_font("fonts/tilda-sans_medium.ttf");
    if (!font) {
      fprintf(stderr, "Unable to load font\n");
      return;
    }
    ft_init = true;
  }

  // Устанавливаем шрифт и его размер для Bitmap
  bm_set_font(b, font);

  int c = bm_text_width(b, text);
  if (c > width_image) {
    shrink_count++;
    if (shrink_count > 6) {
      shrink_count = 6;
    }
    bmft_set_size(font, h / shrink_count);
  } else {
    if (c < width_image / 2) {
      shrink_count--;
      if (shrink_count < 3) {
        shrink_count = 3;
      }
    }
    bmft_set_size(font, h / shrink_count);
  }

  bm_set_color(b, bm_atoi("#FFFFFFFF"));

  // Рисуем текст на Bitmap
  bm_printf(b, 0, h / 2, text);

  // Проходим по каждому пикселю и выводим его на экран
  for (int y = 0; y < h; ++y) {
    for (int x = 0; x < w; ++x) {
      uint32_t pixel = bm_get(b, x, y);  // Получаем цвет пикселя
      uint8_t a = (pixel >> 24) & 0xFF;  // Alpha
      uint8_t r = (pixel >> 16) & 0xFF;  // Red
      uint8_t g = (pixel >> 8) & 0xFF;   // Green
      uint8_t b = pixel & 0xFF;          // Blue

      setPixel(x + x_Disp, y, r, g, b);  // Отправляем пиксель на экран
    }
  }

  x_Disp += 2;

  if (strcmp(text, "Автоматический режим") == 0) {
    x_Disp++;
  }

  if (x_Disp >= width_image) x_Disp = -w;
  // Освобождаем ресурсы
  bm_free(b);
}

void Image::position() {
  int z = 0;
  while (z <= width_image) {
    for (int x = 0; x <= width_image; x++) {
      for (int y = 0; y <= height_image; y++) {
        if ((x >= z) && (((x + y) <= z + 2))) {
          switch ((z / 64) % 4) {
            case 0:
              setPixel(x, y, 255, 0, 0);
              break;
            case 1:
              setPixel(x, y, 255, 255, 255);
              break;
            case 2:
              setPixel(x, y, 0, 255, 0);
              break;
            case 3:
              setPixel(x, y, 0, 0, 255);
              break;
          }
        }
      }
    }
    z += 64;
  }
}

std::vector<uint8_t> Image::reverseBuffer(){

}