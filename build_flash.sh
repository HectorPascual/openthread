make -f examples/Makefile-cc2538 JOINER=1 
arm-none-eabi-objcopy -O binary output/cc2538/bin/ot-cli-ftd output/cc2538/bin/ot-cli-ftd.bin 
sudo python ../cc2538-bsl/cc2538-bsl.py -e -w -v -p /dev/ttyUSB1 -a 0x00200000  output/cc2538/bin/ot-cli-ftd.bin
sudo python -m serial.tools.miniterm /dev/ttyUSB1 115200
