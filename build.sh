#!/bin/bash

echo "Создание одного исполняемого файла..."

# Очистка предыдущих сборок
rm -rf build
mkdir build
cd build

# Конфигурация с оптимизацией размера
cmake .. -DCMAKE_BUILD_TYPE=MinSizeRel

# Сборка
make -j$(nproc)

echo "Готово! Исполняемый файл: build/JobExchange"
echo "Запуск: ./build/JobExchange"
