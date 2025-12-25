#!/bin/bash

set -e

CYAN='\033[0;36m'
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m'

INSTALL_DIR="/opt/JobExchange"
REPO_URL="https://github.com/artemii125/JobExchange/archive/refs/tags/v1.0.0.tar.gz"

echo -e "${CYAN}╔════════════════════════════════════════╗${NC}"
echo -e "${CYAN}║   JobExchange - Установщик v1.0       ║${NC}"
echo -e "${CYAN}╚════════════════════════════════════════╝${NC}\n"

# Проверка прав root
if [ "$EUID" -ne 0 ]; then 
    echo -e "${RED}Запустите с правами root: sudo $0${NC}"
    exit 1
fi

# Определение дистрибутива
detect_distro() {
    if [ -f /etc/os-release ]; then
        . /etc/os-release
        DISTRO=$ID
    else
        echo -e "${RED}Не удалось определить дистрибутив${NC}"
        exit 1
    fi
}

# Установка зависимостей
install_dependencies() {
    echo -e "${CYAN}[1/6] Установка зависимостей...${NC}"
    
    case $DISTRO in
        ubuntu|debian|linuxmint|pop)
            apt update
            apt install -y \
                cmake \
                build-essential \
                qt6-base-dev \
                qt6-base-dev-tools \
                libqt6sql6-psql \
                libqt6sql6 \
                docker.io \
                docker-compose \
                git \
                wget \
                curl
            ;;
        fedora)
            dnf install -y \
                cmake \
                gcc-c++ \
                qt6-qtbase-devel \
                qt6-qtbase \
                qt6-qtsql-postgresql \
                docker \
                docker-compose \
                git \
                wget \
                curl
            ;;
        arch|manjaro)
            pacman -Sy --noconfirm \
                cmake \
                base-devel \
                qt6-base \
                postgresql-libs \
                docker \
                docker-compose \
                git \
                wget \
                curl
            ;;
        opensuse*|sles)
            zypper install -y \
                cmake \
                gcc-c++ \
                qt6-base-devel \
                libqt6-sql-postgresql \
                docker \
                docker-compose \
                git \
                wget \
                curl
            ;;
        *)
            echo -e "${RED}Дистрибутив $DISTRO не поддерживается автоматически${NC}"
            echo -e "${YELLOW}Установите вручную: cmake, g++, Qt6, Docker, git${NC}"
            exit 1
            ;;
    esac
    
    echo -e "${GREEN}✓ Зависимости установлены${NC}"
}

# Настройка Docker
setup_docker() {
    echo -e "\n${CYAN}[2/6] Настройка Docker...${NC}"
    
    systemctl start docker
    systemctl enable docker
    
    # Добавление текущего пользователя в группу docker
    if [ -n "$SUDO_USER" ]; then
        usermod -aG docker $SUDO_USER
        echo -e "${GREEN}✓ Пользователь $SUDO_USER добавлен в группу docker${NC}"
    fi
    
    echo -e "${GREEN}✓ Docker настроен${NC}"
}

# Скачивание исходников
download_sources() {
    echo -e "\n${CYAN}[3/6] Скачивание исходников...${NC}"
    
    TEMP_DIR=$(mktemp -d)
    cd $TEMP_DIR
    
    # Если запускается из папки с проектом, используем текущую директорию
    if [ -f "$OLDPWD/CMakeLists.txt" ]; then
        echo -e "${YELLOW}Используются локальные исходники${NC}"
        cp -r "$OLDPWD" ./JobExchange
    else
        echo -e "${YELLOW}Клонирование из репозитория...${NC}"
        git clone https://github.com/artemii125/JobExchange.git || {
            echo -e "${RED}Ошибка клонирования репозитория${NC}"
            exit 1
        }
    fi
    
    cd JobExchange
    echo -e "${GREEN}✓ Исходники получены${NC}"
}

# Компиляция
compile_project() {
    echo -e "\n${CYAN}[4/6] Компиляция проекта...${NC}"
    
    rm -rf build
    mkdir build && cd build
    
    cmake .. || {
        echo -e "${RED}Ошибка CMake${NC}"
        exit 1
    }
    
    make -j$(nproc) || {
        echo -e "${RED}Ошибка компиляции${NC}"
        exit 1
    }
    
    echo -e "${GREEN}✓ Проект скомпилирован${NC}"
}

# Установка
install_application() {
    echo -e "\n${CYAN}[5/6] Установка приложения...${NC}"
    
    # Создание директорий
    mkdir -p "$INSTALL_DIR/bin"
    mkdir -p "$INSTALL_DIR/db"
    
    # Копирование файлов
    cp build/JobExchange "$INSTALL_DIR/bin/"
    cp -r db/* "$INSTALL_DIR/db/"
    cp docker-compose.yml "$INSTALL_DIR/"
    
    # Создание wrapper скрипта
    cat > /usr/local/bin/JobExchange << 'EOF'
#!/bin/bash

show_help() {
    echo "JobExchange - Система управления биржей труда"
    echo ""
    echo "Использование:"
    echo "  JobExchange           Запустить приложение"
    echo "  JobExchange --help    Показать эту справку"
    echo "  JobExchange --uninstall   Удалить приложение"
    echo ""
}

uninstall() {
    echo "Удаление JobExchange..."
    
    # Остановка базы данных
    cd /opt/JobExchange 2>/dev/null
    if [ -f docker-compose.yml ]; then
        if command -v docker-compose &> /dev/null; then
            sudo docker-compose down -v
        else
            sudo docker compose down -v
        fi
    fi
    
    # Удаление файлов
    sudo rm -rf /opt/JobExchange
    sudo rm -f /usr/local/bin/JobExchange
    sudo rm -f /usr/share/applications/JobExchange.desktop
    
    echo "JobExchange удален"
}

case "$1" in
    --help|-h)
        show_help
        ;;
    --uninstall)
        uninstall
        ;;
    *)
        cd /opt/JobExchange
        ./bin/JobExchange "$@"
        ;;
esac
EOF
    chmod +x /usr/local/bin/JobExchange
    
    # Создание .desktop файла
    cat > /usr/share/applications/JobExchange.desktop << EOF
[Desktop Entry]
Version=1.0
Type=Application
Name=JobExchange
Comment=Система управления биржей труда
Exec=/usr/local/bin/JobExchange
Icon=applications-office
Terminal=false
Categories=Office;Database;
EOF
    
    echo -e "${GREEN}✓ Приложение установлено в $INSTALL_DIR${NC}"
}

# Запуск базы данных
setup_database() {
    echo -e "\n${CYAN}[6/6] Настройка базы данных...${NC}"
    
    cd "$INSTALL_DIR"
    
    # Определение команды docker compose
    if command -v docker-compose &> /dev/null; then
        DOCKER_CMD="docker-compose"
    else
        DOCKER_CMD="docker compose"
    fi
    
    # Остановка старых контейнеров
    $DOCKER_CMD down -v 2>/dev/null || true
    
    # Запуск
    $DOCKER_CMD up -d || {
        echo -e "${RED}Ошибка запуска базы данных${NC}"
        exit 1
    }
    
    echo -e "${YELLOW}Ожидание запуска PostgreSQL...${NC}"
    sleep 5
    
    echo -e "${GREEN}✓ База данных запущена${NC}"
}

# Очистка
cleanup() {
    echo -e "\n${CYAN}Очистка временных файлов...${NC}"
    cd /
    rm -rf $TEMP_DIR
}

# Главная функция
main() {
    detect_distro
    install_dependencies
    setup_docker
    download_sources
    compile_project
    install_application
    setup_database
    cleanup
    
    echo -e "\n${GREEN}╔════════════════════════════════════════╗${NC}"
    echo -e "${GREEN}║   Установка успешно завершена!        ║${NC}"
    echo -e "${GREEN}╚════════════════════════════════════════╝${NC}\n"
    echo -e "Запуск: ${CYAN}JobExchange${NC}"
    echo -e "Или через меню приложений\n"
    echo -e "${YELLOW}Примечание: Перелогиньтесь для применения прав Docker${NC}\n"
}

main
