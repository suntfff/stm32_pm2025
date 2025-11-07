#!/bin/bash
# Умная сборка STM32 проекта с гарантированным обновлением ключевых файлов

set -e  # остановка при любой ошибке

PROJECT_ROOT="$(pwd)"
BUILD_DIR="$PROJECT_ROOT/build"
TARGET="stm32_template"
SRC_KEY_FILES=("src/main.c" "src/stm32_init.c") # ключевые файлы, всегда пересобираем

# 1️⃣ Создаём build, если нет
if [ ! -d "$BUILD_DIR" ]; then
    echo "Создаём папку сборки..."
    mkdir -p "$BUILD_DIR"
    echo "Запускаем первую конфигурацию CMake..."
    CXX=/usr/bin/false cmake -S "$PROJECT_ROOT" -B "$BUILD_DIR" \
        -DCMAKE_C_COMPILER=arm-none-eabi-gcc \
        -DCMAKE_EXPORT_COMPILE_COMMANDS=1
fi

# 2️⃣ Помечаем ключевые файлы как изменённые
for f in "${SRC_KEY_FILES[@]}"; do
    if [ -f "$PROJECT_ROOT/$f" ]; then
        touch "$PROJECT_ROOT/$f"
    fi
done

# 3️⃣ Сборка цели
echo "Сборка цели $TARGET..."
cmake --build "$BUILD_DIR" --target "$TARGET" -- VERBOSE=1

# 4️⃣ Конвертация ELF в BIN
ELF_FILE="$BUILD_DIR/$TARGET.elf"
BIN_FILE="$BUILD_DIR/$TARGET.bin"

echo "Создание бинарника $BIN_FILE..."
arm-none-eabi-objcopy -O binary "$ELF_FILE" "$BIN_FILE"

echo "Сборка завершена успешно! Бинарник готов: $BIN_FILE"

