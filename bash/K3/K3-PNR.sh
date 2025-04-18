#!/bin/bash

echo "Поиск камер в сети..."

ITERATION_FILE="iteration.txt" 
BASE_CAM_IP="192.168.100."
MIN_ITERATION=1
MAX_ITERATION=16
INTERFACE=""
MY_IP=""
MY_NEW_IP=""
MASK="255.255.255.0"

all_interfaces=$(ip link | grep -oP '(?<=\d:\s)[^:@]+' | grep -v lo)

for interface in $all_interfaces; do
    state=$(ip link show "$interface" | grep -oP '(?<=state\s)\w+')
    if [[ "$state" == "UP" ]]; then
        INTERFACE="$interface"
        MY_IP=$(ip addr show "$INTERFACE" | grep -oP '(?<=inet\s)\d+(\.\d+){3}' | head -n 1)
        break
    fi
done

ip_to_int() {
    local ip="$1"
    IFS='.' read -r i1 i2 i3 i4 <<< "$ip"
    echo $(( (i1 << 24) | (i2 << 16) | (i3 << 8) | i4 ))
}

apply_mask() {
    local ip="$1"
    local mask="$2"
    local ip_int=$(ip_to_int "$ip")
    local mask_int=$(ip_to_int "$mask")
    echo $(( ip_int & mask_int ))
}

cam=`nmap -sT -p 554 192.168.1.1-254 -oG - | grep 554/open | cut -d' ' -f2`

if [ -z "$cam" ]; then
    cam=$(/usr/bin/python3 zero_conf.py)
    cam=$(echo "$cam" | tr -d "[]'")
    if [[ ! "$cam" =~ ^[0-9]+\.[0-9]+\.[0-9]+\.[0-9]+$ ]]; then
        echo "zero-conf не вернул корректный IP-адрес."
    fi
else
    echo ""
fi

if [ -z "$cam" ]; then
    cam=`nmap -sT -p 554 192.168.157.1-254 -oG - | grep 554/open | cut -d' ' -f2`
fi

echo "Обнаружены камеры с адресами:"
echo "${cam}"

net1=$(apply_mask "$MY_IP" "$MASK")
net2=$(apply_mask "$cam" "$MASK")

if [ "$net1" -eq "$net2" ]; then
    echo "..."
else
    MY_NEW_IP=$(echo "$cam" | awk -F. '{print $1"."$2"."$3".254"}')
    sudo ip addr add "$MY_NEW_IP/$MASK" dev "$INTERFACE" 
fi

if [ -f "$ITERATION_FILE" ]; then
    ITERATION=$(cat "$ITERATION_FILE")
    if [ "$ITERATION" -lt "$MIN_ITERATION" ] || [ "$ITERATION" -gt "$MAX_ITERATION" ]; then
        ITERATION=$MIN_ITERATION
    fi
else
    ITERATION=$MIN_ITERATION
fi

max_attempts=3
attempt=0
success=0

while [ $attempt -lt $max_attempts ]; do
    attempt=$((attempt + 1))
    echo "Попытка настройки камеры $attempt из $max_attempts..."
    
    ./set-config.sh ${cam}
    if [ $? -eq 0 ]; then
        success=1
        break
    fi
    
    echo "Ошибка настройки камеры, попытка $attempt из $max_attempts"
    sleep 2
done

if [ $success -eq 1 ]; then
    NEW_CAM_IP="${BASE_CAM_IP}${ITERATION}"
    echo "Настройка камеры прошла успешно"
    echo "Запуск воспроизведения видеопотока с камеры. Для завершения нажмите CTRL+C."
    ffplay rtsp://admin:admin123@${NEW_CAM_IP}
else
    echo "ОШИБКА! Не удалось настроить камеру после $max_attempts попыток!"
    exit 1
fi

if [ -n "$MY_NEW_IP" ]; then
    sudo ip addr del "$MY_NEW_IP/$MASK" dev "$INTERFACE"
else
    echo "..."
fi
