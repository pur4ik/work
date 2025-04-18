#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <stdarg.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

class Buev_tcp {
private:
  const char *message;
  fd_set wfds_, rfds_;
  static constexpr const char *BUEV_IP_ADDR = "10.7.8.101";
  static constexpr int BUEV_TCP_PORT = 19004;

  int sock_;                // Дескриптор сокета
  struct sockaddr_in addr_; // Адрес сервера
  struct timeval tv_;       // Таймаут для select

public:
int g_opt = -1;
  ~Buev_tcp() { closeSocket(); }
  void init ();
  int connectToServer();
  int sendMessageAndReceiveResponse(int mode);
  void closeSocket();
};

extern Buev_tcp buevg_tcp;
