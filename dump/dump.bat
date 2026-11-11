:: Change working directory to the script's own folder
cd /d "%~dp0"

echo Starting OpenOCD to dump STM32F1 flash...

openocd ^
  -f interface/stlink.cfg ^
  -f target/stm32f1x.cfg ^
  -c "init" ^
  -c "reset halt" ^
  -c "flash read_bank 0 flash_dump.bin 0 0x10000" ^
  -c "reset" ^
  -c "shutdown"