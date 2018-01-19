FpgaUpgrade: spi.o FpgaUpgrade.o
	arm-phytec-linux-gnueabi-gcc -march=armv7-a -mfloat-abi=hard -mfpu=neon -mtune=cortex-a9 --sysroot=/opt/yogurt/i.MX6-PD15.3.0/sysroots/cortexa9hf-vfp-neon-phytec-linux-gnueabi spi.o FpgaUpgrade.o -o FpgaUpgrade
spi.o: spi.c
	arm-phytec-linux-gnueabi-gcc -march=armv7-a -mfloat-abi=hard -mfpu=neon -mtune=cortex-a9 --sysroot=/opt/yogurt/i.MX6-PD15.3.0/sysroots/cortexa9hf-vfp-neon-phytec-linux-gnueabi -c spi.c -o spi.o
FpgaUpgrade.o: FpgaUpgrade.c
	arm-phytec-linux-gnueabi-gcc -march=armv7-a -mfloat-abi=hard -mfpu=neon -mtune=cortex-a9 --sysroot=/opt/yogurt/i.MX6-PD15.3.0/sysroots/cortexa9hf-vfp-neon-phytec-linux-gnueabi -c FpgaUpgrade.c -o FpgaUpgrade.o
clean:
	rm *.o FpgaUpgrade

