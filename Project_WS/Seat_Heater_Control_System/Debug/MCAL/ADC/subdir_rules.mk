################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
MCAL/ADC/%.obj: ../MCAL/ADC/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ccs1260/ccs/tools/compiler/ti-cgt-arm_20.2.7.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="F:/embeeded/3.RTOS/RTOS_workspace/Seat_Heater_Control_System" --include_path="F:/embeeded/3.RTOS/RTOS_workspace/Seat_Heater_Control_System/Common" --include_path="F:/embeeded/3.RTOS/RTOS_workspace/Seat_Heater_Control_System/MCAL" --include_path="F:/embeeded/3.RTOS/RTOS_workspace/Seat_Heater_Control_System/MCAL/GPIO" --include_path="F:/embeeded/3.RTOS/RTOS_workspace/Seat_Heater_Control_System/MCAL/UART" --include_path="F:/embeeded/3.RTOS/RTOS_workspace/Seat_Heater_Control_System/MCAL/GPTM" --include_path="F:/embeeded/3.RTOS/RTOS_workspace/Seat_Heater_Control_System/FreeRTOS/Source/include" --include_path="F:/embeeded/3.RTOS/RTOS_workspace/Seat_Heater_Control_System/FreeRTOS/Source/portable/CCS/ARM_CM4F" --include_path="C:/ti/ccs1260/ccs/tools/compiler/ti-cgt-arm_20.2.7.LTS/include" --define=ccs="ccs" --define=PART_TM4C123GH6PM -g --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="MCAL/ADC/$(basename $(<F)).d_raw" --obj_directory="MCAL/ADC" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


