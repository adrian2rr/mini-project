################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
LD_SRCS += \
../src/lscript.ld 

C_SRCS += \
../src/game.c \
../src/logicas.c \
../src/main.c \
../src/network.c \
../src/plot.c \
../src/utils.c 

OBJS += \
./src/game.o \
./src/logicas.o \
./src/main.o \
./src/network.o \
./src/plot.o \
./src/utils.o 

C_DEPS += \
./src/game.d \
./src/logicas.d \
./src/main.d \
./src/network.d \
./src/plot.d \
./src/utils.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM gcc compiler'
	arm-xilinx-eabi-gcc -Wall -O0 -g3 -c -fmessage-length=0 -I../../pong_bsp/ps7_cortexa9_0/include -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


