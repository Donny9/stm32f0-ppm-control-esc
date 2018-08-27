# Summary
This is a program that uses stm32f0 to receive control messages from the SOC flight control and control the ESC through PPM.
It can be cross-compiled directly on a Linux machine using the cross tool arm-none-eabi, and the generated second-mechanism file
can be directly burned into the microcontroller.

# Compile
```
make clean
djz:stm32f0-ppm-control-esc$ make clean
djz:stm32f0-ppm-control-esc$ mak
compiling user/main.c
compiling user/sys.c
compiling user/startup.c
compiling user/peripheral.c
compiling user/utils.c
compiling user/ppm_parse.c
compiling cmsis/Device/STM32F10xx/Source/system_stm32f10x.c
compiling cmsis/Device/STM32F10xx/StdPeriph_Driver/src/stm32f10x_adc.c
compiling cmsis/Device/STM32F10xx/StdPeriph_Driver/src/stm32f10x_rcc.c
compiling cmsis/Device/STM32F10xx/StdPeriph_Driver/src/stm32f10x_gpio.c
compiling cmsis/Device/STM32F10xx/StdPeriph_Driver/src/stm32f10x_tim.c
compiling cmsis/Device/STM32F10xx/StdPeriph_Driver/src/misc.c
compiling newlib_stub.c
compiling cmsis/CMSIS/Driver/Source/USART_STM32F10x.c
compiling cmsis/CMSIS/Driver/Source/GPIO_STM32F10x.c
compiling cmsis/CMSIS/Driver/Source/DMA_STM32F10x.c
linking /home/djz/disk/workpath/stm32_code/stm32f0-ppm-control-esc/out/ioboard.elf
   text	   data	    bss	    dec	    hex	filename
  16256	    196	    485	  16937	   4229	/home/djz/disk/workpath/stm32_code/stm32f0-ppm-control-esc/out/ioboard.elf
generating sorted symbols: /home/djz/disk/workpath/stm32_code/stm32f0-ppm-control-esc/out/ioboard.elf.sym
generating listing: /home/djz/disk/workpath/stm32_code/stm32f0-ppm-control-esc/out/ioboard.elf.lst
generating image: /home/djz/disk/workpath/stm32_code/stm32f0-ppm-control-esc/out/ioboard.hex
generating image: /home/djz/disk/workpath/stm32_code/stm32f0-ppm-control-esc/out/ioboard.bin
```

# Board
![image](res/board)
![image](res/wire)
