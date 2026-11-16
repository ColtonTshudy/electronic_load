// Apply reverse-engineered names/comments to the sousim 500W load firmware
import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.*;
import ghidra.program.model.symbol.*;
import ghidra.program.model.address.*;

public class ApplyNames extends GhidraScript {
  String[][] F = {
   {"08000128","__main_thunk",""},{"08000158","memcpy",""},{"0800017c","memset_",""},{"0800018a","memclr",""},
   {"0800018e","memset",""},{"080001a0","strlen",""},{"080001b0","__scatterload",""},{"080001d4","__decompress1","RW-data LZ decompressor (Keil)"},
   {"0800022c","ads_i2c_ack","Soft-I2C ACK (PC14=SDA, PC15=SCL)"},{"08000274","ADC_Cmd",""},{"08000288","ADC_DeInit",""},
   {"080002cc","ADC_GetCalibrationStatus",""},{"080002da","ADC_GetConversionValue",""},{"080002e0","ADC_GetFlagStatus",""},
   {"080002ee","ADC_GetResetCalibrationStatus",""},{"080002fc","ADC_Init",""},{"08000344","ADC_RegularChannelConfig",""},
   {"080003b8","ADC_ResetCalibration",""},{"080003c2","ADC_SoftwareStartConvCmd",""},{"080003d6","ADC_StartCalibration",""},
   {"080003e0","ADC_TempSensorVrefintCmd",""},
   {"080003fc","ads1115_read","Select ADS1115 mux (0..3), single-shot, wait 1 ms, return 16-bit result"},
   {"08000790","modbus_crc16","Table-driven Modbus CRC16"},
   {"080007c4","cal_voltage","Factory voltage calibration (cmd 0x73/0x74/0x76/0x77)"},
   {"08000a98","ads_i2c_wait_ack",""},
   {"08000ae8","cal_current_table","Factory current calibration via USART1 SCPI PSU/DMM (cmd 0x6e)"},
   {"08001ca4","cal_dac_table","Factory DAC calibration (cmd 0x6f/0x79)"},
   {"0800250c","DAC_Cmd",""},{"08002524","DAC_Init",""},{"0800254c","DAC_SetChannel1Data","CV setpoint DAC (PA4)"},
   {"08002560","DAC_SetChannel2Data","CC setpoint DAC (PA5)"},
   {"08002574","dac8562_write","Bit-banged DAC8562: PB12=SCLK PB13=DIN PB14=SYNC (init only, never used later)"},
   {"080025ac","FLASH_ClearFlag",""},{"080025b8","FLASH_ErasePage",""},{"080025f4","FLASH_GetBank1Status",""},
   {"0800262c","FLASH_PrefetchBufferCmd",""},{"08002644","FLASH_ProgramHalfWord",""},{"08002678","FLASH_SetLatency",""},
   {"0800268c","FLASH_Unlock",""},{"080026a4","FLASH_WaitForLastOperation",""},{"080026c8","GPIO_Init",""},
   {"0800276c","GPIO_PinRemapConfig",""},{"080027c4","GPIO_ReadInputDataBit",""},{"080027d2","GPIO_ResetBits",""},
   {"080027d6","GPIO_SetBits",""},{"080027dc","adc_read_avg130","Internal ADC: 130 samples minus min/max, /128"},
   {"08002868","keys_scan","Buttons PA14,PC4..PC7,PA13 + encoder. Returns key code (0xe=none)"},
   {"08002a9c","iwdg_init",""},{"08002ac4","IWDG_ReloadCounter",""},{"08002ad4","IWDG_SetPrescaler",""},
   {"08002ae0","IWDG_SetReload",""},{"08002aec","IWDG_WriteAccessCmd",""},
   {"08002af8","ads1115_build_cfg","ADS1115 config words 0x95E3/0xA5E3/0xB5E3/0xB7E3"},
   {"08002b40","adc_init","PA0,PA6,PA7,PB0,PB1 analog; ADC1 calibrate"},
   {"08002be0","dac_init","Internal DAC ch1/ch2 + DAC8562 init"},{"08002c58","dac8562_init",""},{"08002c8c","dac8562_gpio_init",""},
   {"08002cb0","tim2_pwm_init","TIM2 full remap, CH2 PWM on PB3"},{"08002d50","tim7_init","TIM7 10 kHz tick"},
   {"08002d8c","usart1_init","USART1 19200 PA9/PA10 (factory cal link)"},
   {"08002e0c","usart2_init","USART2 Modbus RTU PA2/PA3, RS485 DE PC13, baud from table"},
   {"08002ebc","keys_gpio_init","SWJ disabled; PA13/PA14, PB5/PB6 pull-up inputs"},
   {"08002f08","range_gpio_init","PB2 current-range output"},{"08002f2c","outputs_gpio_init","PB4 fan, PB10 buzzer, PB11, PA15 load-inhibit"},
   {"08002fb8","ui_draw_cursor_screen",""},{"0800306c","modbus_process","Modbus RTU slave: func 3,4,6,16"},
   {"0800313c","modbus_pack_regs",""},{"080031b4","ads_i2c_nack",""},{"080031fe","nvic_init",""},
   {"08003260","NVIC_Init",""},{"080032c4","NVIC_PriorityGroupConfig",""},{"080032d8","NVIC_SetVectorTable",""},
   {"080032ec","oled_clear_cols",""},{"0800330a","oled_set_pos","SH1106 page/column (+2 offset)"},
   {"08003338","oled_draw_char16","8x16 glyph from font at 0x0800840C"},{"0800338c","oled_draw_char16_cursor","glyph with underline"},
   {"080033e4","oled_i2c_write_byte","Soft-I2C PA8=SCL PB15=SDA"},{"0800343c","oled_i2c_ack",""},{"0800345c","oled_write","addr 0x78; ctrl 0x00 cmd / 0x40 data"},
   {"080034d4","RCC_ADCCLKConfig",""},{"080034e8","RCC_AHBPeriphClockCmd",""},{"08003500","RCC_APB1PeriphClockCmd",""},
   {"08003530","RCC_APB2PeriphResetCmd",""},{"08003548","rcc_init","HSI/2 x6 = 24 MHz"},{"080035d0","RCC_DeInit",""},
   {"08003614","RCC_GetClocksFreq",""},{"080036ac","RCC_GetFlagStatus",""},{"080036dc","RCC_GetSYSCLKSource",""},
   {"080036ec","RCC_HCLKConfig",""},{"08003700","RCC_HSICmd",""},{"0800370c","RCC_PCLK1Config",""},{"08003720","RCC_PCLK2Config",""},
   {"08003734","RCC_PLLCmd",""},{"08003740","RCC_PLLConfig",""},{"08003754","RCC_SYSCLKConfig",""},
   {"08003768","ads1115_read_raw",""},{"080037a0","ads_i2c_read_byte",""},{"0800382a","menu_call_handler",""},
   {"08003834","menu_edit_current_digit",""},{"080038b8","menu_edit_voltage_digit",""},{"08003938","menu_info_page","Encoder cycles info page 0..6"},
   {"080039a4","dac8562_shift8",""},{"080039ec","dac8562_shift16",""},{"08003a3c","SetSysClockTo24",""},
   {"08003ac4","ads_i2c_start",""},{"08003b08","ads_i2c_stop",""},{"08003b5e","SysTick_CLKSourceConfig",""},
   {"08003b64","SystemInit",""},
   {"08003bb8","TIM7_IRQHandler","10 kHz: load enable, setpoint clamp, DACs, 1/10/100/500 ms ticks, fan+buzzer soft PWM"},
   {"08003d7c","TIM_CtrlPWMOutputs",""},{"08003d90","TIM_ClearITPendingBit",""},{"08003d96","TIM_Cmd",""},{"08003daa","TIM_ARRPreloadConfig_",""},
   {"08003dc0","TIM_GetITStatus",""},{"08003dd8","TIM_ITConfig",""},{"08003de8","TIM_OC2Init",""},{"08003e68","TIM_OC2PreloadConfig",""},
   {"08003e7c","TIM_SetCompare2",""},{"08003e80","TIM_TimeBaseInit",""},{"08003f1c","usart2_putc",""},
   {"08003f48","USART1_IRQHandler",""},{"08003fa8","usart1_parse_meter","Parse dd.dddd reading from reference DMM"},
   {"08004054","USART2_IRQHandler",""},{"08004088","USART_ClearFlag",""},{"0800408e","USART_ClearITPendingBit",""},
   {"0800409a","USART_Cmd",""},{"080040ae","USART_GetFlagStatus",""},{"080040bc","USART_GetITStatus",""},
   {"080040fa","USART_ITConfig",""},{"0800412c","USART_Init",""},{"080041d8","USART_ReceiveData",""},{"080041e0","USART_SendData",""},
   {"08004200","usart1_putc",""},{"08004220","modbus_rx_byte",""},{"08004254","rs485_send",""},{"08004290","modbus_frame_timeout",""},
   {"080042dc","ads1115_write_cfg",""},{"080042f4","ads_i2c_write_byte",""},{"0800434c","ads_i2c_write",""},{"08004398","ads_i2c_gpio_init",""},
   {"080043bc","serial_check","Checksum check, result unused"},{"080044b4","delay_ms",""},{"080044cc","delay_us",""},
   {"08004508","lcd_cursor_glyph",""},{"08004550","ui_draw_info_page","CONSUM mAh/Wh, VIN, time, TEMP, POWER, RESIS"},
   {"08004e8c","lcd_degree_glyph",""},{"08004ec8","ui_draw_setpoints","Set V / Set I line with cursor"},
   {"080051d0","protection_check","OVP/OCP/OPP/OTP; trips load off"},{"080052b4","mode_control","CV/CC/CP/CR + memory presets"},
   {"08005dc4","list_mode_step","Modes 9/10 auto-step list"},{"08006130","encoder_scan","PB5/PB6 quadrature"},
   {"080061a0","encoder_tick",""},{"080061e0","read_mcu_temp","Internal temp sensor (ch16)"},
   {"080062a0","read_ntc1","PB0/ch8 NTC via table at 0x08008238"},{"08006340","read_ntc2","PB1/ch9 NTC"},
   {"080063e0","config_load","Copy 0x2BC bytes from 0x0800F000; magic 0x7E4"},
   {"08006428","measure_current","ADS1115 ch0, range PB2, piecewise gain"},{"080068a8","measure_vin","ADS1115 ch3/ch2 auto-range + PA7"},
   {"08006960","measure_voltage","ADS1115 ch1, range via PA1"},{"08006b40","lcd_clear_line",""},{"08006b64","lcd_putc_at",""},
   {"08006b82","lcd_puts_at",""},{"08006bbc","main",""},{"08007814","modbus_sync_regs",""},{"08007a08","config_init",""},
   {"08007a20","config_save","Erase+program page 0x0800F000"},{"08007a7c","set_current_dac","Range select, calibration, slew, DAC ch2"},
   {"08007f2c","set_voltage_dac","DAC ch1"},{"08007f80","lcd_write","HD44780 4-bit on GPIOE (dead code: GPIOE clock never enabled)"}
  };
  String[][] D = {
   {"20000000","SystemCoreClock"},{"20000028","g_ui"},{"20000032","g_cmd"},{"2000005a","g_mb"},{"20000098","g_menu"},
   {"200001a0","g_cal"},{"200001e8","g_dac"},{"20000208","g_mbrx"},{"2000021c","g_tick"},{"2000021f","g_fan_speed"},
   {"20000218","g_meter_value"},{"200002c4","g_keys"},{"200002f0","g_mb_regs"},{"20000324","g_mb_ctx"},{"20000334","g_u1_rx"},
   {"20000348","g_u2_rx"},{"20000370","g_u2_tx"},{"20000398","g_cfg"},{"20000654","g_state"},
   {"0800840c","font_8x16"},{"08008238","ntc_table"},{"080083cc","baud_table"},{"0800f000","config_page"}
  };
  public void run() throws Exception {
    FunctionManager fm=currentProgram.getFunctionManager();
    for(String[] e:F){
      Address a=toAddr(Long.parseLong(e[0],16));
      Function f=fm.getFunctionAt(a);
      if(f==null){ disassemble(a); f=createFunction(a,e[1]); }
      if(f==null){ println("no func "+e[0]); continue; }
      f.setName(e[1],SourceType.USER_DEFINED);
      if(!e[2].isEmpty()) f.setComment(e[2]);
    }
    for(String[] e:D) createLabel(toAddr(Long.parseLong(e[0],16)),e[1],true);
  }
}
