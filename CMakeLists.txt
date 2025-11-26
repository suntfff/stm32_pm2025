cmake_minimum_required(VERSION 3.0)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

project(stm32_template C)

set(TARGET_NAME ${PROJECT_NAME})

set(ARM_TOOLCHAIN_PATH $ENV{ARM_TOOLCHAIN_PATH})
set(CMAKE_C_COMPILER ${ARM_TOOLCHAIN_PATH}arm-none-eabi-gcc)
set(OBJCOPY ${ARM_TOOLCHAIN_PATH}arm-none-eabi-objcopy)
set(STFLASH st-flash)

set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})
set(CMAKE_EXECUTABLE_SUFFIX ".elf")

set(CMAKE_C_FLAGS "-mcpu=cortex-m3 -mthumb -std=c99")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fdata-sections -ffunction-sections -mlittle-endian -g3")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-move-loop-invariants -fsigned-char -ffreestanding")

set(LINKER_FLAGS "-nostdlib -nostartfiles")
set(LINKER_FLAGS "${LINKER_FLAGS} -Xlinker -Map -Xlinker ${TARGET_NAME}.map ")
set(LINKER_FLAGS "${LINKER_FLAGS} -Xlinker -T ${CMAKE_CURRENT_SOURCE_DIR}/stm32f103c8.ld")
set(LINKER_FLAGS "${LINKER_FLAGS} -Wl,--gc-sections ")
set(CMAKE_EXE_LINKER_FLAGS ${LINKER_FLAGS})

add_definitions(-DSTM32F103C8)
add_definitions(-DSTM32F10X_MD)

include_directories(cmsis)
include_directories(stm32)

set(SOURCE_FILES
    stm32/startup/startup_stm32f10x_md.c
    stm32/system_stm32f10x.c
    syscalls/syscalls.c
    main.c)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/.gdbinit DESTINATION ${EXECUTABLE_OUTPUT_PATH})

set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "")
set(CMAKE_SHARED_LIBRARY_LINK_FLAGS "")
set(CMAKE_EXE_LINKER_FLAGS_INIT "")

add_executable(${TARGET_NAME} ${SOURCE_FILES})

set(BIN_TARGET "${EXECUTABLE_OUTPUT_PATH}/${TARGET_NAME}.bin")
set(HEX_TARGET "${EXECUTABLE_OUTPUT_PATH}/${TARGET_NAME}.hex")
set(ELF_TARGET "${EXECUTABLE_OUTPUT_PATH}/${TARGET_NAME}${CMAKE_EXECUTABLE_SUFFIX}")

add_custom_command(OUTPUT ${BIN_TARGET}
                    COMMAND ${OBJCOPY} -O binary ${ELF_TARGET} ${BIN_TARGET}
                    DEPENDS ${ELF_TARGET} ${PROJECT_NAME})
add_custom_target(bin DEPENDS ${BIN_TARGET})

add_custom_command(OUTPUT ${HEX_TARGET}
                    COMMAND ${OBJCOPY} -O ihex ${ELF_TARGET} ${HEX_TARGET}
                    DEPENDS ${ELF_TARGET} ${PROJECT_NAME})
add_custom_target(hex DEPENDS ${HEX_TARGET})

add_custom_command(OUTPUT flash_target
        COMMAND ${STFLASH} write ${BIN_TARGET} 0x8000000
        DEPENDS ${BIN_TARGET})
add_custom_target(flash DEPENDS flash_target)
