cmake_minimum_required(VERSION 3.20)

set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/arm-none-eabi.cmake)

set(PROJECT_NAME drone)
set(CMAKE_EXPORT_COMPILE_COMMANDS true)
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")

project(${PROJECT_NAME} C ASM CXX)

add_compile_options(-mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard -DSTM32F401xx -fdata-sections -ffunction-sections  -Wall -Og
  )
add_link_options(-mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard -DSTM32F401xx -T${CMAKE_SOURCE_DIR}/STM32F401RETx_FLASH.ld --specs=nano.specs -u _printf_float -lc -lm -Wl,-Map=${CMAKE_SOURCE_DIR}/build/${CMAKE_PROJECT_NAME}.map,--cref -Wl,--gc-sections)

include_directories(/usr/include/eigen3/)

include_directories(./Cfg/Template)
aux_source_directory(./Cfg/Template allSrc)

include_directories(./Basic/Inc)
include_directories(./Source)
aux_source_directory(./ allSrc)
aux_source_directory(./Source/ allSrc)

include_directories(./Source/syscalls/inc)
aux_source_directory(./Source/syscalls allSrc)

include_directories(./Source/trace)
aux_source_directory(./Source/trace allSrc)

aux_source_directory(./Source/drivers allSrc)
include_directories(./Source/drivers/Inc)
aux_source_directory(./Source/drivers drvSrc)
add_library(drivers STATIC ${drvSrc})

include_directories(./Source/lib/inc)
aux_source_directory(./Source/lib mymathSrc)
add_library(mymath STATIC ${mymathSrc})

include_directories(./Source/OS)
include_directories(./Source/OS/Ports/stm32f401/GNU)
aux_source_directory(./Source/OS ucOSSrc)
aux_source_directory(./Source/OS/Ports/stm32f401/GNU ucOSSrc)
add_library(ucOS STATIC ${ucOSSrc})

include_directories(./Source/lib/embeddedLapack/src/Lapack/Include)
include_directories(./Source/lib/embeddedLapack/src/LinearAlgebra)
include_directories(./Source/lib/embeddedLapack/src/qpOASES/Header)
include_directories(./Source/lib/embeddedLapack/src/qpOASES/Header/qpOASES_e)
aux_source_directory(./Source/lib/embeddedLapack/src/qpOASES/Source lapSrc)
aux_source_directory(./Source/lib/embeddedLapack/src/LinearAlgebra lapSrc)
aux_source_directory(./Source/lib/embeddedLapack/src/Lapack/F2c lapSrc)
aux_source_directory(./Source/lib/embeddedLapack/src/Lapack/Blas lapSrc)
aux_source_directory(./Source/lib/embeddedLapack/src/Lapack/Scr lapSrc)
aux_source_directory(./Source/lib/embeddedLapack/src/Lapack/Install lapSrc)
add_library(lap STATIC ${lapSrc})

set_property(SOURCE ./startup_stm32f401xe.s APPEND PROPERTY COMPILE_OPTIONS "-x" "assembler-with-cpp")
set(allSrc ${allSrc} ./startup_stm32f401xe.s)

set_property(SOURCE ./Source/OS/Ports/stm32f401/GNU/os_cpu_a.s APPEND PROPERTY COMPILE_OPTIONS "-x" "assembler-with-cpp")
set(allSrc ${allSrc} ./Source/OS/Ports/stm32f401/GNU/os_cpu_a.s)

add_executable(${CMAKE_PROJECT_NAME}.elf ${allSrc})

target_link_libraries(${CMAKE_PROJECT_NAME}.elf drivers)
target_link_libraries(${CMAKE_PROJECT_NAME}.elf mymath)
target_link_libraries(${CMAKE_PROJECT_NAME}.elf ucOS)
target_link_libraries(${CMAKE_PROJECT_NAME}.elf lap)

add_custom_command(
  TARGET ${CMAKE_PROJECT_NAME}.elf POST_BUILD
  COMMAND
  ${CMAKE_OBJCOPY} -O binary  -S ${CMAKE_PROJECT_NAME}.elf ${CMAKE_PROJECT_NAME}.bin;

  COMMAND
  ${CMAKE_OBJCOPY} -O ihex ${CMAKE_PROJECT_NAME}.elf ${CMAKE_PROJECT_NAME}.hex;

  COMMAND
  arm-none-eabi-size --format=berkeley "drone.elf"
  )

add_custom_target(
  load
  DEPENDS ${CMAKE_PROJECT_NAME}.elf
  COMMAND
  openocd -f interface/stlink.cfg -f target/stm32f4x.cfg -c "program ${CMAKE_SOURCE_DIR}/build/${CMAKE_PROJECT_NAME}.hex verify reset exit"
  )
