#include "socket.h"
#include "../utils.h"

Socket _socket;

void Socket::init_socket()
{
    struct sockaddr_in addr;

    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1)
    {
        util.take_logs("Ошибка при создании сокета!", 'l');
        socket_fd = -1;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if (inet_pton(AF_INET, ip, &addr.sin_addr) <= 0)
    {
        util.take_logs("Ошибка IP-адреса!", 'l');
        close(socket_fd);
        socket_fd = -1;
    }

    if (connect(socket_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        util.take_logs("Ошибка при подключении!", 'l');
        close(socket_fd);
        socket_fd = -1;
    }
    if (socket_fd != -1){
        load_connection = true;
    }
}

int Socket::send(const char *str)
{
    ssize_t writed = write(socket_fd, str, 12);

    if (writed < 0)
    {
        util.take_logs("Ошибка при отправке данных!", 'l');
        return -1;
    }
    return 0;
}

int Socket::listen_reply()
{
    struct timeval timeout;
    timeout.tv_sec = 3;
    timeout.tv_usec = 0;

    if (setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0)
    {
        std::cerr << "Ошибка при установке таймаута для сокета" << std::endl;
        return -1;
    }

    char buffer[128];
    memset(buffer, 0, sizeof(buffer));
    int reply = recv(socket_fd, buffer, sizeof(buffer), 0);
    if (reply == 0)
    {
        std::cout << "Соединение закрыто удаленной стороной" << std::endl;
        return -1;
    }
    else if (reply > 0)
    {
        std::string message(buffer);
        std::string finish_message = "TRACE OFF";
        if (message.find(finish_message) != std::string::npos)
        {
            return 0;
            util.take_logs("Значение отправлено успешно!", 'l');
        }
        else
        {
            return -1;
            util.take_logs("Ошибка получения ответа!", 'l');
        }
    }
}
