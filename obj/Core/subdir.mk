################################################################################
# MRS Version: 1.9.1
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/core_riscv.c 

OBJS += \
./Core/core_riscv.o 

C_DEPS += \
./Core/core_riscv.d 


# Each subdirectory must supply rules for building sources it contributes
Core/%.o: ../Core/%.c
	@	@	riscv-none-embed-gcc -march=rv32imacxw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused  -g -I"C:\MRS_DATA\workspace\CH32V203-FreeRTOS-Keypad\Debug" -I"C:\MRS_DATA\workspace\CH32V203-FreeRTOS-Keypad\CANopenNode" -I"C:\MRS_DATA\workspace\CH32V203-FreeRTOS-Keypad\CANopenNode\301" -I"C:\MRS_DATA\workspace\CH32V203-FreeRTOS-Keypad\CANopenNode\303" -I"C:\MRS_DATA\workspace\CH32V203-FreeRTOS-Keypad\CANopenNode\304" -I"C:\MRS_DATA\workspace\CH32V203-FreeRTOS-Keypad\CANopenNode\305" -I"C:\MRS_DATA\workspace\CH32V203-FreeRTOS-Keypad\CANopenNode\309" -I"C:\MRS_DATA\workspace\CH32V203-FreeRTOS-Keypad\CANopenNode\keypad" -I"C:\MRS_DATA\workspace\CH32V203-FreeRTOS-Keypad\Core" -I"C:\MRS_DATA\workspace\CH32V203-FreeRTOS-Keypad\User" -I"C:\MRS_DATA\workspace\CH32V203-FreeRTOS-Keypad\Peripheral\inc" -I"C:\MRS_DATA\workspace\CH32V203-FreeRTOS-Keypad\FreeRTOS" -I"C:\MRS_DATA\workspace\CH32V203-FreeRTOS-Keypad\FreeRTOS\include" -I"C:\MRS_DATA\workspace\CH32V203-FreeRTOS-Keypad\FreeRTOS\portable" -I"C:\MRS_DATA\workspace\CH32V203-FreeRTOS-Keypad\FreeRTOS\portable\Common" -I"C:\MRS_DATA\workspace\CH32V203-FreeRTOS-Keypad\FreeRTOS\portable\GCC\RISC-V" -I"C:\MRS_DATA\workspace\CH32V203-FreeRTOS-Keypad\FreeRTOS\portable\GCC\RISC-V\chip_specific_extensions\RV32I_PFIC_no_extensions" -I"C:\MRS_DATA\workspace\CH32V203-FreeRTOS-Keypad\FreeRTOS\portable\MemMang" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
