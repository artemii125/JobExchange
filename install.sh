#!/bin/bash

# Цвета
CYAN='\033[0;36m'
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m'

INSTALL_DIR="/opt/JobExchange"

echo -e "${CYAN}=== Установка JobExchange ===${NC}"

# 1. Установка зависимостей
echo -e "\n${CYAN}[1/4] Проверка и установка зависимостей...${NC}"
if [ -f /etc/debian_version ]; then
    sudo apt update && sudo apt install -y cmake build-essential qt6-base-dev libqt6sql6-psql docker.io docker-compose-plugin
elif [ -f /etc/fedora-release ]; then
    sudo dnf install -y cmake gcc-c++ qt6-qtbase-devel qt6-qtsql-postgresql docker docker-compose-plugin
elif [ -f /etc/arch-release ]; then
    sudo pacman -S --noconfirm cmake base-devel qt6-base postgresql-libs docker docker-compose
else
    echo -e "${RED}Дистрибутив не определен. Установите зависимости вручную.${NC}"
fi

# Включение Docker демона, если он не запущен
sudo systemctl start docker
sudo systemctl enable docker

# 2. Сборка
echo -e "\n${CYAN}[2/4] Компиляция проекта...${NC}"
rm -rf build # Чистим старую сборку
mkdir -p build && cd build
cmake ..
if ! make -j$(nproc); then
    echo -e "${RED}Ошибка компиляции!${NC}"
    exit 1
fi
cd ..

# 3. "Ручная" установка в /opt (чтобы сохранить пути к queries.sql)
echo -e "\n${CYAN}[3/4] Установка программы в $INSTALL_DIR...${NC}"

# Создаем папку установки
sudo mkdir -p "$INSTALL_DIR/bin"
sudo mkdir -p "$INSTALL_DIR/db"

# Копируем бинарник
sudo cp build/JobExchange "$INSTALL_DIR/bin/"

# Копируем SQL файлы (ВАЖНО для твоего кода)
sudo cp db/queries.sql "$INSTALL_DIR/db/"
# init.sql нужен только докеру, в папку bin его можно не класть, 
# но docker-compose должен лежать рядом для запуска базы.
sudo cp docker-compose.yml "$INSTALL_DIR/"
sudo cp -r db "$INSTALL_DIR/" 

# Создаем скрипт запуска в /usr/bin, чтобы запускать из терминала
echo -e "#!/bin/bash\ncd $INSTALL_DIR/bin\n./JobExchange" | sudo tee /usr/local/bin/JobExchange > /dev/null
sudo chmod +x /usr/local/bin/JobExchange

# 4. Запуск Базы Данных из папки установки
echo -e "\n${CYAN}[4/4] Настройка базы данных...${NC}"
cd "$INSTALL_DIR"

# Проверка: docker-compose или docker compose?
if command -v docker-compose &> /dev/null; then
    DOCKER_CMD="docker-compose"
else
    DOCKER_CMD="docker compose"
fi

echo "Используется команда: $DOCKER_CMD"
sudo $DOCKER_CMD down -v 2>/dev/null # Сброс старой если была
if sudo $DOCKER_CMD up -d; then
    echo -e "${GREEN}База данных успешно запущена!${NC}"
else
    echo -e "${RED}Ошибка запуска Docker.${NC}"
    exit 1
fi

echo -e "\n${GREEN}==============================================${NC}"
echo -e "${GREEN}Установка завершена!${NC}"
echo -e "Запускайте программу командой: ${CYAN}JobExchange${NC}"