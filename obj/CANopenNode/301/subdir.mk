################################################################################
# MRS Version: 1.9.1
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../CANopenNode/301/CO_Emergency.c \
../CANopenNode/301/CO_HBconsumer.c \
../CANopenNode/301/CO_NMT_Heartbeat.c \
../CANopenNode/301/CO_ODinterface.c \
../CANopenNode/301/CO_PDO.c \
../CANopenNode/301/CO_SDOclient.c \
../CANopenNode/301/CO_SDOserver.c \
../CANopenNode/301/CO_SYNC.c \
../CANopenNode/301/CO_TIME.c \
../CANopenNode/301/CO_fifo.c \
../CANopenNode/301/crc16-ccitt.c 

OBJS += \
./CANopenNode/301/CO_Emergency.o \
./CANopenNode/301/CO_HBconsumer.o \
./CANopenNode/301/CO_NMT_Heartbeat.o \
./CANopenNode/301/CO_ODinterface.o \
./CANopenNode/301/CO_PDO.o \
./CANopenNode/301/CO_SDOclient.o \
./CANopenNode/301/CO_SDOserver.o \
./CANopenNode/301/CO_SYNC.o \
./CANopenNode/301/CO_TIME.o \
./CANopenNode/301/CO_fifo.o \
./CANopenNode/301/crc16-ccitt.o 

C_DEPS += \
./CANopenNode/301/CO_Emergency.d \
./CANopenNode/301/CO_HBconsumer.d \
./CANopenNode/301/CO_NMT_Heartbeat.d \
./CANopenNode/301/CO_ODinterface.d \
./CANopenNode/301/CO_PDO.d \
./CANopenNode/301/CO_SDOclient.d \
./CANopenNode/301/CO_SDOserver.d \
./CANopenNode/301/CO_SYNC.d \
./CANopenNode/301/CO_TIME.d \
./CANopenNode/301/CO_fifo.d \
./CANopenNode/301/crc16-ccitt.d 


# Each subdirectory must supply rules for building sources it contributes
CANopenNode/301/%.o: ../CANopenNode/301/%.c
	@	@	riscv-none-embed-gcc -march=rv32imacxw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused  -g -I"C:\MRS_DATA\workspace\CH32V203-FreeRTOS-Keypad\Debug" -I"C:\MRS_DATA\workspace\CH32V203-FreeRTOS-Keypad\CANopenNode" -I"C:\MRS_DATA\workspace\CH32V203-FreeRTOS-Keypad\CANopenNode\301" -I"C:\MRS_DATA\workspace\CH32V203-FreeRTOS-Keypad\CANopenNode\303" -I"C:\MRS_DATA\workspace\CH32V203-FreeRTOS-Keypad\CANopenNode\304" -I"C:\MRS_DATA\workspace\CH32V203-FreeRTOS-Keypad\CANopenNode\305" -I"C:\MRS_DATA\workspace\CH32V203-FreeRTOS-Keypad\CANopenNode\309" -I"C:\MRS_DATA\workspace\CH32V203-FreeRTOS-Keypad\CANopenNode\keypad" -I"C:\MRS_DATA\workspace\CH32V203-FreeRTOS-Keypad\Core" -I"C:\MRS_DATA\workspace\CH32V203-FreeRTOS-Keypad\User" -I"C:\MRS_DATA\workspace\CH32V203-FreeRTOS-Keypad\Peripheral\inc" -I"C:\MRS_DATA\workspace\CH32V203-FreeRTOS-Keypad\FreeRTOS" -I"C:\MRS_DATA\workspace\CH32V203-FreeRTOS-Keypad\FreeRTOS\include" -I"C:\MRS_DATA\workspace\CH32V203-FreeRTOS-Keypad\FreeRTOS\portable" -I"C:\MRS_DATA\workspace\CH32V203-FreeRTOS-Keypad\FreeRTOS\portable\Common" -I"C:\MRS_DATA\workspace\CH32V203-FreeRTOS-Keypad\FreeRTOS\portable\GCC\RISC-V" -I"C:\MRS_DATA\workspace\CH32V203-FreeRTOS-Keypad\FreeRTOS\portable\GCC\RISC-V\chip_specific_extensions\RV32I_PFIC_no_extensions" -I"C:\MRS_DATA\workspace\CH32V203-FreeRTOS-Keypad\FreeRTOS\portable\MemMang" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

