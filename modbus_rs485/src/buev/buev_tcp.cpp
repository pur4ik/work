#include "buev_tcp.h"

Buev_tcp buevg_tcp;

void Buev_tcp:: init() {
  {
    // Создание сокета
    sock_ = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_ < 0) {
      std::cerr << "ERROR: Can't open TCP socket!" << std::endl;
      return;
    }

    // Установка неблокирующего режима
    int flags = fcntl(sock_, F_GETFL, 0);
    if (flags == -1 || fcntl(sock_, F_SETFL, flags | O_NONBLOCK) == -1) {
      std::cerr << "ERROR: Can't set non-blocking mode!" << std::endl;
      closeSocket();
      return;
    }

    // Настройка адреса сервера
    memset(&addr_, 0, sizeof(addr_));
    addr_.sin_family = AF_INET;
    addr_.sin_port = htons(BUEV_TCP_PORT);
    if (inet_pton(AF_INET, BUEV_IP_ADDR, &addr_.sin_addr) <= 0) {
      std::cerr << "ERROR: Invalid BUEV IP address!" << std::endl;
      closeSocket();
      return;
    }

    // Подключение к серверу
    if (connectToServer() < 0) {
      return;
    }

    // Возвращение в блокирующий режим
    if (fcntl(sock_, F_SETFL, flags & ~O_NONBLOCK) == -1) {
      std::cerr << "ERROR: Can't set blocking mode!" << std::endl;
      closeSocket();
      return;
    }

    std::cout << "Connected to BUEV TCP service." << std::endl;
  }
}

int Buev_tcp::connectToServer() {
  int res = connect(sock_, reinterpret_cast<struct sockaddr *>(&addr_),
                    sizeof(addr_));
  if (res < 0) {
    if (errno == EINPROGRESS) {
      FD_ZERO(&wfds_);
      FD_SET(sock_, &wfds_);
      tv_.tv_sec = 3;
      tv_.tv_usec = 0;

      res = select(sock_ + 1, nullptr, &wfds_, nullptr, &tv_);
      if (res < 0) {
        std::cerr << "ERROR: TCP socket select() failed during connection!"
                  << std::endl;
        return -1;
      } else if (res == 0) {
        std::cerr << "ERROR: Connect to BUEV TCP service timeout!" << std::endl;
        return -1;
      }
    } else {
      std::cerr << "ERROR: Can't connect to BUEV TCP service!" << std::endl;
      return -1;
    }
  }
  return 0;
}

int Buev_tcp::sendMessageAndReceiveResponse(int mode) {
  char buf[24];
  memset(buf, 0, sizeof(buf));

  if (mode == 1) {
    message = "TRACE 1 \n";
  } else {
    message = "TRACE 0 \n";
  }

  // Отправка сообщения
  int msglen = strlen(message);
  int res = write(sock_, message, msglen);
  if (res < 0) {
    std::cerr << "ERROR: Can't send message to BUEV TCP service!" << std::endl;
    return -1;
  }

  // Ожидание ответа
  FD_ZERO(&rfds_);
  FD_SET(sock_, &rfds_);
  tv_.tv_sec = 0;
  tv_.tv_usec = 500000; // 500 мс
  res = select(sock_ + 1, &rfds_, nullptr, nullptr, &tv_);
  if (res < 0) {
    std::cerr << "ERROR: TCP socket select() failed!" << std::endl;
    return -1;
  } else if (res == 0) {
    std::cerr << "ERROR: BUEV TCP service response timeout!" << std::endl;
    return -1;
  }

  // Чтение ответа
  res = read(sock_, buf, sizeof(buf) - 1);
  if (res < 0) {
    std::cerr << "ERROR: Can't read response from BUEV TCP service!"
              << std::endl;
    return -1;
  }

  buf[res] = '\0'; // Добавляем завершающий символ
  std::cout << "Response: " << buf << std::endl;

  return 0;
}

void Buev_tcp::closeSocket() {
  if (sock_ >= 0) {
    close(sock_);
    sock_ = -1; // Обнуляем дескриптор после закрытия
  }
}