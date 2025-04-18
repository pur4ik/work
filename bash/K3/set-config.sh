#!/bin/sh

IP=192.168.1.108
CFGFILE=dahua-camera-config-minimal-k3.cfg
USER=admin
PASS=admin123

ITERATION_FILE="iteration.txt"
BASE_IP="192.168.100."
MIN_ITERATION=1
MAX_ITERATION=16

if [ -f "$ITERATION_FILE" ]; then
    ITERATION=$(cat "$ITERATION_FILE")
else
    ITERATION=$MIN_ITERATION
fi

if [ "$ITERATION" -lt "$MIN_ITERATION" ] || [ "$ITERATION" -gt "$MAX_ITERATION"  ]; then
    ITERATION=$MIN_ITERATION
fi

NEW_IP="${BASE_IP}${ITERATION}"

if [ ! -z "$1" ]; then
    IP=$1
fi

if [ ! -z "$2" ]; then
    CFGFILE=$2
fi

# Функция для отправки команды до тех пор, пока ответ не будет "OK"
send_command_until_ok() {
    local url="$1"
    local description="$2"
    local max_attempts=3
    local attempt=0
    
    while [ $attempt -lt $max_attempts ]; do
        resp=$(wget "$url" -o /dev/null -O - 2> /dev/null)
        resp=$(echo "$resp" | sed -e 's/\r//gm')
        echo "$resp"
        if [ "$resp" = "OK" ]; then
            return 0
        else
            attempt=$((attempt + 1))
            echo "Попытка $attempt из $max_attempts: не удалось отправить $description..."
            sleep 1
        fi
    done
    
    echo "Ошибка: превышено максимальное количество попыток ($max_attempts) для $description"
    return 1
}

# Установка DHCP
DHCP_param="Network.eth0.DhcpEnable=false"
bline=$(echo "$DHCP_param" | sed -e 's/&/%26/g')
echo "$DHCP_param"
send_command_until_ok "http://${USER}:${PASS}@${IP}/cgi-bin/configManager.cgi?action=setConfig&${bline}" "DHCP"
if [ $? -ne 0 ]; then
    exit 1
fi

# Чтение конфигурационного файла и отправка команд
while IFS= read -r line; do
    echo "$line"
    bline=$(echo "$line" | sed -e 's/&/%26/g')
    send_command_until_ok "http://${USER}:${PASS}@${IP}/cgi-bin/configManager.cgi?action=setConfig&${bline}"
    if [ $? -ne 0 ]; then
        exit 1
    fi

    # Проверка, что значение действительно установлено
    #req=$(echo "$line" | cut -d'=' -f1 | sed -e 's/&/%26/g')
    #val=$(echo "$line" | cut -d'=' -f2- | sed -e 's/&/%26/g')

    #resp=$(wget "http://${USER}:${PASS}@${IP}/cgi-bin/configManager.cgi?action=getConfig&name=${req}" -o /dev/null -O - 2> /dev/null)
   # newval=$(echo "$resp" | sed -e 's/\r//gm' -e 's/^[^=]*=//gm')
    #if [ "$val" != "$newval" ]; then
     #   echo "Ошибка: параметр не был установлен (ожидалось: $val, получено: $newval)"
    #    exit 1
   # fi
done < "$CFGFILE"

# Установка нового IP-адреса
param="Network.eth0.IPAddress=${NEW_IP}"
bline=$(echo "$param" | sed -e 's/&/%26/g')
echo "$param"
send_command_until_ok "http://${USER}:${PASS}@${IP}/cgi-bin/configManager.cgi?action=setConfig&${bline}"
if [ $? -ne 0 ]; then
    exit 1
fi

# Обновление счетчика итераций
ITERATION=$((ITERATION + 1))
echo "$ITERATION" > "$ITERATION_FILE"

exit 0
