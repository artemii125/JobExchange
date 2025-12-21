#!/bin/bash

# Цвета для вывода
CYAN='\033[0;36m'
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m'

echo -e "${CYAN}Начало автоматической установки JobExchange...${NC}"

# 1. Определение дистрибутива и менеджера пакетов
if [ -f /etc/debian_version ]; then
    PKM="sudo apt update && sudo apt install -y"
    DEPS="cmake build-essential qt6-base-dev libqt6sql6-psql docker-compose"
elif [ -f /etc/fedora-release ]; then
    PKM="sudo dnf install -y"
    DEPS="cmake gcc-c++ qt6-qtbase-devel qt6-qtsql-postgresql docker-compose"
elif [ -f /etc/arch-release ]; then
    PKM="sudo pacman -S --noconfirm"
    DEPS="cmake base-devel qt6-base docker-compose"
else
    echo -e "${RED}Неподдерживаемый дистрибутив. Установите Qt6 и Docker вручную.${NC}"
    exit 1
fi

# 2. Установка системных зависимостей
echo -e "\n${CYAN}[1/3] Установка зависимостей...${NC}"
eval $PKM $DEPS

# 3. Сборка и глобальная установка
echo -e "\n${CYAN}[2/3] Сборка и копирование файлов...${NC}"
mkdir -p build && cd build
cmake ..
if make -j$(nproc); then
    sudo make install
    sudo ldconfig
else
    echo -e "${RED}Ошибка компиляции!${NC}"
    exit 1
fi

# 4. Запуск базы данных
echo -e "\n${CYAN}[3/3] Запуск Docker-контейнера с БД...${NC}"
cd ..
if sudo docker-compose up -d; then
    echo -e "${GREEN}База данных запущена успешно.${NC}"
else
    echo -e "${RED}Ошибка запуска Docker. Убедитесь, что сервис docker активен.${NC}"
fi

echo -e "\n${GREEN}Установка завершена! Теперь вы можете запустить программу командой: JobExchange${NC}"