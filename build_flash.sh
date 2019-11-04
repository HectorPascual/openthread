make -f examples/Makefile-cc2538 JOINER=1 COMMISSIONER=1 COAP=1 
sudo arm-none-eabi-objcopy -O binary output/cc2538/bin/ot-cli-ftd output/cc2538/bin/ot-cli-ftd.bin 

ls /dev/ttyUSB* | grep -oP "USB\K\d" | while read line; do
 sudo python ../cc2538-bsl/cc2538-bsl.py -e -w -v -p /dev/ttyUSB$line -a 0x00200000  output/cc2538/bin/ot-cli-ftd.bin &
done



