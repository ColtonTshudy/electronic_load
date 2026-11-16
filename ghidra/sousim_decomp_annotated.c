// ===== __main_thunk @ 08000128 (size 4)

void __main_thunk(void)

{
  (*0x8006bbd)();
  return;
}


// ===== Reset_Handler @ 08000134 (size 8)

void Reset_Handler(void)

{
  (*0x8003b65)();
                    /* WARNING: Could not recover jumptable at 0x0800013a. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*0x8000121)();
  return;
}


// ===== WWDG_IRQHandler @ 0800014e (size 2)

void WWDG_IRQHandler(void)

{
  do {
                    /* WARNING: Do nothing block with infinite loop */
  } while( true );
}


// ===== memcpy @ 08000158 (size 36)

void memcpy(undefined4 *param_1,undefined4 *param_2,uint param_3)

{
  undefined4 uVar1;
  bool bVar2;
  
  if ((((uint)param_1 | (uint)param_2) & 3) == 0) {
    for (; 3 < param_3; param_3 = param_3 - 4) {
      uVar1 = *param_2;
      param_2 = param_2 + 1;
      *param_1 = uVar1;
      param_1 = param_1 + 1;
    }
  }
  while (bVar2 = param_3 != 0, param_3 = param_3 - 1, bVar2) {
    *(undefined1 *)param_1 = *(undefined1 *)param_2;
    param_1 = (undefined4 *)((int)param_1 + 1);
    param_2 = (undefined4 *)((int)param_2 + 1);
  }
  return;
}


// ===== memset_ @ 0800017c (size 14)

void memset_(undefined1 *param_1,int param_2,undefined1 param_3)

{
  bool bVar1;
  
  while (bVar1 = param_2 != 0, param_2 = param_2 + -1, bVar1) {
    *param_1 = param_3;
    param_1 = param_1 + 1;
  }
  return;
}


// ===== memclr @ 0800018a (size 4)

void memclr(undefined4 param_1,undefined4 param_2)

{
  memset_(param_1,param_2,0);
  return;
}


// ===== memset @ 0800018e (size 18)

undefined4 memset(undefined4 param_1,undefined4 param_2,undefined4 param_3)

{
  memset_(param_1,param_3,param_2);
  return param_1;
}


// ===== strlen @ 080001a0 (size 14)

int strlen(char *param_1)

{
  char cVar1;
  char *pcVar2;
  char *pcVar3;
  
  pcVar3 = param_1;
  do {
    pcVar2 = pcVar3 + 1;
    cVar1 = *pcVar3;
    pcVar3 = pcVar2;
  } while (cVar1 != '\0');
  return (int)pcVar2 - (int)(param_1 + 1);
}


// ===== __scatterload @ 080001b0 (size 36)

undefined4 __scatterload(void)

{
  undefined4 *puVar1;
  byte *pbVar2;
  byte *pbVar3;
  byte *extraout_r1;
  byte *pbVar4;
  int extraout_r2;
  uint uVar5;
  int iVar6;
  int extraout_r3;
  uint uVar7;
  undefined4 *puVar8;
  uint uVar9;
  int unaff_r7;
  
  puVar1 = 0x800861c;
  for (puVar8 = 0x80085fc; puVar8 < puVar1; puVar8 = puVar8 + 4) {
    (*(code *)puVar8[3])(*puVar8,puVar8[1],puVar8[2]);
  }
  __main_thunk();
  *(short *)(unaff_r7 + 0x2e) = (short)puVar8;
  *(short *)(extraout_r3 + 0x30) = (short)puVar8;
  pbVar2 = (byte *)0x0;
  pbVar4 = extraout_r1;
  do {
    uVar9 = (uint)*pbVar2;
    uVar7 = uVar9 & 7;
    pbVar3 = pbVar2 + 1;
    if ((*pbVar2 & 7) == 0) {
      pbVar3 = pbVar2 + 2;
      uVar7 = (uint)pbVar2[1];
    }
    uVar5 = (int)uVar9 >> 4;
    if (uVar5 == 0) {
      uVar5 = (uint)*pbVar3;
      pbVar3 = pbVar3 + 1;
    }
    while (uVar7 = uVar7 - 1, uVar7 != 0) {
      *pbVar4 = *pbVar3;
      pbVar3 = pbVar3 + 1;
      pbVar4 = pbVar4 + 1;
    }
    if ((int)(uVar9 << 0x1c) < 0) {
      pbVar2 = pbVar3 + 1;
      iVar6 = uVar5 + 2;
      pbVar3 = pbVar4 + -(uint)*pbVar3;
      while (iVar6 = iVar6 + -1, -1 < iVar6) {
        *pbVar4 = *pbVar3;
        pbVar4 = pbVar4 + 1;
        pbVar3 = pbVar3 + 1;
      }
    }
    else {
      while (uVar5 = uVar5 - 1, pbVar2 = pbVar3, -1 < (int)uVar5) {
        *pbVar4 = 0;
        pbVar4 = pbVar4 + 1;
      }
    }
  } while (pbVar4 < extraout_r1 + extraout_r2);
  return 0;
}


// ===== __decompress1 @ 080001d4 (size 86)

/* RW-data LZ decompressor (Keil) */

undefined4 __decompress1(byte *param_1,byte *param_2,int param_3)

{
  byte *pbVar1;
  uint uVar2;
  int iVar3;
  uint uVar4;
  byte *pbVar5;
  uint uVar6;
  
  pbVar5 = param_2 + param_3;
  do {
    uVar6 = (uint)*param_1;
    uVar4 = uVar6 & 7;
    pbVar1 = param_1 + 1;
    if ((*param_1 & 7) == 0) {
      pbVar1 = param_1 + 2;
      uVar4 = (uint)param_1[1];
    }
    uVar2 = (int)uVar6 >> 4;
    if (uVar2 == 0) {
      uVar2 = (uint)*pbVar1;
      pbVar1 = pbVar1 + 1;
    }
    while (uVar4 = uVar4 - 1, uVar4 != 0) {
      *param_2 = *pbVar1;
      pbVar1 = pbVar1 + 1;
      param_2 = param_2 + 1;
    }
    if ((int)(uVar6 << 0x1c) < 0) {
      param_1 = pbVar1 + 1;
      iVar3 = uVar2 + 2;
      pbVar1 = param_2 + -(uint)*pbVar1;
      while (iVar3 = iVar3 + -1, -1 < iVar3) {
        *param_2 = *pbVar1;
        param_2 = param_2 + 1;
        pbVar1 = pbVar1 + 1;
      }
    }
    else {
      while (uVar2 = uVar2 - 1, param_1 = pbVar1, -1 < (int)uVar2) {
        *param_2 = 0;
        param_2 = param_2 + 1;
      }
    }
  } while (param_2 < pbVar5);
  return 0;
}


// ===== ads_i2c_ack @ 0800022c (size 68)

/* Soft-I2C ACK (PC14=SDA, PC15=SCL) */

void ads_i2c_ack(void)

{
  undefined4 uVar1;
  
  uVar1 = 0x40011000/*GPIOC*/;
  GPIO_ResetBits(0x40011000/*GPIOC*/,0x4000);
  GPIO_SetBits(uVar1,0x8000);
  delay_us(0xf);
  GPIO_ResetBits(uVar1,0x8000);
  GPIO_SetBits(uVar1,0x4000);
  delay_us(0xf);
  return;
}


// ===== ADC_Cmd @ 08000274 (size 20)

void ADC_Cmd(int param_1,int param_2)

{
  uint uVar1;
  
  if (param_2 == 0) {
    uVar1 = *(uint *)(param_1 + 8) & 0xfffffffe;
  }
  else {
    uVar1 = *(uint *)(param_1 + 8) | 1;
  }
  *(uint *)(param_1 + 8) = uVar1;
  return;
}


// ===== ADC_DeInit @ 08000288 (size 56)

void ADC_DeInit(int param_1)

{
  if (param_1 == 0x40012400/*ADC1*/) {
    param_1 = param_1 >> 0x15;
  }
  else if (param_1 == 0x40012800) {
    param_1 = param_1 >> 0x14;
  }
  else {
    if (param_1 != 0x40013c00) {
      return;
    }
    param_1 = 0x8000;
  }
  RCC_APB2PeriphResetCmd(param_1,1);
  RCC_APB2PeriphResetCmd(param_1,0);
  return;
}


// ===== ADC_GetCalibrationStatus @ 080002cc (size 14)

bool ADC_GetCalibrationStatus(int param_1)

{
  return *(int *)(param_1 + 8) << 0x1d < 0;
}


// ===== ADC_GetConversionValue @ 080002da (size 6)

uint ADC_GetConversionValue(int param_1)

{
  return *(uint *)(param_1 + 0x4c) & 0xffff;
}


// ===== ADC_GetFlagStatus @ 080002e0 (size 14)

bool ADC_GetFlagStatus(uint *param_1,uint param_2)

{
  return (*param_1 & param_2) != 0;
}


// ===== ADC_GetResetCalibrationStatus @ 080002ee (size 14)

bool ADC_GetResetCalibrationStatus(int param_1)

{
  return *(int *)(param_1 + 8) << 0x1c < 0;
}


// ===== ADC_Init @ 080002fc (size 62)

void ADC_Init(int param_1,uint *param_2)

{
  *(uint *)(param_1 + 4) =
       *param_2 | *(uint *)(param_1 + 4) & 0xfff0feff | (uint)(byte)param_2[1] << 8;
  *(uint *)(param_1 + 8) =
       param_2[3] | param_2[2] |
       *(uint *)(param_1 + 8) & 0xfff1f7fd | (uint)*(byte *)((int)param_2 + 5) << 1;
  *(uint *)(param_1 + 0x2c) =
       *(uint *)(param_1 + 0x2c) & 0xff0fffff | ((byte)param_2[4] - 1 & 0xff) << 0x14;
  return;
}


// ===== ADC_RegularChannelConfig @ 08000344 (size 116)

void ADC_RegularChannelConfig(int param_1,uint param_2,uint param_3,int param_4)

{
  uint uVar1;
  
  if (param_2 < 10) {
    *(uint *)(param_1 + 0x10) =
         *(uint *)(param_1 + 0x10) & ~(7 << (param_2 * 3 & 0xff)) | param_4 << (param_2 * 3 & 0xff);
  }
  else {
    uVar1 = (param_2 - 10) * 3;
    *(uint *)(param_1 + 0xc) =
         *(uint *)(param_1 + 0xc) & ~(7 << (uVar1 & 0xff)) | param_4 << (uVar1 & 0xff);
  }
  if (param_3 < 7) {
    uVar1 = (param_3 - 1) * 5;
    *(uint *)(param_1 + 0x34) =
         *(uint *)(param_1 + 0x34) & ~(0x1f << (uVar1 & 0xff)) | param_2 << (uVar1 & 0xff);
    return;
  }
  if (param_3 < 0xd) {
    uVar1 = (param_3 - 7) * 5;
    *(uint *)(param_1 + 0x30) =
         *(uint *)(param_1 + 0x30) & ~(0x1f << (uVar1 & 0xff)) | param_2 << (uVar1 & 0xff);
    return;
  }
  uVar1 = (param_3 - 0xd) * 5;
  *(uint *)(param_1 + 0x2c) =
       *(uint *)(param_1 + 0x2c) & ~(0x1f << (uVar1 & 0xff)) | param_2 << (uVar1 & 0xff);
  return;
}


// ===== ADC_ResetCalibration @ 080003b8 (size 10)

void ADC_ResetCalibration(int param_1)

{
  *(uint *)(param_1 + 8) = *(uint *)(param_1 + 8) | 8;
  return;
}


// ===== ADC_SoftwareStartConvCmd @ 080003c2 (size 20)

void ADC_SoftwareStartConvCmd(int param_1,int param_2)

{
  uint uVar1;
  
  if (param_2 == 0) {
    uVar1 = *(uint *)(param_1 + 8) & 0xffafffff;
  }
  else {
    uVar1 = *(uint *)(param_1 + 8) | 0x500000;
  }
  *(uint *)(param_1 + 8) = uVar1;
  return;
}


// ===== ADC_StartCalibration @ 080003d6 (size 10)

void ADC_StartCalibration(int param_1)

{
  *(uint *)(param_1 + 8) = *(uint *)(param_1 + 8) | 4;
  return;
}


// ===== ADC_TempSensorVrefintCmd @ 080003e0 (size 22)

void ADC_TempSensorVrefintCmd(int param_1)

{
  uint uVar1;
  
  if (param_1 == 0) {
    uVar1 = *0x40012408/*ADC1+0x8*/ & 0xff7fffff;
  }
  else {
    uVar1 = *0x40012408/*ADC1+0x8*/ | 0x800000;
  }
  *0x40012408/*ADC1+0x8*/ = uVar1;
  return;
}


// ===== ads1115_read @ 080003fc (size 40)

/* Select ADS1115 mux (0..3), single-shot, wait 1 ms, return 16-bit result */

uint ads1115_read(void)

{
  uint uVar1;
  
  ads1115_build_cfg();
  ads1115_write_cfg();
  delay_ms(1);
  ads1115_read_raw();
  uVar1 = 0x200001dc[1] + *0x200001dc * 0x100;
  if (0xfffe < uVar1) {
    uVar1 = 0;
  }
  return uVar1;
}


// ===== BusFault_Handler @ 08000428 (size 2)

void BusFault_Handler(void)

{
  do {
                    /* WARNING: Do nothing block with infinite loop */
  } while( true );
}


// ===== modbus_crc16 @ 08000790 (size 44)

/* Table-driven Modbus CRC16 */

uint modbus_crc16(byte *param_1,uint param_2)

{
  uint uVar1;
  uint uVar2;
  uint uVar3;
  bool bVar4;
  
  uVar2 = 0xff;
  uVar1 = 0xff;
  while (bVar4 = param_2 != 0, param_2 = param_2 - 1 & 0xff, bVar4) {
    uVar3 = *param_1 ^ uVar1;
    uVar1 = *(byte *)(0x8008038 + uVar3) ^ uVar2;
    uVar2 = (uint)*(byte *)(0x8008038 + 0x100 + uVar3);
    param_1 = param_1 + 1;
  }
  return uVar2 | uVar1 << 8;
}


// ===== cal_voltage @ 080007c4 (size 694)

/* Factory voltage calibration (cmd 0x73/0x74/0x76/0x77) */

void cal_voltage(void)

{
  ushort uVar1;
  short sVar2;
  ushort uVar3;
  ushort uVar4;
  short *psVar5;
  int iVar6;
  int iVar7;
  ushort *puVar8;
  int iVar9;
  uint uVar10;
  uint uVar11;
  
  iVar9 = 0x20000654;
  puVar8 = 0x200001f8;
  iVar7 = 0x20000398;
  iVar6 = 0x200001a0;
  psVar5 = 0x20000032;
  uVar1 = *(ushort *)(0x200001a0 + 0x12);
  sVar2 = *0x20000032;
  uVar3 = *(ushort *)(0x20000398 + 0x40);
  uVar11 = *(uint *)(0x20000654 + 8);
  if (sVar2 == 0x73) {
    if (*(short *)(0x200001a0 + 0x10) == 0) {
      *(undefined2 *)(0x200001a0 + 0xe) = 3000;
      *(undefined2 *)(iVar6 + 0x10) = 1;
      *(undefined2 *)(iVar7 + 0x20) = 0;
      *(undefined4 *)(iVar9 + 4) = 1;
      *(undefined4 *)(iVar9 + 0x10) = 3000;
      *puVar8 = uVar3;
    }
    uVar10 = (uint)*(ushort *)(iVar6 + 0xe);
    if (uVar11 != uVar10) {
      if (uVar11 < uVar10) {
        *puVar8 = *puVar8 - 1;
      }
      else {
        *puVar8 = *puVar8 + 1;
      }
    }
    *(ushort *)(iVar6 + 0x12) = uVar1 + 1;
    if ((9 < uVar1) && ((uVar11 == uVar10 || (uVar11 == uVar10 + 1)))) {
      *(undefined2 *)(iVar6 + 0x10) = 0;
      *(undefined2 *)(iVar6 + 0x12) = 0;
      *(undefined2 *)(iVar7 + 0x20) = 1;
      *(undefined4 *)(iVar9 + 4) = 0;
      *(ushort *)(iVar7 + 0x40) = *puVar8;
LAB_0800090c:
      *psVar5 = 0x78;
    }
LAB_08000804:
    uVar1 = *0x20000056;
    if (uVar1 < 0xf0b) {
      *(ushort *)(iVar7 + 0x30) = uVar1;
    }
    else {
      *(ushort *)(iVar7 + 0x32) = uVar1;
    }
    if (*psVar5 == 0x76) goto LAB_08000930;
    if (*psVar5 == 0x77) goto LAB_080009e6;
  }
  else {
    if (sVar2 == 0x74) {
      if (*(short *)(0x200001a0 + 0x10) == 0) {
        *(undefined2 *)(0x200001a0 + 0xe) = 10000;
        *(undefined2 *)(iVar6 + 0x10) = 1;
        *(undefined2 *)(iVar7 + 0x20) = 0;
        *(undefined4 *)(iVar9 + 4) = 1;
        *(undefined4 *)(iVar9 + 0x10) = 10000;
        uVar4 = *(ushort *)(iVar7 + 0x42);
        *puVar8 = uVar4;
        if (59999 < uVar4) {
          *puVar8 = uVar3 << 1;
        }
      }
      uVar10 = (uint)*(ushort *)(iVar6 + 0xe);
      if (uVar11 != uVar10) {
        if (uVar11 < uVar10) {
          *puVar8 = *puVar8 - 1;
        }
        else {
          *puVar8 = *puVar8 + 1;
        }
      }
      *(ushort *)(iVar6 + 0x12) = uVar1 + 1;
      if ((9 < uVar1) && ((uVar11 == uVar10 || (uVar11 == uVar10 + 1)))) {
        *(undefined2 *)(iVar6 + 0x10) = 0;
        *(undefined2 *)(iVar7 + 0x20) = 1;
        *(undefined4 *)(iVar9 + 4) = 0;
        *(ushort *)(iVar7 + 0x42) = *puVar8;
        goto LAB_0800090c;
      }
      goto LAB_08000804;
    }
    if (sVar2 != 0x76) goto LAB_08000804;
LAB_08000930:
    uVar1 = *(ushort *)(iVar6 + 0x14);
    *(ushort *)(iVar6 + 0x14) = uVar1 + 1;
    if (9 < uVar1) {
      *(undefined2 *)(iVar6 + 0x14) = 0;
      puVar8 = 0x20000056;
      *(undefined2 *)(iVar6 + 0xe) = 10000;
      uVar1 = *(ushort *)(iVar7 + 0x32);
      *puVar8 = uVar1;
      if (uVar11 < 0xf0b) {
        *(undefined2 *)(iVar6 + 0xe) = 3000;
        uVar1 = *(ushort *)(iVar7 + 0x30);
        *puVar8 = uVar1;
        if (uVar11 != 3000) {
          if (3000 < uVar11) {
            *(ushort *)(iVar7 + 0x30) = uVar1 - 1;
          }
          if (uVar11 < 3000) {
            *(short *)(iVar7 + 0x30) = *(short *)(iVar7 + 0x30) + 1;
          }
        }
        uVar1 = *(ushort *)(iVar6 + 0x12);
        *(ushort *)(iVar6 + 0x12) = uVar1 + 1;
        if ((9 < uVar1) && (0xbb6 < uVar11)) {
          uVar10 = 0xbb9;
          goto LAB_080009d8;
        }
      }
      else {
        if (uVar11 != 10000) {
          if (10000 < uVar11) {
            *(ushort *)(iVar7 + 0x32) = uVar1 - 1;
          }
          if (uVar11 < 10000) {
            *(short *)(iVar7 + 0x32) = *(short *)(iVar7 + 0x32) + 1;
          }
        }
        uVar1 = *(ushort *)(iVar6 + 0x12);
        *(ushort *)(iVar6 + 0x12) = uVar1 + 1;
        if ((9 < uVar1) && (9999 < uVar11)) {
          uVar10 = 0x2711;
LAB_080009d8:
          if (uVar11 <= uVar10) {
            *psVar5 = 0x78;
          }
        }
      }
    }
  }
  uVar1 = *0x20000058;
  if (uVar1 != 0) {
    if ((uint)uVar1 < *(ushort *)(iVar7 + 0x34) + 1000) {
      *(ushort *)(iVar7 + 0x34) = uVar1;
    }
    else {
      *(ushort *)(iVar7 + 0x36) = uVar1;
    }
  }
  if (*psVar5 != 0x77) {
    return;
  }
LAB_080009e6:
  uVar1 = *(ushort *)(iVar6 + 0x14);
  *(ushort *)(iVar6 + 0x14) = uVar1 + 1;
  if (9 < uVar1) {
    *(undefined2 *)(iVar6 + 0x14) = 0;
    puVar8 = 0x20000058;
    *(undefined2 *)(iVar6 + 0xe) = 60000;
    uVar1 = *(ushort *)(iVar7 + 0x36);
    *puVar8 = uVar1;
    uVar11 = *(uint *)(iVar9 + 0x24);
    if (uVar11 < 0xc351) {
      *(undefined2 *)(iVar6 + 0xe) = 30000;
      uVar1 = *(ushort *)(iVar7 + 0x34);
      *puVar8 = uVar1;
      if (uVar11 != 30000) {
        if (30000 < uVar11) {
          *(ushort *)(iVar7 + 0x34) = uVar1 - 1;
        }
        if (uVar11 < 30000) {
          *(short *)(iVar7 + 0x34) = *(short *)(iVar7 + 0x34) + 1;
        }
      }
      uVar1 = *(ushort *)(iVar6 + 0x12);
      *(ushort *)(iVar6 + 0x12) = uVar1 + 1;
      if (uVar1 < 10) {
        return;
      }
      if (uVar11 < 0x7531) {
        return;
      }
      uVar10 = 0x7535;
    }
    else {
      if (uVar11 != 60000) {
        if (60000 < uVar11) {
          *(ushort *)(iVar7 + 0x36) = uVar1 - 1;
        }
        if (uVar11 < 60000) {
          *(short *)(iVar7 + 0x36) = *(short *)(iVar7 + 0x36) + 1;
        }
      }
      uVar1 = *(ushort *)(iVar6 + 0x12);
      *(ushort *)(iVar6 + 0x12) = uVar1 + 1;
      if (uVar1 < 10) {
        return;
      }
      if (uVar11 < 0xea61) {
        return;
      }
      uVar10 = 0xea65;
    }
    if (uVar11 <= uVar10) {
      *psVar5 = 0x78;
    }
  }
  return;
}


// ===== ads_i2c_wait_ack @ 08000a98 (size 76)

undefined4 ads_i2c_wait_ack(void)

{
  undefined4 uVar1;
  int iVar2;
  
  uVar1 = 0x40011000/*GPIOC*/;
  GPIO_SetBits(0x40011000/*GPIOC*/,0x4000);
  GPIO_SetBits(uVar1,0x8000);
  iVar2 = GPIO_ReadInputDataBit(uVar1,0x4000);
  GPIO_ResetBits(uVar1,0x8000);
  if (iVar2 != 0) {
    return 0;
  }
  return 1;
}


// ===== cal_current_table @ 08000ae8 (size 4518)

/* Factory current calibration via USART1 SCPI PSU/DMM (cmd 0x6e) */

void cal_current_table(void)

{
  undefined2 uVar1;
  ushort uVar2;
  short *psVar3;
  int iVar4;
  int iVar5;
  uint *puVar6;
  uint uVar7;
  uint extraout_r1;
  uint uVar8;
  uint uVar9;
  uint uVar10;
  uint uVar11;
  uint uVar12;
  bool bVar13;
  bool bVar14;
  
  iVar5 = 0x20000654;
  iVar4 = 0x20000398;
  uVar10 = 0x200001a0;
  psVar3 = 0x20000032;
  if (*0x20000032 != 0x6e) {
    return;
  }
  uVar11 = 0;
  if (*(int *)(0x200001a0 + 0x34) == 0) {
    usart1_putc(0x56);
    usart1_putc(0x4f);
    usart1_putc(0x4c);
    usart1_putc(0x54);
    usart1_putc(0x20);
    usart1_putc(0x35);
    usart1_putc(0x2e);
    usart1_putc(0x30);
    usart1_putc(0xd);
    usart1_putc(10);
    usart1_putc(0x43);
    usart1_putc(0x55);
    usart1_putc(0x52);
    usart1_putc(0x52);
    usart1_putc(0x20);
    usart1_putc(0x30);
    usart1_putc(0x2e);
    usart1_putc(0x30);
    usart1_putc(0x35);
    usart1_putc(0xd);
    usart1_putc(10);
    *(ushort *)(uVar10 + 0x22) = *(ushort *)(iVar4 + 0xc) / 10000;
    *(undefined2 *)(iVar4 + 0x20) = 0;
    *(undefined4 *)(iVar5 + 4) = 1;
    *(undefined4 *)(iVar5 + 0x10) = 0;
    uVar2 = *(ushort *)(iVar4 + 0x50);
    *(undefined4 *)(uVar10 + 0x34) = 0x32;
    *(uint *)(uVar10 + 0x38) = (uint)uVar2;
  }
  if ((uint)*(ushort *)(iVar4 + 0xc) < *(uint *)(uVar10 + 0x34)) {
    *(undefined4 *)(uVar10 + 0x34) = 0;
    *(undefined4 *)(iVar5 + 4) = 0;
    *(undefined2 *)(iVar4 + 0x20) = 1;
    *psVar3 = 0x78;
  }
  usart1_putc(0x4d);
  usart1_putc(0x45);
  usart1_putc(0x41);
  usart1_putc(0x53);
  usart1_putc(0x75);
  usart1_putc(0x72);
  usart1_putc(0x65);
  usart1_putc(0x3a);
  usart1_putc(0x43);
  usart1_putc(0x55);
  usart1_putc(0x52);
  usart1_putc(0x52);
  usart1_putc(0x65);
  usart1_putc(0x6e);
  usart1_putc(0x74);
  usart1_putc(0x3f);
  usart1_putc(0xd);
  usart1_putc(10);
  *(undefined2 *)(uVar10 + 0x26) = 10;
  *(undefined2 *)(uVar10 + 0x2a) = 0x32;
  puVar6 = 0x20000218;
  uVar2 = *(ushort *)(uVar10 + 0x24);
  *(ushort *)(uVar10 + 0x24) = uVar2 + 1;
  if (((9 < uVar2) && (*(undefined2 *)(uVar10 + 0x24) = 0, *(int *)(iVar5 + 4) != 0)) &&
     (uVar7 = *puVar6, *(uint *)(iVar5 + 0xc) != uVar7)) {
    if (*(uint *)(iVar5 + 0xc) < uVar7) {
      *(int *)(uVar10 + 0x38) = *(int *)(uVar10 + 0x38) + 1;
    }
    else {
      *(int *)(uVar10 + 0x38) = *(int *)(uVar10 + 0x38) + -1;
    }
  }
  uVar2 = *(ushort *)(uVar10 + 0x28);
  *(ushort *)(uVar10 + 0x28) = uVar2 + 1;
  bVar13 = 0x31 < uVar2;
LAB_08000c80:
  do {
    if (!bVar13) {
      return;
    }
    *(short *)(uVar10 + 0x28) = (short)uVar11;
    if (*(int *)(uVar10 + 0x34) == 0x32) {
      measure_voltage();
      if ((*(uint *)(uVar10 + 0x34) - 1 <= *(uint *)(iVar5 + 0xc)) &&
         (*(uint *)(iVar5 + 0xc) <= *(uint *)(uVar10 + 0x34))) {
        *(undefined2 *)(iVar4 + 0x50) = *(undefined2 *)(uVar10 + 0x38);
        *(undefined4 *)(uVar10 + 0x34) = 100;
        usart1_putc(0x43);
        usart1_putc(0x55);
        usart1_putc(0x52);
        usart1_putc(0x52);
        usart1_putc(0x20);
        usart1_putc(0x30);
        usart1_putc(0x2e);
        usart1_putc(0x31);
        usart1_putc(0xd);
        usart1_putc(10);
      }
    }
    if (*(int *)(uVar10 + 0x34) != 100) goto LAB_08000d3a;
    uVar7 = *(uint *)(iVar5 + 0xc);
    bVar14 = 0x62 < uVar7;
LAB_08000cee:
    bVar13 = false;
  } while (!bVar14);
  bVar14 = 99 < uVar7;
  bVar13 = uVar7 == 100;
LAB_08000cf2:
  do {
    if (bVar14 && !bVar13) {
      return;
    }
    *(undefined2 *)(iVar4 + 0x52) = *(undefined2 *)(uVar10 + 0x38);
    *(undefined4 *)(uVar10 + 0x34) = 200;
    usart1_putc(0x43);
    usart1_putc(0x55);
    usart1_putc(0x52);
    usart1_putc(0x52);
    usart1_putc(0x20);
    usart1_putc(0x30);
    usart1_putc(0x2e);
    usart1_putc(0x32);
    usart1_putc(0xd);
    usart1_putc(10);
LAB_08000d3a:
    uVar11 = 300;
    uVar8 = uVar11;
    if (*(int *)(uVar10 + 0x34) != 200) goto LAB_08000d98;
    uVar7 = *(uint *)(iVar5 + 0xc);
    bVar13 = 0xc6 < uVar7;
LAB_08000d48:
    if (!bVar13) goto LAB_08000c80;
    bVar14 = 199 < uVar7;
    bVar13 = uVar7 == 200;
    uVar7 = uVar11;
LAB_08000d4c:
    if (!bVar14 || bVar13) {
      *(undefined2 *)(iVar4 + 0x54) = *(undefined2 *)(uVar10 + 0x38);
      *(uint *)(uVar10 + 0x34) = uVar7;
      usart1_putc(0x43);
      usart1_putc(0x55);
      usart1_putc(0x52);
      usart1_putc(0x52);
      usart1_putc(0x20);
      usart1_putc(0x30);
      usart1_putc(0x2e);
      usart1_putc(0x33);
      usart1_putc(0xd);
      usart1_putc(10);
      uVar8 = uVar7;
LAB_08000d98:
      uVar11 = 400;
      if (*(uint *)(uVar10 + 0x34) != uVar8) goto LAB_08000df8;
      uVar7 = *(uint *)(iVar5 + 0xc);
      bVar13 = 0x12a < uVar7;
LAB_08000dac:
      bVar14 = false;
      if (bVar13) {
        bVar14 = uVar8 <= uVar7;
        bVar13 = uVar7 == uVar8;
        uVar7 = uVar11;
LAB_08000db0:
        if (!bVar14 || bVar13) {
          *(undefined2 *)(iVar4 + 0x56) = *(undefined2 *)(uVar10 + 0x38);
          *(uint *)(uVar10 + 0x34) = uVar7;
          usart1_putc(0x43);
          usart1_putc(0x55);
          usart1_putc(0x52);
          usart1_putc(0x52);
          usart1_putc(0x20);
          usart1_putc(0x30);
          usart1_putc(0x2e);
          usart1_putc(0x34);
          usart1_putc(0xd);
          usart1_putc(10);
          uVar11 = uVar7;
LAB_08000df8:
          uVar8 = uVar11;
          uVar11 = 500;
          if (*(uint *)(uVar10 + 0x34) != uVar8) goto LAB_08000e58;
          uVar7 = *(uint *)(iVar5 + 0xc);
          bVar14 = 0x18e < uVar7;
LAB_08000e0c:
          bVar13 = false;
          if (bVar14) {
            bVar14 = uVar8 <= uVar7;
            bVar13 = uVar7 == uVar8;
            uVar7 = uVar11;
LAB_08000e10:
            if (!bVar14 || bVar13) {
              *(undefined2 *)(iVar4 + 0x58) = *(undefined2 *)(uVar10 + 0x38);
              *(uint *)(uVar10 + 0x34) = uVar7;
              usart1_putc(0x43);
              usart1_putc(0x55);
              usart1_putc(0x52);
              usart1_putc(0x52);
              usart1_putc(0x20);
              usart1_putc(0x30);
              usart1_putc(0x2e);
              usart1_putc(0x35);
              usart1_putc(0xd);
              usart1_putc(10);
              uVar11 = uVar7;
LAB_08000e58:
              uVar8 = uVar11;
              uVar11 = 600;
              if (*(uint *)(uVar10 + 0x34) != uVar8) goto LAB_08000eb8;
              uVar7 = *(uint *)(iVar5 + 0xc);
              bVar14 = 0x1f2 < uVar7;
LAB_08000e6c:
              bVar13 = false;
              if (bVar14) {
                bVar14 = uVar8 <= uVar7;
                bVar13 = uVar7 == uVar8;
                uVar7 = uVar11;
LAB_08000e70:
                if (!bVar14 || bVar13) {
                  *(undefined2 *)(iVar4 + 0x5a) = *(undefined2 *)(uVar10 + 0x38);
                  *(uint *)(uVar10 + 0x34) = uVar7;
                  usart1_putc(0x43);
                  usart1_putc(0x55);
                  usart1_putc(0x52);
                  usart1_putc(0x52);
                  usart1_putc(0x20);
                  usart1_putc(0x30);
                  usart1_putc(0x2e);
                  usart1_putc(0x36);
                  usart1_putc(0xd);
                  usart1_putc(10);
                  uVar11 = uVar7;
LAB_08000eb8:
                  uVar8 = uVar11;
                  uVar11 = 700;
                  if (*(uint *)(uVar10 + 0x34) != uVar8) goto LAB_08000f2e;
                  uVar7 = *(uint *)(iVar5 + 0xc);
                  bVar13 = 0x256 < uVar7;
LAB_08000ecc:
                  bVar14 = false;
                  if (bVar13) {
                    bVar14 = uVar8 <= uVar7;
                    bVar13 = uVar7 == uVar8;
                    uVar7 = uVar11;
LAB_08000ed0:
                    if (!bVar14 || bVar13) {
                      *(undefined2 *)(iVar4 + 0x5c) = *(undefined2 *)(uVar10 + 0x38);
                      *(uint *)(uVar10 + 0x34) = uVar7;
                      usart1_putc(0x43);
                      usart1_putc(0x55);
                      usart1_putc(0x52);
                      usart1_putc(0x52);
                      usart1_putc(0x20);
                      usart1_putc(0x30);
                      usart1_putc(0x2e);
                      usart1_putc(0x37);
                      usart1_putc(0xd);
                      usart1_putc(10);
                      uVar11 = uVar7;
LAB_08000f2e:
                      uVar8 = uVar11;
                      uVar11 = 800;
                      if (*(uint *)(uVar10 + 0x34) != uVar8) goto LAB_08000f8e;
                      uVar7 = *(uint *)(iVar5 + 0xc);
                      bVar13 = 0x2ba < uVar7;
LAB_08000f42:
                      bVar14 = false;
                      if (bVar13) {
                        bVar14 = uVar8 <= uVar7;
                        bVar13 = uVar7 == uVar8;
                        uVar7 = uVar11;
LAB_08000f46:
                        if (!bVar14 || bVar13) {
                          *(undefined2 *)(iVar4 + 0x5e) = *(undefined2 *)(uVar10 + 0x38);
                          *(uint *)(uVar10 + 0x34) = uVar7;
                          usart1_putc(0x43);
                          usart1_putc(0x55);
                          usart1_putc(0x52);
                          usart1_putc(0x52);
                          usart1_putc(0x20);
                          usart1_putc(0x30);
                          usart1_putc(0x2e);
                          usart1_putc(0x38);
                          usart1_putc(0xd);
                          usart1_putc(10);
                          uVar11 = uVar7;
LAB_08000f8e:
                          uVar8 = uVar11;
                          uVar11 = 900;
                          if (*(uint *)(uVar10 + 0x34) != uVar8) goto LAB_08000fee;
                          uVar7 = *(uint *)(iVar5 + 0xc);
                          bVar14 = 0x31e < uVar7;
LAB_08000fa2:
                          bVar13 = false;
                          if (bVar14) {
                            bVar14 = uVar8 <= uVar7;
                            bVar13 = uVar7 == uVar8;
                            uVar7 = uVar11;
LAB_08000fa6:
                            if (!bVar14 || bVar13) {
                              *(undefined2 *)(iVar4 + 0x60) = *(undefined2 *)(uVar10 + 0x38);
                              *(uint *)(uVar10 + 0x34) = uVar7;
                              usart1_putc(0x43);
                              usart1_putc(0x55);
                              usart1_putc(0x52);
                              usart1_putc(0x52);
                              usart1_putc(0x20);
                              usart1_putc(0x30);
                              usart1_putc(0x2e);
                              usart1_putc(0x39);
                              usart1_putc(0xd);
                              usart1_putc(10);
                              uVar8 = extraout_r1;
                              uVar11 = uVar7;
LAB_08000fee:
                              uVar12 = 1000;
                              uVar7 = uVar10;
                              if (*(uint *)(uVar10 + 0x34) != uVar11) goto LAB_0800104c;
                              uVar7 = *(uint *)(iVar5 + 0xc);
                              bVar14 = 0x382 < uVar7;
                              while (bVar13 = false, bVar14) {
                                bVar14 = uVar11 <= uVar7;
                                bVar13 = uVar7 == uVar11;
                                uVar7 = uVar11;
LAB_08001004:
                                if (bVar14 && !bVar13) goto LAB_08000f46;
                                *(undefined2 *)(iVar4 + 0x62) = *(undefined2 *)(uVar10 + 0x38);
                                *(uint *)(uVar10 + 0x34) = uVar12;
                                usart1_putc(0x43);
                                usart1_putc(0x55);
                                usart1_putc(0x52);
                                usart1_putc(0x52);
                                usart1_putc(0x20);
                                usart1_putc(0x31);
                                usart1_putc(0x2e);
                                usart1_putc(0x30);
                                usart1_putc(0xd);
                                usart1_putc(10);
                                uVar7 = uVar10;
LAB_0800104c:
                                uVar10 = uVar12;
                                uVar11 = uVar7;
                                if (*(uint *)(uVar7 + 0x34) == uVar10) {
                                  uVar8 = *(uint *)(iVar5 + 0xc);
                                  bVar14 = 0x3e6 < uVar8;
                                  if (!bVar14) goto LAB_08000fa2;
                                  bVar14 = uVar10 <= uVar8;
                                  bVar13 = uVar8 == uVar10;
                                  if (bVar14 && !bVar13) goto LAB_08000fa6;
                                  *(undefined2 *)(iVar4 + 100) = *(undefined2 *)(uVar7 + 0x38);
                                  *(undefined4 *)(uVar7 + 0x34) = 2000;
                                  usart1_putc(0x43);
                                  usart1_putc(0x55);
                                  usart1_putc(0x52);
                                  usart1_putc(0x52);
                                  usart1_putc(0x20);
                                  usart1_putc(0x32);
                                  usart1_putc(0x2e);
                                  usart1_putc(0x30);
                                  usart1_putc(0xd);
                                  usart1_putc(10);
                                }
                                uVar9 = 2000;
                                uVar12 = 3000;
                                if (*(int *)(uVar7 + 0x34) != 2000) goto LAB_08001114;
                                uVar8 = *(uint *)(iVar5 + 0xc);
                                bVar13 = 0x7ce < uVar8;
                                while (bVar14 = false, bVar13) {
                                  bVar14 = uVar9 <= uVar8;
                                  bVar13 = uVar8 == uVar9;
                                  while( true ) {
                                    if (bVar14 && !bVar13) goto LAB_08001004;
                                    *(undefined2 *)(iVar4 + 0x66) = *(undefined2 *)(uVar7 + 0x38);
                                    *(uint *)(uVar7 + 0x34) = uVar12;
                                    usart1_putc(0x43);
                                    usart1_putc(0x55);
                                    usart1_putc(0x52);
                                    usart1_putc(0x52);
                                    usart1_putc(0x20);
                                    usart1_putc(0x33);
                                    usart1_putc(0x2e);
                                    usart1_putc(0x30);
                                    usart1_putc(0xd);
                                    usart1_putc(10);
LAB_08001114:
                                    uVar8 = *(uint *)(uVar7 + 0x34);
                                    uVar9 = 4000;
                                    if (((uVar8 == uVar12) && (uVar8 - 1 <= *(uint *)(iVar5 + 0xc)))
                                       && (*(uint *)(iVar5 + 0xc) <= uVar8)) {
                                      uVar1 = *(undefined2 *)(uVar7 + 0x38);
                                      *(undefined2 *)(iVar4 + 0x68) = uVar1;
                                      *(undefined2 *)(iVar4 + 0x6a) = uVar1;
                                      *(undefined2 *)(iVar4 + 0x6c) = uVar1;
                                      *(undefined4 *)(uVar7 + 0x34) = 4000;
                                      usart1_putc(0x43);
                                      usart1_putc(0x55);
                                      usart1_putc(0x52);
                                      usart1_putc(0x52);
                                      usart1_putc(0x20);
                                      usart1_putc(0x34);
                                      usart1_putc(0x2e);
                                      usart1_putc(0x30);
                                      usart1_putc(0xd);
                                      usart1_putc(10);
                                    }
                                    uVar12 = 0x157c;
                                    if (*(int *)(uVar7 + 0x34) != 4000) goto LAB_080011e2;
                                    uVar8 = *(uint *)(iVar5 + 0xc);
                                    bVar13 = false;
                                    if (uVar8 < 3999) break;
                                    bVar14 = 3999 < uVar8;
                                    bVar13 = uVar8 == 4000;
                                    if (uVar8 < 0xfa1) {
                                      *(undefined2 *)(iVar4 + 0x6a) = *(undefined2 *)(uVar7 + 0x38);
                                      *(undefined4 *)(uVar7 + 0x34) = 0x157c;
                                      usart1_putc(0x43);
                                      usart1_putc(0x55);
                                      usart1_putc(0x52);
                                      usart1_putc(0x52);
                                      usart1_putc(0x20);
                                      usart1_putc(0x35);
                                      usart1_putc(0x2e);
                                      usart1_putc(0x35);
                                      usart1_putc(0xd);
                                      usart1_putc(10);
LAB_080011e2:
                                      if (((*(int *)(uVar7 + 0x34) == 0x157c) &&
                                          (0x157c - *(ushort *)(iVar4 + 0xc) / 10000 <=
                                           *(uint *)(iVar5 + 0xc))) &&
                                         (*(uint *)(iVar5 + 0xc) < 0x157d)) {
                                        *(undefined2 *)(iVar4 + 0x6c) =
                                             *(undefined2 *)(uVar7 + 0x38);
                                        *(uint *)(uVar7 + 0x38) = (uint)*(ushort *)(iVar4 + 0x6e);
                                        *(undefined4 *)(uVar7 + 0x34) = 6000;
                                        usart1_putc(0x43);
                                        usart1_putc(0x55);
                                        usart1_putc(0x52);
                                        usart1_putc(0x52);
                                        usart1_putc(0x20);
                                        usart1_putc(0x36);
                                        usart1_putc(0x2e);
                                        usart1_putc(0x30);
                                        usart1_putc(0xd);
                                        usart1_putc(10);
                                      }
                                      if (((*(int *)(uVar7 + 0x34) == 6000) &&
                                          (6000 - *(ushort *)(uVar7 + 0x22) <=
                                           *(uint *)(iVar5 + 0xc))) &&
                                         (*(uint *)(iVar5 + 0xc) < 0x1771)) {
                                        *(undefined2 *)(iVar4 + 0x6e) =
                                             *(undefined2 *)(uVar7 + 0x38);
                                        *(undefined4 *)(uVar7 + 0x34) = 7000;
                                        usart1_putc(0x43);
                                        usart1_putc(0x55);
                                        usart1_putc(0x52);
                                        usart1_putc(0x52);
                                        usart1_putc(0x20);
                                        usart1_putc(0x37);
                                        usart1_putc(0x2e);
                                        usart1_putc(0x30);
                                        usart1_putc(0xd);
                                        usart1_putc(10);
                                      }
                                      if (((*(int *)(uVar7 + 0x34) == 7000) &&
                                          (7000 - *(ushort *)(uVar7 + 0x22) <=
                                           *(uint *)(iVar5 + 0xc))) &&
                                         (*(uint *)(iVar5 + 0xc) < 0x1b59)) {
                                        *(undefined2 *)(iVar4 + 0x70) =
                                             *(undefined2 *)(uVar7 + 0x38);
                                        *(undefined4 *)(uVar7 + 0x34) = 8000;
                                        usart1_putc(0x43);
                                        usart1_putc(0x55);
                                        usart1_putc(0x52);
                                        usart1_putc(0x52);
                                        usart1_putc(0x20);
                                        usart1_putc(0x38);
                                        usart1_putc(0x2e);
                                        usart1_putc(0x30);
                                        usart1_putc(0xd);
                                        usart1_putc(10);
                                      }
                                      if (*(int *)(uVar7 + 0x34) == 8000) {
                                        if (*(uint *)(iVar5 + 0xc) <
                                            8000 - *(ushort *)(uVar7 + 0x22)) {
                                          return;
                                        }
                                        if (8000 < *(uint *)(iVar5 + 0xc)) {
                                          return;
                                        }
                                        *(undefined2 *)(iVar4 + 0x72) =
                                             *(undefined2 *)(uVar7 + 0x38);
                                        *(undefined4 *)(uVar7 + 0x34) = 9000;
                                        usart1_putc(0x43);
                                        usart1_putc(0x55);
                                        usart1_putc(0x52);
                                        usart1_putc(0x52);
                                        usart1_putc(0x20);
                                        usart1_putc(0x39);
                                        usart1_putc(0x2e);
                                        usart1_putc(0x30);
                                        usart1_putc(0xd);
                                        usart1_putc(10);
                                      }
                                      if (((*(int *)(uVar7 + 0x34) == 9000) &&
                                          (9000 - *(ushort *)(uVar7 + 0x22) <=
                                           *(uint *)(iVar5 + 0xc))) &&
                                         (*(uint *)(iVar5 + 0xc) < 0x2329)) {
                                        *(undefined2 *)(iVar4 + 0x74) =
                                             *(undefined2 *)(uVar7 + 0x38);
                                        *(undefined4 *)(uVar7 + 0x34) = 10000;
                                        usart1_putc(0x43);
                                        usart1_putc(0x55);
                                        usart1_putc(0x52);
                                        usart1_putc(0x52);
                                        usart1_putc(0x20);
                                        usart1_putc(0x31);
                                        usart1_putc(0x30);
                                        usart1_putc(0x2e);
                                        usart1_putc(0x30);
                                        usart1_putc(0xd);
                                        usart1_putc(10);
                                      }
                                      if (((*(int *)(uVar7 + 0x34) == 10000) &&
                                          (10000 - *(ushort *)(uVar7 + 0x22) <=
                                           *(uint *)(iVar5 + 0xc))) &&
                                         (*(uint *)(iVar5 + 0xc) < 0x2711)) {
                                        uVar1 = *(undefined2 *)(uVar7 + 0x38);
                                        *(undefined2 *)(iVar4 + 0x76) = uVar1;
                                        *(undefined2 *)(iVar4 + 0x78) = uVar1;
                                        *(undefined2 *)(iVar4 + 0x7a) = uVar1;
                                        *(undefined2 *)(iVar4 + 0x7c) = uVar1;
                                        *(undefined2 *)(iVar4 + 0x7e) = uVar1;
                                        *(undefined2 *)(iVar4 + 0x80) = uVar1;
                                        *(undefined2 *)(iVar4 + 0x82) = uVar1;
                                        *(undefined2 *)(iVar4 + 0x84) = uVar1;
                                        *(undefined2 *)(iVar4 + 0x86) = uVar1;
                                        *(undefined2 *)(iVar4 + 0x88) = uVar1;
                                        *(undefined2 *)(iVar4 + 0x8a) = uVar1;
                                        *(undefined2 *)(iVar4 + 0x94) = uVar1;
                                        *(undefined2 *)(iVar4 + 0x9e) = uVar1;
                                        *(undefined2 *)(iVar4 + 0xa8) = uVar1;
                                        *(undefined2 *)(iVar4 + 0xb2) = uVar1;
                                        *(undefined2 *)(iVar4 + 0xbc) = uVar1;
                                        *(undefined2 *)(iVar4 + 0xc6) = uVar1;
                                        *(undefined2 *)(iVar4 + 0xd0) = uVar1;
                                        *(undefined2 *)(iVar4 + 0xda) = uVar1;
                                        *(undefined2 *)(iVar4 + 0xee) = uVar1;
                                        *(undefined2 *)(iVar4 + 0x102) = uVar1;
                                        *(undefined2 *)(iVar4 + 0x116) = uVar1;
                                        *(undefined2 *)(iVar4 + 0x128) = uVar1;
                                        *(undefined4 *)(uVar7 + 0x34) = 11000;
                                        usart1_putc(0x43);
                                        usart1_putc(0x55);
                                        usart1_putc(0x52);
                                        usart1_putc(0x52);
                                        usart1_putc(0x20);
                                        usart1_putc(0x31);
                                        usart1_putc(0x31);
                                        usart1_putc(0x2e);
                                        usart1_putc(0x30);
                                        usart1_putc(0xd);
                                        usart1_putc(10);
                                      }
                                      if (((*(int *)(uVar7 + 0x34) == 11000) &&
                                          (11000 - *(ushort *)(uVar7 + 0x22) <=
                                           *(uint *)(iVar5 + 0xc))) &&
                                         (*(uint *)(iVar5 + 0xc) < 0x2af9)) {
                                        *(undefined2 *)(iVar4 + 0x78) =
                                             *(undefined2 *)(uVar7 + 0x38);
                                        *(undefined4 *)(uVar7 + 0x34) = 12000;
                                        usart1_putc(0x43);
                                        usart1_putc(0x55);
                                        usart1_putc(0x52);
                                        usart1_putc(0x52);
                                        usart1_putc(0x20);
                                        usart1_putc(0x31);
                                        usart1_putc(0x32);
                                        usart1_putc(0x2e);
                                        usart1_putc(0x30);
                                        usart1_putc(0xd);
                                        usart1_putc(10);
                                      }
                                      if (((*(int *)(uVar7 + 0x34) == 12000) &&
                                          (12000 - *(ushort *)(uVar7 + 0x22) <=
                                           *(uint *)(iVar5 + 0xc))) &&
                                         (*(uint *)(iVar5 + 0xc) < 0x2ee1)) {
                                        *(undefined2 *)(iVar4 + 0x7a) =
                                             *(undefined2 *)(uVar7 + 0x38);
                                        *(undefined4 *)(uVar7 + 0x34) = 13000;
                                        usart1_putc(0x43);
                                        usart1_putc(0x55);
                                        usart1_putc(0x52);
                                        usart1_putc(0x52);
                                        usart1_putc(0x20);
                                        usart1_putc(0x31);
                                        usart1_putc(0x33);
                                        usart1_putc(0x2e);
                                        usart1_putc(0x30);
                                        usart1_putc(0xd);
                                        usart1_putc(10);
                                      }
                                      if (((*(int *)(uVar7 + 0x34) == 13000) &&
                                          (13000 - *(ushort *)(uVar7 + 0x22) <=
                                           *(uint *)(iVar5 + 0xc))) &&
                                         (*(uint *)(iVar5 + 0xc) < 0x32c9)) {
                                        *(undefined2 *)(iVar4 + 0x7c) =
                                             *(undefined2 *)(uVar7 + 0x38);
                                        *(undefined4 *)(uVar7 + 0x34) = 14000;
                                        usart1_putc(0x43);
                                        usart1_putc(0x55);
                                        usart1_putc(0x52);
                                        usart1_putc(0x52);
                                        usart1_putc(0x20);
                                        usart1_putc(0x31);
                                        usart1_putc(0x34);
                                        usart1_putc(0x2e);
                                        usart1_putc(0x30);
                                        usart1_putc(0xd);
                                        usart1_putc(10);
                                      }
                                      if (((*(int *)(uVar7 + 0x34) == 14000) &&
                                          (14000 - *(ushort *)(uVar7 + 0x22) <=
                                           *(uint *)(iVar5 + 0xc))) &&
                                         (*(uint *)(iVar5 + 0xc) < 0x36b1)) {
                                        *(undefined2 *)(iVar4 + 0x7e) =
                                             *(undefined2 *)(uVar7 + 0x38);
                                        *(undefined4 *)(uVar7 + 0x34) = 15000;
                                        usart1_putc(0x43);
                                        usart1_putc(0x55);
                                        usart1_putc(0x52);
                                        usart1_putc(0x52);
                                        usart1_putc(0x20);
                                        usart1_putc(0x31);
                                        usart1_putc(0x35);
                                        usart1_putc(0x2e);
                                        usart1_putc(0x30);
                                        usart1_putc(0xd);
                                        usart1_putc(10);
                                      }
                                      if (((*(int *)(uVar7 + 0x34) == 15000) &&
                                          (15000 - *(ushort *)(uVar7 + 0x22) <=
                                           *(uint *)(iVar5 + 0xc))) &&
                                         (*(uint *)(iVar5 + 0xc) < 0x3a99)) {
                                        *(undefined2 *)(iVar4 + 0x80) =
                                             *(undefined2 *)(uVar7 + 0x38);
                                        *(undefined4 *)(uVar7 + 0x34) = 16000;
                                        usart1_putc(0x43);
                                        usart1_putc(0x55);
                                        usart1_putc(0x52);
                                        usart1_putc(0x52);
                                        usart1_putc(0x20);
                                        usart1_putc(0x31);
                                        usart1_putc(0x36);
                                        usart1_putc(0x2e);
                                        usart1_putc(0x30);
                                        usart1_putc(0xd);
                                        usart1_putc(10);
                                      }
                                      if (*(int *)(uVar7 + 0x34) == 16000) {
                                        if (*(uint *)(iVar5 + 0xc) <
                                            16000 - *(ushort *)(uVar7 + 0x22)) {
                                          return;
                                        }
                                        if (16000 < *(uint *)(iVar5 + 0xc)) {
                                          return;
                                        }
                                        *(undefined2 *)(iVar4 + 0x82) =
                                             *(undefined2 *)(uVar7 + 0x38);
                                        *(undefined4 *)(uVar7 + 0x34) = 17000;
                                        usart1_putc(0x43);
                                        usart1_putc(0x55);
                                        usart1_putc(0x52);
                                        usart1_putc(0x52);
                                        usart1_putc(0x20);
                                        usart1_putc(0x31);
                                        usart1_putc(0x37);
                                        usart1_putc(0x2e);
                                        usart1_putc(0x30);
                                        usart1_putc(0xd);
                                        usart1_putc(10);
                                      }
                                      if (((*(int *)(uVar7 + 0x34) == 17000) &&
                                          (17000 - *(ushort *)(uVar7 + 0x22) <=
                                           *(uint *)(iVar5 + 0xc))) &&
                                         (*(uint *)(iVar5 + 0xc) < 0x4269)) {
                                        *(undefined2 *)(iVar4 + 0x84) =
                                             *(undefined2 *)(uVar7 + 0x38);
                                        *(undefined4 *)(uVar7 + 0x34) = 18000;
                                        usart1_putc(0x43);
                                        usart1_putc(0x55);
                                        usart1_putc(0x52);
                                        usart1_putc(0x52);
                                        usart1_putc(0x20);
                                        usart1_putc(0x31);
                                        usart1_putc(0x38);
                                        usart1_putc(0x2e);
                                        usart1_putc(0x30);
                                        usart1_putc(0xd);
                                        usart1_putc(10);
                                      }
                                      if (((*(int *)(uVar7 + 0x34) == 18000) &&
                                          (18000 - *(ushort *)(uVar7 + 0x22) <=
                                           *(uint *)(iVar5 + 0xc))) &&
                                         (*(uint *)(iVar5 + 0xc) < 0x4651)) {
                                        *(undefined2 *)(iVar4 + 0x86) =
                                             *(undefined2 *)(uVar7 + 0x38);
                                        *(undefined4 *)(uVar7 + 0x34) = 19000;
                                        usart1_putc(0x43);
                                        usart1_putc(0x55);
                                        usart1_putc(0x52);
                                        usart1_putc(0x52);
                                        usart1_putc(0x20);
                                        usart1_putc(0x31);
                                        usart1_putc(0x39);
                                        usart1_putc(0x2e);
                                        usart1_putc(0x30);
                                        usart1_putc(0xd);
                                        usart1_putc(10);
                                      }
                                      if (((*(int *)(uVar7 + 0x34) == 19000) &&
                                          (19000 - *(ushort *)(uVar7 + 0x22) <=
                                           *(uint *)(iVar5 + 0xc))) &&
                                         (*(uint *)(iVar5 + 0xc) < 0x4a39)) {
                                        *(undefined2 *)(iVar4 + 0x88) =
                                             *(undefined2 *)(uVar7 + 0x38);
                                        *(undefined4 *)(uVar7 + 0x34) = 20000;
                                        usart1_putc(0x43);
                                        usart1_putc(0x55);
                                        usart1_putc(0x52);
                                        usart1_putc(0x52);
                                        usart1_putc(0x20);
                                        usart1_putc(0x32);
                                        usart1_putc(0x30);
                                        usart1_putc(0x2e);
                                        usart1_putc(0x30);
                                        usart1_putc(0xd);
                                        usart1_putc(10);
                                      }
                                      if (((*(int *)(uVar7 + 0x34) == 20000) &&
                                          (20000 - *(ushort *)(uVar7 + 0x22) <=
                                           *(uint *)(iVar5 + 0xc))) &&
                                         (*(uint *)(iVar5 + 0xc) < 0x4e21)) {
                                        uVar1 = *(undefined2 *)(uVar7 + 0x38);
                                        *(undefined2 *)(iVar4 + 0x8a) = uVar1;
                                        *(undefined2 *)(iVar4 + 0x94) = uVar1;
                                        *(undefined2 *)(iVar4 + 0x9e) = uVar1;
                                        *(undefined2 *)(iVar4 + 0xa8) = uVar1;
                                        *(undefined2 *)(iVar4 + 0xb2) = uVar1;
                                        *(undefined2 *)(iVar4 + 0xbc) = uVar1;
                                        *(undefined2 *)(iVar4 + 0xc6) = uVar1;
                                        *(undefined2 *)(iVar4 + 0xd0) = uVar1;
                                        *(undefined2 *)(iVar4 + 0xda) = uVar1;
                                        *(undefined2 *)(iVar4 + 0xee) = uVar1;
                                        *(undefined2 *)(iVar4 + 0x102) = uVar1;
                                        *(undefined2 *)(iVar4 + 0x116) = uVar1;
                                        *(undefined2 *)(iVar4 + 0x128) = uVar1;
                                        *(undefined4 *)(uVar7 + 0x34) = 25000;
                                        usart1_putc(0x43);
                                        usart1_putc(0x55);
                                        usart1_putc(0x52);
                                        usart1_putc(0x52);
                                        usart1_putc(0x20);
                                        usart1_putc(0x32);
                                        usart1_putc(0x35);
                                        usart1_putc(0x2e);
                                        usart1_putc(0x30);
                                        usart1_putc(0xd);
                                        usart1_putc(10);
                                      }
                                      if (*(int *)(uVar7 + 0x34) == 25000) {
                                        uVar11 = *puVar6;
                                        bVar13 = uVar11 == 0x4e83;
                                        do {
                                          if (bVar13) {
                                            *(undefined4 *)(uVar7 + 0x34) = 0xffff;
                                            return;
                                          }
                                          bVar13 = true;
                                        } while ((uVar11 == 0x4e84) ||
                                                (bVar13 = true, uVar11 == 0x4e85));
                                      }
                                      if ((*(int *)(uVar7 + 0x34) == 25000) &&
                                         ((25000 - 25000 / uVar10 <= *(uint *)(iVar5 + 0xc) &&
                                          (*(uint *)(iVar5 + 0xc) < 0x61a9)))) {
                                        *(undefined2 *)(iVar4 + 0x94) =
                                             *(undefined2 *)(uVar7 + 0x38);
                                        *(undefined4 *)(uVar7 + 0x34) = 30000;
                                        usart1_putc(0x43);
                                        usart1_putc(0x55);
                                        usart1_putc(0x52);
                                        usart1_putc(0x52);
                                        usart1_putc(0x20);
                                        usart1_putc(0x33);
                                        usart1_putc(0x30);
                                        usart1_putc(0x2e);
                                        usart1_putc(0x30);
                                        usart1_putc(0xd);
                                        usart1_putc(10);
                                      }
                                      if (((*(int *)(uVar7 + 0x34) == 30000) &&
                                          (30000 - 30000 / uVar10 <= *(uint *)(iVar5 + 0xc))) &&
                                         (*(uint *)(iVar5 + 0xc) < 0x7531)) {
                                        uVar1 = *(undefined2 *)(uVar7 + 0x38);
                                        *(undefined2 *)(iVar4 + 0x9e) = uVar1;
                                        *(undefined2 *)(iVar4 + 0xa8) = uVar1;
                                        *(undefined2 *)(iVar4 + 0xb2) = uVar1;
                                        *(undefined2 *)(iVar4 + 0xbc) = uVar1;
                                        *(undefined2 *)(iVar4 + 0xc6) = uVar1;
                                        *(undefined2 *)(iVar4 + 0xd0) = uVar1;
                                        *(undefined2 *)(iVar4 + 0xda) = uVar1;
                                        *(undefined2 *)(iVar4 + 0xee) = uVar1;
                                        *(undefined2 *)(iVar4 + 0x102) = uVar1;
                                        *(undefined2 *)(iVar4 + 0x116) = uVar1;
                                        *(undefined2 *)(iVar4 + 0x128) = uVar1;
                                        *(undefined4 *)(uVar7 + 0x34) = 35000;
                                        usart1_putc(0x43);
                                        usart1_putc(0x55);
                                        usart1_putc(0x52);
                                        usart1_putc(0x52);
                                        usart1_putc(0x20);
                                        usart1_putc(0x33);
                                        usart1_putc(0x35);
                                        usart1_putc(0x2e);
                                        usart1_putc(0x30);
                                        usart1_putc(0xd);
                                        usart1_putc(10);
                                      }
                                      if (((*(int *)(uVar7 + 0x34) == 35000) &&
                                          (35000 - 35000 / uVar10 <= *(uint *)(iVar5 + 0xc))) &&
                                         (*(uint *)(iVar5 + 0xc) < 0x88b9)) {
                                        *(undefined2 *)(iVar4 + 0xa8) =
                                             *(undefined2 *)(uVar7 + 0x38);
                                        *(undefined4 *)(uVar7 + 0x34) = 40000;
                                        usart1_putc(0x43);
                                        usart1_putc(0x55);
                                        usart1_putc(0x52);
                                        usart1_putc(0x52);
                                        usart1_putc(0x20);
                                        usart1_putc(0x34);
                                        usart1_putc(0x30);
                                        usart1_putc(0x2e);
                                        usart1_putc(0x30);
                                        usart1_putc(0xd);
                                        usart1_putc(10);
                                      }
                                      if (((*(int *)(uVar7 + 0x34) == 40000) &&
                                          (40000 - 40000 / uVar10 <= *(uint *)(iVar5 + 0xc))) &&
                                         (*(uint *)(iVar5 + 0xc) < 0x9c41)) {
                                        uVar1 = *(undefined2 *)(uVar7 + 0x38);
                                        *(undefined2 *)(iVar4 + 0xb2) = uVar1;
                                        *(undefined2 *)(iVar4 + 0xbc) = uVar1;
                                        *(undefined2 *)(iVar4 + 0xc6) = uVar1;
                                        *(undefined2 *)(iVar4 + 0xd0) = uVar1;
                                        *(undefined2 *)(iVar4 + 0xda) = uVar1;
                                        *(undefined2 *)(iVar4 + 0xee) = uVar1;
                                        *(undefined2 *)(iVar4 + 0x102) = uVar1;
                                        *(undefined2 *)(iVar4 + 0x116) = uVar1;
                                        *(undefined2 *)(iVar4 + 0x128) = uVar1;
                                        *(undefined4 *)(uVar7 + 0x34) = 45000;
                                        usart1_putc(0x43);
                                        usart1_putc(0x55);
                                        usart1_putc(0x52);
                                        usart1_putc(0x52);
                                        usart1_putc(0x20);
                                        usart1_putc(0x34);
                                        usart1_putc(0x35);
                                        usart1_putc(0x2e);
                                        usart1_putc(0x30);
                                        usart1_putc(0xd);
                                        usart1_putc(10);
                                      }
                                      if (((*(int *)(uVar7 + 0x34) == 45000) &&
                                          (45000 - 45000 / uVar10 <= *(uint *)(iVar5 + 0xc))) &&
                                         (*(uint *)(iVar5 + 0xc) < 0xafc9)) {
                                        *(undefined2 *)(iVar4 + 0xbc) =
                                             *(undefined2 *)(uVar7 + 0x38);
                                        *(undefined4 *)(uVar7 + 0x34) = 50000;
                                        usart1_putc(0x43);
                                        usart1_putc(0x55);
                                        usart1_putc(0x52);
                                        usart1_putc(0x52);
                                        usart1_putc(0x20);
                                        usart1_putc(0x35);
                                        usart1_putc(0x30);
                                        usart1_putc(0x2e);
                                        usart1_putc(0x30);
                                        usart1_putc(0xd);
                                        usart1_putc(10);
                                      }
                                      if (((*(int *)(uVar7 + 0x34) == 50000) &&
                                          (50000 - 50000 / uVar10 <= *(uint *)(iVar5 + 0xc))) &&
                                         (*(uint *)(iVar5 + 0xc) < 0xc351)) {
                                        uVar1 = *(undefined2 *)(uVar7 + 0x38);
                                        *(undefined2 *)(iVar4 + 0xc6) = uVar1;
                                        *(undefined2 *)(iVar4 + 0xd0) = uVar1;
                                        *(undefined2 *)(iVar4 + 0xda) = uVar1;
                                        *(undefined2 *)(iVar4 + 0xee) = uVar1;
                                        *(undefined2 *)(iVar4 + 0x102) = uVar1;
                                        *(undefined2 *)(iVar4 + 0x116) = uVar1;
                                        *(undefined2 *)(iVar4 + 0x128) = uVar1;
                                        *(undefined4 *)(uVar7 + 0x34) = 55000;
                                        usart1_putc(0x43);
                                        usart1_putc(0x55);
                                        usart1_putc(0x52);
                                        usart1_putc(0x52);
                                        usart1_putc(0x20);
                                        usart1_putc(0x35);
                                        usart1_putc(0x35);
                                        usart1_putc(0x2e);
                                        usart1_putc(0x30);
                                        usart1_putc(0xd);
                                        usart1_putc(10);
                                      }
                                      if (((*(int *)(uVar7 + 0x34) == 55000) &&
                                          (55000 - 55000 / uVar10 <= *(uint *)(iVar5 + 0xc))) &&
                                         (*(uint *)(iVar5 + 0xc) < 0xd6d9)) {
                                        *(undefined2 *)(iVar4 + 0xd0) =
                                             *(undefined2 *)(uVar7 + 0x38);
                                        *(undefined4 *)(uVar7 + 0x34) = 60000;
                                        usart1_putc(0x43);
                                        usart1_putc(0x55);
                                        usart1_putc(0x52);
                                        usart1_putc(0x52);
                                        usart1_putc(0x20);
                                        usart1_putc(0x36);
                                        usart1_putc(0x30);
                                        usart1_putc(0x2e);
                                        usart1_putc(0x30);
                                        usart1_putc(0xd);
                                        usart1_putc(10);
                                      }
                                      if (((*(int *)(uVar7 + 0x34) == 60000) &&
                                          (60000 - 60000 / uVar10 <= *(uint *)(iVar5 + 0xc))) &&
                                         (*(uint *)(iVar5 + 0xc) < 0xea61)) {
                                        uVar1 = *(undefined2 *)(uVar7 + 0x38);
                                        *(undefined2 *)(iVar4 + 0xda) = uVar1;
                                        *(undefined2 *)(iVar4 + 0xee) = uVar1;
                                        *(undefined2 *)(iVar4 + 0x102) = uVar1;
                                        *(undefined2 *)(iVar4 + 0x116) = uVar1;
                                        *(undefined2 *)(iVar4 + 0x128) = uVar1;
                                        *(undefined4 *)(uVar7 + 0x34) = 61000;
                                        usart1_putc(0x43);
                                        usart1_putc(0x55);
                                        usart1_putc(0x52);
                                        usart1_putc(0x52);
                                        usart1_putc(0x20);
                                        usart1_putc(0x36);
                                        usart1_putc(0x35);
                                        usart1_putc(0x2e);
                                        usart1_putc(0x30);
                                        usart1_putc(0xd);
                                        usart1_putc(10);
                                        return;
                                      }
                                      return;
                                    }
                                  }
                                }
                              }
                              goto LAB_08000f42;
                            }
                            goto LAB_08000ed0;
                          }
                          goto LAB_08000ecc;
                        }
                        goto LAB_08000e70;
                      }
                      goto LAB_08000e6c;
                    }
                    goto LAB_08000e10;
                  }
                  goto LAB_08000e0c;
                }
                goto LAB_08000db0;
              }
              goto LAB_08000dac;
            }
            goto LAB_08000d4c;
          }
          goto LAB_08000d48;
        }
        goto LAB_08000cf2;
      }
      goto LAB_08000cee;
    }
  } while( true );
}


// ===== cal_dac_table @ 08001ca4 (size 2118)

/* Factory DAC calibration (cmd 0x6f/0x79) */

void cal_dac_table(void)

{
  ushort uVar1;
  short sVar2;
  int iVar3;
  short *psVar4;
  int iVar5;
  int iVar6;
  uint *puVar7;
  undefined2 *puVar8;
  uint uVar9;
  int iVar10;
  undefined2 uVar11;
  uint uVar12;
  bool bVar13;
  bool bVar14;
  
  iVar6 = 0x200001a0;
  iVar5 = 0x20000398;
  psVar4 = 0x200001fa;
  iVar3 = 0x20000654;
  if (*0x20000032 == 0x79) {
    uVar9 = *(uint *)(0x20000654 + 0x14);
    sVar2 = *0x200001fa;
    if ((uVar9 == 0) || (*(ushort *)(0x20000398 + 0xc) < uVar9)) {
      *(short *)(0x20000398 + 0x12a) = sVar2;
    }
    else if (uVar9 < 0xb) {
      *(short *)(0x20000398 + 300) = sVar2;
    }
    else if (uVar9 < 0x15) {
      *(short *)(0x20000398 + 0x12e) = sVar2;
    }
    else if (uVar9 < 0x1f) {
      *(short *)(0x20000398 + 0x130) = sVar2;
    }
    else if (uVar9 < 0x29) {
      *(short *)(0x20000398 + 0x132) = sVar2;
    }
    else if (uVar9 < 0x33) {
      *(short *)(0x20000398 + 0x134) = sVar2;
    }
    else if (uVar9 < 0x3d) {
      *(short *)(0x20000398 + 0x136) = sVar2;
    }
    else if (uVar9 < 0x47) {
      *(short *)(0x20000398 + 0x138) = sVar2;
    }
    else if (uVar9 < 0x51) {
      *(short *)(0x20000398 + 0x13a) = sVar2;
    }
    else if (uVar9 < 0x5b) {
      *(short *)(0x20000398 + 0x13c) = sVar2;
    }
    else if (uVar9 < 0x65) {
      *(short *)(0x20000398 + 0x13e) = sVar2;
    }
    else if (uVar9 < 0xc9) {
      *(short *)(0x20000398 + 0x140) = sVar2;
    }
    else if (uVar9 < 0x12d) {
      *(short *)(0x20000398 + 0x142) = sVar2;
    }
    else if (uVar9 < 0x191) {
      *(short *)(0x20000398 + 0x144) = sVar2;
    }
    else if (uVar9 < 0x1f5) {
      *(short *)(0x20000398 + 0x146) = sVar2;
    }
    else if (uVar9 < 0x259) {
      *(short *)(0x20000398 + 0x148) = sVar2;
    }
    else if (uVar9 < 0x2bd) {
      *(short *)(0x20000398 + 0x14a) = sVar2;
    }
    else if (uVar9 < 0x321) {
      *(short *)(0x20000398 + 0x14c) = sVar2;
    }
    else if (uVar9 < 0x385) {
      *(short *)(0x20000398 + 0x14e) = sVar2;
    }
    else if (uVar9 < 0x3e9) {
      *(short *)(0x20000398 + 0x150) = sVar2;
    }
    else if (uVar9 < 0x7d1) {
      *(short *)(0x20000398 + 0x152) = sVar2;
    }
    else if (uVar9 < 0xbb9) {
      *(short *)(0x20000398 + 0x154) = sVar2;
    }
    else if (uVar9 < 0xfa1) {
      *(short *)(0x20000398 + 0x156) = sVar2;
    }
    else if (uVar9 < 0x157d) {
      *(short *)(0x20000398 + 0x158) = sVar2;
    }
    else if (uVar9 < 0x1771) {
      *(short *)(0x20000398 + 0x15a) = sVar2;
    }
    else if (uVar9 < 0x1b59) {
      *(short *)(0x20000398 + 0x15c) = sVar2;
    }
    else if (uVar9 < 0x1f41) {
      *(short *)(0x20000398 + 0x15e) = sVar2;
    }
    else if (uVar9 < 0x2329) {
      *(short *)(0x20000398 + 0x160) = sVar2;
    }
    else if (uVar9 < 0x2711) {
      *(short *)(0x20000398 + 0x162) = sVar2;
    }
    else if (uVar9 < 0x4e21) {
      *(short *)(0x20000398 + 0x176) = sVar2;
    }
    else if (uVar9 < 0x7531) {
      *(short *)(0x20000398 + 0x18a) = sVar2;
    }
    else if (uVar9 < 0x9c41) {
      *(short *)(0x20000398 + 0x19e) = sVar2;
    }
    else if (uVar9 < 0xc351) {
      *(short *)(0x20000398 + 0x1b2) = sVar2;
    }
    else if (uVar9 < 0xea61) {
      *(short *)(0x20000398 + 0x1c6) = sVar2;
    }
    *0x20000032 = 0x78;
    return;
  }
  if (*0x20000032 != 0x6f) {
    return;
  }
  if (*(short *)(0x200001a0 + 0x18) != 1) {
    *(undefined2 *)(0x200001a0 + 0x18) = 1;
    *(undefined4 *)(iVar6 + 0x34) = 10;
    *(undefined4 *)(iVar3 + 4) = 1;
    *psVar4 = *(short *)(iVar5 + 300);
  }
  usart1_putc(0x4d);
  usart1_putc(0x45);
  usart1_putc(0x41);
  usart1_putc(0x53);
  usart1_putc(0x75);
  usart1_putc(0x72);
  usart1_putc(0x65);
  usart1_putc(0x3a);
  usart1_putc(0x43);
  usart1_putc(0x55);
  usart1_putc(0x52);
  usart1_putc(0x52);
  usart1_putc(0x65);
  usart1_putc(0x6e);
  usart1_putc(0x74);
  usart1_putc(0x3f);
  usart1_putc(0xd);
  usart1_putc(10);
  puVar7 = 0x20000218;
  if (29999 < *(uint *)(iVar6 + 0x34)) {
    if (((*(uint *)(iVar6 + 0x34) == 30000) && (0x128 < *(uint *)(iVar3 + 8))) &&
       (*(uint *)(iVar3 + 8) < 0x130)) {
      uVar1 = *(ushort *)(iVar6 + 0x20);
      *(ushort *)(iVar6 + 0x20) = uVar1 + 1;
      if (299 < uVar1) {
        *(undefined4 *)(iVar3 + 4) = 1;
      }
    }
    else {
      *(undefined2 *)(iVar6 + 0x20) = 0;
    }
    uVar9 = *puVar7;
    if (((uVar9 == 0x4e83) || (uVar9 == 0x4e84)) || (uVar9 == 0x4e85)) {
      *(undefined2 *)(iVar6 + 0x16) = 1;
      *(undefined4 *)(iVar3 + 4) = 0;
LAB_08001f5a:
      measure_voltage();
    }
    else if (*(int *)(iVar3 + 4) == 0) goto LAB_08001f5a;
    if (*(short *)(iVar6 + 0x16) != 0) {
      *puVar7 = *(uint *)(iVar3 + 0xc);
    }
  }
  uVar9 = *(uint *)(iVar3 + 0x14);
  if (uVar9 < 0x157c) {
    if ((uVar9 - 0x19 < *puVar7) && (*puVar7 < uVar9 + 0x19)) {
      uVar11 = 10;
      goto LAB_08001fac;
    }
LAB_08001fb0:
    *(undefined2 *)(iVar6 + 0x1c) = 5;
  }
  else {
    if ((*puVar7 <= uVar9 - uVar9 / 500) || (uVar9 / 500 + uVar9 <= *puVar7)) goto LAB_08001fb0;
    uVar11 = 0x32;
LAB_08001fac:
    *(undefined2 *)(iVar6 + 0x1c) = uVar11;
  }
  uVar1 = *(ushort *)(iVar6 + 0x1a);
  *(ushort *)(iVar6 + 0x1a) = uVar1 + 1;
  if ((((*(ushort *)(iVar6 + 0x1c) <= uVar1) &&
       (*(undefined2 *)(iVar6 + 0x1a) = 0, *(int *)(iVar3 + 4) != 0)) &&
      (uVar12 = *puVar7, uVar12 != 0)) && (uVar9 != uVar12)) {
    if (uVar9 < uVar12) {
      *psVar4 = *psVar4 + -1;
    }
    else {
      *psVar4 = *psVar4 + 1;
    }
  }
  uVar1 = *(ushort *)(iVar6 + 0x1e);
  *(ushort *)(iVar6 + 0x1e) = uVar1 + 1;
  if (uVar1 < 5) goto LAB_0800246c;
  *(undefined2 *)(iVar6 + 0x1e) = 0;
  iVar10 = *(int *)(iVar6 + 0x34);
  *(int *)(iVar3 + 0x14) = iVar10;
  if (iVar10 == 10) {
    bVar14 = *puVar7 != 9;
    bVar13 = *puVar7 == 10;
    goto LAB_08002082;
  }
  if (iVar10 == 0x14) goto LAB_08002094;
  if (iVar10 == 0x1e) goto LAB_080020c6;
  if (iVar10 == 0x28) goto LAB_080020de;
  if (iVar10 == 0x32) {
    while( true ) {
      uVar9 = *puVar7 - 0x2d;
      bVar14 = 4 < uVar9;
      bVar13 = uVar9 == 5;
      if (uVar9 < 6) break;
LAB_08002082:
      do {
        do {
          do {
            if (bVar14 && !bVar13) goto LAB_0800246c;
            *(short *)(iVar5 + 300) = *psVar4;
            *(undefined4 *)(iVar6 + 0x34) = 0x14;
            *psVar4 = *(short *)(iVar5 + 0x12e);
LAB_08002094:
            uVar9 = *puVar7 - 0x12;
            bVar14 = 1 < uVar9;
            bVar13 = uVar9 == 2;
          } while (2 < uVar9);
          *(short *)(iVar5 + 0x12e) = *psVar4;
          *(undefined4 *)(iVar6 + 0x34) = 0x1e;
          *psVar4 = *(short *)(iVar5 + 0x130);
LAB_080020c6:
          uVar9 = *puVar7 - 0x1b;
          bVar14 = 2 < uVar9;
          bVar13 = uVar9 == 3;
        } while (3 < uVar9);
        *(short *)(iVar5 + 0x130) = *psVar4;
        *(undefined4 *)(iVar6 + 0x34) = 0x28;
        *psVar4 = *(short *)(iVar5 + 0x132);
LAB_080020de:
        uVar9 = *puVar7 - 0x24;
        bVar14 = 3 < uVar9;
        bVar13 = uVar9 == 4;
      } while (4 < uVar9);
      *(short *)(iVar5 + 0x132) = *psVar4;
      *(undefined4 *)(iVar6 + 0x34) = 0x32;
      *psVar4 = *(short *)(iVar5 + 0x134);
    }
    *(short *)(iVar5 + 0x134) = *psVar4;
    *(undefined4 *)(iVar6 + 0x34) = 0x3c;
    *psVar4 = *(short *)(iVar5 + 0x136);
LAB_08002124:
    if (5 < *puVar7 - 0x37) goto LAB_0800246c;
    *(short *)(iVar5 + 0x136) = *psVar4;
    *(undefined4 *)(iVar6 + 0x34) = 0x46;
    *psVar4 = *(short *)(iVar5 + 0x138);
LAB_0800214e:
    if (5 < *puVar7 - 0x41) goto LAB_0800246c;
    *(short *)(iVar5 + 0x138) = *psVar4;
    *(undefined4 *)(iVar6 + 0x34) = 0x50;
    *psVar4 = *(short *)(iVar5 + 0x13a);
LAB_08002166:
    if (5 < *puVar7 - 0x4b) goto LAB_0800246c;
    *(short *)(iVar5 + 0x13a) = *psVar4;
    *(undefined4 *)(iVar6 + 0x34) = 0x5a;
    *psVar4 = *(short *)(iVar5 + 0x13c);
LAB_0800217e:
    if (5 < *puVar7 - 0x55) goto LAB_0800246c;
    *(short *)(iVar5 + 0x13c) = *psVar4;
    *(undefined4 *)(iVar6 + 0x34) = 100;
    *psVar4 = *(short *)(iVar5 + 0x13e);
LAB_08002196:
    if (5 < *puVar7 - 0x5f) goto LAB_0800246c;
    *(short *)(iVar5 + 0x13e) = *psVar4;
    *(undefined4 *)(iVar6 + 0x34) = 200;
    *psVar4 = *(short *)(iVar5 + 0x140);
LAB_080021ae:
    if (10 < *puVar7 - 0xbe) goto LAB_0800246c;
    *(short *)(iVar5 + 0x140) = *psVar4;
    *(undefined4 *)(iVar6 + 0x34) = 300;
    *psVar4 = *(short *)(iVar5 + 0x142);
LAB_080021c8:
    if (10 < *puVar7 - 0x122) goto LAB_0800246c;
    *(short *)(iVar5 + 0x142) = *psVar4;
    *(undefined4 *)(iVar6 + 0x34) = 400;
    *psVar4 = *(short *)(iVar5 + 0x144);
LAB_080021e4:
    if (10 < *puVar7 - 0x186) goto LAB_0800246c;
    *(short *)(iVar5 + 0x144) = *psVar4;
    *(undefined4 *)(iVar6 + 0x34) = 500;
    *psVar4 = *(short *)(iVar5 + 0x146);
LAB_08002200:
    if (10 < *puVar7 - 0x1ea) goto LAB_0800246c;
    *(short *)(iVar5 + 0x146) = *psVar4;
    *(undefined4 *)(iVar6 + 0x34) = 600;
    *psVar4 = *(short *)(iVar5 + 0x148);
LAB_0800221c:
    if (10 < *puVar7 - 0x24e) goto LAB_0800246c;
    *(short *)(iVar5 + 0x148) = *psVar4;
    *(undefined4 *)(iVar6 + 0x34) = 700;
    *psVar4 = *(short *)(iVar5 + 0x14a);
LAB_0800223c:
    if (10 < *puVar7 - 0x2b2) goto LAB_0800246c;
    *(short *)(iVar5 + 0x14a) = *psVar4;
    *(undefined4 *)(iVar6 + 0x34) = 800;
    *psVar4 = *(short *)(iVar5 + 0x14c);
LAB_08002258:
    if (10 < *puVar7 - 0x316) goto LAB_0800246c;
    *(short *)(iVar5 + 0x14c) = *psVar4;
    *(undefined4 *)(iVar6 + 0x34) = 900;
    *psVar4 = *(short *)(iVar5 + 0x14e);
LAB_08002274:
    if (10 < *puVar7 - 0x37a) goto LAB_0800246c;
    *(short *)(iVar5 + 0x14e) = *psVar4;
    *(undefined4 *)(iVar6 + 0x34) = 1000;
    *psVar4 = *(short *)(iVar5 + 0x150);
LAB_08002290:
    if (10 < *puVar7 - 0x3de) goto LAB_0800246c;
    *(short *)(iVar5 + 0x150) = *psVar4;
    *(undefined4 *)(iVar6 + 0x34) = 2000;
    *psVar4 = *(short *)(iVar5 + 0x152);
LAB_080022ac:
    if (10 < *puVar7 - 0x7c6) goto LAB_0800246c;
    *(short *)(iVar5 + 0x152) = *psVar4;
    *(undefined4 *)(iVar6 + 0x34) = 3000;
    *psVar4 = *(short *)(iVar5 + 0x154);
LAB_0800205e:
    if (10 < *puVar7 - 0xbae) goto LAB_08002142;
    *(short *)(iVar5 + 0x154) = *psVar4;
    *(undefined4 *)(iVar6 + 0x34) = 4000;
    *psVar4 = *(short *)(iVar5 + 0x156);
LAB_080022f4:
    if (10 < *puVar7 - 0xf96) goto LAB_0800246c;
    *(short *)(iVar5 + 0x156) = *psVar4;
    *(undefined4 *)(iVar6 + 0x34) = 0x157c;
    *psVar4 = *(short *)(iVar5 + 0x158);
LAB_080022d4:
    if (10 < *puVar7 - 0x1572) goto LAB_08002310;
    *(short *)(iVar5 + 0x158) = *psVar4;
    *(undefined4 *)(iVar6 + 0x34) = 6000;
    *psVar4 = *(short *)(iVar5 + 0x15a);
LAB_0800231a:
    if (0x14 < *puVar7 - 0x175c) goto LAB_08002336;
    *(short *)(iVar5 + 0x15a) = *psVar4;
    *(undefined4 *)(iVar6 + 0x34) = 7000;
LAB_08002340:
    if (0x14 < *puVar7 - 0x1b44) goto LAB_0800235a;
    *(short *)(iVar5 + 0x15c) = *psVar4;
    *(undefined4 *)(iVar6 + 0x34) = 8000;
LAB_08002384:
    if (0x14 < *puVar7 - 0x1f2c) goto LAB_0800246c;
    *(short *)(iVar5 + 0x15e) = *psVar4;
    *(undefined4 *)(iVar6 + 0x34) = 9000;
LAB_0800236a:
    if (0x14 < *puVar7 - 0x2314) goto LAB_0800239e;
    *(short *)(iVar5 + 0x160) = *psVar4;
    *(undefined4 *)(iVar6 + 0x34) = 10000;
LAB_080023a8:
    if (0x14 < *puVar7 - 0x26fc) goto LAB_080023c2;
    *(short *)(iVar5 + 0x162) = *psVar4;
    *(undefined4 *)(iVar6 + 0x34) = 20000;
LAB_080023cc:
    if (0x14 < *puVar7 - 0x4e0c) goto LAB_080023e4;
    *(short *)(iVar5 + 0x176) = *psVar4;
    *(undefined4 *)(iVar6 + 0x34) = 30000;
LAB_080023ea:
    if (0x1e < *puVar7 - 0x7512) goto LAB_08002404;
    *(short *)(iVar5 + 0x18a) = *psVar4;
    *(undefined4 *)(iVar6 + 0x34) = 40000;
LAB_0800240e:
    if (0x28 < *puVar7 - 0x9c18) goto LAB_08002428;
    *(short *)(iVar5 + 0x19e) = *psVar4;
    *(undefined4 *)(iVar6 + 0x34) = 50000;
LAB_08002432:
    if (0x32 < *puVar7 - 0xc31e) goto LAB_0800244c;
    *(short *)(iVar5 + 0x1b2) = *psVar4;
    *(undefined4 *)(iVar6 + 0x34) = 60000;
  }
  else {
    if (iVar10 == 0x3c) goto LAB_08002124;
    if (iVar10 == 0x46) goto LAB_0800214e;
    if (iVar10 == 0x50) goto LAB_08002166;
    if (iVar10 == 0x5a) goto LAB_0800217e;
    if (iVar10 == 100) goto LAB_08002196;
    if (iVar10 == 200) goto LAB_080021ae;
    if (iVar10 == 300) goto LAB_080021c8;
    if (iVar10 == 400) goto LAB_080021e4;
    if (iVar10 == 500) goto LAB_08002200;
    if (iVar10 == 600) goto LAB_0800221c;
    if (iVar10 == 700) goto LAB_0800223c;
    if (iVar10 == 800) goto LAB_08002258;
    if (iVar10 == 900) goto LAB_08002274;
    if (iVar10 == 1000) goto LAB_08002290;
    if (iVar10 == 2000) goto LAB_080022ac;
    if (iVar10 == 3000) goto LAB_0800205e;
LAB_08002142:
    if (*(int *)(iVar6 + 0x34) == 4000) goto LAB_080022f4;
    if (*(int *)(iVar6 + 0x34) == 0x157c) goto LAB_080022d4;
LAB_08002310:
    if (*(int *)(iVar6 + 0x34) == 6000) goto LAB_0800231a;
LAB_08002336:
    if (*(int *)(iVar6 + 0x34) == 7000) goto LAB_08002340;
LAB_0800235a:
    if (*(int *)(iVar6 + 0x34) == 8000) goto LAB_08002384;
    if (*(int *)(iVar6 + 0x34) == 9000) goto LAB_0800236a;
LAB_0800239e:
    if (*(int *)(iVar6 + 0x34) == 10000) goto LAB_080023a8;
LAB_080023c2:
    if (*(int *)(iVar6 + 0x34) == 20000) goto LAB_080023cc;
LAB_080023e4:
    if (*(int *)(iVar6 + 0x34) == 30000) goto LAB_080023ea;
LAB_08002404:
    if (*(int *)(iVar6 + 0x34) == 40000) goto LAB_0800240e;
LAB_08002428:
    if (*(int *)(iVar6 + 0x34) == 50000) goto LAB_08002432;
LAB_0800244c:
    if (*(int *)(iVar6 + 0x34) != 60000) goto LAB_0800246c;
  }
  if (*puVar7 - 0xea24 < 0x3d) {
    *(short *)(iVar5 + 0x1c6) = *psVar4;
    *(undefined4 *)(iVar6 + 0x34) = 0x11170;
  }
LAB_0800246c:
  uVar9 = (uint)*(ushort *)(iVar5 + 0xc);
  if (uVar9 < *(uint *)(iVar6 + 0x34)) {
    if (2999 < uVar9) {
      *(short *)(iVar5 + 0x12a) = *(short *)(iVar5 + 0x154);
      *psVar4 = *(short *)(iVar5 + 0x154);
    }
    if (9999 < uVar9) {
      *(short *)(iVar5 + 0x12a) = *(short *)(iVar5 + 0x162);
      *psVar4 = *(short *)(iVar5 + 0x162);
    }
    if (19999 < uVar9) {
      *(short *)(iVar5 + 0x12a) = *(short *)(iVar5 + 0x176);
      *psVar4 = *(short *)(iVar5 + 0x176);
    }
    if (29999 < uVar9) {
      *(short *)(iVar5 + 0x12a) = *(short *)(iVar5 + 0x18a);
      *psVar4 = *(short *)(iVar5 + 0x18a);
    }
    if (39999 < uVar9) {
      *(short *)(iVar5 + 0x12a) = *(short *)(iVar5 + 0x19e);
      *psVar4 = *(short *)(iVar5 + 0x19e);
    }
    if (49999 < uVar9) {
      *(short *)(iVar5 + 0x12a) = *(short *)(iVar5 + 0x1b2);
      *psVar4 = *(short *)(iVar5 + 0x1b2);
    }
    if (59999 < uVar9) {
      *(short *)(iVar5 + 0x12a) = *(short *)(iVar5 + 0x1c6);
      *psVar4 = *(short *)(iVar5 + 0x1c6);
    }
    *(undefined2 *)(iVar6 + 0x18) = 0;
    *(undefined2 *)(iVar6 + 0x16) = 0;
    *(undefined4 *)(iVar6 + 0x34) = 0;
    puVar8 = 0x20000032;
    *(undefined4 *)(iVar3 + 0x14) = 0;
    *puVar8 = 0x78;
  }
  return;
}


// ===== DAC_Cmd @ 0800250c (size 20)

void DAC_Cmd(uint param_1,int param_2)

{
  uint uVar1;
  
  uVar1 = 1 << (param_1 & 0xff);
  if (param_2 == 0) {
    uVar1 = *0x40007400/*DAC*/ & ~uVar1;
  }
  else {
    uVar1 = *0x40007400/*DAC*/ | uVar1;
  }
  *0x40007400/*DAC*/ = uVar1;
  return;
}


// ===== DAC_Init @ 08002524 (size 36)

void DAC_Init(uint param_1,uint *param_2)

{
  *0x40007400/*DAC*/ =
       (*param_2 | param_2[1] | param_2[2] | param_2[3]) << (param_1 & 0xff) |
       *0x40007400/*DAC*/ & ~(0xffe << (param_1 & 0xff));
  return;
}


// ===== DAC_SetChannel1Data @ 0800254c (size 14)

/* CV setpoint DAC (PA4) */

void DAC_SetChannel1Data(int param_1,undefined4 param_2)

{
  *(undefined4 *)(param_1 + 0x40007400/*DAC*/ + 8) = param_2;
  return;
}


// ===== DAC_SetChannel2Data @ 08002560 (size 14)

/* CC setpoint DAC (PA5) */

void DAC_SetChannel2Data(int param_1,undefined4 param_2)

{
  *(undefined4 *)(param_1 + 0x40007400/*DAC*/ + 0x14) = param_2;
  return;
}


// ===== dac8562_write @ 08002574 (size 46)

/* Bit-banged DAC8562: PB12=SCLK PB13=DIN PB14=SYNC (init only, never used later) */

void dac8562_write(undefined4 param_1,undefined4 param_2)

{
  undefined4 uVar1;
  
  uVar1 = 0x40010c00/*GPIOB*/;
  GPIO_ResetBits(0x40010c00/*GPIOB*/,0x4000);
  dac8562_shift8(param_1);
  dac8562_shift16(param_2);
  GPIO_SetBits(uVar1,0x4000);
  return;
}


// ===== DebugMon_Handler @ 080025a8 (size 2)

void DebugMon_Handler(void)

{
  return;
}


// ===== FLASH_ClearFlag @ 080025ac (size 6)

void FLASH_ClearFlag(undefined4 param_1)

{
  *(undefined4 *)(0x40022000/*FLASH*/ + 0xc) = param_1;
  return;
}


// ===== FLASH_ErasePage @ 080025b8 (size 56)

void FLASH_ErasePage(undefined4 param_1)

{
  int iVar1;
  int iVar2;
  
  iVar2 = FLASH_WaitForLastOperation(0xb0000);
  iVar1 = 0x40022000/*FLASH*/;
  if (iVar2 == 4) {
    *(uint *)(0x40022000/*FLASH*/ + 0x10) = *(uint *)(0x40022000/*FLASH*/ + 0x10) | 2;
    *(undefined4 *)(iVar1 + 0x14) = param_1;
    *(uint *)(iVar1 + 0x10) = *(uint *)(iVar1 + 0x10) | 0x40;
    FLASH_WaitForLastOperation(0xb0000);
    *(uint *)(iVar1 + 0x10) = *(uint *)(iVar1 + 0x10) & 0x1ffd;
  }
  return;
}


// ===== FLASH_GetBank1Status @ 080025f4 (size 34)

undefined4 FLASH_GetBank1Status(void)

{
  undefined4 uVar1;
  
  uVar1 = 4;
  if ((*(uint *)(0x40022000/*FLASH*/ + 0xc) & 1) != 0) {
    return 1;
  }
  if (*(int *)(0x40022000/*FLASH*/ + 0xc) << 0x1d < 0) {
    uVar1 = 2;
  }
  else if (*(int *)(0x40022000/*FLASH*/ + 0xc) << 0x1b < 0) {
    return 3;
  }
  return uVar1;
}


// ===== FLASH_PrefetchBufferCmd @ 0800262c (size 18)

void FLASH_PrefetchBufferCmd(uint param_1)

{
  uint *puVar1;
  
  puVar1 = 0x40022000/*FLASH*/;
  *0x40022000/*FLASH*/ = *0x40022000/*FLASH*/ & 0xffffffef;
  *puVar1 = *puVar1 | param_1;
  return;
}


// ===== FLASH_ProgramHalfWord @ 08002644 (size 48)

void FLASH_ProgramHalfWord(undefined2 *param_1,undefined2 param_2)

{
  int iVar1;
  int iVar2;
  
  iVar2 = FLASH_WaitForLastOperation(0x2000);
  iVar1 = 0x40022000/*FLASH*/;
  if (iVar2 == 4) {
    *(uint *)(0x40022000/*FLASH*/ + 0x10) = *(uint *)(0x40022000/*FLASH*/ + 0x10) | 1;
    *param_1 = param_2;
    FLASH_WaitForLastOperation(0x2000);
    *(uint *)(iVar1 + 0x10) = *(uint *)(iVar1 + 0x10) & 0x1ffe;
  }
  return;
}


// ===== FLASH_SetLatency @ 08002678 (size 14)

void FLASH_SetLatency(uint param_1)

{
  *0x40022000/*FLASH*/ = *0x40022000/*FLASH*/ & 0x38 | param_1;
  return;
}


// ===== FLASH_Unlock @ 0800268c (size 12)

void FLASH_Unlock(void)

{
  int iVar1;
  
  iVar1 = 0x40022000/*FLASH*/;
  *(undefined4 *)(0x40022000/*FLASH*/ + 4) = 0x45670123;
  *(undefined4 *)(iVar1 + 4) = 0xcdef89ab;
  return;
}


// ===== FLASH_WaitForLastOperation @ 080026a4 (size 34)

int FLASH_WaitForLastOperation(void)

{
  int iVar1;
  int extraout_r3;
  int extraout_r3_00;
  int iVar2;
  
  iVar1 = FLASH_GetBank1Status();
  iVar2 = extraout_r3;
  while (iVar1 == 1) {
    if (iVar2 == 0) goto LAB_080026be;
    iVar1 = FLASH_GetBank1Status();
    iVar2 = extraout_r3_00 + -1;
  }
  if (iVar2 == 0) {
LAB_080026be:
    iVar1 = 5;
  }
  return iVar1;
}


// ===== GPIO_Init @ 080026c8 (size 162)

void GPIO_Init(uint *param_1,ushort *param_2)

{
  uint uVar1;
  uint uVar2;
  uint uVar3;
  uint uVar4;
  
  uVar3 = 0;
  uVar1 = *(byte *)((int)param_2 + 3) & 0xf;
  if ((int)((uint)*(byte *)((int)param_2 + 3) << 0x1b) < 0) {
    uVar1 = uVar1 | (byte)param_2[1];
  }
  if ((char)*param_2 != '\0') {
    uVar2 = *param_1;
    do {
      uVar4 = 1 << (uVar3 & 0xff);
      if ((*param_2 & uVar4) == uVar4) {
        uVar2 = uVar1 << (uVar3 << 2 & 0xff) | uVar2 & ~(0xf << (uVar3 << 2 & 0xff));
        if (*(char *)((int)param_2 + 3) == '(') {
          param_1[5] = uVar4;
        }
        else if (*(char *)((int)param_2 + 3) == 'H') {
          param_1[4] = uVar4;
        }
      }
      uVar3 = uVar3 + 1;
    } while (uVar3 < 8);
    *param_1 = uVar2;
  }
  if (0xff < *param_2) {
    uVar3 = param_1[1];
    uVar2 = 0;
    do {
      uVar4 = 1 << (uVar2 + 8 & 0xff);
      if ((*param_2 & uVar4) == uVar4) {
        uVar3 = uVar3 & ~(0xf << (uVar2 << 2 & 0xff)) | uVar1 << (uVar2 << 2 & 0xff);
        if (*(char *)((int)param_2 + 3) == '(') {
          param_1[5] = uVar4;
        }
        if (*(char *)((int)param_2 + 3) == 'H') {
          param_1[4] = uVar4;
        }
      }
      uVar2 = uVar2 + 1;
    } while (uVar2 < 8);
    param_1[1] = uVar3;
  }
  return;
}


// ===== GPIO_PinRemapConfig @ 0800276c (size 82)

void GPIO_PinRemapConfig(uint param_1,int param_2)

{
  uint uVar1;
  int iVar2;
  uint uVar3;
  uint uVar4;
  
  iVar2 = 0x40010000/*AFIO*/;
  if ((int)param_1 < 0) {
    uVar3 = *(uint *)(0x40010000/*AFIO*/ + 0x1c);
  }
  else {
    uVar3 = *(uint *)(0x40010000/*AFIO*/ + 4);
  }
  uVar4 = (param_1 & 0xffff) << ((param_1 >> 0x15 & 0xf) << 4);
  if ((param_1 & 0x3fffff) >> 0x14 == 3) {
    uVar3 = uVar3 & 0xf0ffffff;
    *(uint *)(0x40010000/*AFIO*/ + 4) = *(uint *)(0x40010000/*AFIO*/ + 4) & 0xf0ffffff;
  }
  else {
    uVar1 = uVar4;
    if ((int)(param_1 << 0xb) < 0) {
      uVar1 = 3 << ((param_1 & 0xfffff) >> 0x10);
    }
    uVar3 = uVar3 & ~uVar1 | 0xf000000;
  }
  if (param_2 != 0) {
    uVar3 = uVar3 | uVar4;
  }
  if (-1 < (int)param_1) {
    *(uint *)(iVar2 + 4) = uVar3;
    return;
  }
  *(uint *)(iVar2 + 0x1c) = uVar3;
  return;
}


// ===== GPIO_ReadInputDataBit @ 080027c4 (size 14)

bool GPIO_ReadInputDataBit(int param_1,uint param_2)

{
  return (*(uint *)(param_1 + 8) & param_2) != 0;
}


// ===== GPIO_ResetBits @ 080027d2 (size 4)

void GPIO_ResetBits(int param_1,undefined4 param_2)

{
  *(undefined4 *)(param_1 + 0x14) = param_2;
  return;
}


// ===== GPIO_SetBits @ 080027d6 (size 4)

void GPIO_SetBits(int param_1,undefined4 param_2)

{
  *(undefined4 *)(param_1 + 0x10) = param_2;
  return;
}


// ===== adc_read_avg130 @ 080027dc (size 130)

/* Internal ADC: 130 samples minus min/max, /128 */

uint adc_read_avg130(undefined4 param_1)

{
  undefined4 uVar1;
  int iVar2;
  uint uVar3;
  uint uVar4;
  uint uVar5;
  int iVar6;
  uint uVar7;
  ushort local_118 [132];
  
  ADC_RegularChannelConfig(0x40012400/*ADC1*/,param_1,1,5);
  iVar6 = 0;
  memclr(local_118,0x104);
  uVar1 = 0x40012400/*ADC1*/;
  uVar7 = 0;
  do {
    ADC_SoftwareStartConvCmd(uVar1,1);
    do {
      iVar2 = ADC_GetFlagStatus(uVar1,2);
    } while (iVar2 == 0);
    iVar2 = ADC_GetConversionValue(uVar1);
    local_118[uVar7] = (ushort)iVar2;
    uVar7 = uVar7 + 1 & 0xffff;
    iVar6 = iVar6 + iVar2;
  } while (uVar7 < 0x82);
  uVar7 = (uint)local_118[0];
  uVar5 = 1;
  uVar3 = uVar7;
  do {
    uVar4 = (uint)local_118[uVar5];
    if (uVar3 < uVar4) {
      uVar3 = uVar4;
    }
    if (uVar4 < uVar7) {
      uVar7 = uVar4;
    }
    uVar5 = uVar5 + 1 & 0xffff;
  } while (uVar5 < 0x82);
  return ((iVar6 - uVar3) - uVar7 & 0xfffff) >> 4;
}


// ===== keys_scan @ 08002868 (size 532)

/* Buttons PA14,PC4..PC7,PA13 + encoder. Returns key code (0xe=none) */

undefined4 keys_scan(void)

{
  char cVar1;
  int iVar2;
  undefined4 uVar3;
  int iVar4;
  undefined1 uVar5;
  undefined4 uVar6;
  
  uVar6 = 0xe;
  encoder_scan();
  iVar2 = 0x200002c4;
  if (*(char *)(0x200002c4 + 4) == '\x01') {
    *(undefined1 *)(0x200002c4 + 4) = 0;
    return 2;
  }
  if (*(char *)(0x200002c4 + 4) == -1) {
    *(undefined1 *)(0x200002c4 + 4) = 0;
    return 3;
  }
  if (*(byte *)(0x200002c4 + 5) < 0x32) {
    *(byte *)(0x200002c4 + 5) = *(byte *)(0x200002c4 + 5) + 1;
  }
  else {
    *(undefined1 *)(0x200002c4 + 5) = 0;
    uVar3 = 0x40010800/*GPIOA*/;
    *(undefined1 *)(iVar2 + 8) = *(undefined1 *)(iVar2 + 7);
    uVar5 = GPIO_ReadInputDataBit(uVar3,0x4000);
    *(undefined1 *)(iVar2 + 7) = uVar5;
    iVar4 = 0x40011000/*GPIOC*/;
    *(undefined1 *)(iVar2 + 0xd) = *(undefined1 *)(iVar2 + 0xc);
    uVar5 = GPIO_ReadInputDataBit(iVar4,0x10);
    *(undefined1 *)(iVar2 + 0xc) = uVar5;
    *(undefined1 *)(iVar2 + 0x12) = *(undefined1 *)(iVar2 + 0x11);
    uVar5 = GPIO_ReadInputDataBit(iVar4,0x20);
    *(undefined1 *)(iVar2 + 0x11) = uVar5;
    *(undefined1 *)(iVar2 + 0x17) = *(undefined1 *)(iVar2 + 0x16);
    uVar5 = GPIO_ReadInputDataBit(iVar4,0x40);
    *(undefined1 *)(iVar2 + 0x16) = uVar5;
    *(undefined1 *)(iVar2 + 0x1c) = *(undefined1 *)(iVar2 + 0x1b);
    uVar5 = GPIO_ReadInputDataBit(iVar4,0x80);
    *(undefined1 *)(iVar2 + 0x1b) = uVar5;
    *(undefined1 *)(iVar2 + 0x21) = *(undefined1 *)(iVar2 + 0x20);
    uVar5 = GPIO_ReadInputDataBit(uVar3,iVar4 >> 0x11);
    *(undefined1 *)(iVar2 + 0x20) = uVar5;
    if (*(char *)(iVar2 + 10) == '\0') {
      *(undefined1 *)(iVar2 + 9) = 0;
    }
    else {
      *(char *)(iVar2 + 9) = *(char *)(iVar2 + 9) + '\x01';
    }
    if (*(char *)(iVar2 + 0xf) == '\0') {
      *(undefined1 *)(iVar2 + 0xe) = 0;
    }
    else {
      *(char *)(iVar2 + 0xe) = *(char *)(iVar2 + 0xe) + '\x01';
    }
    if (*(char *)(iVar2 + 0x14) == '\0') {
      *(undefined1 *)(iVar2 + 0x13) = 0;
    }
    else {
      *(char *)(iVar2 + 0x13) = *(char *)(iVar2 + 0x13) + '\x01';
    }
    if (*(char *)(iVar2 + 0x19) == '\0') {
      *(undefined1 *)(iVar2 + 0x18) = 0;
    }
    else {
      *(char *)(iVar2 + 0x18) = *(char *)(iVar2 + 0x18) + '\x01';
    }
    if (*(char *)(iVar2 + 0x1e) == '\0') {
      *(undefined1 *)(iVar2 + 0x1d) = 0;
    }
    else {
      *(char *)(iVar2 + 0x1d) = *(char *)(iVar2 + 0x1d) + '\x01';
    }
    if (*(char *)(iVar2 + 0x23) == '\0') {
      *(undefined1 *)(iVar2 + 0x22) = 0;
    }
    else {
      *(char *)(iVar2 + 0x22) = *(char *)(iVar2 + 0x22) + '\x01';
    }
  }
  cVar1 = *(char *)(iVar2 + 7);
  if ((cVar1 == '\0') && (*(char *)(iVar2 + 8) != '\0')) {
    *(undefined1 *)(iVar2 + 6) = 1;
LAB_0800296c:
    if (0xb < *(byte *)(iVar2 + 9)) {
      uVar6 = 1;
      *(undefined1 *)(iVar2 + 6) = 0;
      goto LAB_08002976;
    }
    if (cVar1 == '\0') goto LAB_08002988;
    uVar6 = 0;
    *(undefined1 *)(iVar2 + 6) = 0;
LAB_08002980:
    *(undefined1 *)(iVar2 + 10) = 0;
  }
  else {
    if (*(char *)(iVar2 + 6) == '\x01') goto LAB_0800296c;
LAB_08002976:
    if (cVar1 != '\0') goto LAB_08002980;
LAB_08002988:
    *(undefined1 *)(iVar2 + 10) = 1;
  }
  cVar1 = *(char *)(iVar2 + 0xc);
  if ((cVar1 == '\0') && (*(char *)(iVar2 + 0xd) != '\0')) {
    *(undefined1 *)(iVar2 + 0xb) = 1;
LAB_0800299a:
    if (0xb < *(byte *)(iVar2 + 0xe)) {
      uVar6 = 5;
      *(undefined1 *)(iVar2 + 0xb) = 0;
      goto LAB_080029a4;
    }
    if (cVar1 != '\0') {
      uVar6 = 4;
      *(undefined1 *)(iVar2 + 0xb) = 0;
      goto LAB_080029ae;
    }
LAB_080029b6:
    *(undefined1 *)(iVar2 + 0xf) = 1;
  }
  else {
    if (*(char *)(iVar2 + 0xb) == '\x01') goto LAB_0800299a;
LAB_080029a4:
    if (cVar1 == '\0') goto LAB_080029b6;
LAB_080029ae:
    *(undefined1 *)(iVar2 + 0xf) = 0;
  }
  cVar1 = *(char *)(iVar2 + 0x11);
  if ((cVar1 == '\0') && (*(char *)(iVar2 + 0x12) != '\0')) {
    *(undefined1 *)(iVar2 + 0x10) = 1;
LAB_080029c8:
    if (0xb < *(byte *)(iVar2 + 0x13)) {
      uVar6 = 7;
      *(undefined1 *)(iVar2 + 0x10) = 0;
      goto LAB_080029d2;
    }
    if (cVar1 != '\0') {
      uVar6 = 6;
      *(undefined1 *)(iVar2 + 0x10) = 0;
      goto LAB_080029dc;
    }
LAB_080029e4:
    *(undefined1 *)(iVar2 + 0x14) = 1;
  }
  else {
    if (*(char *)(iVar2 + 0x10) == '\x01') goto LAB_080029c8;
LAB_080029d2:
    if (cVar1 == '\0') goto LAB_080029e4;
LAB_080029dc:
    *(undefined1 *)(iVar2 + 0x14) = 0;
  }
  cVar1 = *(char *)(iVar2 + 0x16);
  if ((cVar1 == '\0') && (*(char *)(iVar2 + 0x17) != '\0')) {
    *(undefined1 *)(iVar2 + 0x15) = 1;
LAB_080029f6:
    if (0xb < *(byte *)(iVar2 + 0x18)) {
      uVar6 = 9;
      *(undefined1 *)(iVar2 + 0x15) = 0;
      goto LAB_08002a00;
    }
    if (cVar1 != '\0') {
      uVar6 = 8;
      *(undefined1 *)(iVar2 + 0x15) = 0;
      goto LAB_08002a0a;
    }
LAB_08002a12:
    *(undefined1 *)(iVar2 + 0x19) = 1;
  }
  else {
    if (*(char *)(iVar2 + 0x15) == '\x01') goto LAB_080029f6;
LAB_08002a00:
    if (cVar1 == '\0') goto LAB_08002a12;
LAB_08002a0a:
    *(undefined1 *)(iVar2 + 0x19) = 0;
  }
  cVar1 = *(char *)(iVar2 + 0x1b);
  if ((cVar1 == '\0') && (*(char *)(iVar2 + 0x1c) != '\0')) {
    *(undefined1 *)(iVar2 + 0x1a) = 1;
LAB_08002a24:
    if (0xb < *(byte *)(iVar2 + 0x1d)) {
      uVar6 = 0xb;
      *(undefined1 *)(iVar2 + 0x1a) = 0;
      goto LAB_08002a2e;
    }
    if (cVar1 != '\0') {
      uVar6 = 10;
      *(undefined1 *)(iVar2 + 0x1a) = 0;
      goto LAB_08002a38;
    }
LAB_08002a44:
    *(undefined1 *)(iVar2 + 0x1e) = 1;
  }
  else {
    if (*(char *)(iVar2 + 0x1a) == '\x01') goto LAB_08002a24;
LAB_08002a2e:
    if (cVar1 == '\0') goto LAB_08002a44;
LAB_08002a38:
    *(undefined1 *)(iVar2 + 0x1e) = 0;
  }
  cVar1 = *(char *)(iVar2 + 0x20);
  if ((cVar1 == '\0') && (*(char *)(iVar2 + 0x21) != '\0')) {
    *(undefined1 *)(iVar2 + 0x1f) = 1;
LAB_08002a58:
    if (*(byte *)(iVar2 + 0x22) < 0xc) {
      if (cVar1 != '\0') {
        uVar6 = 0xc;
        *(undefined1 *)(iVar2 + 0x1f) = 0;
        goto LAB_08002a6e;
      }
      goto LAB_08002a76;
    }
    uVar6 = 0xd;
    *(undefined1 *)(iVar2 + 0x1f) = 0;
  }
  else if (*(char *)(iVar2 + 0x1f) == '\x01') goto LAB_08002a58;
  if (cVar1 != '\0') {
LAB_08002a6e:
    *(undefined1 *)(iVar2 + 0x23) = 0;
    return uVar6;
  }
LAB_08002a76:
  *(undefined1 *)(iVar2 + 0x23) = 1;
  return uVar6;
}


// ===== HardFault_Handler @ 08002a88 (size 2)

void HardFault_Handler(void)

{
  do {
                    /* WARNING: Do nothing block with infinite loop */
  } while( true );
}


// ===== iwdg_init @ 08002a9c (size 48)

void iwdg_init(undefined4 param_1,undefined4 param_2)

{
  IWDG_WriteAccessCmd(0x5555);
  IWDG_SetPrescaler(param_1);
  IWDG_SetReload(param_2);
  IWDG_ReloadCounter();
  *0x40003000/*IWDG*/ = 0xcccc;
  return;
}


// ===== IWDG_ReloadCounter @ 08002ac4 (size 10)

void IWDG_ReloadCounter(void)

{
  *0x40003000/*IWDG*/ = 0xaaaa;
  return;
}


// ===== IWDG_SetPrescaler @ 08002ad4 (size 6)

void IWDG_SetPrescaler(undefined4 param_1)

{
  *(undefined4 *)(0x40003000/*IWDG*/ + 4) = param_1;
  return;
}


// ===== IWDG_SetReload @ 08002ae0 (size 6)

void IWDG_SetReload(undefined4 param_1)

{
  *(undefined4 *)(0x40003000/*IWDG*/ + 8) = param_1;
  return;
}


// ===== IWDG_WriteAccessCmd @ 08002aec (size 6)

void IWDG_WriteAccessCmd(undefined4 param_1)

{
  *0x40003000/*IWDG*/ = param_1;
  return;
}


// ===== ads1115_build_cfg @ 08002af8 (size 68)

/* ADS1115 config words 0x95E3/0xA5E3/0xB5E3/0xB7E3 */

void ads1115_build_cfg(int param_1,undefined2 param_2)

{
  undefined1 *puVar1;
  
  puVar1 = 0x200001cf;
  if (param_1 == 0) {
    param_2 = 0x95e3;
  }
  else if (param_1 == 1) {
    param_2 = 0xa5e3;
  }
  else if (param_1 == 2) {
    param_2 = 0xb5e3;
  }
  else if (param_1 == 3) {
    param_2 = 0xb7e3;
  }
  *0x200001cf = 0x90;
  puVar1[1] = 1;
  puVar1[2] = (char)((ushort)param_2 >> 8);
  puVar1[3] = (char)param_2;
  puVar1[-3] = 0x90;
  puVar1[-2] = 0;
  puVar1[-1] = 0x91;
  return;
}


// ===== adc_init @ 08002b40 (size 146)

/* PA0,PA6,PA7,PB0,PB1 analog; ADC1 calibrate */

void adc_init(void)

{
  undefined4 uVar1;
  int iVar2;
  undefined4 local_28;
  undefined1 local_24;
  undefined1 local_23;
  undefined4 local_20;
  undefined4 uStack_1c;
  undefined1 local_18;
  undefined2 local_14;
  undefined1 local_11;
  
  local_14 = 0xc1;
  local_11 = 0;
  GPIO_Init(0x40010800/*GPIOA*/,&local_14);
  local_14 = 3;
  local_11 = 0;
  GPIO_Init(0x40010c00/*GPIOB*/,&local_14);
  uVar1 = 0x40012400/*ADC1*/;
  ADC_DeInit(0x40012400/*ADC1*/);
  local_20 = 0xe0000;
  uStack_1c = 0;
  local_24 = 0;
  local_18 = 1;
  local_28 = 0;
  local_23 = 0;
  ADC_Init(uVar1,&local_28);
  ADC_Cmd(uVar1,1);
  ADC_ResetCalibration(uVar1);
  do {
    iVar2 = ADC_GetResetCalibrationStatus(uVar1);
  } while (iVar2 != 0);
  ADC_StartCalibration(uVar1);
  do {
    iVar2 = ADC_GetCalibrationStatus(uVar1);
  } while (iVar2 != 0);
  ADC_SoftwareStartConvCmd(uVar1,1);
  ADC_TempSensorVrefintCmd(1);
  ads_i2c_gpio_init();
  return;
}


// ===== dac_init @ 08002be0 (size 116)

/* Internal DAC ch1/ch2 + DAC8562 init */

void dac_init(void)

{
  undefined4 uVar1;
  undefined4 local_20;
  undefined4 local_1c;
  undefined4 local_18;
  undefined4 uStack_14;
  undefined2 local_10;
  undefined1 local_e;
  undefined1 local_d;
  
  uVar1 = 0x40010800/*GPIOA*/;
  local_10 = 0x30;
  local_d = 0;
  local_e = 3;
  GPIO_Init(0x40010800/*GPIOA*/,&local_10);
  GPIO_SetBits(uVar1,0x30);
  local_18 = 0;
  uStack_14 = 2;
  local_20 = 0;
  local_1c = 0;
  DAC_Init(0,&local_20);
  DAC_Init(0x10,&local_20);
  DAC_Cmd(0,1);
  DAC_Cmd(0x10,1);
  DAC_SetChannel1Data(0);
  DAC_SetChannel2Data(0);
  range_gpio_init();
  dac8562_init();
  delay_ms(100);
  return;
}


// ===== dac8562_init @ 08002c58 (size 50)

void dac8562_init(void)

{
  dac8562_gpio_init();
  dac8562_write(0x28,1);
  dac8562_write(0x20,3);
  dac8562_write(0x38,1);
  dac8562_write(2,0);
  dac8562_write(0x30,3);
  return;
}


// ===== dac8562_gpio_init @ 08002c8c (size 32)

void dac8562_gpio_init(void)

{
  undefined4 local_8;
  
  local_8 = 0x10017000;
  GPIO_Init(0x40010c00/*GPIOB*/,&local_8);
  return;
}


// ===== tim2_pwm_init @ 08002cb0 (size 146)

/* TIM2 full remap, CH2 PWM on PB3 */

void tim2_pwm_init(undefined2 param_1,undefined2 param_2)

{
  undefined2 local_30;
  undefined2 local_2e;
  undefined2 local_2a;
  undefined2 local_28;
  undefined2 local_20;
  undefined2 local_1e;
  undefined2 local_1c;
  undefined2 local_1a;
  undefined2 local_14;
  undefined1 local_12;
  undefined1 local_11;
  
  GPIO_PinRemapConfig(0x300200,1);
  GPIO_PinRemapConfig(0x180300,1);
  local_14 = 8;
  local_11 = 0x18;
  local_12 = 3;
  GPIO_Init(0x40010c00/*GPIOB*/,&local_14);
  local_1a = 0;
  local_1e = 0;
  local_20 = param_2;
  local_1c = param_1;
  TIM_TimeBaseInit(0x40000000,&local_20);
  local_30 = 0x70;
  local_2e = 1;
  local_28 = 2;
  local_2a = 0;
  TIM_OC2Init(0x40000000,&local_30);
  TIM_OC2PreloadConfig(0x40000000,8);
  TIM_ARRPreloadConfig_(0x40000000,1);
  TIM_CtrlPWMOutputs(0x40000000,1);
  TIM_Cmd(0x40000000,1);
  return;
}


// ===== tim7_init @ 08002d50 (size 54)

/* TIM7 10 kHz tick */

void tim7_init(void)

{
  undefined4 uVar1;
  undefined4 in_r3;
  undefined4 local_18;
  undefined4 local_14;
  undefined4 uStack_10;
  
  uVar1 = 0x40001400/*TIM7*/;
  local_14 = 99;
  local_18 = 0x17;
  uStack_10 = in_r3;
  TIM_TimeBaseInit(0x40001400/*TIM7*/,&local_18);
  TIM_ITConfig(uVar1,1);
  TIM_Cmd(uVar1,1);
  return;
}


// ===== usart1_init @ 08002d8c (size 118)

/* USART1 19200 PA9/PA10 (factory cal link) */

void usart1_init(void)

{
  undefined4 uVar1;
  undefined4 local_20;
  undefined2 local_1c;
  undefined2 local_1a;
  undefined2 local_18;
  undefined2 local_16;
  undefined2 local_14;
  short local_10;
  undefined1 local_e;
  undefined1 local_d;
  
  uVar1 = 0x40010800/*GPIOA*/;
  local_10 = 0x200;
  local_e = 1;
  local_d = 0x18;
  GPIO_Init(0x40010800/*GPIOA*/,&local_10);
  local_10 = (short)((int)uVar1 >> 0x14);
  local_d = 4;
  GPIO_Init(uVar1,&local_10);
  uVar1 = 0x40013800/*USART1*/;
  local_20 = 0x4b00;
  local_1c = 0;
  local_1a = 0;
  local_18 = 0;
  local_14 = 0;
  local_16 = 0xc;
  USART_Init(0x40013800/*USART1*/,&local_20);
  USART_ITConfig(uVar1,0x525,1);
  USART_Cmd(uVar1,1);
  return;
}


// ===== usart2_init @ 08002e0c (size 156)

/* USART2 Modbus RTU PA2/PA3, RS485 DE PC13, baud from table */

void usart2_init(void)

{
  undefined4 uVar1;
  undefined4 auStack_60 [16];
  undefined2 local_20;
  undefined1 local_1e;
  undefined1 local_1d;
  undefined4 local_1c;
  undefined2 local_18;
  undefined2 local_16;
  undefined2 local_14;
  undefined2 local_12;
  undefined2 local_10;
  
  memcpy(auStack_60,0x80083cc,0x40);
  local_20 = 0x2000;
  local_1e = 3;
  local_1d = 0x10;
  GPIO_Init(0x40011000/*GPIOC*/,&local_20);
  uVar1 = 0x40010800/*GPIOA*/;
  local_20 = 4;
  local_1e = 1;
  local_1d = 0x18;
  GPIO_Init(0x40010800/*GPIOA*/,&local_20);
  local_20 = 8;
  local_1d = 4;
  GPIO_Init(uVar1,&local_20);
  uVar1 = 0x40004400/*USART2*/;
  local_1c = auStack_60[*(byte *)(0x20000208 + 0xe)];
  local_18 = 0;
  local_16 = 0;
  local_14 = 0;
  local_10 = 0;
  local_12 = 0xc;
  USART_Init(0x40004400/*USART2*/,&local_1c);
  USART_ITConfig(uVar1,0x525,1);
  USART_Cmd(uVar1,1);
  return;
}


// ===== keys_gpio_init @ 08002ebc (size 62)

/* SWJ disabled; PA13/PA14, PB5/PB6 pull-up inputs */

void keys_gpio_init(void)

{
  undefined4 in_r3;
  undefined4 local_10;
  
  local_10 = in_r3;
  GPIO_PinRemapConfig(0x300400,1);
  local_10 = 0x48026000;
  GPIO_Init(0x40010800/*GPIOA*/,&local_10);
  local_10 = 0x48020060;
  GPIO_Init(0x40010c00/*GPIOB*/,&local_10);
  return;
}


// ===== range_gpio_init @ 08002f08 (size 30)

/* PB2 current-range output */

void range_gpio_init(void)

{
  undefined4 local_8;
  
  local_8 = 0x10030004;
  GPIO_Init(0x40010c00/*GPIOB*/,&local_8);
  return;
}


// ===== outputs_gpio_init @ 08002f2c (size 130)

/* PB4 fan, PB10 buzzer, PB11, PA15 load-inhibit */

void outputs_gpio_init(void)

{
  undefined4 uVar1;
  undefined4 local_18;
  
  uVar1 = 0x40010c00/*GPIOB*/;
  local_18 = 0x10030010;
  GPIO_Init(0x40010c00/*GPIOB*/,&local_18);
  GPIO_ResetBits(uVar1,0x10);
  local_18 = 0x10030400;
  GPIO_Init(uVar1,&local_18);
  GPIO_ResetBits(uVar1,0x400);
  local_18 = 0x10030800;
  GPIO_Init(uVar1,&local_18);
  GPIO_ResetBits(uVar1,0x800);
  uVar1 = 0x40010800/*GPIOA*/;
  local_18 = 0x10038000;
  GPIO_Init(0x40010800/*GPIOA*/,&local_18);
  GPIO_SetBits(uVar1,0x8000);
  return;
}


// ===== ui_draw_cursor_screen @ 08002fb8 (size 158)

void ui_draw_cursor_screen(undefined4 param_1,undefined4 param_2,int param_3,int param_4)

{
  undefined1 *puVar1;
  int iVar2;
  undefined1 *puVar3;
  char *pcVar4;
  undefined1 uVar5;
  uint uVar6;
  
  iVar2 = 0x20000654;
  puVar1 = 0x20000178;
  *0x20000178 = (char)param_4;
  puVar3 = 0x200002c2;
  uVar6 = (uint)*(byte *)(param_3 + param_4);
  puVar1[1] = *(byte *)(param_3 + param_4);
  pcVar4 = 0x200002c1;
  if (uVar6 < 5) {
    lcd_clear_line(0);
    ui_draw_setpoints(0,*(undefined4 *)(iVar2 + 0x10),*(undefined4 *)(iVar2 + 0x14),
                      puVar1[1] + '\x01');
    uVar5 = 0;
  }
  else {
    if (4 < uVar6 - 5) {
      if (uVar6 != 10) {
        return;
      }
      if (*0x200002c1 == '\x02') {
        measure_vin();
      }
      lcd_clear_line(0);
      oled_draw_char16(0x18,4,(*(uint *)(iVar2 + 0x14) % 100) / 10);
      *puVar3 = 2;
      ui_draw_info_page(2,*pcVar4,*(undefined4 *)(iVar2 + 0x30),*(undefined4 *)(iVar2 + 0x34),
                        *(undefined4 *)(iVar2 + 0x24),*(undefined4 *)(iVar2 + 0x38),
                        *(undefined4 *)(iVar2 + 0x3c),*(undefined4 *)(iVar2 + 0x28),
                        *(undefined4 *)(iVar2 + 0x2c));
      return;
    }
    ui_draw_setpoints(1,*(undefined4 *)(iVar2 + 0x10),*(undefined4 *)(iVar2 + 0x14),uVar6 - 4 & 0xff
                     );
    uVar5 = 1;
  }
  *puVar3 = uVar5;
  return;
}


// ===== MemManage_Handler @ 08003068 (size 2)

void MemManage_Handler(void)

{
  do {
                    /* WARNING: Do nothing block with infinite loop */
  } while( true );
}


// ===== modbus_process @ 0800306c (size 1024)

/* Modbus RTU slave: func 3,4,6,16 */

char * modbus_process(void)

{
  ushort uVar1;
  int *piVar2;
  int *piVar3;
  int *piVar4;
  int *piVar5;
  char *pcVar6;
  ushort *puVar7;
  undefined2 uVar8;
  ushort uVar9;
  char *pcVar10;
  byte bVar11;
  byte bVar12;
  byte *pbVar13;
  byte *pbVar14;
  uint uVar15;
  char *pcVar16;
  uint uVar17;
  uint uVar18;
  int *piVar19;
  int iVar20;
  undefined4 unaff_r4;
  int iVar21;
  undefined4 unaff_r5;
  char cVar22;
  undefined4 unaff_r6;
  undefined4 unaff_lr;
  
  puVar7 = 0x20000096;
  piVar19 = 0x20000324;
  pcVar10 = (char *)*0x20000324;
  if ((*pcVar10 != (char)0x20000324[3]) && (*pcVar10 != '\0')) {
    return pcVar10;
  }
  uVar17 = (uint)*(byte *)(0x20000324 + 2);
  *(char *)0x20000096 = pcVar10[uVar17 - 1];
  *(char *)((int)puVar7 + 1) = pcVar10[uVar17 - 2];
  pcVar10 = (char *)modbus_crc16(pcVar10,uVar17 - 2 & 0xff);
  if (pcVar10 != (char *)(uint)*puVar7) {
    return pcVar10;
  }
  *(byte *)(puVar7 + -0x1d) = 0;
  *(byte *)(puVar7 + -0x1e) = 0;
  pcVar10 = (char *)*piVar19;
  if (*pcVar10 == '\0') {
    *(byte *)((int)puVar7 + -0x39) = 1;
  }
  else {
    *(byte *)((int)puVar7 + -0x39) = 0;
  }
  pcVar6 = 0x2000005a;
  piVar5 = 0x20000324;
  pbVar14 = 0x2000005a;
  piVar4 = 0x20000324;
  pbVar13 = 0x2000005a;
  piVar3 = 0x20000324;
  pcVar16 = 0x2000005a;
  piVar2 = 0x20000324;
  cVar22 = pcVar10[1];
  if (cVar22 == '\x03') {
    if ((char *)(uint)*(byte *)((int)puVar7 + -0x39) != (char *)0x0) {
      return (char *)(uint)*(byte *)((int)puVar7 + -0x39);
    }
    iVar20 = *0x20000324;
    uVar9 = *(ushort *)(iVar20 + 2);
    uVar15 = ((uVar9 & 0x7f) << 8 | (uint)(uVar9 >> 8)) * 2;
    *(short *)(0x2000005a + 4) = (short)uVar15;
    uVar9 = *(ushort *)(iVar20 + 4);
    iVar20 = ((uVar9 & 0x7f) << 8 | (uint)(uVar9 >> 8)) * 2;
    piVar19 = (int *)(iVar20 + 5U & 0xff);
    *(short *)(pcVar16 + 6) = (short)iVar20;
    *(char *)((int)piVar2 + 9) = (char)(iVar20 + 5U);
    uVar17 = piVar2[1];
    if (iVar20 - 2U < 0x19) {
      if ((uVar15 < 0x1b) && (iVar20 + uVar15 < 0x1b)) {
        if ((int *)(uint)*(byte *)((int)piVar2 + 10) < piVar19) {
          *pcVar16 = '\x04';
        }
        else if (*pcVar16 == '\0') {
          *(undefined1 *)(uVar17 + 1) = 3;
          piVar19 = 0x20000324;
          uVar9 = *(ushort *)(pcVar16 + 6);
          *(char *)(piVar2[1] + 2) = (char)uVar9;
          uVar1 = *(ushort *)(pcVar16 + 4);
          pcVar16[1] = (char)uVar1;
          piVar19 = piVar19 + -0xd;
          while (uVar17 = (uint)(byte)pcVar16[1], uVar17 < (uint)uVar1 + (uint)uVar9) {
            *(undefined1 *)(piVar2[1] + (byte)pcVar16[2] + 3) =
                 *(undefined1 *)((int)piVar19 + (0x19 - uVar17));
            pcVar16[2] = pcVar16[2] + '\x01';
            pcVar16[1] = pcVar16[1] + '\x01';
          }
          goto LAB_08000490;
        }
      }
      else {
        *pcVar16 = '\x02';
      }
    }
    else {
      *pcVar16 = '\x03';
    }
    *(undefined1 *)((int)piVar2 + 9) = 5;
    *(undefined1 *)(uVar17 + 1) = 0x83;
    *(char *)(piVar2[1] + 2) = *pcVar16;
LAB_08000490:
    *(undefined1 *)piVar2[1] = *(undefined1 *)*piVar2;
    uVar8 = modbus_crc16(piVar2[1],*(char *)((int)piVar2 + 9) + -2,uVar17,piVar19,unaff_r4,unaff_r5,
                         unaff_r6);
    pcVar10 = 0x2000005a;
    pbVar13 = (byte *)(0x2000005a + 0x3c);
    *(undefined2 *)pbVar13 = uVar8;
    *(char *)(piVar2[1] + (uint)*(byte *)((int)piVar2 + 9) + -2) = pcVar10[0x3d];
    bVar11 = *pbVar13;
    *(byte *)(piVar2[1] + (uint)*(byte *)((int)piVar2 + 9) + -1) = bVar11;
    return (char *)(uint)bVar11;
  }
  if (cVar22 != '\x04') {
    if (cVar22 != '\x06') {
      if (cVar22 != '\x10') {
        *(undefined1 *)((int)piVar19 + 9) = 5;
        *(char *)piVar19[1] = *pcVar10;
        *(byte *)(piVar19[1] + 1) = *(byte *)(*piVar19 + 1) | 0x80;
        *(undefined1 *)(piVar19[1] + 2) = 1;
        uVar9 = modbus_crc16(piVar19[1],2);
        *puVar7 = uVar9;
        *(byte *)(piVar19[1] + 3) = *(byte *)((int)puVar7 + 1);
        uVar9 = *puVar7;
        *(byte *)(piVar19[1] + 4) = (byte)uVar9;
        return (char *)(uint)(byte)uVar9;
      }
      if ((char *)(uint)*(byte *)((int)puVar7 + -0x39) != (char *)0x1) {
        return (char *)(uint)*(byte *)((int)puVar7 + -0x39);
      }
      cVar22 = '\x03';
      iVar21 = *0x20000324;
      uVar18 = ((*(ushort *)(iVar21 + 2) & 0x7f) << 8 | (uint)(*(ushort *)(iVar21 + 2) >> 8)) * 2;
      *(short *)(0x2000005a + 4) = (short)uVar18;
      uVar17 = (*(ushort *)(iVar21 + 4) & 0x7f) << 8 | (uint)(*(ushort *)(iVar21 + 4) >> 8);
      uVar15 = uVar17 * 2;
      *(short *)(pcVar6 + 6) = (short)uVar15;
      *(undefined1 *)((int)piVar5 + 9) = 8;
      piVar19 = 0x20000324;
      iVar20 = piVar5[1];
      if ((((uVar17 == 0) || (0x1a < uVar15)) || (*(byte *)(piVar5 + 2) - 9 != uVar15)) ||
         ((cVar22 = '\x02', 0x1a < uVar18 || (0x1a < uVar18 + uVar15)))) {
        *pcVar6 = cVar22;
      }
      else if (*(byte *)((int)piVar5 + 10) < 8) {
        *pcVar6 = '\x04';
      }
      else if (*pcVar6 == '\0') {
        pcVar6[1] = (char)uVar18;
        while( true ) {
          bVar11 = pcVar6[1];
          if (uVar18 + uVar15 <= (uint)bVar11) break;
          bVar12 = pcVar6[2];
          *(undefined1 *)((int)piVar19 + (-0x1b - (uint)bVar11)) =
               *(undefined1 *)(iVar21 + bVar12 + 7);
          pcVar6[2] = bVar12 + 1;
          pcVar6[1] = bVar11 + 1;
        }
        *(undefined1 *)(iVar20 + 1) = 0x10;
        *(undefined1 *)(piVar5[1] + 2) = *(undefined1 *)(*piVar5 + 2);
        *(undefined1 *)(piVar5[1] + 3) = *(undefined1 *)(*piVar5 + 3);
        uVar18 = piVar5[1];
        *(undefined1 *)(uVar18 + 4) = 0;
        *(char *)(piVar5[1] + 5) = (char)(*(ushort *)(pcVar6 + 6) >> 1);
        goto LAB_08000702;
      }
      *(undefined1 *)((int)piVar5 + 9) = 5;
      *(undefined1 *)(iVar20 + 1) = 0x90;
      *(char *)(piVar5[1] + 2) = *pcVar6;
LAB_08000702:
      *(undefined1 *)piVar5[1] = *(undefined1 *)*piVar5;
      uVar8 = modbus_crc16(piVar5[1],*(char *)((int)piVar5 + 9) + -2,uVar18,iVar20,unaff_r4,unaff_r5
                           ,unaff_r6);
      pcVar10 = 0x2000005a;
      pcVar16 = 0x2000005a + 0x3c;
      *(undefined2 *)pcVar16 = uVar8;
      *(char *)(piVar5[1] + (uint)*(byte *)((int)piVar5 + 9) + -2) = pcVar10[0x3d];
      *(char *)(piVar5[1] + (uint)*(byte *)((int)piVar5 + 9) + -1) = *pcVar16;
      return (char *)0x0;
    }
    if ((char *)(uint)*(byte *)((int)puVar7 + -0x39) != (char *)0x0) {
      return (char *)(uint)*(byte *)((int)puVar7 + -0x39);
    }
    iVar20 = *0x20000324;
    uVar17 = (*(ushort *)(iVar20 + 2) & 0x7f) << 8 | (uint)(*(ushort *)(iVar20 + 2) >> 8);
    uVar15 = uVar17 * 2;
    *(short *)(0x2000005a + 4) = (short)uVar15;
    *(ushort *)(pbVar14 + 6) = *(ushort *)(iVar20 + 4) << 8 | *(ushort *)(iVar20 + 4) >> 8;
    bVar11 = *(byte *)(piVar4 + 2);
    *(byte *)((int)piVar4 + 9) = bVar11;
    piVar19 = 0x20000324;
    if (uVar15 < 0x1b) {
      if (bVar11 <= *(byte *)((int)piVar4 + 10)) {
        bVar11 = *pbVar14;
        if (bVar11 == 0) {
          iVar21 = uVar17 * -2;
          *(undefined1 *)((int)0x20000324 + iVar21 + -0x1b) = *(undefined1 *)(iVar20 + 4);
          *(undefined1 *)((int)piVar19 + iVar21 + -0x1c) = *(undefined1 *)(iVar20 + 5);
          bVar11 = 0;
          while( true ) {
            pbVar14[1] = bVar11;
            uVar17 = (uint)bVar11;
            if (*(byte *)(piVar4 + 2) <= uVar17) break;
            *(undefined1 *)(piVar4[1] + uVar17) = *(undefined1 *)(*piVar4 + uVar17);
            bVar11 = pbVar14[1] + 1;
          }
          return (char *)0x0;
        }
        goto LAB_08000618;
      }
      bVar12 = 4;
    }
    else {
      bVar12 = 2;
    }
    *pbVar14 = bVar12;
LAB_08000618:
    iVar20 = piVar4[1];
    *(undefined1 *)(iVar20 + 1) = 0x86;
    *(byte *)(piVar4[1] + 2) = *pbVar14;
    *(undefined1 *)((int)piVar4 + 9) = 5;
    *(undefined1 *)piVar4[1] = *(undefined1 *)*piVar4;
    uVar8 = modbus_crc16(piVar4[1],*(char *)((int)piVar4 + 9) + -2,iVar20,bVar11,unaff_r4,unaff_r5,
                         unaff_lr);
    pbVar13 = 0x2000005a;
    pbVar14 = 0x2000005a + 0x3c;
    *(undefined2 *)pbVar14 = uVar8;
    *(byte *)(piVar4[1] + (uint)*(byte *)((int)piVar4 + 9) + -2) = pbVar13[0x3d];
    *(byte *)(piVar4[1] + (uint)*(byte *)((int)piVar4 + 9) + -1) = *pbVar14;
    return (char *)0x0;
  }
  if ((char *)(uint)*(byte *)((int)puVar7 + -0x39) != (char *)0x0) {
    return (char *)(uint)*(byte *)((int)puVar7 + -0x39);
  }
  iVar20 = *0x20000324;
  uVar17 = (uint)(*(ushort *)(iVar20 + 2) >> 8) * 2;
  *(short *)(0x2000005a + 4) = (short)uVar17;
  iVar20 = (uint)(*(ushort *)(iVar20 + 4) >> 8) * 2;
  *(short *)(pbVar13 + 6) = (short)iVar20;
  uVar15 = iVar20 + 5U & 0xff;
  *(char *)((int)piVar3 + 9) = (char)(iVar20 + 5U);
  iVar21 = piVar3[1];
  if (iVar20 - 2U < 0x19) {
    if ((uVar17 < 0x1b) && (iVar20 + uVar17 < 0x1b)) {
      uVar17 = (uint)*(byte *)((int)piVar3 + 10);
      bVar11 = 4;
      if (uVar17 < uVar15) goto LAB_08000550;
      uVar17 = (uint)*pbVar13;
      if (uVar17 == 0) {
        *(undefined1 *)(iVar21 + 1) = 4;
        piVar19 = 0x20000324;
        uVar9 = *(ushort *)(pbVar13 + 6);
        *(char *)(piVar3[1] + 2) = (char)uVar9;
        uVar17 = (uint)*(ushort *)(pbVar13 + 4);
        pbVar13[1] = (byte)*(ushort *)(pbVar13 + 4);
        uVar15 = (int)piVar19 - 0x1a;
        uVar18 = uVar9 + uVar17;
        while (uVar17 = uVar17 & 0xff, uVar17 < uVar18) {
          *(undefined1 *)(piVar3[1] + pbVar13[2] + 3) = *(undefined1 *)((uVar15 - uVar17) + 0x19);
          pbVar13[2] = pbVar13[2] + 1;
          uVar17 = pbVar13[1] + 1;
          pbVar13[1] = (byte)uVar17;
        }
        goto LAB_08000566;
      }
    }
    else {
      *pbVar13 = 2;
    }
  }
  else {
    bVar11 = 3;
LAB_08000550:
    *pbVar13 = bVar11;
  }
  *(undefined1 *)((int)piVar3 + 9) = 5;
  *(undefined1 *)(iVar21 + 1) = 0x84;
  *(byte *)(piVar3[1] + 2) = *pbVar13;
LAB_08000566:
  *(undefined1 *)piVar3[1] = *(undefined1 *)*piVar3;
  uVar8 = modbus_crc16(piVar3[1],*(char *)((int)piVar3 + 9) + -2,uVar17,uVar15,unaff_r4,unaff_r5,
                       unaff_r6,unaff_lr);
  pbVar13 = 0x2000005a;
  pbVar14 = 0x2000005a + 0x3c;
  *(undefined2 *)pbVar14 = uVar8;
  *(byte *)(piVar3[1] + (uint)*(byte *)((int)piVar3 + 9) + -2) = pbVar13[0x3d];
  bVar11 = *pbVar14;
  *(byte *)(piVar3[1] + (uint)*(byte *)((int)piVar3 + 9) + -1) = bVar11;
  return (char *)(uint)bVar11;
}


// ===== modbus_pack_regs @ 0800313c (size 112)

void modbus_pack_regs(void)

{
  int iVar1;
  undefined2 *puVar2;
  
  puVar2 = 0x200002f0;
  iVar1 = 0x2000005a;
  0x200002f0[0xc] = *(undefined2 *)(0x2000005a + 0x22);
  puVar2[0xb] = *(undefined2 *)(iVar1 + 0x24);
  puVar2[10] = *(undefined2 *)(iVar1 + 0x26);
  puVar2[9] = *(undefined2 *)(iVar1 + 0x28);
  puVar2[8] = *(undefined2 *)(iVar1 + 0x2a);
  puVar2[7] = *(undefined2 *)(iVar1 + 0x2c);
  puVar2[6] = *(undefined2 *)(iVar1 + 0x2e);
  puVar2[5] = *(undefined2 *)(iVar1 + 0x30);
  puVar2[4] = *(undefined2 *)(iVar1 + 0x32);
  puVar2[3] = *(undefined2 *)(iVar1 + 0x34);
  puVar2[2] = *(undefined2 *)(iVar1 + 0x36);
  puVar2[1] = *(undefined2 *)(iVar1 + 0x38);
  *puVar2 = *(undefined2 *)(iVar1 + 0x3a);
  puVar2[0x19] = *(undefined2 *)(iVar1 + 8);
  puVar2[0x18] = *(undefined2 *)(iVar1 + 10);
  puVar2[0x17] = *(undefined2 *)(iVar1 + 0xc);
  puVar2[0x16] = *(undefined2 *)(iVar1 + 0xe);
  puVar2[0x15] = *(undefined2 *)(iVar1 + 0x10);
  puVar2[0x14] = *(undefined2 *)(iVar1 + 0x12);
  puVar2[0x13] = *(undefined2 *)(iVar1 + 0x14);
  puVar2[0x12] = *(undefined2 *)(iVar1 + 0x16);
  puVar2[0x11] = *(undefined2 *)(iVar1 + 0x18);
  puVar2[0x10] = *(undefined2 *)(iVar1 + 0x1a);
  puVar2[0xf] = *(undefined2 *)(iVar1 + 0x1c);
  puVar2[0xe] = *(undefined2 *)(iVar1 + 0x1e);
  puVar2[0xd] = *(undefined2 *)(iVar1 + 0x20);
  return;
}


// ===== ads_i2c_nack @ 080031b4 (size 68)

void ads_i2c_nack(void)

{
  undefined4 uVar1;
  
  uVar1 = 0x40011000/*GPIOC*/;
  GPIO_SetBits(0x40011000/*GPIOC*/,0x4000);
  GPIO_SetBits(uVar1,0x8000);
  delay_us(0xf);
  GPIO_ResetBits(uVar1,0x8000);
  GPIO_ResetBits(uVar1,0x4000);
  delay_us(0xf);
  return;
}


// ===== NMI_Handler @ 080031fc (size 2)

void NMI_Handler(void)

{
  return;
}


// ===== nvic_init @ 080031fe (size 98)

void nvic_init(void)

{
  undefined4 in_r3;
  undefined4 local_10;
  
  local_10 = in_r3;
  NVIC_SetVectorTable(0x8000000,0);
  NVIC_PriorityGroupConfig(0x600);
  local_10 = 0x1000037;
  NVIC_Init(&local_10);
  local_10 = 0x1000125;
  NVIC_Init(&local_10);
  local_10 = 0x1000126;
  NVIC_Init(&local_10);
  return;
}


// ===== NVIC_Init @ 08003260 (size 94)

void NVIC_Init(byte *param_1)

{
  byte bVar1;
  uint uVar2;
  
  bVar1 = *param_1;
  if (param_1[3] != 0) {
    uVar2 = 0x700 - (*0xe000ed0c/*SCB+0xc*/ & 0x700) >> 8;
    (&NVIC_IP)[bVar1] =
         (char)(((uint)param_1[2] & 0xfU >> (uVar2 & 0xff) | (uint)param_1[1] << (4 - uVar2 & 0xff))
               << 4);
    *(int *)(&NVIC_ISER + (uint)(*param_1 >> 5) * 4) = 1 << (*param_1 & 0x1f);
    return;
  }
  *(int *)(&NVIC_ICER + (uint)(bVar1 >> 5) * 4) = 1 << (bVar1 & 0x1f);
  return;
}


// ===== NVIC_PriorityGroupConfig @ 080032c4 (size 10)

void NVIC_PriorityGroupConfig(uint param_1)

{
  *0xe000ed0c/*SCB+0xc*/ = param_1 | 0x5fa0000;
  return;
}


// ===== NVIC_SetVectorTable @ 080032d8 (size 12)

void NVIC_SetVectorTable(uint param_1,uint param_2)

{
  *0xe000ed08/*SCB+0x8*/ = param_2 & 0x1fffff80 | param_1;
  return;
}


// ===== oled_clear_cols @ 080032ec (size 30)

void oled_clear_cols(undefined4 param_1,undefined4 param_2,uint param_3)

{
  uint uVar1;
  
  oled_set_pos();
  for (uVar1 = 0; uVar1 < param_3; uVar1 = uVar1 + 1 & 0xff) {
    oled_write(0,0x40);
  }
  return;
}


// ===== oled_set_pos @ 0800330a (size 44)

/* SH1106 page/column (+2 offset) */

void oled_set_pos(int param_1,uint param_2)

{
  oled_write(param_2 | 0xb0,0);
  oled_write(param_1 + 2U & 0xf,0);
  oled_write((param_1 + 2U & 0xff) >> 4 | 0x10,0);
  return;
}


// ===== oled_draw_char16 @ 08003338 (size 78)

/* 8x16 glyph from font at 0x0800840C */

void oled_draw_char16(undefined4 param_1,char param_2,int param_3)

{
  int iVar1;
  uint uVar2;
  
  uVar2 = 0;
  oled_set_pos();
  iVar1 = 0x800840c;
  do {
    oled_write(*(undefined1 *)(iVar1 + uVar2 + param_3 * 0x10),0x40);
    uVar2 = uVar2 + 1 & 0xff;
  } while (uVar2 < 8);
  oled_set_pos(param_1,param_2 + '\x01');
  uVar2 = 0;
  do {
    oled_write(*(undefined1 *)(uVar2 + param_3 * 0x10 + iVar1 + 8),0x40);
    uVar2 = uVar2 + 1 & 0xff;
  } while (uVar2 < 8);
  return;
}


// ===== oled_draw_char16_cursor @ 0800338c (size 82)

/* glyph with underline */

void oled_draw_char16_cursor(undefined4 param_1,char param_2,int param_3)

{
  int iVar1;
  uint uVar2;
  
  uVar2 = 0;
  oled_set_pos();
  iVar1 = 0x800840c;
  do {
    oled_write(*(undefined1 *)(iVar1 + uVar2 + param_3 * 0x10),0x40);
    uVar2 = uVar2 + 1 & 0xff;
  } while (uVar2 < 8);
  oled_set_pos(param_1,param_2 + '\x01');
  uVar2 = 0;
  do {
    oled_write(*(byte *)(uVar2 + param_3 * 0x10 + iVar1 + 8) | 0x80,0x40);
    uVar2 = uVar2 + 1 & 0xff;
  } while (uVar2 < 8);
  return;
}


// ===== oled_i2c_write_byte @ 080033e4 (size 80)

/* Soft-I2C PA8=SCL PB15=SDA */

void oled_i2c_write_byte(uint param_1)

{
  undefined4 uVar1;
  undefined4 uVar2;
  byte bVar3;
  
  uVar1 = 0x40010800/*GPIOA*/;
  GPIO_ResetBits(0x40010800/*GPIOA*/,0x100);
  uVar2 = 0x40010c00/*GPIOB*/;
  bVar3 = 0;
  do {
    if ((int)(param_1 << 0x18) < 0) {
      GPIO_SetBits();
    }
    else {
      GPIO_ResetBits(uVar2,0x8000);
    }
    param_1 = (param_1 & 0x7f) << 1;
    GPIO_SetBits(uVar1,0x100);
    GPIO_ResetBits(uVar1,0x100);
    bVar3 = bVar3 + 1;
  } while (bVar3 < 8);
  return;
}


// ===== oled_i2c_ack @ 0800343c (size 28)

void oled_i2c_ack(void)

{
  undefined4 uVar1;
  
  uVar1 = 0x40010800/*GPIOA*/;
  GPIO_SetBits(0x40010800/*GPIOA*/,0x100);
  GPIO_ResetBits(uVar1,0x100);
  return;
}


// ===== oled_write @ 0800345c (size 108)

/* addr 0x78; ctrl 0x00 cmd / 0x40 data */

void oled_write(undefined4 param_1,undefined4 param_2)

{
  undefined4 uVar1;
  undefined4 uVar2;
  
  uVar1 = 0x40010800/*GPIOA*/;
  GPIO_SetBits(0x40010800/*GPIOA*/,0x100);
  uVar2 = 0x40010c00/*GPIOB*/;
  GPIO_SetBits(0x40010c00/*GPIOB*/,0x8000);
  GPIO_ResetBits(uVar2,0x8000);
  GPIO_ResetBits(uVar1,0x100);
  oled_i2c_write_byte(0x78);
  oled_i2c_ack();
  oled_i2c_write_byte(param_2);
  oled_i2c_ack();
  oled_i2c_write_byte(param_1);
  oled_i2c_ack();
  GPIO_SetBits(uVar1,0x100);
  GPIO_ResetBits(uVar2,0x8000);
  GPIO_SetBits(uVar2,0x8000);
  return;
}


// ===== PendSV_Handler @ 080034d0 (size 2)

void PendSV_Handler(void)

{
  return;
}


// ===== RCC_ADCCLKConfig @ 080034d4 (size 14)

void RCC_ADCCLKConfig(uint param_1)

{
  *(uint *)(0x40021000/*RCC*/ + 4) = *(uint *)(0x40021000/*RCC*/ + 4) & 0xffff3fff | param_1;
  return;
}


// ===== RCC_AHBPeriphClockCmd @ 080034e8 (size 18)

void RCC_AHBPeriphClockCmd(uint param_1,int param_2)

{
  if (param_2 == 0) {
    param_1 = *(uint *)(0x40021000/*RCC*/ + 0x14) & ~param_1;
  }
  else {
    param_1 = *(uint *)(0x40021000/*RCC*/ + 0x14) | param_1;
  }
  *(uint *)(0x40021000/*RCC*/ + 0x14) = param_1;
  return;
}


// ===== RCC_APB1PeriphClockCmd @ 08003500 (size 18)

void RCC_APB1PeriphClockCmd(uint param_1,int param_2)

{
  if (param_2 == 0) {
    param_1 = *(uint *)(0x40021000/*RCC*/ + 0x1c) & ~param_1;
  }
  else {
    param_1 = *(uint *)(0x40021000/*RCC*/ + 0x1c) | param_1;
  }
  *(uint *)(0x40021000/*RCC*/ + 0x1c) = param_1;
  return;
}


// ===== RCC_APB2PeriphResetCmd @ 08003530 (size 18)

void RCC_APB2PeriphResetCmd(uint param_1,int param_2)

{
  if (param_2 == 0) {
    param_1 = *(uint *)(0x40021000/*RCC*/ + 0xc) & ~param_1;
  }
  else {
    param_1 = *(uint *)(0x40021000/*RCC*/ + 0xc) | param_1;
  }
  *(uint *)(0x40021000/*RCC*/ + 0xc) = param_1;
  return;
}


// ===== rcc_init @ 08003548 (size 150)

/* WARNING: Removing unreachable block (ram,0x08003524) */
/* HSI/2 x6 = 24 MHz */

void rcc_init(void)

{
  int iVar1;
  
  RCC_DeInit();
  RCC_HSICmd(1);
  do {
    iVar1 = RCC_GetFlagStatus(0x21);
  } while (iVar1 == 0);
  RCC_HCLKConfig(0);
  RCC_PCLK2Config(0);
  RCC_PCLK1Config(0);
  RCC_ADCCLKConfig(0x4000);
  FLASH_SetLatency(2);
  FLASH_PrefetchBufferCmd(0x10);
  RCC_PLLConfig(0,0x100000);
  RCC_PLLCmd(1);
  do {
    iVar1 = RCC_GetFlagStatus(0x39);
  } while (iVar1 == 0);
  RCC_SYSCLKConfig(2);
  while (iVar1 = RCC_GetSYSCLKSource(), iVar1 != 8) {
    RCC_AHBPeriphClockCmd(3,1);
  }
  RCC_APB1PeriphClockCmd(0x20020027);
  *(uint *)(0x40021000/*RCC*/ + 0x18) = *(uint *)(0x40021000/*RCC*/ + 0x18) | 0x4a3d;
  return;
}


// ===== RCC_DeInit @ 080035d0 (size 54)

void RCC_DeInit(void)

{
  uint *puVar1;
  
  puVar1 = 0x40021000/*RCC*/;
  *0x40021000/*RCC*/ = *0x40021000/*RCC*/ | 1;
  puVar1[1] = puVar1[1] & 0xf8ff0000;
  *puVar1 = *puVar1 & 0xfef6ffff;
  *puVar1 = *puVar1 & 0xfffbffff;
  puVar1[1] = puVar1[1] & 0xff80ffff;
  puVar1[2] = 0x9f0000;
  puVar1[0xb] = 0;
  return;
}


// ===== RCC_GetClocksFreq @ 08003614 (size 136)

void RCC_GetClocksFreq(uint *param_1)

{
  int iVar1;
  uint uVar2;
  uint uVar3;
  int iVar4;
  
  iVar1 = 0x40021000/*RCC*/;
  uVar3 = *(uint *)(0x40021000/*RCC*/ + 4) & 0xc;
  uVar2 = 0x7a1200;
  if (((uVar3 != 0) && (uVar3 != 4)) && (uVar3 == 8)) {
    iVar4 = ((*(uint *)(0x40021000/*RCC*/ + 4) & 0x3c0000) >> 0x12) + 2;
    if ((*(uint *)(0x40021000/*RCC*/ + 4) & 0x10000) == 0) {
      *param_1 = 0x3d0900 * iVar4;
      goto LAB_0800365a;
    }
    uVar2 = iVar4 * (0x7a1200 / ((*(uint *)(0x40021000/*RCC*/ + 0x2c) & 0xf) + 1));
  }
  *param_1 = uVar2;
LAB_0800365a:
  iVar4 = 0x20000018;
  uVar2 = *param_1 >> *(sbyte *)(0x20000018 + (*(uint *)(iVar1 + 4) >> 4 & 0xf));
  param_1[1] = uVar2;
  param_1[2] = uVar2 >> *(sbyte *)(iVar4 + (*(uint *)(iVar1 + 4) >> 8 & 7));
  uVar2 = uVar2 >> *(sbyte *)(iVar4 + (*(uint *)(iVar1 + 4) >> 0xb & 7));
  param_1[3] = uVar2;
  param_1[4] = uVar2 / *(byte *)(iVar4 + -4 + (*(uint *)(iVar1 + 4) >> 0xe & 3));
  return;
}


// ===== RCC_GetFlagStatus @ 080036ac (size 44)

bool RCC_GetFlagStatus(uint param_1)

{
  uint uVar1;
  
  if (param_1 >> 5 == 1) {
    uVar1 = *0x40021000/*RCC*/;
  }
  else if (param_1 >> 5 == 2) {
    uVar1 = 0x40021000/*RCC*/[8];
  }
  else {
    uVar1 = 0x40021000/*RCC*/[9];
  }
  return (1 << (param_1 & 0x1f) & uVar1) != 0;
}


// ===== RCC_GetSYSCLKSource @ 080036dc (size 10)

uint RCC_GetSYSCLKSource(void)

{
  return *(uint *)(0x40021000/*RCC*/ + 4) & 0xc;
}


// ===== RCC_HCLKConfig @ 080036ec (size 14)

void RCC_HCLKConfig(uint param_1)

{
  *(uint *)(0x40021000/*RCC*/ + 4) = *(uint *)(0x40021000/*RCC*/ + 4) & 0xffffff0f | param_1;
  return;
}


// ===== RCC_HSICmd @ 08003700 (size 6)

void RCC_HSICmd(undefined4 param_1)

{
  *0x42420000 = param_1;
  return;
}


// ===== RCC_PCLK1Config @ 0800370c (size 14)

void RCC_PCLK1Config(uint param_1)

{
  *(uint *)(0x40021000/*RCC*/ + 4) = *(uint *)(0x40021000/*RCC*/ + 4) & 0xfffff8ff | param_1;
  return;
}


// ===== RCC_PCLK2Config @ 08003720 (size 16)

void RCC_PCLK2Config(int param_1)

{
  *(uint *)(0x40021000/*RCC*/ + 4) = *(uint *)(0x40021000/*RCC*/ + 4) & 0xffffc7ff | param_1 << 3;
  return;
}


// ===== RCC_PLLCmd @ 08003734 (size 6)

void RCC_PLLCmd(undefined4 param_1)

{
  *(undefined4 *)(0x42420000 + 0x60) = param_1;
  return;
}


// ===== RCC_PLLConfig @ 08003740 (size 16)

void RCC_PLLConfig(uint param_1,uint param_2)

{
  *(uint *)(0x40021000/*RCC*/ + 4) = param_1 | param_2 | *(uint *)(0x40021000/*RCC*/ + 4) & 0xffc2ffff;
  return;
}


// ===== RCC_SYSCLKConfig @ 08003754 (size 14)

void RCC_SYSCLKConfig(uint param_1)

{
  *(uint *)(0x40021000/*RCC*/ + 4) = *(uint *)(0x40021000/*RCC*/ + 4) & 0xfffffffc | param_1;
  return;
}


// ===== ads1115_read_raw @ 08003768 (size 102)

void ads1115_read_raw(void)

{
  int iVar1;
  int iVar2;
  undefined4 uVar3;
  uint uVar4;
  
  ads_i2c_start();
  do {
    iVar1 = ads_i2c_write(0x200001cc,2);
  } while (iVar1 == 0);
  ads_i2c_start();
  iVar1 = 0x200001cc;
  do {
    iVar2 = ads_i2c_write(iVar1 + 2,1);
  } while (iVar2 == 0);
  iVar1 = 0x200001cc + 0x10;
  for (uVar4 = 0; uVar4 < 2; uVar4 = uVar4 + 1 & 0xff) {
    uVar3 = ads_i2c_read_byte();
    *(undefined4 *)(iVar1 + uVar4 * 4) = uVar3;
    if (uVar4 == 1) {
      ads_i2c_nack();
    }
    else {
      ads_i2c_ack();
    }
  }
  ads_i2c_stop();
  return;
}


// ===== ads_i2c_read_byte @ 080037a0 (size 80)

uint ads_i2c_read_byte(void)

{
  undefined4 uVar1;
  int iVar2;
  uint uVar3;
  byte bVar4;
  
  uVar1 = 0x40011000/*GPIOC*/;
  uVar3 = 0;
  bVar4 = 0;
  do {
    GPIO_SetBits(uVar1,0x4000);
    GPIO_SetBits(uVar1,0x8000);
    uVar3 = (uVar3 & 0x7f) << 1;
    iVar2 = GPIO_ReadInputDataBit(uVar1,0x4000);
    if (iVar2 != 0) {
      uVar3 = uVar3 | 1;
    }
    GPIO_ResetBits(uVar1,0x8000);
    bVar4 = bVar4 + 1;
  } while (bVar4 < 8);
  return uVar3;
}


// ===== menu_call_handler @ 0800382a (size 10)

void menu_call_handler(int param_1)

{
                    /* WARNING: Could not recover jumptable at 0x08003832. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)(param_1 + 8))();
  return;
}


// ===== menu_edit_current_digit @ 08003834 (size 118)

void menu_edit_current_digit(undefined4 param_1,int param_2,int param_3,int param_4)

{
  char cVar1;
  undefined1 *puVar2;
  int iVar3;
  uint uVar4;
  uint uVar5;
  uint unaff_r5;
  
  puVar2 = 0x20000176;
  *0x20000176 = (char)param_4;
  cVar1 = *(char *)(param_3 + param_4);
  puVar2[1] = cVar1;
  iVar3 = 0x20000654;
  if (cVar1 == '\x05') {
    unaff_r5 = 10000;
  }
  else if (cVar1 == '\x06') {
    unaff_r5 = 1000;
  }
  else if (cVar1 == '\a') {
    unaff_r5 = 100;
  }
  else if (cVar1 == '\b') {
    unaff_r5 = 10;
  }
  else if (cVar1 == '\t') {
    unaff_r5 = 1;
  }
  uVar5 = *(uint *)(0x20000654 + 0x14);
  if (param_2 == 2) {
    uVar4 = (uint)*(ushort *)(0x20000398 + 0xc);
    if (uVar5 + unaff_r5 < (uint)*(ushort *)(0x20000398 + 0xc)) {
      uVar4 = uVar5 + unaff_r5;
    }
  }
  else {
    if (param_2 != 3) goto LAB_08003886;
    if (unaff_r5 < uVar5) {
      uVar4 = uVar5 - unaff_r5;
    }
    else {
      uVar4 = 0;
    }
  }
  *(uint *)(0x20000654 + 0x14) = uVar4;
LAB_08003886:
  ui_draw_setpoints(1,*(undefined4 *)(iVar3 + 0x10),*(undefined4 *)(iVar3 + 0x14),cVar1 + -4);
  set_current_dac(*(undefined4 *)(iVar3 + 0x14));
  return;
}


// ===== menu_edit_voltage_digit @ 080038b8 (size 116)

void menu_edit_voltage_digit(undefined4 param_1,int param_2,int param_3,int param_4)

{
  char cVar1;
  undefined1 *puVar2;
  int iVar3;
  uint uVar4;
  uint uVar5;
  uint unaff_r5;
  
  puVar2 = 0x20000174;
  *0x20000174 = (char)param_4;
  cVar1 = *(char *)(param_3 + param_4);
  puVar2[1] = cVar1;
  iVar3 = 0x20000654;
  if (cVar1 == '\0') {
    unaff_r5 = 10000;
  }
  else if (cVar1 == '\x01') {
    unaff_r5 = 1000;
  }
  else if (cVar1 == '\x02') {
    unaff_r5 = 100;
  }
  else if (cVar1 == '\x03') {
    unaff_r5 = 10;
  }
  else if (cVar1 == '\x04') {
    unaff_r5 = 1;
  }
  uVar5 = *(uint *)(0x20000654 + 0x10);
  if (param_2 == 2) {
    uVar4 = (uint)*(ushort *)(0x20000398 + 10);
    if (uVar5 + unaff_r5 < (uint)*(ushort *)(0x20000398 + 10)) {
      uVar4 = uVar5 + unaff_r5;
    }
  }
  else {
    if (param_2 != 3) goto LAB_08003908;
    if (unaff_r5 < uVar5) {
      uVar4 = uVar5 - unaff_r5;
    }
    else {
      uVar4 = 0;
    }
  }
  *(uint *)(0x20000654 + 0x10) = uVar4;
LAB_08003908:
  ui_draw_setpoints(0,*(undefined4 *)(iVar3 + 0x10),*(undefined4 *)(iVar3 + 0x14),cVar1 + '\x01');
  set_voltage_dac(*(undefined4 *)(iVar3 + 0x10));
  return;
}


// ===== menu_info_page @ 08003938 (size 96)

/* Encoder cycles info page 0..6 */

void menu_info_page(undefined4 param_1,int param_2)

{
  undefined *puVar1;
  char cVar2;
  char cVar3;
  undefined1 uVar4;
  
  puVar1 = 0x200002c1;
  cVar3 = *0x200002c1;
  if (param_2 == 2) {
    cVar2 = cVar3 + '\x01';
    cVar3 = cVar3 + '\x01';
    *0x200002c1 = cVar2;
    if (cVar3 == '\a') {
      uVar4 = 0;
LAB_08003968:
      *puVar1 = uVar4;
      goto LAB_08003970;
    }
  }
  else if (param_2 == 3) {
    if (cVar3 == '\0') {
      uVar4 = 6;
      goto LAB_08003968;
    }
    cVar3 = cVar3 + -1;
    *0x200002c1 = cVar3;
  }
  if (cVar3 == '\x02') {
    measure_vin();
  }
LAB_08003970:
  ui_draw_info_page(*0x200002c2,*puVar1,*(undefined4 *)(PTR_g_state_0800399c + 0x30),
                    *(undefined4 *)(PTR_g_state_0800399c + 0x34),
                    *(undefined4 *)(PTR_g_state_0800399c + 0x24),
                    *(undefined4 *)(PTR_g_state_0800399c + 0x38),
                    *(undefined4 *)(PTR_g_state_0800399c + 0x3c),
                    *(undefined4 *)(PTR_g_state_0800399c + 0x28),
                    *(undefined4 *)(PTR_g_state_0800399c + 0x2c));
  return;
}


// ===== dac8562_shift8 @ 080039a4 (size 68)

void dac8562_shift8(uint param_1)

{
  undefined4 uVar1;
  byte bVar2;
  
  uVar1 = 0x40010c00/*GPIOB*/;
  bVar2 = 0;
  do {
    GPIO_SetBits(uVar1,0x1000);
    if ((int)(param_1 << 0x18) < 0) {
      GPIO_SetBits();
    }
    else {
      GPIO_ResetBits(uVar1,0x2000);
    }
    param_1 = (param_1 & 0x7f) << 1;
    GPIO_ResetBits(uVar1,0x1000);
    bVar2 = bVar2 + 1;
  } while (bVar2 < 8);
  return;
}


// ===== dac8562_shift16 @ 080039ec (size 72)

void dac8562_shift16(uint param_1)

{
  undefined4 uVar1;
  byte bVar2;
  
  uVar1 = 0x40010c00/*GPIOB*/;
  bVar2 = 0;
  do {
    GPIO_SetBits(uVar1,0x1000);
    if ((int)(param_1 << 0x10) < 0) {
      GPIO_SetBits();
    }
    else {
      GPIO_ResetBits(uVar1,0x2000);
    }
    param_1 = (param_1 & 0x7fff) << 1;
    GPIO_ResetBits(uVar1,0x1000);
    bVar2 = bVar2 + 1;
  } while (bVar2 < 0x10);
  return;
}


// ===== SVC_Handler @ 08003a38 (size 2)

void SVC_Handler(void)

{
  return;
}


// ===== SetSysClockTo24 @ 08003a3c (size 130)

void SetSysClockTo24(void)

{
  uint *puVar1;
  int local_8;
  
  puVar1 = 0x40021000/*RCC*/;
  local_8 = 0;
  *0x40021000/*RCC*/ = *0x40021000/*RCC*/ | 0x10000;
  do {
    local_8 = local_8 + 1;
    if ((*puVar1 & 0x20000) != 0) break;
  } while (local_8 != 0x500);
  if ((int)(*puVar1 << 0xe) < 0) {
    puVar1[1] = puVar1[1];
    puVar1[1] = puVar1[1];
    puVar1[1] = puVar1[1];
    puVar1[1] = puVar1[1] & 0xffc0ffff;
    puVar1[1] = puVar1[1] | 0x130000;
    *puVar1 = *puVar1 | 0x1000000;
    do {
    } while (-1 < (int)(*puVar1 << 6));
    puVar1[1] = puVar1[1] & 0xfffffffc;
    puVar1[1] = puVar1[1] | 2;
    do {
    } while ((puVar1[1] & 0xf) >> 2 != 2);
    return;
  }
  return;
}


// ===== ads_i2c_start @ 08003ac4 (size 64)

void ads_i2c_start(void)

{
  undefined4 uVar1;
  
  uVar1 = 0x40011000/*GPIOC*/;
  GPIO_SetBits(0x40011000/*GPIOC*/,0x8000);
  GPIO_SetBits(uVar1,0x4000);
  delay_us(0xf);
  GPIO_ResetBits(uVar1,0x4000);
  delay_us(0xf);
  GPIO_ResetBits(uVar1,0x8000);
  delay_us(0xf);
  return;
}


// ===== ads_i2c_stop @ 08003b08 (size 54)

void ads_i2c_stop(void)

{
  undefined4 uVar1;
  
  uVar1 = 0x40011000/*GPIOC*/;
  GPIO_ResetBits(0x40011000/*GPIOC*/,0x4000);
  delay_us(0xf);
  GPIO_SetBits(uVar1,0x8000);
  delay_us(0xf);
  GPIO_SetBits(uVar1,0x4000);
  delay_us(0xf);
  return;
}


// ===== SysTick_Handler @ 08003b5c (size 2)

void SysTick_Handler(void)

{
  return;
}


// ===== SysTick_CLKSourceConfig @ 08003b5e (size 30)

/* WARNING: Removing unreachable block (ram,0x08003b4e) */

void SysTick_CLKSourceConfig(void)

{
  uint uVar1;
  
  uVar1 = SysTick;
  SysTick = uVar1 | 4;
  return;
}


// ===== SystemInit @ 08003b64 (size 68)

void SystemInit(void)

{
  uint *puVar1;
  
  puVar1 = 0x40021000/*RCC*/;
  *0x40021000/*RCC*/ = *0x40021000/*RCC*/ | 1;
  puVar1[1] = puVar1[1] & 0xf8ff0000;
  *puVar1 = *puVar1 & 0xfef6ffff;
  *puVar1 = *puVar1 & 0xfffbffff;
  puVar1[1] = puVar1[1] & 0xff80ffff;
  puVar1[2] = 0x9f0000;
  puVar1[0xb] = 0;
  SetSysClockTo24();
  *0xe000ed08/*SCB+0x8*/ = 0x8000000;
  return;
}


// ===== TIM7_IRQHandler @ 08003bb8 (size 414)

/* 10 kHz: load enable, setpoint clamp, DACs, 1/10/100/500 ms ticks, fan+buzzer soft PWM */

void TIM7_IRQHandler(void)

{
  undefined4 uVar1;
  int iVar2;
  undefined1 *puVar3;
  short *psVar4;
  short sVar5;
  ushort uVar6;
  int iVar7;
  int iVar8;
  
  uVar1 = 0x40001400/*TIM7*/;
  iVar7 = TIM_GetITStatus(0x40001400/*TIM7*/,1);
  if (iVar7 == 0) {
    return;
  }
  TIM_ClearITPendingBit(uVar1,1);
  list_mode_step();
  iVar7 = 0x20000654;
  if (*(int *)(0x20000654 + 4) == 1) {
    GPIO_ResetBits(0x40010800/*GPIOA*/,0x8000);
    GPIO_ResetBits(0x40010c00/*GPIOB*/,0x800);
  }
  else {
    GPIO_SetBits();
  }
  iVar2 = 0x20000398;
  if ((uint)*(ushort *)(0x20000398 + 10) < *(uint *)(iVar7 + 0x10)) {
    *(uint *)(iVar7 + 0x10) = (uint)*(ushort *)(0x20000398 + 10);
  }
  if ((uint)*(ushort *)(iVar2 + 0xc) < *(uint *)(iVar7 + 0x14)) {
    *(uint *)(iVar7 + 0x14) = (uint)*(ushort *)(iVar2 + 0xc);
  }
  if ((uint)*(ushort *)(iVar2 + 0xe) < *(uint *)(iVar7 + 0x18)) {
    *(uint *)(iVar7 + 0x18) = (uint)*(ushort *)(iVar2 + 0xe);
  }
  set_voltage_dac(*(undefined4 *)(iVar7 + 0x10));
  iVar8 = *(int *)(iVar7 + 0x14);
  if ((uint)*(ushort *)(iVar2 + 0xe) + (uint)(*(ushort *)(iVar2 + 0xe) >> 2) <=
      (uint)(((ulonglong)(uint)(iVar8 * *(int *)(iVar7 + 8)) / 1000) / (ulonglong)*0x200001e4)) {
    iVar8 = 0;
    *(undefined4 *)(iVar7 + 0x14) = 0;
  }
  set_current_dac(iVar8);
  iVar2 = 0x42218000;
  puVar3 = 0x2000021c;
  uVar6 = *(ushort *)(0x2000021c + 0x12);
  *(ushort *)(0x2000021c + 0x12) = uVar6 + 1;
  if (uVar6 < 9) {
LAB_08003d0e:
    if (*(short *)(puVar3 + 6) != 0) {
LAB_08003d12:
      uVar6 = *(short *)(puVar3 + 10) + 1;
      *(ushort *)(puVar3 + 10) = uVar6;
      if (uVar6 < 10) {
        if (1 < uVar6) goto LAB_08003d2c;
      }
      else {
        *(undefined2 *)(puVar3 + 10) = 0;
      }
      *(undefined4 *)(iVar2 + 0x1a8) = 1;
      goto LAB_08003d30;
    }
  }
  else {
    *(undefined2 *)(puVar3 + 0x12) = 0;
    modbus_frame_timeout();
    encoder_tick();
    protection_check();
    uVar6 = *(ushort *)(puVar3 + 0x1a);
    *(ushort *)(puVar3 + 0x1a) = uVar6 + 1;
    if (8 < uVar6) {
      *(undefined2 *)(puVar3 + 0x1a) = 0;
      *puVar3 = 1;
    }
    uVar6 = *(ushort *)(puVar3 + 0x14);
    *(ushort *)(puVar3 + 0x14) = uVar6 + 1;
    if (0x62 < uVar6) {
      *(undefined2 *)(puVar3 + 0x14) = 0;
      puVar3[1] = 1;
      IWDG_ReloadCounter();
    }
    uVar6 = *(ushort *)(puVar3 + 0x16);
    *(ushort *)(puVar3 + 0x16) = uVar6 + 1;
    if (0x1f2 < uVar6) {
      *(undefined2 *)(puVar3 + 0x16) = 0;
      puVar3[2] = 1;
      sVar5 = *(short *)(puVar3 + 0x10) + 1;
      *(short *)(puVar3 + 0x10) = sVar5;
      if (sVar5 == 2) {
        *(undefined2 *)(puVar3 + 0x10) = 0;
      }
      else if (sVar5 == 1) {
        *(int *)(iVar7 + 0x38) = *(int *)(iVar7 + 0x38) + 1;
      }
    }
    if ((puVar3[4] == '\x01') &&
       (uVar6 = *(ushort *)(puVar3 + 0xe), *(ushort *)(puVar3 + 0xe) = uVar6 + 1, 0xbb6 < uVar6)) {
      *(undefined2 *)(puVar3 + 0xe) = 0;
      puVar3[4] = 0;
    }
    uVar6 = *(ushort *)(puVar3 + 0x18);
    *(ushort *)(puVar3 + 0x18) = uVar6 + 1;
    psVar4 = 0x20000032;
    if (uVar6 < 0x18) goto LAB_08003d0e;
    *(undefined2 *)(puVar3 + 0x18) = 0;
    if (((*psVar4 == 2) || (puVar3[5] != '\0')) || (puVar3[4] != '\0')) {
      uVar6 = *(short *)(puVar3 + 8) + 1;
      *(ushort *)(puVar3 + 8) = uVar6;
      if (5 < uVar6) {
        *(undefined2 *)(puVar3 + 6) = 0;
        if (9 < uVar6) {
          *(undefined2 *)(puVar3 + 8) = 0;
        }
        goto LAB_08003d2c;
      }
      *(undefined2 *)(puVar3 + 6) = 1;
      goto LAB_08003d12;
    }
    *(undefined2 *)(puVar3 + 6) = 0;
  }
LAB_08003d2c:
  *(undefined4 *)(iVar2 + 0x1a8) = 0;
LAB_08003d30:
  if ((byte)puVar3[3] != 0) {
    sVar5 = *(short *)(puVar3 + 0xc);
    *(ushort *)(puVar3 + 0xc) = sVar5 + 1U;
    if (199 < (ushort)(sVar5 + 1U)) {
      *(undefined2 *)(puVar3 + 0xc) = 0;
    }
    if (*(ushort *)(puVar3 + 0xc) <= (ushort)(byte)puVar3[3]) {
      *(undefined4 *)(iVar2 + 400) = 1;
      return;
    }
  }
  *(undefined4 *)(iVar2 + 400) = 0;
  return;
}


// ===== TIM_CtrlPWMOutputs @ 08003d7c (size 20)

void TIM_CtrlPWMOutputs(ushort *param_1,int param_2)

{
  ushort uVar1;
  
  if (param_2 == 0) {
    uVar1 = *param_1 & 0xff7f;
  }
  else {
    uVar1 = *param_1 | 0x80;
  }
  *param_1 = uVar1;
  return;
}


// ===== TIM_ClearITPendingBit @ 08003d90 (size 6)

void TIM_ClearITPendingBit(int param_1,ushort param_2)

{
  *(ushort *)(param_1 + 0x10) = ~param_2;
  return;
}


// ===== TIM_Cmd @ 08003d96 (size 20)

void TIM_Cmd(ushort *param_1,int param_2)

{
  ushort uVar1;
  
  if (param_2 == 0) {
    uVar1 = *param_1 & 0xfffe;
  }
  else {
    uVar1 = *param_1 | 1;
  }
  *param_1 = uVar1;
  return;
}


// ===== TIM_ARRPreloadConfig_ @ 08003daa (size 22)

void TIM_ARRPreloadConfig_(int param_1,int param_2)

{
  ushort uVar1;
  
  uVar1 = *(ushort *)(param_1 + 0x44);
  if (param_2 == 0) {
    uVar1 = (ushort)(((uint)uVar1 << 0x11) >> 0x11);
  }
  else {
    uVar1 = uVar1 | 0x8000;
  }
  *(ushort *)(param_1 + 0x44) = uVar1;
  return;
}


// ===== TIM_GetITStatus @ 08003dc0 (size 24)

undefined4 TIM_GetITStatus(int param_1,ushort param_2)

{
  undefined4 uVar1;
  
  uVar1 = 0;
  if (((*(ushort *)(param_1 + 0x10) & param_2) != 0) &&
     ((*(ushort *)(param_1 + 0xc) & param_2) != 0)) {
    uVar1 = 1;
  }
  return uVar1;
}


// ===== TIM_ITConfig @ 08003dd8 (size 16)

void TIM_ITConfig(int param_1,ushort param_2,int param_3)

{
  if (param_3 == 0) {
    param_2 = *(ushort *)(param_1 + 0xc) & ~param_2;
  }
  else {
    param_2 = *(ushort *)(param_1 + 0xc) | param_2;
  }
  *(ushort *)(param_1 + 0xc) = param_2;
  return;
}


// ===== TIM_OC2Init @ 08003de8 (size 120)

void TIM_OC2Init(int param_1,short *param_2)

{
  short sVar1;
  ushort uVar2;
  ushort uVar3;
  
  *(ushort *)(param_1 + 0x20) = *(ushort *)(param_1 + 0x20) & 0xffef;
  uVar3 = *(ushort *)(param_1 + 4);
  sVar1 = *param_2;
  uVar2 = param_2[1] << 4 | param_2[4] << 4 | *(ushort *)(param_1 + 0x20) & 0xffdf;
  if ((param_1 == 0x40012c00/*TIM1*/) || (param_1 == 0x40013400)) {
    uVar2 = param_2[2] << 4 | (param_2[5] << 4 | uVar2 & 0xff7f) & 0xffbf;
    uVar3 = param_2[7] << 2 | param_2[6] << 2 | uVar3 & 0xf3ff;
  }
  *(ushort *)(param_1 + 4) = uVar3;
  *(ushort *)(param_1 + 0x18) = sVar1 << 8 | *(ushort *)(param_1 + 0x18) & 0x8cff;
  *(short *)(param_1 + 0x38) = param_2[3];
  *(ushort *)(param_1 + 0x20) = uVar2;
  return;
}


// ===== TIM_OC2PreloadConfig @ 08003e68 (size 20)

void TIM_OC2PreloadConfig(int param_1,short param_2)

{
  *(ushort *)(param_1 + 0x18) = param_2 << 8 | *(ushort *)(param_1 + 0x18) & 0xf7ff;
  return;
}


// ===== TIM_SetCompare2 @ 08003e7c (size 4)

void TIM_SetCompare2(int param_1,undefined2 param_2)

{
  *(undefined2 *)(param_1 + 0x38) = param_2;
  return;
}


// ===== TIM_TimeBaseInit @ 08003e80 (size 114)

void TIM_TimeBaseInit(ushort *param_1,ushort *param_2)

{
  ushort *puVar1;
  ushort *puVar2;
  ushort uVar3;
  
  puVar2 = 0x40013400;
  puVar1 = 0x40012c00/*TIM1*/;
  uVar3 = *param_1;
  if ((((param_1 == 0x40012c00/*TIM1*/) || (param_1 == 0x40013400)) || (param_1 == (ushort *)&TIM2)) ||
     (((param_1 == 0x40000400/*TIM3*/ || (param_1 == 0x40000800/*TIM4*/)) || (param_1 == 0x40000c00)))) {
    uVar3 = param_2[1] | uVar3 & 0xff8f;
  }
  if ((param_1 != 0x40001000/*TIM6*/) && (param_1 != 0x40001400/*TIM7*/)) {
    uVar3 = param_2[3] | uVar3 & 0xfcff;
  }
  *param_1 = uVar3;
  param_1[0x16] = param_2[2];
  param_1[0x14] = *param_2;
  if ((((param_1 == puVar1) || (param_1 == puVar2)) || (param_1 == 0x40014000/*TIM15*/)) ||
     ((param_1 == 0x40014400/*TIM16*/ || (param_1 == 0x40014800/*TIM17*/)))) {
    param_1[0x18] = (ushort)(byte)param_2[4];
  }
  param_1[10] = 1;
  return;
}


// ===== usart2_putc @ 08003f1c (size 38)

void usart2_putc(undefined4 param_1)

{
  undefined4 uVar1;
  int iVar2;
  
  uVar1 = 0x40004400/*USART2*/;
  USART_SendData(0x40004400/*USART2*/,param_1);
  do {
    iVar2 = USART_GetFlagStatus(uVar1,0x80);
  } while (iVar2 == 0);
  do {
    iVar2 = USART_GetFlagStatus(uVar1,0x40);
  } while (iVar2 != 1);
  return;
}


// ===== USART1_IRQHandler @ 08003f48 (size 84)

void USART1_IRQHandler(void)

{
  byte bVar1;
  undefined4 uVar2;
  undefined1 uVar3;
  int iVar4;
  
  uVar2 = 0x40013800/*USART1*/;
  iVar4 = USART_GetITStatus(0x40013800/*USART1*/,0x525);
  if (iVar4 != 0) {
    USART_ClearITPendingBit(uVar2,0x525);
    uVar3 = USART_ReceiveData(uVar2);
    iVar4 = 0x20000208;
    bVar1 = *(byte *)(0x20000208 + 0xf);
    *(undefined1 *)(0x20000334 + (uint)bVar1) = uVar3;
    *(byte *)(iVar4 + 0xf) = bVar1 + 1;
  }
  iVar4 = USART_GetFlagStatus(uVar2,8);
  if (iVar4 == 1) {
    USART_ReceiveData(uVar2);
    USART_ClearFlag(uVar2,8);
  }
  USART_ClearFlag(uVar2,0x525);
  return;
}


// ===== usart1_parse_meter @ 08003fa8 (size 164)

/* Parse dd.dddd reading from reference DMM */

void usart1_parse_meter(void)

{
  char cVar1;
  int iVar2;
  char *pcVar3;
  
  pcVar3 = 0x20000334;
  iVar2 = 0x20000208;
  if (7 < *(byte *)(0x20000208 + 0xf)) {
    *(undefined1 *)(0x20000208 + 0xf) = 0;
    if ((pcVar3[2] == '.') && (pcVar3[7] == '\n')) {
      cVar1 = *pcVar3;
      *(byte *)(iVar2 + 8) = cVar1 - 0x30U;
      if (9 < (byte)(cVar1 - 0x30U)) {
        *(undefined1 *)(iVar2 + 8) = 0;
      }
      cVar1 = pcVar3[1];
      *(byte *)(iVar2 + 9) = cVar1 - 0x30U;
      if (9 < (byte)(cVar1 - 0x30U)) {
        *(undefined1 *)(iVar2 + 9) = 0;
      }
      cVar1 = pcVar3[3];
      *(byte *)(iVar2 + 10) = cVar1 - 0x30U;
      if (9 < (byte)(cVar1 - 0x30U)) {
        *(undefined1 *)(iVar2 + 10) = 0;
      }
      cVar1 = pcVar3[4];
      *(byte *)(iVar2 + 0xb) = cVar1 - 0x30U;
      if (9 < (byte)(cVar1 - 0x30U)) {
        *(undefined1 *)(iVar2 + 0xb) = 0;
      }
      cVar1 = pcVar3[5];
      *(byte *)(iVar2 + 0xc) = cVar1 - 0x30U;
      if (9 < (byte)(cVar1 - 0x30U)) {
        *(undefined1 *)(iVar2 + 0xc) = 0;
      }
      cVar1 = pcVar3[6];
      *(byte *)(iVar2 + 0xd) = cVar1 - 0x30U;
      if (9 < (byte)(cVar1 - 0x30U)) {
        *(undefined1 *)(iVar2 + 0xd) = 0;
      }
      *(uint *)(iVar2 + 0x10) =
           (uint)*(byte *)(iVar2 + 8) * 10000 + (uint)*(byte *)(iVar2 + 9) * 1000 +
           (uint)*(byte *)(iVar2 + 10) * 100 + (uint)*(byte *)(iVar2 + 0xb) * 10 +
           (uint)*(byte *)(iVar2 + 0xc);
    }
  }
  return;
}


// ===== USART2_IRQHandler @ 08004054 (size 46)

void USART2_IRQHandler(void)

{
  undefined4 uVar1;
  undefined1 uVar2;
  int iVar3;
  
  uVar1 = 0x40004400/*USART2*/;
  iVar3 = USART_GetITStatus(0x40004400/*USART2*/,0x525);
  if (iVar3 != 0) {
    uVar2 = USART_ReceiveData(uVar1);
    modbus_rx_byte(uVar2);
    USART_ClearITPendingBit(uVar1,0x525);
    return;
  }
  return;
}


// ===== USART_ClearFlag @ 08004088 (size 6)

void USART_ClearFlag(ushort *param_1,ushort param_2)

{
  *param_1 = ~param_2;
  return;
}


// ===== USART_ClearITPendingBit @ 0800408e (size 12)

void USART_ClearITPendingBit(ushort *param_1,uint param_2)

{
  *param_1 = ~(ushort)(1 << (param_2 >> 8 & 0xff));
  return;
}


// ===== USART_Cmd @ 0800409a (size 20)

void USART_Cmd(int param_1,int param_2)

{
  ushort uVar1;
  
  if (param_2 == 0) {
    uVar1 = *(ushort *)(param_1 + 0xc) & 0xdfff;
  }
  else {
    uVar1 = *(ushort *)(param_1 + 0xc) | 0x2000;
  }
  *(ushort *)(param_1 + 0xc) = uVar1;
  return;
}


// ===== USART_GetFlagStatus @ 080040ae (size 14)

bool USART_GetFlagStatus(ushort *param_1,ushort param_2)

{
  return (*param_1 & param_2) != 0;
}


// ===== USART_GetITStatus @ 080040bc (size 62)

undefined4 USART_GetITStatus(ushort *param_1,uint param_2)

{
  uint uVar1;
  undefined4 uVar2;
  
  uVar2 = 0;
  uVar1 = (param_2 & 0xff) >> 5;
  if (uVar1 == 1) {
    uVar1 = (uint)param_1[6];
  }
  else if (uVar1 == 2) {
    uVar1 = (uint)param_1[8];
  }
  else {
    uVar1 = (uint)param_1[10];
  }
  if (((uVar1 & 1 << (param_2 & 0x1f)) != 0) && (((uint)*param_1 & 1 << (param_2 >> 8 & 0xff)) != 0)
     ) {
    uVar2 = 1;
  }
  return uVar2;
}


// ===== USART_ITConfig @ 080040fa (size 48)

void USART_ITConfig(int param_1,uint param_2,int param_3)

{
  uint *puVar1;
  uint uVar2;
  uint uVar3;
  
  uVar3 = (param_2 & 0xff) >> 5;
  uVar2 = 1 << (param_2 & 0x1f);
  if (uVar3 == 1) {
    puVar1 = (uint *)(param_1 + 0xc);
  }
  else if (uVar3 == 2) {
    puVar1 = (uint *)(param_1 + 0x10);
  }
  else {
    puVar1 = (uint *)(param_1 + 0x14);
  }
  if (param_3 == 0) {
    uVar2 = *puVar1 & ~uVar2;
  }
  else {
    uVar2 = *puVar1 | uVar2;
  }
  *puVar1 = uVar2;
  return;
}


// ===== USART_Init @ 0800412c (size 166)

void USART_Init(int param_1,int *param_2)

{
  ushort uVar1;
  uint uVar2;
  uint uVar3;
  undefined1 auStack_20 [8];
  int local_18;
  int local_14;
  
  *(ushort *)(param_1 + 0x10) = *(ushort *)((int)param_2 + 6) | *(ushort *)(param_1 + 0x10) & 0xcfff
  ;
  *(ushort *)(param_1 + 0xc) =
       *(ushort *)(param_2 + 1) | *(ushort *)(param_2 + 2) |
       *(ushort *)((int)param_2 + 10) | *(ushort *)(param_1 + 0xc) & 0xe9f3;
  *(ushort *)(param_1 + 0x14) = *(ushort *)(param_2 + 3) | *(ushort *)(param_1 + 0x14) & 0xfcff;
  RCC_GetClocksFreq(auStack_20);
  if (param_1 == 0x40013800/*USART1*/) {
    local_18 = local_14;
  }
  if ((int)((uint)*(ushort *)(param_1 + 0xc) << 0x10) < 0) {
    uVar3 = *param_2 << 1;
  }
  else {
    uVar3 = *param_2 << 2;
  }
  uVar3 = (uint)(local_18 * 0x19) / uVar3;
  uVar2 = uVar3 % 100;
  if ((int)((uint)*(ushort *)(param_1 + 0xc) << 0x10) < 0) {
    uVar1 = (ushort)((uVar2 * 8 + 0x32) / 100) & 7;
  }
  else {
    uVar1 = (ushort)((uVar2 * 0x10 + 0x32) / 100) & 0xf;
  }
  *(ushort *)(param_1 + 8) = uVar1 | (ushort)(uVar3 / 100 << 4);
  return;
}


// ===== USART_ReceiveData @ 080041d8 (size 8)

ushort USART_ReceiveData(int param_1)

{
  return *(ushort *)(param_1 + 4) & 0x1ff;
}


// ===== USART_SendData @ 080041e0 (size 32)

void USART_SendData(undefined4 param_1,undefined4 param_2)

{
  int iVar1;
  int extraout_r2;
  int extraout_r3;
  int iVar2;
  int extraout_r3_00;
  
  USART_ClearFlag(param_1,0x40,param_2,param_1);
  *(ushort *)(extraout_r3 + 4) = (ushort)((uint)(extraout_r2 << 0x17) >> 0x17);
  iVar2 = extraout_r3;
  do {
    iVar1 = USART_GetFlagStatus(iVar2,0x40);
    iVar2 = extraout_r3_00;
  } while (iVar1 == 0);
  return;
}


// ===== usart1_putc @ 08004200 (size 26)

void usart1_putc(undefined4 param_1)

{
  undefined4 uVar1;
  int iVar2;
  
  uVar1 = 0x40013800/*USART1*/;
  USART_SendData(0x40013800/*USART1*/,param_1);
  do {
    iVar2 = USART_GetFlagStatus(uVar1,0x80);
  } while (iVar2 == 0);
  return;
}


// ===== modbus_rx_byte @ 08004220 (size 44)

void modbus_rx_byte(undefined1 param_1)

{
  byte bVar1;
  char *pcVar2;
  
  pcVar2 = 0x20000208;
  0x20000208[5] = '\x01';
  pcVar2[6] = '\0';
  if (pcVar2[4] == '\0' && *pcVar2 == '\0') {
    bVar1 = pcVar2[7];
    if (bVar1 < 0x28) {
      *(undefined1 *)(0x20000348 + (uint)bVar1) = param_1;
      pcVar2[7] = bVar1 + 1;
      return;
    }
  }
  pcVar2[4] = '\x01';
  return;
}


// ===== rs485_send @ 08004254 (size 56)

void rs485_send(undefined1 *param_1,uint param_2)

{
  undefined4 uVar1;
  uint uVar2;
  
  uVar1 = 0x40011000/*GPIOC*/;
  GPIO_SetBits(0x40011000/*GPIOC*/,0x2000);
  for (uVar2 = 0; uVar2 < param_2; uVar2 = uVar2 + 1 & 0xff) {
    usart2_putc(*param_1);
    param_1 = param_1 + 1;
  }
  GPIO_ResetBits(uVar1,0x2000);
  return;
}


// ===== modbus_frame_timeout @ 08004290 (size 64)

void modbus_frame_timeout(void)

{
  char cVar1;
  undefined1 *puVar2;
  char *pcVar3;
  
  puVar2 = 0x20000208;
  if ((0x20000208[5] == '\x01') &&
     (cVar1 = 0x20000208[6], 0x20000208[6] = cVar1 + 1U, pcVar3 = 0x20000348,
     10 < (byte)(cVar1 + 1U))) {
    puVar2[5] = 0;
    if (((*pcVar3 == puVar2[2]) && (puVar2[4] == '\0')) ||
       ((*pcVar3 == puVar2[3] && (puVar2[4] == '\0')))) {
      puVar2[1] = puVar2[7];
      *puVar2 = 1;
    }
    puVar2[7] = 0;
    puVar2[4] = 0;
  }
  return;
}


// ===== UsageFault_Handler @ 080042d8 (size 2)

void UsageFault_Handler(void)

{
  do {
                    /* WARNING: Do nothing block with infinite loop */
  } while( true );
}


// ===== ads1115_write_cfg @ 080042dc (size 20)

void ads1115_write_cfg(void)

{
  int iVar1;
  
  ads_i2c_start();
  do {
    iVar1 = ads_i2c_write(0x200001cf,4);
  } while (iVar1 == 0);
  return;
}


// ===== ads_i2c_write_byte @ 080042f4 (size 82)

void ads_i2c_write_byte(uint param_1)

{
  undefined4 uVar1;
  uint uVar2;
  
  uVar1 = 0x40011000/*GPIOC*/;
  uVar2 = 0;
  do {
    if ((int)(param_1 << 0x18) < 0) {
      GPIO_SetBits();
    }
    else {
      GPIO_ResetBits(uVar1,0x4000);
    }
    GPIO_SetBits(uVar1,0x8000);
    GPIO_ResetBits(uVar1,0x8000);
    param_1 = (param_1 & 0x7f) << 1;
    uVar2 = uVar2 + 1;
  } while (uVar2 < 8);
  GPIO_SetBits(uVar1,0x4000);
  return;
}


// ===== ads_i2c_write @ 0800434c (size 46)

undefined4 ads_i2c_write(undefined1 *param_1,uint param_2)

{
  int iVar1;
  uint uVar2;
  
  uVar2 = 0;
  while( true ) {
    if (param_2 <= uVar2) {
      ads_i2c_stop();
      return 1;
    }
    ads_i2c_write_byte(*param_1);
    iVar1 = ads_i2c_wait_ack();
    if (iVar1 == 0) break;
    uVar2 = uVar2 + 1;
    param_1 = param_1 + 1;
  }
  ads_i2c_stop();
  return 0;
}


// ===== ads_i2c_gpio_init @ 08004398 (size 32)

void ads_i2c_gpio_init(void)

{
  undefined4 local_8;
  
  local_8 = 0x1003c000;
  GPIO_Init(0x40011000/*GPIOC*/,&local_8);
  return;
}


// ===== serial_check @ 080043bc (size 238)

/* Checksum check, result unused */

undefined4 serial_check(uint param_1,int param_2)

{
  uint uVar1;
  uint uVar2;
  uint uVar3;
  uint uVar4;
  uint uVar5;
  
  uVar4 = (param_1 - 0xf4240 * (param_1 / 0xf4240)) / 0x186a0 & 0xff;
  uVar5 = (param_1 - 0x186a0 * (param_1 / 0x186a0)) / 10000 & 0xff;
  uVar1 = (param_1 % 10000) / 1000;
  uVar2 = (param_1 % 1000) / 100;
  uVar3 = (param_1 % 100) / 10;
  if (((param_1 % 10) * 7 + uVar4) % 10 +
      ((uVar5 + uVar4 * 2) % 10) * 0x186a0 + ((uVar5 * 3 + uVar1) % 10) * 10000 +
      ((uVar2 + uVar1 * 4) % 10) * 1000 + ((uVar2 * 5 + uVar3) % 10) * 100 +
      ((param_1 % 10 + uVar3 * 6) % 10) * 10 == param_2) {
    return 1;
  }
  return 0;
}


// ===== delay_ms @ 080044b4 (size 22)

void delay_ms(int param_1)

{
  for (; param_1 != 0; param_1 = param_1 + -1) {
    delay_us(1000);
  }
  return;
}


// ===== delay_us @ 080044cc (size 52)

void delay_us(int param_1)

{
  uint uVar1;
  
  DAT_e000e014 = param_1 * (*0x20000000 / 0xf4240);
  DAT_e000e018 = 0;
  uVar1 = SysTick;
  SysTick = uVar1 | 1;
  do {
    uVar1 = SysTick;
    if ((uVar1 & 1) == 0) break;
  } while (-1 < (int)(uVar1 << 0xf));
  uVar1 = SysTick;
  SysTick = uVar1 & 0xfffffffe;
  DAT_e000e018 = 0;
  return;
}


// ===== lcd_cursor_glyph @ 08004508 (size 66)

void lcd_cursor_glyph(undefined4 param_1,undefined4 param_2,int param_3)

{
  uint uVar1;
  int iVar2;
  
  uVar1 = 0;
  iVar2 = 0x20000241 + param_3 * 8;
  do {
    lcd_write(0,uVar1 + 0x40 & 0xff);
    lcd_write(1,*(undefined1 *)(iVar2 + uVar1 + -0x180));
    uVar1 = uVar1 + 1 & 0xff;
  } while (uVar1 < 8);
  lcd_putc_at(param_1,param_2,0);
  return;
}


// ===== ui_draw_info_page @ 08004550 (size 2282)

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
/* CONSUM mAh/Wh, VIN, time, TEMP, POWER, RESIS */

void ui_draw_info_page(int param_1,undefined4 param_2,uint param_3,uint param_4,uint param_5,
                      uint param_6,uint param_7,uint param_8,uint param_9)

{
  char cVar1;
  char *pcVar2;
  undefined4 uVar3;
  uint uVar4;
  uint uVar5;
  uint uVar6;
  uint uVar7;
  uint uVar8;
  uint uVar9;
  uint uVar10;
  
  pcVar2 = _DAT_08004954;
  cVar1 = *_DAT_08004954;
  *_DAT_08004954 = cVar1 + '\x01';
  if ((char)(cVar1 + '\x01') == '\x02') {
    *pcVar2 = '\0';
  }
  if (param_1 == 2) {
    lcd_write(0,0x88);
  }
  switch(param_2) {
  case 0:
    if (param_1 == 2) {
      lcd_clear_line(0);
      lcd_puts_at(0,0,"CONSUM:");
      lcd_write(0,0x88);
    }
    uVar4 = param_3 / 1000;
    if (0x1869f < param_3 / 1000) {
      uVar4 = 0x1869f;
    }
    uVar9 = uVar4 / 10000 & 0xff;
    uVar5 = (uVar4 % 10000) / 1000;
    uVar6 = (uVar4 % 1000) / 100;
    uVar7 = (uVar4 % 100) / 10;
    if (param_1 == 2) {
      lcd_write(1,uVar9 + 0x30 & 0xff);
      lcd_write(1,uVar5 + 0x30);
      lcd_write(1,uVar6 + 0x30);
      lcd_write(1,uVar7 + 0x30);
      lcd_write(1,uVar4 % 10 + 0x30);
      lcd_write(1,0x6d);
      lcd_write(1,0x41);
      lcd_write(1,0x48);
    }
    oled_draw_char16(0,0,uVar9);
    oled_draw_char16(8,0,uVar5);
    oled_draw_char16(0x10,0,uVar6);
    oled_draw_char16(0x18,0,uVar7);
    oled_draw_char16(0x20,0,uVar4 % 10);
    oled_draw_char16(0x28,0,0x11);
    oled_draw_char16(0x30,0,10);
    uVar4 = 0xe;
    break;
  case 1:
    if (param_1 == 2) {
      lcd_clear_line(0);
      lcd_puts_at(0,0,"CONSUM:");
      lcd_write(0,0x88);
    }
    uVar4 = param_4 / 1000;
    if (0xf423f < param_4 / 1000) {
      uVar4 = 0xf423f;
    }
    uVar6 = 0x1869f + 1;
    uVar10 = uVar4 / uVar6 & 0xff;
    uVar8 = (uVar4 - uVar6 * (uVar4 / uVar6)) / 10000 & 0xff;
    uVar7 = (uVar4 % 10000) / 1000;
    uVar9 = (uVar4 % 1000) / 100;
    uVar5 = (uVar4 % 100) / 10;
    if (uVar4 < uVar6) {
      if (param_1 == 2) {
        lcd_write(1,uVar8 + 0x30 & 0xff);
        lcd_write(1,uVar7 + 0x30);
        lcd_write(1,uVar9 + 0x30);
        lcd_write(1,0x2e);
        lcd_write(1,uVar5 + 0x30);
        lcd_write(1,uVar4 % 10 + 0x30);
      }
      oled_draw_char16(0,0,uVar8);
      oled_draw_char16(8,0,uVar7);
      oled_draw_char16(0x10,0,0x15);
      oled_draw_char16(0x18,0,uVar9);
      oled_draw_char16(0x20,0,uVar5);
      uVar5 = uVar4 % 10;
    }
    else {
      if (param_1 == 2) {
        lcd_write(1,uVar10 + 0x30 & 0xff);
        lcd_write(1,uVar8 + 0x30 & 0xff);
        lcd_write(1,uVar7 + 0x30);
        lcd_write(1,0x2e);
        lcd_write(1,uVar9 + 0x30);
        lcd_write(1,uVar5 + 0x30);
      }
      oled_draw_char16(0,0,uVar10);
      oled_draw_char16(8,0,uVar8);
      oled_draw_char16(0x10,0,uVar7);
      oled_draw_char16(0x18,0,0x15);
      oled_draw_char16(0x20,0,uVar9);
    }
    oled_draw_char16(0x28,0,uVar5);
    oled_draw_char16(0x30,0,0x13);
    oled_draw_char16(0x38,0,0xe);
    if (param_1 != 2) {
      return;
    }
    lcd_write(1,0x57);
    lcd_write(1,0x48);
    return;
  case 2:
    if (param_1 == 2) {
      lcd_clear_line(0);
      lcd_puts_at(0,0,&0x3a4e4956);
      lcd_write(0,0x88);
    }
    uVar4 = param_5 / 10000 & 0xff;
    uVar5 = (param_5 % 10000) / 1000;
    uVar6 = (param_5 % 1000) / 100;
    uVar7 = (param_5 % 100) / 10;
    if (param_1 == 2) {
      lcd_write(1,uVar4 + 0x30 & 0xff);
      lcd_write(1,uVar5 + 0x30);
      lcd_write(1,0x2e);
      lcd_write(1,uVar6 + 0x30);
      lcd_write(1,uVar7 + 0x30);
      lcd_write(1,param_5 % 10 + 0x30);
      lcd_write(1,0x20);
      lcd_write(1,0x56);
    }
    oled_draw_char16(0,0,uVar4);
    oled_draw_char16(8,0,uVar5);
    oled_draw_char16(0x10,0,0x15);
    oled_draw_char16(0x18,0,uVar6);
    oled_draw_char16(0x20,0,uVar7);
    oled_draw_char16(0x28,0,param_5 % 10);
    oled_clear_cols(0x30,0,8);
    oled_clear_cols(0x30,1,8);
    uVar4 = 0x12;
    break;
  case 3:
    if (param_1 == 2) {
      lcd_clear_line(0);
      lcd_puts_at(0,0,"time:");
      lcd_write(0,0x88);
    }
    uVar4 = 0x57e3f;
    if (param_6 < 0x57e3f) {
      uVar4 = param_6;
    }
    uVar8 = uVar4 / 36000 & 0xff;
    uVar6 = (uVar4 / 0xe10) % 10;
    uVar5 = (uVar4 % 0xe10) / 600;
    uVar9 = ((uVar4 % 0xe10) / 0x3c) % 10;
    uVar7 = (uVar4 % 0x3c) / 10;
    uVar4 = (uVar4 % 0x3c) % 10;
    if (param_1 == 2) {
      lcd_write(1,uVar8 + 0x30 & 0xff);
      lcd_write(1,uVar6 + 0x30);
      pcVar2 = 0x20000238;
      if (*0x20000238 == '\0') {
        uVar3 = 0x3a;
      }
      else {
        uVar3 = 0x20;
      }
      lcd_write(1,uVar3);
      lcd_write(1,uVar5 + 0x30);
      lcd_write(1,uVar9 + 0x30);
      if (*pcVar2 == '\0') {
        uVar3 = 0x3a;
      }
      else {
        uVar3 = 0x20;
      }
      lcd_write(1,uVar3);
      lcd_write(1,uVar7 + 0x30);
      lcd_write(1,uVar4 + 0x30);
    }
    oled_draw_char16(0,0,uVar8);
    oled_draw_char16(8,0,uVar6);
    pcVar2 = 0x20000238;
    if (*0x20000238 == '\0') {
      oled_draw_char16(0x10,0,0x14);
    }
    else {
      oled_clear_cols(0x10,0,8);
      oled_clear_cols(0x10,1,8);
    }
    oled_draw_char16(0x18,0,uVar5);
    oled_draw_char16(0x20,0,uVar9);
    if (*pcVar2 == '\0') {
      oled_draw_char16(0x28,0,0x14);
    }
    else {
      oled_clear_cols(0x28,0,8);
      oled_clear_cols(0x28,1,8);
    }
    oled_draw_char16(0x30,0,uVar7);
    break;
  case 4:
    if (param_1 == 2) {
      lcd_clear_line(0);
      lcd_puts_at(0,0,"TEMP:");
      lcd_write(0,0x8a);
    }
    uVar4 = (param_7 % 1000) / 100;
    uVar5 = (param_7 % 100) / 10;
    if (param_1 == 2) {
      if (9000 < param_7) {
        lcd_write(1,0x2d);
      }
      lcd_write(1,uVar4 + 0x30);
      lcd_write(1,uVar5 + 0x30);
      lcd_write(1,0x2e);
      lcd_write(1,param_7 % 10 + 0x30);
      lcd_degree_glyph(0,0xe);
      lcd_putc_at(0,0xf,0x43);
    }
    oled_clear_cols(0,0,8);
    oled_clear_cols(0,1,8);
    oled_clear_cols(8,0);
    oled_clear_cols(8,1);
    oled_draw_char16(0x10,0,uVar4);
    oled_draw_char16(0x18,0,uVar5);
    oled_draw_char16(0x20,0,0x15);
    oled_draw_char16(0x28,0,param_7 % 10);
    oled_clear_cols(0x30,0,8);
    oled_clear_cols(0x30,1,8);
    uVar4 = 0xb;
    break;
  case 5:
    if (param_1 == 2) {
      lcd_clear_line(0);
      lcd_puts_at(0,0,"POWER:");
      lcd_write(0,0x88);
    }
    uVar4 = param_8 / 0x186a0 & 0xff;
    uVar5 = (param_8 - 0x186a0 * (param_8 / 0x186a0)) / 10000 & 0xff;
    uVar6 = (param_8 % 10000) / 1000;
    uVar7 = (param_8 % 1000) / 100;
    uVar9 = (param_8 % 100) / 10;
    if (param_1 == 2) {
      lcd_write(1,uVar4 + 0x30 & 0xff);
      lcd_write(1,uVar5 + 0x30 & 0xff);
      lcd_write(1,uVar6 + 0x30);
      lcd_write(1,uVar7 + 0x30);
      lcd_write(1,0x2e);
      lcd_write(1,uVar9 + 0x30);
      lcd_write(1,param_8 % 10 + 0x30);
      lcd_write(1,0x57);
    }
    oled_draw_char16(0,0,uVar4);
    oled_draw_char16(8,0,uVar5);
    oled_draw_char16(0x10,0,uVar6);
    oled_draw_char16(0x18,0,0x15);
    oled_draw_char16(0x20,0,uVar7);
    oled_draw_char16(0x28,0,uVar9);
    oled_draw_char16(0x30,0,param_8 % 10);
    uVar4 = 0x13;
    break;
  case 6:
    if (param_1 == 2) {
      lcd_clear_line(0);
      lcd_puts_at(0,0,"RESIS:");
      lcd_write(0,0x88);
    }
    uVar4 = param_9 / 0x186a0 & 0xff;
    uVar9 = (param_9 - 0x186a0 * (param_9 / 0x186a0)) / 10000 & 0xff;
    uVar5 = (param_9 % 10000) / 1000;
    uVar6 = (param_9 % 1000) / 100;
    uVar7 = (param_9 % 100) / 10;
    if (param_1 == 2) {
      lcd_write(1,uVar4 + 0x30 & 0xff);
      lcd_write(1,uVar9 + 0x30 & 0xff);
      lcd_write(1,uVar5 + 0x30);
      lcd_write(1,uVar6 + 0x30);
      lcd_write(1,0x2e);
      lcd_write(1,uVar7 + 0x30);
      lcd_write(1,param_9 % 10 + 0x30);
      lcd_write(1,0x52);
    }
    oled_draw_char16(0,0,uVar4);
    oled_draw_char16(8,0,uVar9);
    oled_draw_char16(0x10,0,uVar5);
    oled_draw_char16(0x18,0,uVar6);
    oled_draw_char16(0x20,0,0x15);
    oled_draw_char16(0x28,0,uVar7);
    oled_draw_char16(0x30,0,param_9 % 10);
    uVar4 = 0xf;
    break;
  default:
    return;
  }
  oled_draw_char16(0x38,0,uVar4);
  return;
}


// ===== lcd_degree_glyph @ 08004e8c (size 54)

void lcd_degree_glyph(undefined4 param_1,undefined4 param_2)

{
  int iVar1;
  uint uVar2;
  
  iVar1 = 0x20000239;
  uVar2 = 0;
  do {
    lcd_write(0,uVar2 + 0x40 & 0xff);
    lcd_write(1,*(undefined1 *)(iVar1 + uVar2));
    uVar2 = uVar2 + 1 & 0xff;
  } while (uVar2 < 8);
  lcd_putc_at(param_1,param_2,0);
  return;
}


// ===== ui_draw_setpoints @ 08004ec8 (size 776)

/* Set V / Set I line with cursor */

void ui_draw_setpoints(int param_1,uint param_2,uint param_3,int param_4)

{
  uint uVar1;
  uint uVar2;
  uint uVar3;
  undefined4 uVar4;
  uint uVar5;
  
  uVar1 = param_2 / 10000 & 0xff;
  uVar2 = (param_2 % 10000) / 1000;
  uVar3 = (param_2 % 1000) / 100;
  uVar5 = (param_2 % 100) / 10;
  param_2 = param_2 % 10;
  lcd_write(0,0x80);
  lcd_write(1);
  lcd_write(1);
  lcd_write(1);
  lcd_write(1,0x2e);
  lcd_write(1);
  lcd_write(1);
  lcd_write(1,0x76);
  oled_draw_char16(0,6,0x1a);
  oled_draw_char16(8,6,0x1b);
  oled_draw_char16(0x10,6,0x1c);
  oled_draw_char16(0x18,6,0x14);
  if (param_1 == 0) {
    oled_draw_char16(0x20,6,uVar1);
    oled_draw_char16(0x28,6,uVar2);
    oled_draw_char16(0x30,6,uVar3);
    oled_draw_char16(0x38,6,0x15);
    oled_draw_char16(0x40,6,uVar5);
    oled_draw_char16(0x48,6,param_2);
    oled_draw_char16(0x50,6,0x12);
    if (param_4 == 1) {
      lcd_cursor_glyph(0,0,uVar1 + 0x30 & 0xff);
      uVar4 = 0x20;
    }
    else if (param_4 == 2) {
      lcd_cursor_glyph(0,1,uVar2 + 0x30);
      uVar4 = 0x28;
      uVar1 = uVar2;
    }
    else if (param_4 == 3) {
      lcd_cursor_glyph(0,2,uVar3 + 0x30);
      uVar4 = 0x30;
      uVar1 = uVar3;
    }
    else if (param_4 == 4) {
      lcd_cursor_glyph(0,4,uVar5 + 0x30);
      uVar4 = 0x40;
      uVar1 = uVar5;
    }
    else {
      if (param_4 != 5) goto LAB_08005060;
      lcd_cursor_glyph(0,5,param_2 + 0x30);
      uVar4 = 0x48;
      uVar1 = param_2;
    }
    oled_draw_char16_cursor(uVar4,6,uVar1);
  }
LAB_08005060:
  uVar1 = param_3 / 10000 & 0xff;
  uVar2 = (param_3 % 10000) / 1000;
  uVar3 = (param_3 % 1000) / 100;
  uVar5 = (param_3 % 100) / 10;
  param_3 = param_3 % 10;
  lcd_write(0,0x88);
  lcd_write(1);
  lcd_write(1);
  lcd_write(1,0x2e);
  lcd_write(1);
  lcd_write(1);
  lcd_write(1);
  lcd_write(1,0x41);
  if (param_1 != 1) {
    return;
  }
  oled_draw_char16(0x20,6,uVar1);
  oled_draw_char16(0x28,6,uVar2);
  oled_draw_char16(0x30,6,0x15);
  oled_draw_char16(0x38,6,uVar3);
  oled_draw_char16(0x40,6,uVar5);
  oled_draw_char16(0x48,6,param_3);
  oled_draw_char16(0x50,6,10);
  if (param_4 == 1) {
    lcd_cursor_glyph(0,8,uVar1 + 0x30 & 0xff);
    uVar4 = 0x20;
  }
  else if (param_4 == 2) {
    lcd_cursor_glyph(0,9,uVar2 + 0x30);
    uVar4 = 0x28;
    uVar1 = uVar2;
  }
  else if (param_4 == 3) {
    lcd_cursor_glyph(0,0xb,uVar3 + 0x30);
    uVar4 = 0x38;
    uVar1 = uVar3;
  }
  else if (param_4 == 4) {
    lcd_cursor_glyph(0,0xc,uVar5 + 0x30);
    uVar4 = 0x40;
    uVar1 = uVar5;
  }
  else {
    if (param_4 != 5) {
      return;
    }
    lcd_cursor_glyph(0,0xd,param_3 + 0x30);
    uVar4 = 0x48;
    uVar1 = param_3;
  }
  oled_draw_char16_cursor(uVar4,6,uVar1);
  return;
}


// ===== protection_check @ 080051d0 (size 202)

/* OVP/OCP/OPP/OTP; trips load off */

void protection_check(void)

{
  ushort uVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  undefined1 *puVar5;
  undefined4 uVar6;
  uint uVar7;
  
  iVar3 = 0x2000017a;
  iVar2 = 0x20000398;
  if ((*0x20000032 != 0x6e) && (*0x20000032 != 0x6f)) {
    uVar1 = *(ushort *)(0x20000398 + 0xc);
    if (uVar1 < 0x157d) {
      *(undefined2 *)(0x2000017a + 0x1c) = 600;
    }
    else {
      *(undefined2 *)(0x2000017a + 0x1c) = 3000;
    }
    uVar6 = 0x40010c00/*GPIOB*/;
    puVar5 = 0x20000221;
    iVar4 = 0x20000654;
    if ((((*(ushort *)(iVar2 + 10) + 300 < *(uint *)(0x20000654 + 8)) ||
         ((uint)uVar1 + (uint)*(ushort *)(iVar3 + 0x1c) < *(uint *)(0x20000654 + 0xc))) ||
        (uVar7 = (uint)*(ushort *)(iVar2 + 0xe), uVar7 + uVar7 / 10 < *(uint *)(0x20000654 + 0x28)
        )) || ((0x55 < *(uint *)(0x20000654 + 0x40) || (0x55 < *(uint *)(0x20000654 + 0x44)))))
    {
      uVar1 = *(ushort *)(iVar3 + 0x16);
      *(ushort *)(iVar3 + 0x16) = uVar1 + 1;
      if (0x1d < uVar1) {
        *(undefined2 *)(iVar3 + 0x16) = 0;
        *puVar5 = 1;
        *(undefined2 *)(iVar3 + 0x1a) = 1;
        *(undefined4 *)(iVar4 + 4) = 0;
      }
      uVar1 = *(ushort *)(iVar3 + 0x18);
      *(ushort *)(iVar3 + 0x18) = uVar1 + 1;
      if (99 < uVar1) {
        *(undefined2 *)(iVar3 + 0x18) = 0;
        GPIO_SetBits(uVar6);
        return;
      }
    }
    else {
      *(undefined2 *)(iVar3 + 0x16) = 0;
      *(undefined2 *)(iVar3 + 0x18) = 0;
      if (*(short *)(iVar3 + 0x1a) != 0) {
        uVar1 = *(ushort *)(iVar3 + 0x14);
        *(ushort *)(iVar3 + 0x14) = uVar1 + 1;
        if (2999 < uVar1) {
          *(undefined2 *)(iVar3 + 0x14) = 0;
          *puVar5 = 0;
          *(undefined2 *)(iVar3 + 0x1a) = 0;
          GPIO_ResetBits(uVar6,0x800);
          return;
        }
      }
    }
  }
  return;
}


// ===== mode_control @ 080052b4 (size 2756)

/* CV/CC/CP/CR + memory presets */

void mode_control(void)

{
  short sVar1;
  short sVar2;
  short sVar3;
  short sVar4;
  short sVar5;
  short sVar6;
  short sVar7;
  short sVar8;
  short sVar9;
  ushort uVar10;
  short sVar11;
  short sVar12;
  short sVar13;
  short *psVar14;
  int iVar15;
  int iVar16;
  int iVar17;
  undefined2 uVar18;
  int iVar19;
  uint uVar20;
  uint uVar21;
  uint uVar22;
  ushort uVar23;
  uint uVar24;
  int iVar25;
  int iVar26;
  uint uVar27;
  int iVar28;
  uint uVar29;
  uint uVar30;
  ushort *puVar31;
  bool bVar32;
  bool bVar33;
  bool bVar34;
  
  iVar17 = 0x20000398;
  iVar16 = 0x20000398;
  iVar15 = 0x20000654;
  psVar14 = 0x2000017a;
  iVar19 = 0x20000398;
  sVar11 = *(short *)(0x20000398 + 0x20);
  sVar12 = *0x2000017a;
  sVar13 = 0x2000017a[1];
  sVar1 = 0x2000017a[2];
  sVar2 = 0x2000017a[3];
  sVar3 = 0x2000017a[4];
  sVar4 = 0x2000017a[5];
  sVar5 = 0x2000017a[6];
  sVar6 = 0x2000017a[7];
  sVar7 = 0x2000017a[8];
  sVar8 = 0x2000017a[9];
  if (sVar11 == 0) {
    *(undefined4 *)(0x20000654 + 0x14) = 0;
  }
  else {
    if (sVar11 != 1) {
      sVar9 = 0x2000017a[0xf];
      uVar23 = 0x2000017a[0x10];
      uVar10 = 0x2000017a[0x11];
      uVar30 = *(ushort *)(0x20000398 + 0x2c) + 0x1e;
      uVar20 = (uint)*(ushort *)(0x20000398 + 0xc);
      uVar22 = *(uint *)(0x20000654 + 8);
      uVar21 = *(uint *)(0x20000654 + 0xc);
      uVar29 = (uint)*(ushort *)(0x20000398 + 0x2e);
      iVar25 = *(int *)(0x20000654 + 4);
      if (sVar11 == 2) {
        *(undefined4 *)(0x20000654 + 0x1c) = 0;
        if (((((((sVar12 != 0) && (sVar13 != 0)) && (sVar1 != 0)) && ((sVar2 != 0 && (sVar3 != 0))))
             && ((sVar4 != 0 && ((sVar5 != 0 && (sVar6 != 0)))))) && (sVar7 != 0)) && (sVar8 != 0))
        {
          if (iVar25 == 0) {
            return;
          }
          uVar27 = (uint)*0x200001e4 * 1000 * *(int *)(0x20000654 + 0x18);
          if ((sVar9 != 0) && (uVar30 < uVar22)) {
            psVar14[0x10] = 0;
            psVar14[0x11] = 0;
            uVar27 = uVar27 / uVar22;
            *(undefined4 *)(iVar15 + 0x10) = 0;
            goto LAB_08005544;
          }
          psVar14[0x10] = uVar23 + 1;
          if (uVar23 < 10) {
            return;
          }
          psVar14[0x10] = 0;
          if (uVar20 / 1000 + uVar29 < uVar21) {
            psVar14[0xf] = 0;
            uVar27 = uVar27 / uVar21;
            *(undefined4 *)(iVar15 + 0x14) = 0;
            goto LAB_0800558a;
          }
          psVar14[0x11] = uVar10 + 1;
          if (uVar10 < 10) {
            return;
          }
          psVar14[0x11] = 0;
          goto joined_r0x080054ec;
        }
        goto LAB_080053fe;
      }
      iVar28 = *(int *)(0x20000654 + 8);
      uVar27 = *(uint *)(0x20000654 + 0x1c);
      iVar26 = *(int *)(0x20000654 + 0xc);
      if (sVar11 == 3) {
        *(undefined4 *)(0x20000654 + 0x18) = 0;
        if (((((sVar12 == 0) || (sVar13 == 0)) || (sVar1 == 0)) || ((sVar2 == 0 || (sVar3 == 0))))
           || (((sVar4 == 0 || ((sVar5 == 0 || (sVar6 == 0)))) || ((sVar7 == 0 || (sVar8 == 0))))))
        goto LAB_080053fe;
        if (iVar25 == 0) {
          return;
        }
        if ((sVar9 != 0) && (uVar30 < uVar22)) {
          psVar14[0x10] = 0;
          psVar14[0x11] = 0;
          *(undefined4 *)(iVar15 + 0x10) = 0;
LAB_080055d4:
          uVar27 = (uint)(iVar28 * 100) / uVar27;
LAB_08005544:
          *(uint *)(iVar15 + 0x14) = uVar27;
          return;
        }
LAB_08005548:
        psVar14[0x10] = uVar23 + 1;
        if (uVar23 < 10) {
          return;
        }
        psVar14[0x10] = 0;
        if (uVar20 / 1000 + uVar29 < uVar21) {
          psVar14[0xf] = 0;
          uVar27 = (uVar27 * iVar26) / 100;
          *(undefined4 *)(iVar15 + 0x14) = 0;
LAB_0800558a:
          *(uint *)(iVar15 + 0x10) = uVar27;
          return;
        }
        psVar14[0x11] = uVar10 + 1;
        if (uVar10 < 10) {
          return;
        }
        psVar14[0x11] = 0;
joined_r0x080054ec:
        if ((uVar30 <= uVar22) && (uVar20 / 1000 + uVar29 <= uVar21)) {
          return;
        }
        psVar14[0xf] = 1;
        return;
      }
      if (sVar11 == 4) goto LAB_080053cc;
      if (sVar11 == 5) {
        *(undefined4 *)(0x20000654 + 0x18) = 0;
        if (((((sVar12 == 0) || (sVar13 == 0)) || (sVar1 == 0)) ||
            (((sVar2 == 0 || (sVar3 == 0)) ||
             (((sVar4 == 0 || ((sVar5 == 0 || (sVar6 == 0)))) || (sVar7 == 0)))))) || (sVar8 == 0))
        goto LAB_080053fe;
        if (iVar25 == 0) {
          return;
        }
        if ((sVar9 != 0) && (uVar30 < uVar22)) {
          psVar14[0x10] = 0;
          psVar14[0x11] = 0;
          goto LAB_080055d4;
        }
        goto LAB_08005548;
      }
      uVar30 = (uint)*0x20000042;
      uVar29 = (uint)*0x20000044;
      uVar22 = (uint)*0x20000046;
      uVar21 = (uint)*0x20000048;
      sVar9 = *(short *)(0x20000398 + 0x22);
      uVar20 = (uint)*(ushort *)(0x20000398 + 10);
      if (sVar11 == 0xb) {
        if (sVar12 != 0) {
          *0x2000017a = 0;
          uVar27 = (uint)*(ushort *)(iVar19 + 0x1f2);
          uVar23 = *(ushort *)(iVar19 + 0x256);
          if (sVar9 == 0) {
            *(uint *)(iVar15 + 0x10) = uVar27;
          }
          else if (sVar9 == 1) {
            *(uint *)(iVar15 + 0x14) = uVar27;
          }
          else if (sVar9 == 2) {
            *(uint *)(iVar15 + 0x18) = uVar27;
          }
          else {
            if (sVar9 != 3) goto LAB_0800561e;
            *(uint *)(iVar15 + 0x1c) = uVar27;
          }
          *(uint *)(iVar15 + 4) = (uint)uVar23;
        }
LAB_0800561e:
        psVar14[1] = 1;
        psVar14[2] = 1;
        psVar14[3] = 1;
        psVar14[4] = 1;
        iVar19 = 0x20000398;
        uVar27 = *(uint *)(iVar15 + 0x10);
        if (uVar27 != uVar20) {
          uVar23 = (ushort)uVar27;
          *(ushort *)(0x20000398 + 0x1f2) = uVar23;
          puVar31 = 0x20000042;
          *(undefined2 *)(iVar19 + 0x256) = *(undefined2 *)(iVar15 + 4);
          if (uVar27 != uVar30) goto LAB_08005650;
        }
        uVar30 = *(uint *)(iVar15 + 0x14);
        if (uVar30 != 0) {
          uVar23 = (ushort)uVar30;
          *(ushort *)(iVar19 + 0x1f2) = uVar23;
          puVar31 = 0x20000044;
          *(undefined2 *)(iVar19 + 0x256) = *(undefined2 *)(iVar15 + 4);
          if (uVar30 != uVar29) goto LAB_08005680;
        }
        uVar29 = *(uint *)(iVar15 + 0x18);
        if (uVar29 != 0) {
          *(short *)(iVar19 + 0x1f2) = (short)uVar29;
          *(undefined2 *)(iVar19 + 0x256) = *(undefined2 *)(iVar15 + 4);
          puVar31 = 0x20000046;
          if (uVar29 != uVar22) {
LAB_080057a0:
            uVar23 = (ushort)uVar29;
LAB_0800590e:
            *puVar31 = uVar23;
            *(uint *)(iVar15 + 0x10) = uVar20;
            *(undefined4 *)(iVar15 + 0x14) = 0;
            *(undefined4 *)(iVar15 + 0x1c) = 0;
            uVar18 = 2;
            goto LAB_080056e0;
          }
        }
        uVar22 = *(uint *)(iVar15 + 0x1c);
        if (uVar22 == 0) {
          return;
        }
        uVar18 = (undefined2)uVar22;
        *(undefined2 *)(iVar19 + 0x1f2) = uVar18;
        *(undefined2 *)(iVar19 + 0x256) = *(undefined2 *)(iVar15 + 4);
      }
      else {
        if (sVar11 != 0xc) {
          if (sVar11 == 0xd) {
            if (sVar1 != 0) {
              0x2000017a[2] = 0;
              uVar27 = (uint)*(ushort *)(iVar16 + 0x1f6);
              uVar23 = *(ushort *)(iVar16 + 0x25a);
              if (sVar9 == 0) {
                *(uint *)(iVar15 + 0x10) = uVar27;
              }
              else if (sVar9 == 1) {
                *(uint *)(iVar15 + 0x14) = uVar27;
              }
              else if (sVar9 == 2) {
                *(uint *)(iVar15 + 0x18) = uVar27;
              }
              else {
                if (sVar9 != 3) goto LAB_080057da;
                *(uint *)(iVar15 + 0x1c) = uVar27;
              }
              *(uint *)(iVar15 + 4) = (uint)uVar23;
            }
LAB_080057da:
            *psVar14 = 1;
            psVar14[1] = 1;
            psVar14[3] = 1;
            psVar14[4] = 1;
            uVar27 = *(uint *)(iVar15 + 0x10);
            if (uVar27 != uVar20) {
              uVar23 = (ushort)uVar27;
              *(ushort *)(iVar19 + 0x1f6) = uVar23;
              puVar31 = 0x20000042;
              *(undefined2 *)(iVar19 + 0x25a) = *(undefined2 *)(iVar15 + 4);
              if (uVar27 != uVar30) {
LAB_08005d46:
                *puVar31 = uVar23;
LAB_08005c80:
                *(undefined4 *)(iVar15 + 0x14) = 0;
                *(undefined4 *)(iVar15 + 0x18) = 0;
                *(undefined4 *)(iVar15 + 0x1c) = 0;
                *(undefined2 *)(iVar19 + 0x22) = 0;
                return;
              }
            }
            uVar30 = *(uint *)(iVar15 + 0x14);
            if (uVar30 != 0) {
              uVar23 = (ushort)uVar30;
              *(ushort *)(iVar19 + 0x1f6) = uVar23;
              puVar31 = 0x20000044;
              *(undefined2 *)(iVar19 + 0x25a) = *(undefined2 *)(iVar15 + 4);
              if (uVar30 != uVar29) goto LAB_08005824;
            }
            uVar29 = *(uint *)(iVar15 + 0x18);
            if (uVar29 == 0) {
LAB_08005854:
              uVar22 = *(uint *)(iVar15 + 0x1c);
              if (uVar22 == 0) {
                return;
              }
              uVar18 = (undefined2)uVar22;
              *(undefined2 *)(iVar19 + 0x1f6) = uVar18;
              *(undefined2 *)(iVar19 + 0x25a) = *(undefined2 *)(iVar15 + 4);
              goto LAB_08005c02;
            }
            uVar23 = (ushort)uVar29;
            *(ushort *)(iVar19 + 0x1f6) = uVar23;
            *(undefined2 *)(iVar19 + 0x25a) = *(undefined2 *)(iVar15 + 4);
            puVar31 = 0x20000046;
            if (uVar29 == uVar22) goto LAB_08005854;
            goto LAB_08005d88;
          }
          uVar27 = (uint)*(ushort *)(0x20000398 + 0x1fa);
          uVar23 = *(ushort *)(0x20000398 + 0x25e);
          if (sVar11 == 0xe) {
            if (sVar2 != 0) {
              0x2000017a[3] = 0;
              uVar24 = (uint)*(ushort *)(iVar16 + 0x1f8);
              uVar10 = *(ushort *)(iVar16 + 0x25c);
              if (sVar9 == 0) {
                *(uint *)(iVar15 + 0x10) = uVar24;
              }
              else if (sVar9 == 1) {
                *(uint *)(iVar15 + 0x14) = uVar24;
              }
              else {
                if (sVar9 != 2) {
                  if (sVar9 == 3) {
                    *(uint *)(iVar15 + 4) = (uint)uVar23;
                    *(uint *)(iVar15 + 0x1c) = uVar27;
                  }
                  goto LAB_080058a2;
                }
                *(uint *)(iVar15 + 0x18) = uVar24;
              }
              *(uint *)(iVar15 + 4) = (uint)uVar10;
            }
LAB_080058a2:
            *psVar14 = 1;
            psVar14[1] = 1;
            psVar14[2] = 1;
            psVar14[4] = 1;
            iVar19 = 0x20000398;
            uVar27 = *(uint *)(iVar15 + 0x10);
            if (uVar27 != uVar20) {
              uVar23 = (ushort)uVar27;
              *(ushort *)(0x20000398 + 0x1f8) = uVar23;
              puVar31 = 0x20000042;
              *(undefined2 *)(iVar19 + 0x25c) = *(undefined2 *)(iVar15 + 4);
              if (uVar27 != uVar30) {
LAB_08005650:
                *puVar31 = uVar23;
                *(undefined4 *)(iVar15 + 0x14) = 0;
                *(undefined4 *)(iVar15 + 0x18) = 0;
                *(undefined4 *)(iVar15 + 0x1c) = 0;
                *(undefined2 *)(iVar19 + 0x22) = 0;
                return;
              }
            }
            uVar30 = *(uint *)(iVar15 + 0x14);
            if (uVar30 != 0) {
              uVar23 = (ushort)uVar30;
              *(ushort *)(iVar19 + 0x1f8) = uVar23;
              puVar31 = 0x20000044;
              *(undefined2 *)(iVar19 + 0x25c) = *(undefined2 *)(iVar15 + 4);
              if (uVar30 != uVar29) {
LAB_08005680:
                *puVar31 = uVar23;
                *(undefined4 *)(iVar15 + 0x18) = 0;
                *(uint *)(iVar15 + 0x10) = uVar20;
                *(undefined4 *)(iVar15 + 0x1c) = 0;
                *(undefined2 *)(iVar19 + 0x22) = 1;
                return;
              }
            }
            uVar29 = *(uint *)(iVar15 + 0x18);
            if (uVar29 != 0) {
              uVar23 = (ushort)uVar29;
              *(ushort *)(iVar19 + 0x1f8) = uVar23;
              *(undefined2 *)(iVar19 + 0x25c) = *(undefined2 *)(iVar15 + 4);
              puVar31 = 0x20000046;
              if (uVar29 != uVar22) goto LAB_0800590e;
            }
            uVar22 = *(uint *)(iVar15 + 0x1c);
            bVar34 = uVar22 == 0;
LAB_08005920:
            if (bVar34) {
              return;
            }
            uVar18 = (undefined2)uVar22;
            *(undefined2 *)(iVar19 + 0x1f8) = uVar18;
            *(undefined2 *)(iVar19 + 0x25c) = *(undefined2 *)(iVar15 + 4);
            goto LAB_0800578e;
          }
          if (sVar11 == 0xf) {
            if (sVar3 != 0) {
              0x2000017a[4] = 0;
              if (sVar9 == 0) {
                *(uint *)(iVar15 + 0x10) = uVar27;
              }
              else if (sVar9 == 1) {
                *(uint *)(iVar15 + 0x14) = uVar27;
              }
              else if (sVar9 == 2) {
                *(uint *)(iVar15 + 0x18) = uVar27;
              }
              else {
                if (sVar9 != 3) goto LAB_0800595c;
                *(uint *)(iVar15 + 0x1c) = uVar27;
              }
              *(uint *)(iVar15 + 4) = (uint)uVar23;
            }
LAB_0800595c:
            *psVar14 = 1;
            psVar14[1] = 1;
            psVar14[2] = 1;
            psVar14[3] = 1;
            uVar27 = *(uint *)(iVar15 + 0x10);
            if (uVar27 != uVar20) {
              uVar23 = (ushort)uVar27;
              *(ushort *)(iVar19 + 0x1fa) = uVar23;
              puVar31 = 0x20000042;
              *(undefined2 *)(iVar19 + 0x25e) = *(undefined2 *)(iVar15 + 4);
              if (uVar27 != uVar30) goto LAB_08005d46;
            }
            uVar30 = *(uint *)(iVar15 + 0x14);
            if (uVar30 != 0) {
              uVar23 = (ushort)uVar30;
              *(ushort *)(iVar19 + 0x1fa) = uVar23;
              puVar31 = 0x20000044;
              *(undefined2 *)(iVar19 + 0x25e) = *(undefined2 *)(iVar15 + 4);
              bVar34 = uVar30 == uVar29;
LAB_080059a4:
              if (!bVar34) {
LAB_08005824:
                *puVar31 = uVar23;
                *(undefined4 *)(iVar15 + 0x18) = 0;
                *(uint *)(iVar15 + 0x10) = uVar20;
                *(undefined4 *)(iVar15 + 0x1c) = 0;
                *(undefined2 *)(iVar19 + 0x22) = 1;
                return;
              }
            }
            uVar29 = *(uint *)(iVar15 + 0x18);
            if (uVar29 == 0) {
LAB_080059c2:
              uVar22 = *(uint *)(iVar15 + 0x1c);
              bVar32 = uVar22 == 0;
              goto LAB_080059c6;
            }
            uVar23 = (ushort)uVar29;
            *(ushort *)(iVar19 + 0x1fa) = uVar23;
            *(undefined2 *)(iVar19 + 0x25e) = *(undefined2 *)(iVar15 + 4);
            puVar31 = 0x20000046;
            if (uVar29 == uVar22) goto LAB_080059c2;
          }
          else {
            if (sVar11 != 0x10) {
              if (sVar11 == 0x11) {
                if (sVar5 != 0) {
                  0x2000017a[6] = 0;
                  uVar27 = (uint)*(ushort *)(iVar16 + 0x1fe);
                  uVar23 = *(ushort *)(iVar16 + 0x262);
                  if (sVar9 == 0) {
                    *(uint *)(iVar15 + 0x10) = uVar27;
                  }
                  else if (sVar9 == 1) {
                    *(uint *)(iVar15 + 0x14) = uVar27;
                  }
                  else if (sVar9 == 2) {
                    *(uint *)(iVar15 + 0x18) = uVar27;
                  }
                  else {
                    if (sVar9 != 3) goto LAB_08005ac2;
                    *(uint *)(iVar15 + 0x1c) = uVar27;
                  }
                  *(uint *)(iVar15 + 4) = (uint)uVar23;
                }
LAB_08005ac2:
                *psVar14 = 1;
                psVar14[1] = 1;
                psVar14[2] = 1;
                psVar14[3] = 1;
                uVar27 = *(uint *)(iVar15 + 0x10);
                if (uVar27 != uVar20) {
                  uVar23 = (ushort)uVar27;
                  *(ushort *)(iVar19 + 0x1fe) = uVar23;
                  puVar31 = 0x20000042;
                  *(undefined2 *)(iVar19 + 0x262) = *(undefined2 *)(iVar15 + 4);
                  if (uVar27 != uVar30) goto LAB_08005d46;
                }
                uVar30 = *(uint *)(iVar15 + 0x14);
                if (uVar30 != 0) {
                  *(short *)(iVar19 + 0x1fe) = (short)uVar30;
                  puVar31 = 0x20000044;
                  *(undefined2 *)(iVar19 + 0x262) = *(undefined2 *)(iVar15 + 4);
                  bVar34 = uVar30 == uVar29;
LAB_08005b20:
                  uVar23 = (ushort)uVar30;
                  if (!bVar34) goto LAB_08005a56;
                }
                uVar29 = *(uint *)(iVar15 + 0x18);
                if (uVar29 != 0) {
                  uVar23 = (ushort)uVar29;
                  *(ushort *)(iVar19 + 0x1fe) = uVar23;
                  *(undefined2 *)(iVar19 + 0x262) = *(undefined2 *)(iVar15 + 4);
                  puVar31 = 0x20000046;
                  if (uVar29 != uVar22) goto LAB_08005d88;
                }
                uVar22 = *(uint *)(iVar15 + 0x1c);
                bVar34 = uVar22 == 0;
LAB_08005b40:
                do {
                  bVar33 = true;
                  if (bVar34) {
LAB_08005a76:
                    bVar32 = true;
                    if (bVar33) {
LAB_080059c6:
                      bVar34 = true;
                      if (bVar32) goto LAB_08005920;
                      uVar18 = (undefined2)uVar22;
                      *(undefined2 *)(iVar19 + 0x1fa) = uVar18;
                      *(undefined2 *)(iVar19 + 0x25e) = *(undefined2 *)(iVar15 + 4);
                    }
                    else {
                      uVar18 = (undefined2)uVar22;
                      *(undefined2 *)(iVar19 + 0x1fc) = uVar18;
                      *(undefined2 *)(iVar19 + 0x260) = *(undefined2 *)(iVar15 + 4);
                    }
                  }
                  else {
                    uVar18 = (undefined2)uVar22;
                    *(undefined2 *)(iVar19 + 0x1fe) = uVar18;
                    *(undefined2 *)(iVar19 + 0x262) = *(undefined2 *)(iVar15 + 4);
                  }
LAB_08005c02:
                  bVar34 = true;
                } while (uVar22 == uVar21);
                *0x20000048 = uVar18;
              }
              else {
                if (sVar11 == 0x12) {
                  if (sVar6 != 0) {
                    0x2000017a[7] = 0;
                    uVar27 = (uint)*(ushort *)(iVar17 + 0x200);
                    uVar23 = *(ushort *)(iVar17 + 0x264);
                    if (sVar9 == 0) {
                      *(uint *)(iVar15 + 0x10) = uVar27;
                    }
                    else if (sVar9 == 1) {
                      *(uint *)(iVar15 + 0x14) = uVar27;
                    }
                    else if (sVar9 == 2) {
                      *(uint *)(iVar15 + 0x18) = uVar27;
                    }
                    else {
                      if (sVar9 != 3) goto LAB_08005b88;
                      *(uint *)(iVar15 + 0x1c) = uVar27;
                    }
                    *(uint *)(iVar15 + 4) = (uint)uVar23;
                  }
LAB_08005b88:
                  *psVar14 = 1;
                  psVar14[1] = 1;
                  psVar14[2] = 1;
                  psVar14[3] = 1;
                  uVar27 = *(uint *)(iVar15 + 0x10);
                  if (uVar27 != uVar20) {
                    uVar23 = (ushort)uVar27;
                    *(ushort *)(iVar19 + 0x200) = uVar23;
                    puVar31 = 0x20000042;
                    *(undefined2 *)(iVar19 + 0x264) = *(undefined2 *)(iVar15 + 4);
                    if (uVar27 != uVar30) goto LAB_08005d46;
                  }
                  uVar30 = *(uint *)(iVar15 + 0x14);
                  if (uVar30 != 0) {
                    *(short *)(iVar19 + 0x200) = (short)uVar30;
                    puVar31 = 0x20000044;
                    *(undefined2 *)(iVar19 + 0x264) = *(undefined2 *)(iVar15 + 4);
                    bVar34 = uVar30 == uVar29;
                    if (!bVar34) goto LAB_08005b20;
                  }
                  uVar29 = *(uint *)(iVar15 + 0x18);
                  if (uVar29 != 0) {
                    uVar23 = (ushort)uVar29;
                    *(ushort *)(iVar19 + 0x200) = uVar23;
                    *(undefined2 *)(iVar19 + 0x264) = *(undefined2 *)(iVar15 + 4);
                    puVar31 = 0x20000046;
                    if (uVar29 != uVar22) goto LAB_08005d88;
                  }
                  uVar22 = *(uint *)(iVar15 + 0x1c);
                  bVar34 = true;
                  if (uVar22 != 0) {
                    uVar18 = (undefined2)uVar22;
                    *(undefined2 *)(iVar19 + 0x200) = uVar18;
                    *(undefined2 *)(iVar19 + 0x264) = *(undefined2 *)(iVar15 + 4);
                    goto LAB_08005c02;
                  }
                  goto LAB_08005b40;
                }
                if (sVar11 != 0x13) {
                  if (sVar11 != 0x14) {
                    return;
                  }
                  if (sVar8 != 0) {
                    0x2000017a[9] = 0;
                    uVar27 = (uint)*(ushort *)(iVar19 + 0x204);
                    uVar23 = *(ushort *)(iVar19 + 0x268);
                    if (sVar9 == 0) {
                      *(uint *)(iVar15 + 0x10) = uVar27;
                    }
                    else if (sVar9 == 1) {
                      *(uint *)(iVar15 + 0x14) = uVar27;
                    }
                    else if (sVar9 == 2) {
                      *(uint *)(iVar15 + 0x18) = uVar27;
                    }
                    else {
                      if (sVar9 != 3) goto LAB_08005d16;
                      *(uint *)(iVar15 + 0x1c) = uVar27;
                    }
                    *(uint *)(iVar15 + 4) = (uint)uVar23;
                  }
LAB_08005d16:
                  *psVar14 = 1;
                  psVar14[1] = 1;
                  psVar14[2] = 1;
                  psVar14[3] = 1;
                  uVar27 = *(uint *)(iVar15 + 0x10);
                  if (uVar27 != uVar20) {
                    uVar23 = (ushort)uVar27;
                    *(ushort *)(iVar19 + 0x204) = uVar23;
                    puVar31 = 0x20000042;
                    *(undefined2 *)(iVar19 + 0x268) = *(undefined2 *)(iVar15 + 4);
                    if (uVar27 != uVar30) goto LAB_08005d46;
                  }
                  uVar30 = *(uint *)(iVar15 + 0x14);
                  if (uVar30 != 0) {
                    uVar23 = (ushort)uVar30;
                    *(ushort *)(iVar19 + 0x204) = uVar23;
                    puVar31 = 0x20000044;
                    *(undefined2 *)(iVar19 + 0x268) = *(undefined2 *)(iVar15 + 4);
                    if (uVar30 != uVar29) goto LAB_08005824;
                  }
                  uVar29 = *(uint *)(iVar15 + 0x18);
                  if (uVar29 == 0) {
LAB_08005d9a:
                    uVar22 = *(uint *)(iVar15 + 0x1c);
                    if (uVar22 == 0) {
                      return;
                    }
                    uVar18 = (undefined2)uVar22;
                    *(undefined2 *)(iVar19 + 0x204) = uVar18;
                    *(undefined2 *)(iVar19 + 0x268) = *(undefined2 *)(iVar15 + 4);
                    goto LAB_08005c02;
                  }
                  uVar23 = (ushort)uVar29;
                  *(ushort *)(iVar19 + 0x204) = uVar23;
                  *(undefined2 *)(iVar19 + 0x268) = *(undefined2 *)(iVar15 + 4);
                  puVar31 = 0x20000046;
                  if (uVar29 == uVar22) goto LAB_08005d9a;
                  goto LAB_08005d88;
                }
                if (sVar7 != 0) {
                  0x2000017a[8] = 0;
                  uVar27 = (uint)*(ushort *)(iVar17 + 0x202);
                  uVar23 = *(ushort *)(iVar17 + 0x266);
                  if (sVar9 == 0) {
                    *(uint *)(iVar15 + 0x10) = uVar27;
                  }
                  else if (sVar9 == 1) {
                    *(uint *)(iVar15 + 0x14) = uVar27;
                  }
                  else if (sVar9 == 2) {
                    *(uint *)(iVar15 + 0x18) = uVar27;
                  }
                  else {
                    if (sVar9 != 3) goto LAB_08005c4a;
                    *(uint *)(iVar15 + 0x1c) = uVar27;
                  }
                  *(uint *)(iVar15 + 4) = (uint)uVar23;
                }
LAB_08005c4a:
                *psVar14 = 1;
                psVar14[1] = 1;
                psVar14[2] = 1;
                psVar14[3] = 1;
                uVar27 = *(uint *)(iVar15 + 0x10);
                if (uVar27 != uVar20) {
                  *(ushort *)(iVar19 + 0x202) = (ushort)uVar27;
                  puVar31 = 0x20000042;
                  *(undefined2 *)(iVar19 + 0x266) = *(undefined2 *)(iVar15 + 4);
                  if (uVar27 != uVar30) {
                    *puVar31 = (ushort)uVar27;
                    goto LAB_08005c80;
                  }
                }
                uVar30 = *(uint *)(iVar15 + 0x14);
                if (uVar30 != 0) {
                  *(ushort *)(iVar19 + 0x202) = (ushort)uVar30;
                  puVar31 = 0x20000044;
                  *(undefined2 *)(iVar19 + 0x266) = *(undefined2 *)(iVar15 + 4);
                  if (uVar30 != uVar29) {
                    *puVar31 = (ushort)uVar30;
                    *(undefined4 *)(iVar15 + 0x18) = 0;
                    *(uint *)(iVar15 + 0x10) = uVar20;
                    *(undefined4 *)(iVar15 + 0x1c) = 0;
                    *(undefined2 *)(iVar19 + 0x22) = 1;
                    return;
                  }
                }
                uVar29 = *(uint *)(iVar15 + 0x18);
                if (uVar29 != 0) {
                  uVar23 = (ushort)uVar29;
                  *(ushort *)(iVar19 + 0x202) = uVar23;
                  *(undefined2 *)(iVar19 + 0x266) = *(undefined2 *)(iVar15 + 4);
                  puVar31 = 0x20000046;
                  if (uVar29 != uVar22) goto LAB_08005d88;
                }
                uVar22 = *(uint *)(iVar15 + 0x1c);
                if (uVar22 == 0) {
                  return;
                }
                *(short *)(iVar19 + 0x202) = (short)uVar22;
                *(undefined2 *)(iVar19 + 0x266) = *(undefined2 *)(iVar15 + 4);
                if (uVar22 == uVar21) {
                  return;
                }
                *0x20000048 = (short)uVar22;
              }
              *(uint *)(iVar15 + 0x10) = uVar20;
              *(undefined4 *)(iVar15 + 0x14) = 0;
              *(undefined4 *)(iVar15 + 0x18) = 0;
              uVar18 = 3;
              goto LAB_08005d94;
            }
            if (sVar4 != 0) {
              0x2000017a[5] = 0;
              uVar27 = (uint)*(ushort *)(iVar16 + 0x1fc);
              uVar23 = *(ushort *)(iVar16 + 0x260);
              if (sVar9 == 0) {
                *(uint *)(iVar15 + 0x10) = uVar27;
              }
              else if (sVar9 == 1) {
                *(uint *)(iVar15 + 0x14) = uVar27;
              }
              else if (sVar9 == 2) {
                *(uint *)(iVar15 + 0x18) = uVar27;
              }
              else {
                if (sVar9 != 3) goto LAB_08005a0e;
                *(uint *)(iVar15 + 0x1c) = uVar27;
              }
              *(uint *)(iVar15 + 4) = (uint)uVar23;
            }
LAB_08005a0e:
            *psVar14 = 1;
            psVar14[1] = 1;
            psVar14[2] = 1;
            psVar14[3] = 1;
            uVar27 = *(uint *)(iVar15 + 0x10);
            if (uVar27 != uVar20) {
              uVar23 = (ushort)uVar27;
              *(ushort *)(iVar19 + 0x1fc) = uVar23;
              puVar31 = 0x20000042;
              *(undefined2 *)(iVar19 + 0x260) = *(undefined2 *)(iVar15 + 4);
              if (uVar27 != uVar30) goto LAB_08005d46;
            }
            uVar30 = *(uint *)(iVar15 + 0x14);
            if (uVar30 != 0) {
              uVar23 = (ushort)uVar30;
              *(ushort *)(iVar19 + 0x1fc) = uVar23;
              puVar31 = 0x20000044;
              *(undefined2 *)(iVar19 + 0x260) = *(undefined2 *)(iVar15 + 4);
              bVar34 = uVar30 == uVar29;
LAB_08005a56:
              if (!bVar34) goto LAB_080059a4;
            }
            uVar29 = *(uint *)(iVar15 + 0x18);
            if (uVar29 == 0) {
LAB_08005a72:
              uVar22 = *(uint *)(iVar15 + 0x1c);
              bVar33 = uVar22 == 0;
              goto LAB_08005a76;
            }
            uVar23 = (ushort)uVar29;
            *(ushort *)(iVar19 + 0x1fc) = uVar23;
            *(undefined2 *)(iVar19 + 0x260) = *(undefined2 *)(iVar15 + 4);
            puVar31 = 0x20000046;
            if (uVar29 == uVar22) goto LAB_08005a72;
          }
LAB_08005d88:
          *puVar31 = uVar23;
          *(uint *)(iVar15 + 0x10) = uVar20;
          *(undefined4 *)(iVar15 + 0x14) = 0;
          *(undefined4 *)(iVar15 + 0x1c) = 0;
          uVar18 = 2;
LAB_08005d94:
          *(undefined2 *)(iVar19 + 0x22) = uVar18;
          return;
        }
        if (sVar13 != 0) {
          0x2000017a[1] = 0;
          uVar27 = (uint)*(ushort *)(iVar16 + 500);
          uVar23 = *(ushort *)(iVar16 + 600);
          if (sVar9 == 0) {
            *(uint *)(iVar15 + 0x10) = uVar27;
          }
          else if (sVar9 == 1) {
            *(uint *)(iVar15 + 0x14) = uVar27;
          }
          else if (sVar9 == 2) {
            *(uint *)(iVar15 + 0x18) = uVar27;
          }
          else {
            if (sVar9 != 3) goto LAB_08005718;
            *(uint *)(iVar15 + 0x1c) = uVar27;
          }
          *(uint *)(iVar15 + 4) = (uint)uVar23;
        }
LAB_08005718:
        *psVar14 = 1;
        psVar14[2] = 1;
        psVar14[3] = 1;
        psVar14[4] = 1;
        iVar19 = 0x20000398;
        uVar27 = *(uint *)(iVar15 + 0x10);
        if (uVar27 != uVar20) {
          uVar23 = (ushort)uVar27;
          *(ushort *)(0x20000398 + 500) = uVar23;
          puVar31 = 0x20000042;
          *(undefined2 *)(iVar19 + 600) = *(undefined2 *)(iVar15 + 4);
          if (uVar27 != uVar30) goto LAB_08005650;
        }
        uVar30 = *(uint *)(iVar15 + 0x14);
        if (uVar30 != 0) {
          uVar23 = (ushort)uVar30;
          *(ushort *)(iVar19 + 500) = uVar23;
          puVar31 = 0x20000044;
          *(undefined2 *)(iVar19 + 600) = *(undefined2 *)(iVar15 + 4);
          if (uVar30 != uVar29) goto LAB_08005680;
        }
        uVar29 = *(uint *)(iVar15 + 0x18);
        if (uVar29 != 0) {
          *(short *)(iVar19 + 500) = (short)uVar29;
          *(undefined2 *)(iVar19 + 600) = *(undefined2 *)(iVar15 + 4);
          puVar31 = 0x20000046;
          if (uVar29 != uVar22) goto LAB_080057a0;
        }
        uVar22 = *(uint *)(iVar15 + 0x1c);
        if (uVar22 == 0) {
          return;
        }
        uVar18 = (undefined2)uVar22;
        *(undefined2 *)(iVar19 + 500) = uVar18;
        *(undefined2 *)(iVar19 + 600) = *(undefined2 *)(iVar15 + 4);
      }
LAB_0800578e:
      if (uVar22 == uVar21) {
        return;
      }
      *0x20000048 = uVar18;
      *(uint *)(iVar15 + 0x10) = uVar20;
      *(undefined4 *)(iVar15 + 0x14) = 0;
      *(undefined4 *)(iVar15 + 0x18) = 0;
      uVar18 = 3;
LAB_080056e0:
      *(undefined2 *)(iVar19 + 0x22) = uVar18;
      return;
    }
    *(undefined4 *)(0x20000654 + 0x10) = 0;
  }
LAB_080053cc:
  *(undefined4 *)(iVar15 + 0x18) = 0;
  *(undefined4 *)(iVar15 + 0x1c) = 0;
  if (((((sVar12 != 0) && (sVar13 != 0)) && (sVar1 != 0)) &&
      (((sVar2 != 0 && (sVar3 != 0)) && ((sVar4 != 0 && ((sVar5 != 0 && (sVar6 != 0)))))))) &&
     ((sVar7 != 0 && (sVar8 != 0)))) {
    return;
  }
LAB_080053fe:
  *(undefined4 *)(iVar15 + 4) = 0;
  *psVar14 = 1;
  psVar14[1] = 1;
  psVar14[2] = 1;
  psVar14[3] = 1;
  psVar14[4] = 1;
  psVar14[5] = 1;
  psVar14[6] = 1;
  psVar14[7] = 1;
  psVar14[8] = 1;
  psVar14[9] = 1;
  return;
}


// ===== list_mode_step @ 08005dc4 (size 864)

/* Modes 9/10 auto-step list */

void list_mode_step(void)

{
  ushort uVar1;
  ushort uVar2;
  int iVar3;
  int iVar4;
  uint uVar5;
  uint uVar6;
  uint uVar7;
  uint uVar8;
  uint uVar9;
  uint uVar10;
  uint uVar11;
  bool bVar12;
  
  iVar4 = 0x2000017a;
  iVar3 = 0x20000654;
  uVar6 = 0x20000398;
  uVar1 = *(ushort *)(0x2000017a + 0x24);
  uVar10 = (uint)*(ushort *)(0x20000398 + 0x1f2);
  uVar7 = (uint)*(ushort *)(0x20000398 + 500);
  uVar5 = (uint)*(ushort *)(0x20000398 + 0x22);
  uVar9 = (uint)*(ushort *)(0x20000398 + 600);
  uVar8 = (uint)*(ushort *)(0x20000398 + 0x256);
  uVar11 = *(uint *)(0x20000654 + 0x48);
  if (*(short *)(0x20000398 + 0x20) == 9) {
    if (*(int *)(0x20000654 + 4) == 1) {
      if (uVar11 == 0xb) {
        if (uVar5 == 0) {
          *(uint *)(0x20000654 + 0x10) = uVar10;
        }
        else if (uVar5 == 1) {
          *(uint *)(0x20000654 + 0x14) = uVar10;
        }
        else if (uVar5 == 2) {
          *(uint *)(0x20000654 + 0x18) = uVar10;
        }
        else if (uVar5 == 3) {
          *(uint *)(0x20000654 + 0x1c) = uVar10;
        }
        *(uint *)(iVar3 + 0x4c) = uVar8;
        *(ushort *)(iVar4 + 0x24) = uVar1 + 1;
        if (uVar1 < uVar8) {
          return;
        }
        *(undefined2 *)(iVar4 + 0x24) = 0;
        *(undefined4 *)(iVar3 + 0x48) = 0xc;
      }
      else if (uVar11 < 0xc) {
        return;
      }
      if (uVar5 == 0) {
        *(uint *)(iVar3 + 0x10) = uVar7;
      }
      else if (uVar5 == 1) {
        *(uint *)(iVar3 + 0x14) = uVar7;
      }
      else if (uVar5 == 2) {
        *(uint *)(iVar3 + 0x18) = uVar7;
      }
      else if (uVar5 == 3) {
        *(uint *)(iVar3 + 0x1c) = uVar7;
      }
      *(uint *)(iVar3 + 0x4c) = uVar9;
      uVar1 = *(ushort *)(iVar4 + 0x24);
      *(ushort *)(iVar4 + 0x24) = uVar1 + 1;
      if (uVar1 < uVar9) {
        return;
      }
      goto LAB_0800611a;
    }
  }
  else {
    if (*(short *)(0x20000398 + 0x20) != 10) {
      return;
    }
    if (*(int *)(0x20000654 + 4) == 1) {
      if (uVar11 == 0xb) {
        if (uVar5 == 0) {
          *(uint *)(0x20000654 + 0x10) = uVar10;
        }
        else if (uVar5 == 1) {
          *(uint *)(0x20000654 + 0x14) = uVar10;
        }
        else if (uVar5 == 2) {
          *(uint *)(0x20000654 + 0x18) = uVar10;
        }
        else if (uVar5 == 3) {
          *(uint *)(0x20000654 + 0x1c) = uVar10;
        }
        *(uint *)(iVar3 + 0x4c) = uVar8;
        *(ushort *)(iVar4 + 0x24) = uVar1 + 1;
        bVar12 = uVar8 <= uVar1;
        goto LAB_08005f16;
      }
      if (uVar11 == 0xc) goto LAB_08005f1e;
      if (uVar11 == 0xd) goto LAB_08005f4e;
      if (uVar11 == 0xe) goto LAB_08005f8a;
      if (uVar11 == 0xf) goto LAB_08005fd0;
      if (uVar11 == 0x10) goto LAB_08006012;
      if (uVar11 == 0x11) goto LAB_0800604c;
      if (uVar11 == 0x12) goto LAB_08006086;
      if (uVar11 == 0x13) goto LAB_080060c0;
      if (uVar11 != 0x14) {
        return;
      }
      while( true ) {
        uVar7 = (uint)*(ushort *)(0x20000398 + 0x204);
        if (uVar5 == 0) {
          *(uint *)(iVar3 + 0x10) = uVar7;
        }
        else if (uVar5 == 1) {
          *(uint *)(iVar3 + 0x14) = uVar7;
        }
        else if (uVar5 == 2) {
          *(uint *)(iVar3 + 0x18) = uVar7;
        }
        else if (uVar5 == 3) {
          *(uint *)(iVar3 + 0x1c) = uVar7;
        }
        uVar6 = (uint)*(ushort *)(uVar6 + 0x268);
        *(uint *)(iVar3 + 0x4c) = uVar6;
        uVar5 = (uint)*(ushort *)(iVar4 + 0x24);
        *(ushort *)(iVar4 + 0x24) = *(ushort *)(iVar4 + 0x24) + 1;
        bVar12 = false;
        if (uVar6 <= uVar5) break;
        while( true ) {
          if (!bVar12) goto LAB_08005f82;
          *(undefined2 *)(iVar4 + 0x24) = 0;
          *(undefined4 *)(iVar3 + 0x48) = 0x11;
LAB_0800604c:
          uVar7 = (uint)*(ushort *)(0x20000398 + 0x1fe);
          if (uVar5 == 0) {
            *(uint *)(iVar3 + 0x10) = uVar7;
          }
          else if (uVar5 == 1) {
            *(uint *)(iVar3 + 0x14) = uVar7;
          }
          else if (uVar5 == 2) {
            *(uint *)(iVar3 + 0x18) = uVar7;
          }
          else if (uVar5 == 3) {
            *(uint *)(iVar3 + 0x1c) = uVar7;
          }
          uVar9 = (uint)*(ushort *)(uVar6 + 0x262);
          *(uint *)(iVar3 + 0x4c) = uVar9;
          uVar1 = *(ushort *)(iVar4 + 0x24);
          uVar7 = uVar1 + 1;
          *(short *)(iVar4 + 0x24) = (short)uVar7;
          bVar12 = uVar9 <= uVar1;
          if (!bVar12) goto LAB_08005fc8;
          *(undefined2 *)(iVar4 + 0x24) = 0;
          *(undefined4 *)(iVar3 + 0x48) = 0x12;
LAB_08006086:
          uVar7 = (uint)*(ushort *)(0x20000398 + 0x200);
          if (uVar5 == 0) {
            *(uint *)(iVar3 + 0x10) = uVar7;
          }
          else if (uVar5 == 1) {
            *(uint *)(iVar3 + 0x14) = uVar7;
          }
          else if (uVar5 == 2) {
            *(uint *)(iVar3 + 0x18) = uVar7;
          }
          else if (uVar5 == 3) {
            *(uint *)(iVar3 + 0x1c) = uVar7;
          }
          uVar9 = (uint)*(ushort *)(uVar6 + 0x264);
          *(uint *)(iVar3 + 0x4c) = uVar9;
          uVar1 = *(ushort *)(iVar4 + 0x24);
          uVar7 = uVar1 + 1;
          *(short *)(iVar4 + 0x24) = (short)uVar7;
          bVar12 = uVar9 <= uVar1;
          if (!bVar12) goto LAB_08005fc8;
          *(undefined2 *)(iVar4 + 0x24) = 0;
          *(undefined4 *)(iVar3 + 0x48) = 0x13;
LAB_080060c0:
          uVar7 = (uint)*(ushort *)(0x20000398 + 0x202);
          if (uVar5 == 0) {
            *(uint *)(iVar3 + 0x10) = uVar7;
          }
          else if (uVar5 == 1) {
            *(uint *)(iVar3 + 0x14) = uVar7;
          }
          else if (uVar5 == 2) {
            *(uint *)(iVar3 + 0x18) = uVar7;
          }
          else if (uVar5 == 3) {
            *(uint *)(iVar3 + 0x1c) = uVar7;
          }
          uVar9 = (uint)*(ushort *)(uVar6 + 0x266);
          *(uint *)(iVar3 + 0x4c) = uVar9;
          uVar7 = (uint)*(ushort *)(iVar4 + 0x24);
          *(ushort *)(iVar4 + 0x24) = *(ushort *)(iVar4 + 0x24) + 1;
          bVar12 = false;
          if (uVar9 <= uVar7) break;
          while (!bVar12) {
LAB_08005f16:
            do {
              if (!bVar12) {
                return;
              }
              *(undefined2 *)(iVar4 + 0x24) = 0;
              *(undefined4 *)(iVar3 + 0x48) = 0xc;
LAB_08005f1e:
              if (uVar5 == 0) {
                *(uint *)(iVar3 + 0x10) = uVar7;
              }
              else if (uVar5 == 1) {
                *(uint *)(iVar3 + 0x14) = uVar7;
              }
              else if (uVar5 == 2) {
                *(uint *)(iVar3 + 0x18) = uVar7;
              }
              else if (uVar5 == 3) {
                *(uint *)(iVar3 + 0x1c) = uVar7;
              }
              *(uint *)(iVar3 + 0x4c) = uVar9;
              uVar1 = *(ushort *)(iVar4 + 0x24);
              *(ushort *)(iVar4 + 0x24) = uVar1 + 1;
              if (uVar1 < uVar9) {
                return;
              }
              *(undefined2 *)(iVar4 + 0x24) = 0;
              *(undefined4 *)(iVar3 + 0x48) = 0xd;
LAB_08005f4e:
              uVar7 = (uint)*(ushort *)(0x20000398 + 0x1f6);
              if (uVar5 == 0) {
                *(uint *)(iVar3 + 0x10) = uVar7;
              }
              else if (uVar5 == 1) {
                *(uint *)(iVar3 + 0x14) = uVar7;
              }
              else if (uVar5 == 2) {
                *(uint *)(iVar3 + 0x18) = uVar7;
              }
              else if (uVar5 == 3) {
                *(uint *)(iVar3 + 0x1c) = uVar7;
              }
              uVar2 = *(ushort *)(uVar6 + 0x25a);
              *(uint *)(iVar3 + 0x4c) = (uint)uVar2;
              uVar1 = *(ushort *)(iVar4 + 0x24);
              *(ushort *)(iVar4 + 0x24) = uVar1 + 1;
              bVar12 = (uint)uVar2 <= (uint)uVar1;
LAB_08005f82:
              if (!bVar12) {
                return;
              }
              *(undefined2 *)(iVar4 + 0x24) = 0;
              *(undefined4 *)(iVar3 + 0x48) = 0xe;
LAB_08005f8a:
              uVar7 = (uint)*(ushort *)(0x20000398 + 0x1f8);
              if (uVar5 == 0) {
                *(uint *)(iVar3 + 0x10) = uVar7;
              }
              else if (uVar5 == 1) {
                *(uint *)(iVar3 + 0x14) = uVar7;
              }
              else if (uVar5 == 2) {
                *(uint *)(iVar3 + 0x18) = uVar7;
              }
              else if (uVar5 == 3) {
                *(uint *)(iVar3 + 0x1c) = uVar7;
              }
              uVar9 = (uint)*(ushort *)(uVar6 + 0x25c);
              *(uint *)(iVar3 + 0x4c) = uVar9;
              uVar1 = *(ushort *)(iVar4 + 0x24);
              uVar7 = uVar1 + 1;
              *(short *)(iVar4 + 0x24) = (short)uVar7;
              bVar12 = uVar9 <= uVar1;
LAB_08005fc8:
            } while (!bVar12);
            *(undefined2 *)(iVar4 + 0x24) = 0;
            *(undefined4 *)(iVar3 + 0x48) = 0xf;
LAB_08005fd0:
            uVar7 = (uint)*(ushort *)(0x20000398 + 0x1fa);
            if (uVar5 == 0) {
              *(uint *)(iVar3 + 0x10) = uVar7;
            }
            else if (uVar5 == 1) {
              *(uint *)(iVar3 + 0x14) = uVar7;
            }
            else if (uVar5 == 2) {
              *(uint *)(iVar3 + 0x18) = uVar7;
            }
            else if (uVar5 == 3) {
              *(uint *)(iVar3 + 0x1c) = uVar7;
            }
            uVar9 = (uint)*(ushort *)(uVar6 + 0x25e);
            *(uint *)(iVar3 + 0x4c) = uVar9;
            uVar1 = *(ushort *)(iVar4 + 0x24);
            uVar7 = uVar1 + 1;
            *(short *)(iVar4 + 0x24) = (short)uVar7;
            bVar12 = uVar9 <= uVar1;
          }
          *(undefined2 *)(iVar4 + 0x24) = 0;
          *(undefined4 *)(iVar3 + 0x48) = 0x10;
LAB_08006012:
          uVar7 = (uint)*(ushort *)(0x20000398 + 0x1fc);
          if (uVar5 == 0) {
            *(uint *)(iVar3 + 0x10) = uVar7;
          }
          else if (uVar5 == 1) {
            *(uint *)(iVar3 + 0x14) = uVar7;
          }
          else if (uVar5 == 2) {
            *(uint *)(iVar3 + 0x18) = uVar7;
          }
          else if (uVar5 == 3) {
            *(uint *)(iVar3 + 0x1c) = uVar7;
          }
          uVar2 = *(ushort *)(uVar6 + 0x260);
          *(uint *)(iVar3 + 0x4c) = (uint)uVar2;
          uVar1 = *(ushort *)(iVar4 + 0x24);
          *(ushort *)(iVar4 + 0x24) = uVar1 + 1;
          bVar12 = (uint)uVar2 <= (uint)uVar1;
        }
        *(undefined2 *)(iVar4 + 0x24) = 0;
        *(undefined4 *)(iVar3 + 0x48) = 0x14;
      }
LAB_0800611a:
      *(undefined2 *)(iVar4 + 0x24) = 0;
      *(undefined4 *)(iVar3 + 0x48) = 0xb;
      return;
    }
  }
  *(undefined4 *)(0x20000654 + 0x48) = 0xb;
  *(uint *)(iVar3 + 0x4c) = uVar8;
  return;
}


// ===== encoder_scan @ 08006130 (size 102)

/* PB5/PB6 quadrature */

void encoder_scan(void)

{
  undefined1 *puVar1;
  undefined4 uVar2;
  int iVar3;
  
  uVar2 = 0x40010c00/*GPIOB*/;
  puVar1 = 0x200002c4;
  if (0x200002c4[2] == '\0') {
    iVar3 = GPIO_ReadInputDataBit(0x40010c00/*GPIOB*/,0x20);
    if (iVar3 != 0) {
      iVar3 = GPIO_ReadInputDataBit(uVar2,0x40);
      if (iVar3 != 0) {
        return;
      }
      puVar1[2] = 1;
      puVar1[1] = 0;
      puVar1[0x24] = 1;
      goto LAB_08006182;
    }
    puVar1[2] = 1;
    *puVar1 = 0;
    puVar1[0x24] = 0;
  }
  else {
    iVar3 = GPIO_ReadInputDataBit(0x40010c00/*GPIOB*/,0x20);
    if (iVar3 != 0) {
      iVar3 = GPIO_ReadInputDataBit(uVar2,0x40);
      if (iVar3 != 0) {
        return;
      }
      puVar1[1] = 0;
LAB_08006182:
      puVar1[0x25] = 1;
      goto LAB_08006162;
    }
    *puVar1 = 0;
  }
  puVar1[0x25] = 0;
LAB_08006162:
  puVar1[3] = 0;
  return;
}


// ===== encoder_tick @ 080061a0 (size 58)

void encoder_tick(void)

{
  char cVar1;
  int iVar2;
  
  iVar2 = 0x200002c4;
  cVar1 = *(char *)(0x200002c4 + 3);
  if ((cVar1 != -1) && (*(char *)(0x200002c4 + 3) = cVar1 + '\x01', cVar1 == '\x05')) {
    *(undefined1 *)(iVar2 + 2) = 0;
    *(char *)(iVar2 + 4) = *(char *)(iVar2 + 0x25) - *(char *)(iVar2 + 0x24);
    *(undefined2 *)(iVar2 + 0x28) = *(undefined2 *)(iVar2 + 0x26);
    *(undefined2 *)(iVar2 + 0x26) = 0;
  }
  if (-1 < (int)((uint)*(byte *)(0x200002c4 + 0x26) << 0x18)) {
    *(short *)(iVar2 + 0x26) = *(short *)(iVar2 + 0x26) + 1;
  }
  return;
}


// ===== read_mcu_temp @ 080061e0 (size 182)

/* Internal temp sensor (ch16) */

int read_mcu_temp(void)

{
  undefined4 uVar1;
  int iVar2;
  uint uVar3;
  uint uVar4;
  uint uVar5;
  uint uVar6;
  uint uVar7;
  ushort local_28 [10];
  
  uVar6 = 0;
  memclr(local_28,0x14);
  uVar1 = 0x40012400/*ADC1*/;
  ADC_RegularChannelConfig(0x40012400/*ADC1*/,0x10,1);
  uVar7 = 0;
  do {
    ADC_SoftwareStartConvCmd(uVar1,1);
    do {
      iVar2 = ADC_GetFlagStatus(uVar1,2);
    } while (iVar2 == 0);
    iVar2 = ADC_GetConversionValue(uVar1);
    local_28[uVar7] = (ushort)iVar2;
    uVar7 = uVar7 + 1 & 0xff;
    uVar6 = iVar2 + uVar6 & 0xffff;
  } while (uVar7 < 10);
  uVar7 = (uint)local_28[0];
  uVar5 = 1;
  uVar3 = uVar7;
  do {
    uVar4 = (uint)local_28[uVar5];
    if (uVar3 < uVar4) {
      uVar3 = uVar4;
    }
    if (uVar4 < uVar7) {
      uVar7 = uVar4;
    }
    uVar5 = uVar5 + 1 & 0xff;
  } while (uVar5 < 10);
  iVar2 = ((int)((((int)((((uVar6 - uVar3) - uVar7 & 0x7ffff) >> 3) *
                        (uint)*(ushort *)(0x20000398 + 0x2a)) / 0xfff & 0xffffU) * -10 + 0x37dc) *
                100) / 0x2b + 0x9c4) / 10;
  if (iVar2 < 0) {
    iVar2 = 9000 - iVar2;
  }
  return iVar2;
}


// ===== read_ntc1 @ 080062a0 (size 146)

/* PB0/ch8 NTC via table at 0x08008238 */

void read_ntc1(void)

{
  undefined4 uVar1;
  int iVar2;
  uint uVar3;
  uint uVar4;
  uint uVar5;
  uint uVar6;
  uint uVar7;
  ushort local_28 [10];
  
  uVar6 = 0;
  memclr(local_28,0x14);
  uVar1 = 0x40012400/*ADC1*/;
  ADC_RegularChannelConfig(0x40012400/*ADC1*/,8,1,5);
  uVar7 = 0;
  do {
    ADC_SoftwareStartConvCmd(uVar1,1);
    do {
      iVar2 = ADC_GetFlagStatus(uVar1,2);
    } while (iVar2 == 0);
    iVar2 = ADC_GetConversionValue(uVar1);
    local_28[uVar7] = (ushort)iVar2;
    uVar7 = uVar7 + 1 & 0xff;
    uVar6 = iVar2 + uVar6 & 0xffff;
  } while (uVar7 < 10);
  uVar7 = (uint)local_28[0];
  uVar5 = 1;
  uVar3 = uVar7;
  do {
    uVar4 = (uint)local_28[uVar5];
    if (uVar3 < uVar4) {
      uVar3 = uVar4;
    }
    if (uVar4 < uVar7) {
      uVar7 = uVar4;
    }
    uVar5 = uVar5 + 1 & 0xff;
  } while (uVar5 < 10);
  uVar5 = 0;
  do {
    if (*(uint *)(0x8008238 + uVar5 * 4) <= ((uVar6 - uVar3) - uVar7 & 0x1fffff) >> 5) {
      *(uint *)(0x20000654 + 0x40) = uVar5;
      return;
    }
    uVar5 = uVar5 + 1 & 0xff;
  } while (uVar5 < 100);
  return;
}


// ===== read_ntc2 @ 08006340 (size 146)

/* PB1/ch9 NTC */

void read_ntc2(void)

{
  undefined4 uVar1;
  int iVar2;
  uint uVar3;
  uint uVar4;
  uint uVar5;
  uint uVar6;
  uint uVar7;
  ushort local_28 [10];
  
  uVar6 = 0;
  memclr(local_28,0x14);
  uVar1 = 0x40012400/*ADC1*/;
  ADC_RegularChannelConfig(0x40012400/*ADC1*/,9,1,5);
  uVar7 = 0;
  do {
    ADC_SoftwareStartConvCmd(uVar1,1);
    do {
      iVar2 = ADC_GetFlagStatus(uVar1,2);
    } while (iVar2 == 0);
    iVar2 = ADC_GetConversionValue(uVar1);
    local_28[uVar7] = (ushort)iVar2;
    uVar7 = uVar7 + 1 & 0xff;
    uVar6 = iVar2 + uVar6 & 0xffff;
  } while (uVar7 < 10);
  uVar7 = (uint)local_28[0];
  uVar5 = 1;
  uVar3 = uVar7;
  do {
    uVar4 = (uint)local_28[uVar5];
    if (uVar3 < uVar4) {
      uVar3 = uVar4;
    }
    if (uVar4 < uVar7) {
      uVar7 = uVar4;
    }
    uVar5 = uVar5 + 1 & 0xff;
  } while (uVar5 < 10);
  uVar5 = 0;
  do {
    if (*(uint *)(0x8008238 + uVar5 * 4) <= ((uVar6 - uVar3) - uVar7 & 0x1fffff) >> 5) {
      *(uint *)(0x20000654 + 0x44) = uVar5;
      return;
    }
    uVar5 = uVar5 + 1 & 0xff;
  } while (uVar5 < 100);
  return;
}


// ===== config_load @ 080063e0 (size 58)

/* Copy 0x2BC bytes from 0x0800F000; magic 0x7E4 */

void config_load(void)

{
  int iVar1;
  int *piVar2;
  int iVar3;
  uint uVar4;
  
  piVar2 = 0x20000398;
  iVar1 = 0x800f000;
  uVar4 = 0;
  do {
    piVar2[uVar4] = *(int *)(iVar1 + uVar4 * 4);
    iVar3 = 0x20000654;
    uVar4 = uVar4 + 1 & 0xff;
  } while (uVar4 < 0xaf);
  if (*piVar2 == 0x7e4) {
    if ((uint)*(ushort *)((int)piVar2 + 0x12) <= (uint)*(ushort *)(piVar2 + 3)) {
      *(uint *)(0x20000654 + 0x14) = (uint)*(ushort *)((int)piVar2 + 0x12);
    }
    if ((uint)*(ushort *)(piVar2 + 4) <= (uint)*(ushort *)((int)piVar2 + 10)) {
      *(uint *)(iVar3 + 0x10) = (uint)*(ushort *)(piVar2 + 4);
    }
  }
  return;
}


// ===== measure_current @ 08006428 (size 1124)

/* ADS1115 ch0, range PB2, piecewise gain */

void measure_current(void)

{
  ushort uVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  short *psVar5;
  undefined2 *puVar6;
  int iVar7;
  uint uVar8;
  uint uVar9;
  
  iVar7 = ads1115_read(0);
  iVar4 = 0x20000654;
  iVar3 = 0x200001a0;
  iVar2 = 0x20000398;
  uVar8 = (*(int *)(0x200001a0 + 0x38) * (((uint)*(ushort *)(0x20000398 + 0x28) * iVar7) / 1000)
          ) / 10000;
  *(uint *)(0x20000654 + 0xc) = uVar8;
  if (uVar8 <= *(ushort *)(iVar2 + 0x2e)) {
    *(undefined4 *)(iVar4 + 0xc) = 0;
  }
  psVar5 = 0x20000032;
  if ((*0x20000032 == 0x7da) && (*(int *)(iVar4 + 0xc) != 0)) {
    *(short *)(iVar2 + 0x2e) = (short)*(int *)(iVar4 + 0xc);
LAB_08006488:
    *psVar5 = 0x78;
  }
  else if (*0x20000032 == 0x7db) {
    *(undefined2 *)(iVar2 + 0x2e) = 0;
    goto LAB_08006488;
  }
  if (*(short *)(iVar3 + 0xc) == 0) {
    *(undefined2 *)(iVar3 + 0xc) = 1;
    uVar1 = *(ushort *)(iVar2 + 0xc);
    *(ushort *)(iVar3 + 10) = uVar1 / 1000;
    if (999 < uVar1) {
      *(undefined2 *)(iVar3 + 6) = 6000;
    }
    if (9999 < uVar1) {
      *(undefined2 *)(iVar3 + 6) = 6000;
    }
    if (19999 < uVar1) {
      *(undefined2 *)(iVar3 + 6) = 6000;
    }
    if (29999 < uVar1) {
      *(undefined2 *)(iVar3 + 6) = 6000;
    }
    if (39999 < uVar1) {
      *(undefined2 *)(iVar3 + 6) = 8000;
    }
    if (49999 < uVar1) {
      *(undefined2 *)(iVar3 + 6) = 8000;
    }
    if (59999 < uVar1) {
      *(undefined2 *)(iVar3 + 6) = 8000;
    }
  }
  puVar6 = 0x200001fc;
  if (*psVar5 == 0x6e) {
    return;
  }
  if (*(uint *)(iVar4 + 0x14) < 0x157d) {
    uVar8 = *(uint *)(iVar4 + 0xc);
    if ((uVar8 <= *(ushort *)(iVar3 + 6)) && (*(short *)(iVar3 + 8) == 0)) {
      *(undefined4 *)(0x42218000 + 0x188) = 0;
      *puVar6 = 0;
      uVar9 = (uint)*(ushort *)(iVar3 + 10);
      if (uVar9 + 0x32 < uVar8) {
        if (uVar9 + 100 < uVar8) {
          if (uVar9 + 200 < uVar8) {
            if (uVar9 + 300 < uVar8) {
              if (uVar9 + 400 < uVar8) {
                if (uVar9 + 500 < uVar8) {
                  if (uVar9 + 600 < uVar8) {
                    if (uVar9 + 700 < uVar8) {
                      if (uVar9 + 800 < uVar8) {
                        if (uVar9 + 900 < uVar8) {
                          if (uVar9 + 1000 < uVar8) {
                            if (uVar9 + 2000 < uVar8) {
                              if (uVar9 + 3000 < uVar8) {
                                if (uVar9 + 4000 < uVar8) {
                                  uVar8 = (uint)*(ushort *)(iVar2 + 0x6c);
                                }
                                else {
                                  uVar8 = (uint)*(ushort *)(iVar2 + 0x6a);
                                }
                              }
                              else {
                                uVar8 = (uint)*(ushort *)(iVar2 + 0x68);
                              }
                            }
                            else {
                              uVar8 = (uint)*(ushort *)(iVar2 + 0x66);
                            }
                          }
                          else {
                            uVar8 = (uint)*(ushort *)(iVar2 + 100);
                          }
                        }
                        else {
                          uVar8 = (uint)*(ushort *)(iVar2 + 0x62);
                        }
                      }
                      else {
                        uVar8 = (uint)*(ushort *)(iVar2 + 0x60);
                      }
                    }
                    else {
                      uVar8 = (uint)*(ushort *)(iVar2 + 0x5e);
                    }
                  }
                  else {
                    uVar8 = (uint)*(ushort *)(iVar2 + 0x5c);
                  }
                }
                else {
                  uVar8 = (uint)*(ushort *)(iVar2 + 0x5a);
                }
              }
              else {
                uVar8 = (uint)*(ushort *)(iVar2 + 0x58);
              }
            }
            else {
              uVar8 = (uint)*(ushort *)(iVar2 + 0x56);
            }
          }
          else {
            uVar8 = (uint)*(ushort *)(iVar2 + 0x54);
          }
        }
        else {
          uVar8 = (uint)*(ushort *)(iVar2 + 0x52);
        }
      }
      else {
        uVar8 = (uint)*(ushort *)(iVar2 + 0x50);
      }
      goto LAB_08006708;
    }
    uVar9 = (uint)*(ushort *)(iVar3 + 10);
    if (uVar8 < uVar9 + 0x157c) {
      if (0x157c - uVar9 <= uVar8) {
        return;
      }
      *(undefined2 *)(iVar3 + 8) = 0;
      return;
    }
    *(undefined4 *)(0x42218000 + 0x188) = 1;
    *puVar6 = 1;
    *(undefined2 *)(iVar3 + 8) = 1;
    if (uVar9 + 6000 < uVar8) {
      if (uVar9 + 7000 < uVar8) {
        if (uVar9 + 8000 < uVar8) {
          if (uVar9 + 9000 < uVar8) {
            if (uVar9 + 10000 < uVar8) {
              if (uVar9 + 11000 < uVar8) {
                if (uVar9 + 12000 < uVar8) {
                  if (uVar9 + 13000 < uVar8) {
                    if (uVar9 + 14000 < uVar8) {
                      if (uVar9 + 15000 < uVar8) {
                        if (uVar9 + 16000 < uVar8) {
                          if (uVar9 + 17000 < uVar8) {
                            if (uVar9 + 18000 < uVar8) {
                              if (uVar9 + 19000 < uVar8) {
                                if (uVar9 + 20000 < uVar8) {
                                  if (uVar9 + 25000 < uVar8) {
                                    if (uVar9 + 30000 < uVar8) {
                                      if (uVar9 + 35000 < uVar8) {
                                        if (uVar9 + 40000 < uVar8) {
                                          if (uVar9 + 45000 < uVar8) {
                                            if (uVar9 + 50000 < uVar8) {
                                              if (uVar9 + 55000 < uVar8) {
                                                if (uVar8 <= uVar9 + 60000) goto LAB_0800689a;
                                                goto LAB_080068a0;
                                              }
                                              goto LAB_0800688a;
                                            }
                                            goto LAB_0800687a;
                                          }
                                          goto LAB_0800686a;
                                        }
                                        goto LAB_0800685a;
                                      }
                                      goto LAB_0800684c;
                                    }
                                    goto LAB_0800683c;
                                  }
                                  goto LAB_08006814;
                                }
                                goto LAB_08006804;
                              }
                              goto LAB_080067f6;
                            }
                            goto LAB_080067e6;
                          }
                          goto LAB_080067d6;
                        }
                        goto LAB_080067c6;
                      }
                      goto LAB_080067b8;
                    }
                    goto LAB_080067a8;
                  }
                  goto LAB_08006798;
                }
                goto LAB_08006788;
              }
              goto LAB_08006778;
            }
            goto LAB_08006768;
          }
          goto LAB_0800675c;
        }
        goto LAB_0800674c;
      }
      goto LAB_0800673e;
    }
  }
  else {
    *(undefined4 *)(0x42218000 + 0x188) = 1;
    *puVar6 = 1;
    uVar9 = *(uint *)(iVar4 + 0xc);
    uVar8 = (uint)*(ushort *)(iVar3 + 10);
    if (uVar8 + 6000 < uVar9) {
      if (uVar8 + 7000 < uVar9) {
        if (uVar8 + 8000 < uVar9) {
          if (uVar8 + 9000 < uVar9) {
            if (uVar8 + 10000 < uVar9) {
              if (uVar8 + 11000 < uVar9) {
                if (uVar8 + 12000 < uVar9) {
                  if (uVar8 + 13000 < uVar9) {
                    if (uVar8 + 14000 < uVar9) {
                      if (uVar8 + 15000 < uVar9) {
                        if (uVar8 + 16000 < uVar9) {
                          if (uVar8 + 17000 < uVar9) {
                            if (uVar8 + 18000 < uVar9) {
                              if (uVar8 + 19000 < uVar9) {
                                if (uVar8 + 20000 < uVar9) {
                                  if (uVar8 + 25000 < uVar9) {
                                    if (uVar8 + 30000 < uVar9) {
                                      if (uVar8 + 35000 < uVar9) {
                                        if (uVar8 + 40000 < uVar9) {
                                          if (uVar8 + 45000 < uVar9) {
                                            if (uVar8 + 50000 < uVar9) {
                                              if (uVar8 + 55000 < uVar9) {
                                                if (uVar9 <= uVar8 + 60000) {
LAB_0800689a:
                                                  uVar8 = (uint)*(ushort *)(iVar2 + 0xda);
                                                  goto LAB_08006708;
                                                }
LAB_080068a0:
                                                uVar8 = (uint)*(ushort *)(iVar2 + 0x128);
                                                goto LAB_08006708;
                                              }
LAB_0800688a:
                                              uVar8 = (uint)*(ushort *)(iVar2 + 0xd0);
                                              goto LAB_08006708;
                                            }
LAB_0800687a:
                                            uVar8 = (uint)*(ushort *)(iVar2 + 0xc6);
                                            goto LAB_08006708;
                                          }
LAB_0800686a:
                                          uVar8 = (uint)*(ushort *)(iVar2 + 0xbc);
                                          goto LAB_08006708;
                                        }
LAB_0800685a:
                                        uVar8 = (uint)*(ushort *)(iVar2 + 0xb2);
                                        goto LAB_08006708;
                                      }
LAB_0800684c:
                                      uVar8 = (uint)*(ushort *)(iVar2 + 0xa8);
                                      goto LAB_08006708;
                                    }
LAB_0800683c:
                                    uVar8 = (uint)*(ushort *)(iVar2 + 0x9e);
                                    goto LAB_08006708;
                                  }
LAB_08006814:
                                  uVar8 = (uint)*(ushort *)(iVar2 + 0x94);
                                  goto LAB_08006708;
                                }
LAB_08006804:
                                uVar8 = (uint)*(ushort *)(iVar2 + 0x8a);
                                goto LAB_08006708;
                              }
LAB_080067f6:
                              uVar8 = (uint)*(ushort *)(iVar2 + 0x88);
                              goto LAB_08006708;
                            }
LAB_080067e6:
                            uVar8 = (uint)*(ushort *)(iVar2 + 0x86);
                            goto LAB_08006708;
                          }
LAB_080067d6:
                          uVar8 = (uint)*(ushort *)(iVar2 + 0x84);
                          goto LAB_08006708;
                        }
LAB_080067c6:
                        uVar8 = (uint)*(ushort *)(iVar2 + 0x82);
                        goto LAB_08006708;
                      }
LAB_080067b8:
                      uVar8 = (uint)*(ushort *)(iVar2 + 0x80);
                      goto LAB_08006708;
                    }
LAB_080067a8:
                    uVar8 = (uint)*(ushort *)(iVar2 + 0x7e);
                    goto LAB_08006708;
                  }
LAB_08006798:
                  uVar8 = (uint)*(ushort *)(iVar2 + 0x7c);
                  goto LAB_08006708;
                }
LAB_08006788:
                uVar8 = (uint)*(ushort *)(iVar2 + 0x7a);
                goto LAB_08006708;
              }
LAB_08006778:
              uVar8 = (uint)*(ushort *)(iVar2 + 0x78);
              goto LAB_08006708;
            }
LAB_08006768:
            uVar8 = (uint)*(ushort *)(iVar2 + 0x76);
            goto LAB_08006708;
          }
LAB_0800675c:
          uVar8 = (uint)*(ushort *)(iVar2 + 0x74);
          goto LAB_08006708;
        }
LAB_0800674c:
        uVar8 = (uint)*(ushort *)(iVar2 + 0x72);
        goto LAB_08006708;
      }
LAB_0800673e:
      uVar8 = (uint)*(ushort *)(iVar2 + 0x70);
      goto LAB_08006708;
    }
  }
  uVar8 = (uint)*(ushort *)(iVar2 + 0x6e);
LAB_08006708:
  *(uint *)(iVar3 + 0x38) = uVar8;
  return;
}


// ===== measure_vin @ 080068a8 (size 166)

/* ADS1115 ch3/ch2 auto-range + PA7 */

void measure_vin(void)

{
  int iVar1;
  char *pcVar2;
  undefined2 *puVar3;
  int iVar4;
  int iVar5;
  uint uVar6;
  
  iVar4 = adc_read_avg130(7);
  puVar3 = 0x20000058;
  pcVar2 = 0x200001a0;
  iVar1 = 0x20000654;
  *(uint *)(0x20000654 + 0x20) = (uint)(iVar4 * 0x1de2) / 10000;
  iVar4 = 0x20000398;
  if (pcVar2[2] == '\0') {
    if (pcVar2[1] != '\0') {
      *pcVar2 = '\x01';
      pcVar2[1] = '\0';
      *puVar3 = *(undefined2 *)(iVar4 + 0x34);
    }
    iVar5 = ads1115_read(3);
    *(uint *)(iVar1 + 0x24) = ((uint)*(ushort *)(iVar4 + 0x34) * iVar5) / 10000;
  }
  if (pcVar2[2] == '\x01') {
    if (*pcVar2 != '\0') {
      *pcVar2 = '\0';
      pcVar2[1] = '\x01';
      *puVar3 = *(undefined2 *)(iVar4 + 0x36);
    }
    iVar5 = ads1115_read(2);
    uVar6 = ((uint)*(ushort *)(iVar4 + 0x36) * iVar5) / 10000;
    *(uint *)(iVar1 + 0x24) = uVar6;
  }
  else {
    uVar6 = *(uint *)(iVar1 + 0x24);
  }
  if (uVar6 < 0xffff) {
    if (uVar6 < 0x7d01) {
      if (0x733b < uVar6) {
        return;
      }
      pcVar2[2] = '\0';
      return;
    }
  }
  else {
    *(undefined4 *)(iVar1 + 0x24) = 0xffff;
  }
  pcVar2[2] = '\x01';
  return;
}


// ===== measure_voltage @ 08006960 (size 446)

/* ADS1115 ch1, range via PA1 */

void measure_voltage(void)

{
  short sVar1;
  undefined2 uVar2;
  int iVar3;
  undefined2 *puVar4;
  int iVar5;
  int iVar6;
  undefined4 uVar7;
  undefined2 *puVar8;
  short *psVar9;
  int iVar10;
  uint uVar11;
  uint in_r3;
  undefined4 local_28;
  
  iVar6 = 0x20000398;
  iVar5 = 0x20000654;
  puVar4 = 0x200001f8;
  iVar3 = 0x200001a0;
  local_28 = in_r3;
  if (*(char *)(0x200001a0 + 5) == '\0') {
    if (*(char *)(0x200001a0 + 4) != '\0') {
      local_28 = CONCAT22((short)(in_r3 >> 0x10),2);
      local_28 = local_28 & 0xffffff;
      GPIO_Init(0x40010800/*GPIOA*/,&local_28);
      *(undefined1 *)(iVar3 + 3) = 1;
      *(undefined1 *)(iVar3 + 4) = 0;
      puVar8 = 0x20000204;
      uVar2 = *(undefined2 *)(iVar6 + 0x40);
      *puVar4 = uVar2;
      *puVar8 = uVar2;
      *0x20000056 = *(undefined2 *)(iVar6 + 0x30);
    }
    iVar10 = ads1115_read(1);
    *(uint *)(iVar5 + 8) = ((uint)*(ushort *)(iVar6 + 0x30) * iVar10) / 10000;
  }
  uVar7 = 0x40010800/*GPIOA*/;
  if (*(char *)(iVar3 + 5) == '\x01') {
    if (*(char *)(iVar3 + 3) != '\0') {
      local_28 = 0x10030002;
      GPIO_Init(0x40010800/*GPIOA*/,&local_28);
      GPIO_ResetBits(uVar7,2);
      *(undefined1 *)(iVar3 + 3) = 0;
      *(undefined1 *)(iVar3 + 4) = 1;
      puVar8 = 0x20000204;
      uVar2 = *(undefined2 *)(iVar6 + 0x42);
      *puVar4 = uVar2;
      *puVar8 = uVar2;
      *0x20000056 = *(undefined2 *)(iVar6 + 0x32);
    }
    iVar10 = ads1115_read(1);
    uVar11 = ((uint)*(ushort *)(iVar6 + 0x32) * iVar10) / 10000;
    *(uint *)(iVar5 + 8) = uVar11;
    if (uVar11 < 0x17d5) {
      uVar11 = uVar11 - 3;
      *(uint *)(iVar5 + 8) = uVar11;
    }
    if (uVar11 < 0x13ed) {
      *(uint *)(iVar5 + 8) = uVar11 - 3;
    }
  }
  uVar11 = *(uint *)(iVar5 + 8);
  if (*(ushort *)(iVar6 + 0x2c) < uVar11) {
    if (uVar11 < 0xf0b) {
      if (uVar11 < 0xe10) goto LAB_08006a4a;
    }
    else {
      *(undefined1 *)(iVar3 + 5) = 1;
    }
  }
  else {
    *(undefined4 *)(iVar5 + 8) = 0;
LAB_08006a4a:
    *(undefined1 *)(iVar3 + 5) = 0;
  }
  sVar1 = *(short *)(iVar6 + 0xc);
  if (sVar1 == 3000) {
    *(uint *)(iVar5 + 8) = *(uint *)(iVar5 + 0xc) / 100 + *(int *)(iVar5 + 8);
  }
  if (sVar1 == 10000) {
    *(uint *)(iVar5 + 8) = *(uint *)(iVar5 + 0xc) / 1000 + *(int *)(iVar5 + 8);
  }
  if (sVar1 == 20000) {
    *(uint *)(iVar5 + 8) = *(uint *)(iVar5 + 0xc) / 2000 + *(int *)(iVar5 + 8);
  }
  if (sVar1 == 30000) {
    *(uint *)(iVar5 + 8) = *(uint *)(iVar5 + 0xc) / 3000 + *(int *)(iVar5 + 8);
  }
  if (sVar1 == -0x63c0) {
    *(uint *)(iVar5 + 8) = *(uint *)(iVar5 + 0xc) / 3000 + *(int *)(iVar5 + 8);
  }
  if (sVar1 == -0x3cb0) {
    *(uint *)(iVar5 + 8) = *(uint *)(iVar5 + 0xc) / 4000 + *(int *)(iVar5 + 8);
  }
  if (sVar1 == -0x15a0) {
    *(uint *)(iVar5 + 8) = *(uint *)(iVar5 + 0xc) / 4000 + *(int *)(iVar5 + 8);
  }
  psVar9 = 0x20000032;
  if (*0x20000032 == 0x3f2) {
    if (*(int *)(iVar5 + 8) == 0) goto LAB_08006b08;
    *(short *)(iVar6 + 0x2c) = (short)*(int *)(iVar5 + 8);
  }
  else {
    if (*0x20000032 != 0x3f3) goto LAB_08006b08;
    *(undefined2 *)(iVar6 + 0x2c) = 0;
  }
  *psVar9 = 0x78;
LAB_08006b08:
  if (*(short *)(iVar6 + 0x4e) == 1) {
    *(uint *)(iVar5 + 8) = *(uint *)(iVar5 + 0x24) / 10;
  }
  return;
}


// ===== lcd_clear_line @ 08006b40 (size 36)

void lcd_clear_line(char param_1)

{
  byte bVar1;
  
  lcd_write(0,param_1 * '@' + -0x80);
  bVar1 = 0;
  do {
    lcd_write(1,0x20);
    bVar1 = bVar1 + 1;
  } while (bVar1 < 0x10);
  return;
}


// ===== lcd_putc_at @ 08006b64 (size 30)

void lcd_putc_at(char param_1,char param_2,undefined4 param_3)

{
  lcd_write(0,param_2 + param_1 * '@' + -0x80);
  lcd_write(1,param_3);
  return;
}


// ===== lcd_puts_at @ 08006b82 (size 58)

void lcd_puts_at(int param_1,uint param_2,undefined1 *param_3)

{
  int iVar1;
  uint uVar2;
  
  iVar1 = strlen(param_3);
  uVar2 = iVar1 + param_2;
  lcd_write(0,param_2 + param_1 * 0x40 + 0x80 & 0xff);
  for (; param_2 < (uVar2 & 0xff); param_2 = param_2 + 1 & 0xff) {
    lcd_write(1,*param_3);
    param_3 = param_3 + 1;
  }
  return;
}


// ===== main @ 08006bbc (size 3012)

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void main(void)

{
  byte bVar1;
  short sVar2;
  ushort uVar3;
  char *pcVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  undefined2 *puVar8;
  char *pcVar9;
  ushort *puVar10;
  byte *pbVar11;
  undefined1 *puVar12;
  undefined1 *puVar13;
  undefined1 uVar14;
  char cVar15;
  uint uVar16;
  uint uVar17;
  undefined4 uVar18;
  undefined2 uVar19;
  ushort uVar20;
  uint uVar21;
  bool bVar22;
  uint local_c;
  undefined4 local_8;
  
  iVar5 = 0x20000098;
  local_c = 0;
  local_8 = 0;
  rcc_init();
  nvic_init();
  SysTick_CLKSourceConfig();
  tim7_init();
  outputs_gpio_init();
  keys_gpio_init();
  adc_init();
  dac_init();
  config_init();
  lcd_clear_line(0);
  lcd_clear_line(1);
  iVar6 = 0x20000398;
  if (*(short *)(0x20000398 + 0x24) == 0) {
    *(undefined2 *)(0x20000398 + 0x24) = 1;
    config_save(*(undefined4 *)(iVar6 + 4));
    delay_ms(1000);
  }
  if ((*(ushort *)(iVar6 + 0x26) == 0) || (0xf < *(ushort *)(iVar6 + 0x26))) {
    *(undefined2 *)(iVar6 + 0x26) = 5;
    config_save(*(undefined4 *)(iVar6 + 4));
    delay_ms(1000);
  }
  *0x20000216 = *(undefined1 *)(iVar6 + 0x26);
  uVar14 = *(undefined1 *)(iVar6 + 0x24);
  *0x2000020a = uVar14;
  *0x2000020b = 0;
  iVar7 = 0x20000324;
  *(undefined1 *)(0x20000324 + 0xc) = uVar14;
  *(undefined1 *)(iVar7 + 10) = 0x28;
  usart1_init();
  usart2_init();
  puVar8 = 0x200001f4;
  uVar20 = *(ushort *)(iVar6 + 0xc);
  if (999 < uVar20) {
    *0x200001f4 = 100;
  }
  if (9999 < uVar20) {
    *puVar8 = 100;
  }
  if (19999 < uVar20) {
    *puVar8 = 0x32;
  }
  if (29999 < uVar20) {
    *puVar8 = 0x21;
  }
  if (39999 < uVar20) {
    *puVar8 = 0x21;
  }
  if (49999 < uVar20) {
    *puVar8 = 0x19;
  }
  if (59999 < uVar20) {
    *puVar8 = 0x19;
  }
  pcVar4 = 0x20000028;
  iVar7 = 0x20000654;
  *0x200001fa = *(ushort *)(iVar6 + 0x12a);
  *(undefined2 *)(pcVar4 + 0x1a) = *(undefined2 *)(iVar7 + 0x10);
  *(undefined2 *)(pcVar4 + 0x1c) = *(undefined2 *)(iVar7 + 0x14);
  *(undefined2 *)(pcVar4 + 0x1e) = *(undefined2 *)(iVar7 + 0x18);
  *(undefined2 *)(pcVar4 + 0x20) = *(undefined2 *)(iVar7 + 0x1c);
  *(undefined2 *)(pcVar4 + 0x16) = *(undefined2 *)(iVar6 + 0x26);
  *(undefined2 *)(pcVar4 + 0x18) = *(undefined2 *)(iVar6 + 0x24);
  *(undefined2 *)(pcVar4 + 0x22) = *(undefined2 *)(iVar6 + 0x1a);
  *(undefined2 *)(pcVar4 + 0x24) = *(undefined2 *)(iVar6 + 0x1c);
  *(undefined2 *)(pcVar4 + 0x26) = *(undefined2 *)(iVar6 + 0x1ee);
  *(undefined2 *)(pcVar4 + 0x2a) = *(undefined2 *)(iVar6 + 0x18);
  *(undefined2 *)(pcVar4 + 0x28) = *(undefined2 *)(iVar6 + 0x1ec);
  *(undefined2 *)(pcVar4 + 0x14) = *(undefined2 *)(iVar6 + 0x1ea);
  *(undefined2 *)(pcVar4 + 0x2c) = *(undefined2 *)(iVar6 + 0x4e);
  sVar2 = *(short *)(iVar6 + 0x20);
  if ((((((sVar2 != 0) && (sVar2 != 1)) && (sVar2 != 2)) && ((sVar2 != 3 && (sVar2 != 4)))) &&
      (sVar2 != 5)) && ((sVar2 != 9 && (sVar2 != 10)))) {
    *(undefined2 *)(iVar6 + 0x20) = 1;
  }
  if (*(short *)(0x20000398 + 0x20) == 0) {
    uVar14 = 2;
  }
  else {
    if (*(short *)(0x20000398 + 0x20) != 1) goto LAB_08006d24;
    uVar14 = 7;
  }
  local_c = CONCAT31(local_c._1_3_,uVar14);
LAB_08006d24:
  ui_draw_cursor_screen(iVar5 + (local_c & 0xff) * 0x14);
  tim2_pwm_init(0x18,0);
  TIM_SetCompare2(0x40000000);
  GPIO_ResetBits(0x40010c00/*GPIOB*/,0x800);
  iwdg_init(4,0x271);
  pcVar9 = 0x2000021f;
LAB_08006d60:
  if ((*(short *)(pcVar4 + 10) != 0x6e) && (*(short *)(pcVar4 + 10) != 0x6f)) {
    measure_voltage();
    puVar10 = 0x20000204;
    if ((*(short *)(pcVar4 + 10) != 0x73) && (*(short *)(pcVar4 + 10) != 0x74)) {
      uVar16 = *(uint *)(iVar7 + 0x10);
      if (uVar16 != *(ushort *)(pcVar4 + 0x1a)) {
        *(short *)(pcVar4 + 0x1a) = (short)uVar16;
        *0x200001f8 = *puVar10;
        pcVar4[0xe] = '\0';
        pcVar4[0xf] = '\0';
      }
      puVar10 = 0x200001f8;
      if ((*(int *)(iVar7 + 4) != 0) && (*(short *)(0x20000398 + 0x20) != 1)) {
        uVar21 = (uint)*(ushort *)(0x20000398 + 10);
        uVar17 = *(uint *)(iVar7 + 8);
        if ((uVar21 / 1000 < uVar17) &&
           ((*(ushort *)(0x20000398 + 0xc) / 1000 < *(uint *)(iVar7 + 0xc) && (uVar17 != uVar16)))
           ) {
          uVar20 = *0x20000204;
          uVar3 = *(ushort *)(pcVar4 + 0xe);
          if ((uint)uVar3 < uVar20 / 10) {
            if (uVar16 / 1000 + uVar16 < uVar17) {
              *0x200001f8 = *0x200001f8 + 1;
              *(ushort *)(pcVar4 + 0xe) = uVar3 + 1;
            }
            if (uVar17 < uVar16 - uVar16 / 1000) {
              *0x200001f8 = *0x200001f8 - 1;
              *(short *)(pcVar4 + 0xe) = *(short *)(pcVar4 + 0xe) + 1;
            }
          }
          else {
            if (uVar21 / 500 + uVar16 < uVar17) {
              pcVar4[0xe] = '\0';
              pcVar4[0xf] = '\0';
              *puVar10 = uVar20;
            }
            puVar10 = 0x200001f8;
            if (uVar17 < uVar16 - uVar21 / 500) {
              pcVar4[0xe] = '\0';
              pcVar4[0xf] = '\0';
              *puVar10 = uVar20;
            }
          }
        }
      }
    }
  }
  measure_current();
  iVar6 = 0x20000398;
  uVar16 = (uint)*(ushort *)(pcVar4 + 10);
  bVar22 = uVar16 == 0x6e;
  do {
    do {
      do {
        if (bVar22) goto LAB_080070e8;
        bVar22 = true;
      } while (uVar16 == 0x6f);
      uVar16 = *(uint *)(iVar7 + 4);
      bVar22 = true;
    } while (uVar16 == 0);
    if (*(uint *)(iVar7 + 8) < 0x1f) goto LAB_080070e8;
    uVar17 = *(uint *)(iVar7 + 0x14);
    uVar16 = *(uint *)(iVar7 + 0xc);
    bVar22 = true;
  } while (uVar16 == uVar17);
  if ((uVar17 <= *(ushort *)(0x20000398 + 0x1f0)) && (*0x200001e8 == '\x01')) {
    uVar21 = (uint)*(ushort *)(0x20000398 + 0xc);
    if (uVar21 < 0x157d) {
      if (uVar17 < 0x1f) goto LAB_08006e86;
      uVar20 = *0x200001fa / 6;
    }
    else if (uVar17 < uVar21 / 1000 + 0x78) {
LAB_08006e86:
      uVar20 = *(short *)(0x20000398 + 300) * 6;
    }
    else {
      uVar20 = *0x200001fa / 10;
    }
    *(ushort *)(pcVar4 + 0x12) = uVar20;
    uVar20 = *(ushort *)(pcVar4 + 0x10);
    if (uVar20 < *(ushort *)(pcVar4 + 0x12)) {
      if (uVar17 / 1000 + uVar17 < uVar16) {
        *0x200001fa = *0x200001fa - 1;
        *(ushort *)(pcVar4 + 0x10) = uVar20 + 1;
      }
      if (uVar16 < uVar17 - uVar17 / 1000) {
        *0x200001fa = *0x200001fa + 1;
        *(short *)(pcVar4 + 0x10) = *(short *)(pcVar4 + 0x10) + 1;
      }
    }
    else if ((uVar16 < uVar17 - uVar21 / 500) || (uVar21 / 500 + uVar17 < uVar16)) {
      pcVar4[0x10] = '\0';
      pcVar4[0x11] = '\0';
      if (uVar17 < 0x157d) {
        if (uVar17 < 0xb) {
          uVar19 = *(undefined2 *)(iVar6 + 300);
        }
        else if (uVar17 < 0x15) {
          uVar19 = *(undefined2 *)(iVar6 + 0x12e);
        }
        else if (uVar17 < 0x1f) {
          uVar19 = *(undefined2 *)(iVar6 + 0x130);
        }
        else if (uVar17 < 0x29) {
          uVar19 = *(undefined2 *)(iVar6 + 0x132);
        }
        else if (uVar17 < 0x33) {
          uVar19 = *(undefined2 *)(iVar6 + 0x134);
        }
        else if (uVar17 < 0x3d) {
          uVar19 = *(undefined2 *)(iVar6 + 0x136);
        }
        else if (uVar17 < 0x47) {
          uVar19 = *(undefined2 *)(iVar6 + 0x138);
        }
        else if (uVar17 < 0x51) {
          uVar19 = *(undefined2 *)(iVar6 + 0x13a);
        }
        else if (uVar17 < 0x5b) {
          uVar19 = *(undefined2 *)(iVar6 + 0x13c);
        }
        else if (uVar17 < 0x65) {
          uVar19 = *(undefined2 *)(iVar6 + 0x13e);
        }
        else if (uVar17 < 0xc9) {
          uVar19 = *(undefined2 *)(iVar6 + 0x140);
        }
        else if (uVar17 < 0x12d) {
          uVar19 = *(undefined2 *)(iVar6 + 0x142);
        }
        else if (uVar17 < 0x191) {
          uVar19 = *(undefined2 *)(iVar6 + 0x144);
        }
        else if (uVar17 < 0x1f5) {
          uVar19 = *(undefined2 *)(iVar6 + 0x146);
        }
        else if (uVar17 < 0x259) {
          uVar19 = *(undefined2 *)(iVar6 + 0x148);
        }
        else if (uVar17 < 0x2bd) {
          uVar19 = *(undefined2 *)(iVar6 + 0x14a);
        }
        else if (uVar17 < 0x321) {
          uVar19 = *(undefined2 *)(iVar6 + 0x14c);
        }
        else if (uVar17 < 0x385) {
          uVar19 = *(undefined2 *)(iVar6 + 0x14e);
        }
        else if (uVar17 < 0x3e9) {
          uVar19 = *(undefined2 *)(iVar6 + 0x150);
        }
        else if (uVar17 < 0x7d1) {
          uVar19 = *(undefined2 *)(iVar6 + 0x152);
        }
        else if (uVar17 < 0xbb9) {
          uVar19 = *(undefined2 *)(iVar6 + 0x154);
        }
        else if (uVar17 < 0xfa1) {
          uVar19 = *(undefined2 *)(iVar6 + 0x156);
        }
        else {
          uVar19 = *(undefined2 *)(iVar6 + 0x158);
        }
      }
      else if (uVar17 < 0x1771) {
        uVar19 = *(undefined2 *)(iVar6 + 0x15a);
      }
      else if (uVar17 < 0x1b59) {
        uVar19 = *(undefined2 *)(iVar6 + 0x15c);
      }
      else if (uVar17 < 0x1f41) {
        uVar19 = *(undefined2 *)(iVar6 + 0x15e);
      }
      else if (uVar17 < 0x2329) {
        uVar19 = *(undefined2 *)(iVar6 + 0x160);
      }
      else if (uVar17 < 0x2711) {
        uVar19 = *(undefined2 *)(iVar6 + 0x162);
      }
      else if (uVar17 < 0x4e21) {
        uVar19 = *(undefined2 *)(iVar6 + 0x176);
      }
      else if (uVar17 < 0x7531) {
        uVar19 = *(undefined2 *)(iVar6 + 0x18a);
      }
      else if (uVar17 < 0x9c41) {
        uVar19 = *(undefined2 *)(iVar6 + 0x19e);
      }
      else if (uVar17 < 0xc351) {
        uVar19 = *(undefined2 *)(iVar6 + 0x1b2);
      }
      else {
        if (60000 < uVar17) goto LAB_080070e8;
        uVar19 = *(undefined2 *)(iVar6 + 0x1c6);
      }
      *0x200001fa = uVar19;
    }
  }
LAB_080070e8:
  puVar10 = 0x200001e4;
  *(uint *)(iVar7 + 0x28) =
       (*(uint *)(iVar7 + 0xc) * *(int *)(iVar7 + 8)) / ((uint)*0x200001e4 * 1000);
  *(uint *)(iVar7 + 0x2c) = (uint)(*(int *)(iVar7 + 8) * 100) / *(uint *)(iVar7 + 0xc);
  if (*0x2000021c == '\x01') {
    *0x2000021c = '\0';
    mode_control();
    cal_voltage();
    cal_dac_table();
    cal_current_table();
  }
  if (*0x2000021d == '\x01') {
    *0x2000021d = '\0';
    *pcVar4 = *pcVar4 + '\x01';
    *(short *)(pcVar4 + 0xc) = *(short *)(pcVar4 + 0xc) + 1;
    measure_vin();
  }
  if (*0x2000021e == '\x01') {
    *0x2000021e = '\0';
    read_ntc1();
    read_ntc2();
    uVar18 = read_mcu_temp();
    *(undefined4 *)(iVar7 + 0x3c) = uVar18;
    *(uint *)(iVar7 + 0x30) = (uint)(*(int *)(iVar7 + 0xc) * 500) / 0xe10 + *(int *)(iVar7 + 0x30);
    *(uint *)(iVar7 + 0x34) =
         (uint)(*(int *)(iVar7 + 8) * *(int *)(iVar7 + 0xc)) / 0x1c20 + *(int *)(iVar7 + 0x34);
  }
  if ((*pcVar4 == '2') || (*(short *)(pcVar4 + 10) != 0)) {
    *pcVar4 = '\0';
    iVar6 = 0x20000398;
    sVar2 = *(short *)(pcVar4 + 10);
    if (((sVar2 == 0) &&
        (((*(uint *)(iVar7 + 0x40) < 0x23 && (*(uint *)(iVar7 + 0x44) < 0x23)) &&
         (*(uint *)(iVar7 + 0xc) <
          (uint)*(ushort *)(0x20000398 + 0xc) - (uint)(*(ushort *)(0x20000398 + 0xc) >> 1)))))
       && (*(uint *)(iVar7 + 0x28) < *(ushort *)(0x20000398 + 0xe) / 1000)) {
LAB_08007358:
      *pcVar9 = '\0';
    }
    else {
      uVar16 = *(uint *)(iVar7 + 0xc);
      uVar20 = *(ushort *)(0x20000398 + 0xc);
      if (uVar16 < (uint)uVar20 - (uint)(uVar20 >> 1)) {
        *pcVar9 = '\0';
      }
      else if (*pcVar9 == '\0') {
        *pcVar9 = (char)(uVar20 / 1000 << 1);
      }
      if (uVar16 < 50000) {
        uVar17 = *(uint *)(iVar7 + 0x40);
        if ((0x23 < uVar17) || (0x23 < *(uint *)(iVar7 + 0x44))) {
          *pcVar9 = '\x14';
        }
        if ((0x24 < uVar17) || (0x24 < *(uint *)(iVar7 + 0x44))) {
          *pcVar9 = '(';
        }
        if ((0x25 < uVar17) || (0x25 < *(uint *)(iVar7 + 0x44))) {
          *pcVar9 = '<';
        }
        if ((0x26 < uVar17) || (0x26 < *(uint *)(iVar7 + 0x44))) {
          *pcVar9 = 'P';
        }
        if ((0x27 < uVar17) || (0x27 < *(uint *)(iVar7 + 0x44))) {
          *pcVar9 = 'd';
        }
      }
      uVar17 = *(uint *)(iVar7 + 0x40);
      if ((0x28 < uVar17) || (0x28 < *(uint *)(iVar7 + 0x44))) {
        *pcVar9 = 'x';
      }
      if ((0x29 < uVar17) || (0x29 < *(uint *)(iVar7 + 0x44))) {
        *pcVar9 = -0x74;
      }
      if ((0x2a < uVar17) || (0x2a < *(uint *)(iVar7 + 0x44))) {
        *pcVar9 = -0x60;
      }
      if ((0x2b < uVar17) || (0x2b < *(uint *)(iVar7 + 0x44))) {
        *pcVar9 = -0x4c;
      }
      if ((0x2c < uVar17) || (0x2c < *(uint *)(iVar7 + 0x44))) {
        *pcVar9 = -0x38;
      }
      if ((uVar17 == 0) && (*(int *)(iVar7 + 0x44) == 0)) {
        if (uVar16 < 50000) {
          uVar20 = *(ushort *)(iVar6 + 0xe);
          uVar16 = (uint)uVar20;
          uVar17 = *(uint *)(iVar7 + 0x28);
          if (uVar16 / 0xc < uVar17) {
            *pcVar9 = '\x14';
          }
          if (uVar16 / 0xb < uVar17) {
            *pcVar9 = '(';
          }
          if (uVar16 / 10 < uVar17) {
            *pcVar9 = '<';
          }
          if (uVar16 / 9 < uVar17) {
            *pcVar9 = 'P';
          }
          if (uVar20 >> 3 < uVar17) {
            *pcVar9 = 'd';
          }
        }
        uVar20 = *(ushort *)(iVar6 + 0xe);
        uVar17 = (uint)uVar20;
        uVar16 = *(uint *)(iVar7 + 0x28);
        if (uVar17 / 7 < uVar16) {
          *pcVar9 = 'x';
        }
        if (uVar17 / 6 < uVar16) {
          *pcVar9 = -0x74;
        }
        if (uVar17 / 5 < uVar16) {
          *pcVar9 = -0x60;
        }
        if (uVar20 >> 2 < uVar16) {
          *pcVar9 = -0x4c;
        }
        if (uVar17 / 3 < uVar16) {
          *pcVar9 = -0x38;
        }
      }
      if (sVar2 == 1) {
        *pcVar9 = -0x38;
      }
      else if ((sVar2 == 0x6e) || (sVar2 == 0x6f)) goto LAB_08007358;
    }
  }
  uVar18 = keys_scan();
  pbVar11 = _DAT_080074fc;
  iVar6 = 0x20000398;
  switch(uVar18) {
  case 0:
    bVar1 = *_DAT_080074fc;
    *_DAT_080074fc = bVar1 + 1;
    if (5 < (byte)(bVar1 + 1)) {
      *pbVar11 = 0;
    }
    break;
  case 1:
    lcd_clear_line(0);
    lcd_puts_at(0,4,"Saving...");
    oled_clear_cols(0,6,0x80);
    oled_clear_cols(0,7,0x80);
    oled_draw_char16(0,6,0x10);
    oled_draw_char16(8,6,10);
    oled_draw_char16(0x10,6,0x12);
    oled_draw_char16(0x18,6,0xc);
    oled_draw_char16(0x20,6,0x15);
    oled_draw_char16(0x28,6,0x15);
    oled_draw_char16(0x30,6,0x15);
    iVar6 = 0x20000398;
    *(undefined2 *)(0x20000398 + 0x10) = *(undefined2 *)(iVar7 + 0x10);
    *(undefined2 *)(iVar6 + 0x12) = *(undefined2 *)(iVar7 + 0x14);
    config_save(*(undefined4 *)(iVar6 + 4));
    delay_ms(1000);
    lcd_clear_line(0);
    oled_clear_cols(0,6,0x80);
    oled_clear_cols(0,7,0x80);
    ui_draw_info_page(*0x200002c2,*0x200002c1,*(undefined4 *)(iVar7 + 0x30),
                      *(undefined4 *)(iVar7 + 0x34),*(undefined4 *)(iVar7 + 0x24),
                      *(undefined4 *)(iVar7 + 0x38),*(undefined4 *)(iVar7 + 0x3c),
                      *(undefined4 *)(iVar7 + 0x28),*(undefined4 *)(iVar7 + 0x2c));
    goto LAB_080073f8;
  case 2:
  case 3:
    menu_call_handler(iVar5 + (local_c & 0xff) * 0x14,uVar18,&local_c,local_8);
    break;
  case 4:
    *(uint *)(iVar7 + 4) = (uint)(*(int *)(iVar7 + 4) == 0);
    set_voltage_dac(*(undefined4 *)(iVar7 + 0x10));
    set_current_dac(*(undefined4 *)(iVar7 + 0x14));
    break;
  case 6:
    sVar2 = *(short *)(0x20000398 + 0x20);
    *(short *)(0x20000398 + 0x20) = sVar2 + 1;
    if (sVar2 != 0) {
      *(undefined2 *)(iVar6 + 0x20) = 0;
    }
    sVar2 = *(short *)(iVar6 + 0x20);
    if (sVar2 == 0) {
      local_c = CONCAT31(local_c._1_3_,2);
    }
    else if (sVar2 == 1) {
      cVar15 = '\a';
      goto LAB_080073cc;
    }
    goto LAB_080073f8;
  case 8:
    if (*(short *)(0x20000398 + 0x20) == 0) {
      if ((byte)local_c < 5) {
        local_c = CONCAT31(local_c._1_3_,(byte)local_c + 1);
      }
      else {
        local_c = (uint)local_c._1_3_ << 8;
      }
    }
    else if (*(short *)(0x20000398 + 0x20) == 1) {
      if ((byte)local_c < 10) {
        cVar15 = (byte)local_c + 1;
      }
      else {
LAB_080073f4:
        cVar15 = '\t';
      }
      goto LAB_080073cc;
    }
    goto LAB_080073f8;
  case 10:
    if (*(short *)(0x20000398 + 0x20) != 0) {
      if (*(short *)(0x20000398 + 0x20) != 1) goto LAB_080073f8;
      if ((byte)local_c < 5) goto LAB_080073f4;
    }
    cVar15 = (byte)local_c - 1;
LAB_080073cc:
    local_c = CONCAT31(local_c._1_3_,cVar15);
LAB_080073f8:
    ui_draw_cursor_screen(iVar5 + (local_c & 0xff) * 0x14,uVar18,&local_c,local_8);
    break;
  case 0xc:
    if (*(int *)(iVar7 + 4) == 0) {
      *(undefined4 *)(iVar7 + 4) = 1;
    }
    break;
  case 0xd:
    *0x20000220 = 1;
    iVar6 = 0x20000398;
    *(undefined2 *)(0x20000398 + 0x24) = 1;
    *(undefined2 *)(iVar6 + 0x26) = 5;
    pcVar4[10] = 'x';
    pcVar4[0xb] = '\0';
  }
  modbus_sync_regs();
  usart1_parse_meter();
  if (*(int *)(iVar7 + 4) == 0) {
    if (*(short *)(pcVar4 + 10) == -1) {
      GPIO_SetBits(0x40010c00/*GPIOB*/,0x800);
    }
    else {
      GPIO_ResetBits();
    }
  }
  iVar6 = 0x20000398;
  uVar16 = (uint)*(ushort *)(0x20000398 + 0x1ee);
  if (((uVar16 != 0) && (*(int *)(iVar7 + 4) == 1)) && (*(uint *)(iVar7 + 8) <= uVar16)) {
    *(undefined4 *)(iVar7 + 4) = 0;
  }
  sVar2 = *(short *)(pcVar4 + 10);
  if (sVar2 == 10) {
    *(undefined4 *)(iVar7 + 0x30) = 0;
    *(undefined4 *)(iVar7 + 0x34) = 0;
    pcVar4[10] = '\0';
    pcVar4[0xb] = '\0';
    goto LAB_080076d8;
  }
  if (sVar2 == -6) {
    if (*(uint *)(iVar7 + 0x50) < (uint)*(ushort *)(iVar6 + 0xc)) {
      *(short *)(iVar6 + 0x1f0) = (short)*(uint *)(iVar7 + 0x50);
    }
    else {
      *(ushort *)(iVar6 + 0x1f0) = *(ushort *)(iVar6 + 0xc);
    }
LAB_0800768a:
    pcVar4[10] = 'x';
    pcVar4[0xb] = '\0';
  }
  else {
    if (sVar2 == -5) {
      uVar19 = *(undefined2 *)(iVar7 + 0x50);
      *(undefined2 *)(iVar6 + 10) = uVar19;
      *(undefined2 *)(iVar6 + 0x10) = uVar19;
      goto LAB_0800768a;
    }
    if (sVar2 == -4) {
      *(undefined2 *)(iVar6 + 0xc) = *(undefined2 *)(iVar7 + 0x50);
      *(undefined2 *)(iVar6 + 0x12) = 0;
      goto LAB_0800768a;
    }
    if (sVar2 == -3) {
      *(undefined2 *)(iVar6 + 0xe) = *(undefined2 *)(iVar7 + 0x50);
      *(undefined2 *)(iVar6 + 0x14) = 0;
      goto LAB_0800768a;
    }
    if ((sVar2 == -2) && (1999 < *(uint *)(iVar7 + 0x50))) {
      *(short *)(iVar6 + 0x2a) = (short)*(uint *)(iVar7 + 0x50);
      goto LAB_0800768a;
    }
    if (sVar2 == -8) {
      if (pcVar4[9] == '\0') {
        pcVar4[9] = '\x01';
        *(uint *)(iVar7 + 0x50) = (uint)*(ushort *)(iVar6 + 0x1ec);
      }
      else if (*(uint *)(iVar7 + 0x50) != (uint)*(ushort *)(pcVar4 + 0x28)) {
        uVar19 = (undefined2)*(uint *)(iVar7 + 0x50);
        *(undefined2 *)(iVar6 + 0x1ec) = uVar19;
        *(undefined2 *)(pcVar4 + 0x28) = uVar19;
        pcVar4[9] = '\0';
        goto LAB_0800768a;
      }
    }
    if (sVar2 == -7) {
      if (pcVar4[8] == '\0') {
        pcVar4[8] = '\x01';
        *(uint *)(iVar7 + 0x50) = (uint)*(ushort *)(iVar6 + 0x18);
      }
      else if (*(uint *)(iVar7 + 0x50) != (uint)*(ushort *)(pcVar4 + 0x2a)) {
        uVar19 = (undefined2)*(uint *)(iVar7 + 0x50);
        *(undefined2 *)(iVar6 + 0x18) = uVar19;
        *(undefined2 *)(pcVar4 + 0x2a) = uVar19;
        pcVar4[8] = '\0';
        goto LAB_0800768a;
      }
    }
    if (sVar2 == 0x4d) {
      if (pcVar4[7] == '\0') {
        pcVar4[7] = '\x01';
        uVar20 = *(ushort *)(iVar6 + 0x4e);
LAB_080076d6:
        *(uint *)(iVar7 + 0x50) = (uint)uVar20;
      }
      else if (*(uint *)(iVar7 + 0x50) != (uint)*(ushort *)(pcVar4 + 0x2c)) {
        uVar19 = (undefined2)*(uint *)(iVar7 + 0x50);
        *(undefined2 *)(iVar6 + 0x4e) = uVar19;
        *(undefined2 *)(pcVar4 + 0x2c) = uVar19;
        pcVar4[7] = '\0';
        goto LAB_0800768a;
      }
    }
    else if (sVar2 == 0x58) {
      if (pcVar4[6] == '\0') {
        pcVar4[6] = '\x01';
        *(uint *)(iVar7 + 0x50) = uVar16;
      }
      else if (*(uint *)(iVar7 + 0x50) != (uint)*(ushort *)(pcVar4 + 0x26)) {
        uVar19 = (undefined2)*(uint *)(iVar7 + 0x50);
        *(undefined2 *)(iVar6 + 0x1ee) = uVar19;
        *(undefined2 *)(pcVar4 + 0x26) = uVar19;
        pcVar4[6] = '\0';
        goto LAB_0800768a;
      }
    }
    else if (sVar2 == 500) {
      if (pcVar4[5] == '\0') {
        pcVar4[5] = '\x01';
        uVar20 = *(ushort *)(iVar6 + 0x1ea);
        goto LAB_080076d6;
      }
      if (*(uint *)(iVar7 + 0x50) != (uint)*(ushort *)(pcVar4 + 0x14)) {
        uVar19 = (undefined2)*(uint *)(iVar7 + 0x50);
        *(undefined2 *)(iVar6 + 0x1ea) = uVar19;
        *(undefined2 *)(pcVar4 + 0x14) = uVar19;
        pcVar4[5] = '\0';
        goto LAB_0800768a;
      }
    }
  }
LAB_080076d8:
  if (*(short *)(iVar6 + 0x1ea) == 0) {
    *puVar10 = 1;
  }
  else {
    *puVar10 = 10;
  }
  sVar2 = *(short *)(pcVar4 + 10);
  if (sVar2 == 0x3e9) {
    if (pcVar4[4] == '\0') {
      pcVar4[4] = '\x01';
      *(uint *)(iVar7 + 0x50) = (uint)*(ushort *)(iVar6 + 0x1a);
      goto LAB_08007722;
    }
    uVar16 = *(uint *)(iVar7 + 0x50);
    if ((uVar16 == *(ushort *)(pcVar4 + 0x22)) || (0xabd < uVar16 - 1)) goto LAB_08007722;
    *(short *)(iVar6 + 0x1a) = (short)uVar16;
    *(short *)(pcVar4 + 0x22) = (short)uVar16;
    pcVar4[4] = '\0';
LAB_08007772:
    pcVar4[10] = 'x';
    pcVar4[0xb] = '\0';
  }
  else {
LAB_08007722:
    puVar13 = 0x20000216;
    puVar12 = 0x2000020a;
    if (sVar2 == 1000) {
      if (pcVar4[3] != '\0') {
        uVar16 = *(uint *)(iVar7 + 0x50);
        if ((uVar16 == *(ushort *)(pcVar4 + 0x24)) || (0xabd < uVar16 - 1)) goto LAB_080077b2;
        *(short *)(iVar6 + 0x1c) = (short)uVar16;
        *(short *)(pcVar4 + 0x24) = (short)uVar16;
        pcVar4[3] = '\0';
        goto LAB_08007772;
      }
      pcVar4[3] = '\x01';
      uVar16 = (uint)*(ushort *)(iVar6 + 0x1c);
LAB_080077b0:
      *(uint *)(iVar7 + 0x50) = uVar16;
    }
    else if (sVar2 == 0xe8) {
      if (pcVar4[2] == '\0') {
        pcVar4[2] = '\x01';
        uVar16 = (uint)*(ushort *)(iVar6 + 0x26);
        goto LAB_080077b0;
      }
      uVar16 = *(uint *)(iVar7 + 0x50);
      if ((uVar16 != *(ushort *)(pcVar4 + 0x16)) && (uVar16 < 0x10)) {
        *(short *)(iVar6 + 0x26) = (short)uVar16;
        *(short *)(pcVar4 + 0x16) = (short)uVar16;
        *puVar13 = (char)uVar16;
        usart2_init();
        pcVar4[2] = '\0';
        goto LAB_08007772;
      }
    }
    else if (sVar2 == 0x1e5) {
      if (pcVar4[1] == '\0') {
        pcVar4[1] = '\x01';
        uVar16 = (uint)*(ushort *)(iVar6 + 0x24);
        goto LAB_080077b0;
      }
      uVar16 = *(uint *)(iVar7 + 0x50);
      if ((uVar16 != *(ushort *)(pcVar4 + 0x18)) && (uVar16 != 0)) {
        *(short *)(iVar6 + 0x24) = (short)uVar16;
        *(short *)(pcVar4 + 0x18) = (short)uVar16;
        *puVar12 = (char)uVar16;
        *(char *)(0x20000324 + 0xc) = (char)uVar16;
        pcVar4[1] = '\0';
        goto LAB_08007772;
      }
    }
  }
LAB_080077b2:
  puVar12 = 0x2000020a;
  sVar2 = *(short *)(iVar6 + 0x24);
  if ((sVar2 == *(short *)(pcVar4 + 0x18)) || (sVar2 == 0)) goto LAB_080077d8;
  *(short *)(pcVar4 + 0x18) = sVar2;
  *puVar12 = (char)sVar2;
  *(char *)(0x20000324 + 0xc) = (char)sVar2;
  pcVar4[10] = 'x';
  pcVar4[0xb] = '\0';
  goto LAB_080077e0;
LAB_080077d8:
  if (*(short *)(pcVar4 + 10) == 0x78) {
LAB_080077e0:
    config_save(*(undefined4 *)(iVar6 + 4));
    delay_ms(1000);
    pcVar4[10] = '\0';
    pcVar4[0xb] = '\0';
    *(undefined4 *)(iVar7 + 0x50) = 0;
  }
  goto LAB_08006d60;
}


// ===== modbus_sync_regs @ 08007814 (size 432)

void modbus_sync_regs(void)

{
  short sVar1;
  ushort uVar2;
  undefined2 uVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  ushort *puVar7;
  undefined2 *puVar8;
  undefined2 *puVar9;
  undefined2 *puVar10;
  char *pcVar11;
  undefined4 *puVar12;
  uint uVar13;
  undefined2 uVar14;
  undefined2 uVar15;
  
  iVar6 = 0x20000654;
  iVar5 = 0x2000005a;
  iVar4 = 0x20000398;
  uVar3 = *(undefined2 *)(0x20000398 + 0x24);
  *(undefined2 *)(0x2000005a + 8) = uVar3;
  sVar1 = *(short *)(iVar4 + 0x20);
  uVar15 = *(undefined2 *)(iVar6 + 0x4c);
  uVar14 = *(undefined2 *)(iVar6 + 4);
  if ((sVar1 == 9) || (sVar1 == 10)) {
    *(undefined2 *)(iVar5 + 10) = *(undefined2 *)(iVar6 + 0x48);
    *(undefined2 *)(iVar5 + 0xc) = uVar15;
  }
  else {
    *(short *)(iVar5 + 10) = sVar1;
    *(undefined2 *)(iVar5 + 0xc) = uVar14;
  }
  uVar2 = *0x20000032;
  uVar13 = (uint)uVar2;
  if (uVar13 == 7) {
    uVar15 = *(undefined2 *)(iVar6 + 0x3c);
  }
  else if (uVar13 == 8) {
    uVar15 = *(undefined2 *)(iVar6 + 0x44);
  }
  else {
    uVar15 = *(undefined2 *)(iVar6 + 0x40);
  }
  *(undefined2 *)(iVar5 + 0xe) = uVar15;
  *(undefined2 *)(iVar5 + 0x10) = *(undefined2 *)(iVar6 + 0x20);
  *(undefined2 *)(iVar5 + 0x12) = *(undefined2 *)(iVar6 + 0x24);
  *(undefined2 *)(iVar5 + 0x14) = *(undefined2 *)(iVar6 + 8);
  uVar15 = *0x20000218;
  if (uVar13 == 0x6f) {
    *(undefined2 *)(iVar5 + 0x16) = uVar15;
  }
  else {
    *(undefined2 *)(iVar5 + 0x16) = *(undefined2 *)(iVar6 + 0xc);
  }
  *(undefined2 *)(iVar5 + 0x18) = *(undefined2 *)(iVar6 + 0x28);
  *(undefined2 *)(iVar5 + 0x1a) = *(undefined2 *)(iVar6 + 0x2c);
  *(short *)(iVar5 + 0x1c) = (short)(*(uint *)(iVar6 + 0x30) / 1000);
  *(short *)(iVar5 + 0x1e) = (short)(*(uint *)(iVar6 + 0x34) / 1000);
  *(undefined2 *)(iVar5 + 0x20) = *(undefined2 *)(iVar6 + 0x38);
  *(undefined2 *)(iVar5 + 0x22) = uVar3;
  *(short *)(iVar5 + 0x24) = sVar1;
  *(undefined2 *)(iVar5 + 0x26) = uVar14;
  *(undefined2 *)(iVar5 + 0x28) = *(undefined2 *)(iVar6 + 0x10);
  if (uVar13 != 0x6e) {
    uVar15 = *(undefined2 *)(iVar6 + 0x14);
  }
  *(undefined2 *)(iVar5 + 0x2a) = uVar15;
  *(undefined2 *)(iVar5 + 0x2c) = *(undefined2 *)(iVar6 + 0x18);
  *(undefined2 *)(iVar5 + 0x2e) = *(undefined2 *)(iVar6 + 0x1c);
  puVar9 = 0x200001d8;
  puVar8 = 0x200001f8;
  *(ushort *)(iVar5 + 0x30) = uVar2;
  uVar14 = *puVar8;
  uVar3 = *puVar9;
  if (uVar13 == 0x10) {
    uVar14 = *(undefined2 *)(iVar4 + 0x2e);
    goto switchD_080078d8_caseD_b;
  }
  if (uVar13 < 0x11) {
    if (uVar13 - 0xb < 5) {
                    /* WARNING: Could not recover jumptable at 0x080078d8. Too many branches */
                    /* WARNING: Treating indirect jump as call */
      (*(code *)(&switchD_080078d8::switchdataD_080078dc + (uint)*(byte *)(uVar13 + 0x80078d1) * 2))
                ();
      return;
    }
  }
  else {
    if (uVar13 == 0x11) {
      uVar14 = *(undefined2 *)(iVar4 + 0x2a);
      goto switchD_080078d8_caseD_b;
    }
    if (uVar13 == 0x12) {
      uVar14 = *(undefined2 *)(iVar4 + 0x1f0);
      goto switchD_080078d8_caseD_b;
    }
    if ((uVar13 == 0x73) || (uVar13 == 0x74)) goto switchD_080078d8_caseD_b;
  }
  uVar14 = *(undefined2 *)(iVar6 + 0x50);
switchD_080078d8_caseD_b:
  *(undefined2 *)(iVar5 + 0x32) = uVar14;
  puVar8 = 0x20000058;
  *(undefined2 *)(iVar5 + 0x34) = *0x20000058;
  *(undefined2 *)(iVar5 + 0x36) = *0x20000056;
  if (uVar13 == 0x6e) {
    *(undefined2 *)(iVar5 + 0x38) = uVar3;
  }
  else {
    *(undefined2 *)(iVar5 + 0x38) = *(undefined2 *)(iVar4 + 0x28);
  }
  pcVar11 = 0x20000208;
  puVar9 = 0x200001fa;
  *(undefined2 *)(iVar5 + 0x3a) = *0x200001fa;
  if (*pcVar11 == '\x01') {
    modbus_pack_regs();
    puVar12 = 0x20000324;
    *0x20000324 = 0x20000348;
    puVar12[1] = 0x20000370;
    *(undefined1 *)(puVar12 + 2) = *0x20000209;
    *(undefined1 *)((int)puVar12 + 9) = 0;
    modbus_process();
    if (*(char *)(iVar5 + 3) == '\0') {
      rs485_send(puVar12[1],*(undefined1 *)((int)puVar12 + 9));
    }
    puVar12 = 0x20000324;
    *pcVar11 = '\0';
    puVar7 = 0x20000032;
    *(undefined2 *)(iVar4 + 0x24) = *(undefined2 *)(puVar12 + -7);
    *(undefined2 *)(iVar4 + 0x20) = *(undefined2 *)((int)puVar12 + -0x1e);
    *(uint *)(iVar6 + 4) = (uint)*(ushort *)(puVar12 + -8);
    *(uint *)(iVar6 + 0x10) = (uint)*(ushort *)((int)puVar12 + -0x22);
    *(uint *)(iVar6 + 0x14) = (uint)*(ushort *)(puVar12 + -9);
    *(uint *)(iVar6 + 0x18) = (uint)*(ushort *)((int)puVar12 + -0x26);
    *(uint *)(iVar6 + 0x1c) = (uint)*(ushort *)(puVar12 + -10);
    uVar2 = *(ushort *)((int)puVar12 + -0x2a);
    *puVar7 = uVar2;
    *(uint *)(iVar6 + 0x50) = (uint)*(ushort *)(puVar12 + -0xb);
    puVar10 = 0x20000056;
    *puVar8 = *(undefined2 *)((int)puVar12 + -0x2e);
    *puVar10 = *(undefined2 *)(puVar12 + -0xc);
    if (uVar2 != 0x6e) {
      *(undefined2 *)(iVar4 + 0x28) = *(undefined2 *)((int)puVar12 + -0x32);
    }
    *puVar9 = *(undefined2 *)(puVar12 + -0xd);
  }
  return;
}


// ===== config_init @ 08007a08 (size 20)

undefined4 config_init(void)

{
  config_load();
  serial_check();
  return 1;
}


// ===== config_save @ 08007a20 (size 94)

/* Erase+program page 0x0800F000 */

void config_save(undefined4 param_1)

{
  undefined4 *puVar1;
  int iVar2;
  uint uVar3;
  uint uVar4;
  
  puVar1 = 0x20000398;
  *0x20000398 = 0x7e4;
  puVar1[1] = param_1;
  FLASH_Unlock();
  FLASH_ClearFlag(0x34);
  iVar2 = 0x800f000;
  FLASH_ErasePage(0x800f000);
  uVar4 = 0;
  do {
    uVar3 = puVar1[uVar4];
    FLASH_ProgramHalfWord(iVar2 + uVar4 * 4,uVar3 & 0xffff);
    FLASH_ProgramHalfWord(iVar2 + 2 + uVar4 * 4,uVar3 >> 0x10);
    uVar4 = uVar4 + 1 & 0xff;
  } while (uVar4 < 0xaf);
  *(uint *)(0x40022000/*FLASH*/ + 0x10) = *(uint *)(0x40022000/*FLASH*/ + 0x10) | 0x80;
  return;
}


// ===== set_current_dac @ 08007a7c (size 1156)

/* Range select, calibration, slew, DAC ch2 */

void set_current_dac(uint param_1)

{
  undefined1 *puVar1;
  int iVar2;
  ushort *puVar3;
  undefined2 *puVar4;
  short sVar5;
  int iVar7;
  uint uVar8;
  int iVar9;
  undefined2 uVar10;
  uint uVar11;
  ushort uVar12;
  uint unaff_r7;
  ushort uVar6;
  
  puVar1 = 0x200001e8;
  iVar9 = 0x42218000;
  sVar5 = *0x20000032;
  if (sVar5 == 0x6f) {
    *(uint *)(0x200001e8 + 0x18) = param_1;
    *(undefined2 *)(puVar1 + 0xe) = *(undefined2 *)(puVar1 + 2);
LAB_08007aac:
    *(uint *)(iVar9 + 0x188) = (uint)(0x157c < *0x200001d4);
  }
  else if ((sVar5 == 0x6e) || (sVar5 == 0x6f)) goto LAB_08007aac;
  iVar2 = 0x20000398;
  if ((sVar5 == 0x6e) || (sVar5 == 0x6f)) {
LAB_08007ca8:
    if (*(uint *)(0x20000654 + 0x14) < 0x157d) goto LAB_08007cb0;
LAB_08007cba:
    iVar9 = GPIO_ReadInputDataBit(0x40010c00/*GPIOB*/,4);
    if (iVar9 == 1) {
      uVar8 = (uint)*(ushort *)(puVar1 + 0x12) *
              (uint)*(ushort *)(puVar1 + 0xc) * *(int *)(puVar1 + 0x18);
      goto LAB_08007d02;
    }
    if (*(short *)(iVar2 + 0x1ec) == 0) {
      unaff_r7 = 0;
    }
  }
  else {
    if ((*(int *)(0x20000654 + 4) == 0) &&
       (((*(int *)(0x20000654 + 0xc) == 0 && (*(uint *)(0x20000654 + 0x14) < 0x157d)) &&
        (iVar7 = GPIO_ReadInputDataBit(0x40010c00/*GPIOB*/,4), iVar7 == 1)))) {
      *(undefined4 *)(iVar9 + 0x188) = 0;
      *(undefined2 *)(puVar1 + 0x14) = 0;
    }
    puVar4 = 0x20000038;
    puVar3 = 0x20000044;
    uVar8 = *(uint *)(0x20000654 + 0x14);
    if (uVar8 == *0x20000044) goto LAB_08007ca8;
    *(undefined2 *)(puVar1 + 0xe) = 0;
    *puVar4 = 0;
    if (uVar8 != 0) {
      if (uVar8 < 0x157d) {
        *(undefined4 *)(iVar9 + 0x188) = 0;
        *(undefined2 *)(puVar1 + 0x14) = 0;
        if (uVar8 < 0xb) {
          uVar10 = *(undefined2 *)(iVar2 + 300);
        }
        else if (uVar8 < 0x15) {
          uVar10 = *(undefined2 *)(iVar2 + 0x12e);
        }
        else if (uVar8 < 0x1f) {
          uVar10 = *(undefined2 *)(iVar2 + 0x130);
        }
        else if (uVar8 < 0x29) {
          uVar10 = *(undefined2 *)(iVar2 + 0x132);
        }
        else if (uVar8 < 0x33) {
          uVar10 = *(undefined2 *)(iVar2 + 0x134);
        }
        else if (uVar8 < 0x3d) {
          uVar10 = *(undefined2 *)(iVar2 + 0x136);
        }
        else if (uVar8 < 0x47) {
          uVar10 = *(undefined2 *)(iVar2 + 0x138);
        }
        else if (uVar8 < 0x51) {
          uVar10 = *(undefined2 *)(iVar2 + 0x13a);
        }
        else if (uVar8 < 0x5b) {
          uVar10 = *(undefined2 *)(iVar2 + 0x13c);
        }
        else if (uVar8 < 0x65) {
          uVar10 = *(undefined2 *)(iVar2 + 0x13e);
        }
        else if (uVar8 < 0xc9) {
          uVar10 = *(undefined2 *)(iVar2 + 0x140);
        }
        else if (uVar8 < 0x12d) {
          uVar10 = *(undefined2 *)(iVar2 + 0x142);
        }
        else if (uVar8 < 0x191) {
          uVar10 = *(undefined2 *)(iVar2 + 0x144);
        }
        else if (uVar8 < 0x1f5) {
          uVar10 = *(undefined2 *)(iVar2 + 0x146);
        }
        else if (uVar8 < 0x259) {
          uVar10 = *(undefined2 *)(iVar2 + 0x148);
        }
        else if (uVar8 < 0x2bd) {
          uVar10 = *(undefined2 *)(iVar2 + 0x14a);
        }
        else if (uVar8 < 0x321) {
          uVar10 = *(undefined2 *)(iVar2 + 0x14c);
        }
        else if (uVar8 < 0x385) {
          uVar10 = *(undefined2 *)(iVar2 + 0x14e);
        }
        else if (uVar8 < 0x3e9) {
          uVar10 = *(undefined2 *)(iVar2 + 0x150);
        }
        else if (uVar8 < 0x7d1) {
          uVar10 = *(undefined2 *)(iVar2 + 0x152);
        }
        else if (uVar8 < 0xbb9) {
          uVar10 = *(undefined2 *)(iVar2 + 0x154);
        }
        else if (uVar8 < 0xfa1) {
          uVar10 = *(undefined2 *)(iVar2 + 0x156);
        }
        else {
          uVar10 = *(undefined2 *)(iVar2 + 0x158);
        }
LAB_08007ca4:
        *(undefined2 *)(puVar1 + 0x12) = uVar10;
      }
      else {
        *(undefined4 *)(iVar9 + 0x188) = 1;
        *(undefined2 *)(puVar1 + 0x14) = 1;
        if (uVar8 < 0x1771) {
          uVar10 = *(undefined2 *)(iVar2 + 0x15a);
          goto LAB_08007ca4;
        }
        if (uVar8 < 0x1b59) {
          uVar10 = *(undefined2 *)(iVar2 + 0x15c);
          goto LAB_08007ca4;
        }
        if (uVar8 < 0x1f41) {
          uVar10 = *(undefined2 *)(iVar2 + 0x15e);
          goto LAB_08007ca4;
        }
        if (uVar8 < 0x2329) {
          uVar10 = *(undefined2 *)(iVar2 + 0x160);
          goto LAB_08007ca4;
        }
        if (uVar8 < 0x2711) {
          uVar10 = *(undefined2 *)(iVar2 + 0x162);
          goto LAB_08007ca4;
        }
        if (uVar8 < 0x4e21) {
          uVar10 = *(undefined2 *)(iVar2 + 0x176);
          goto LAB_08007ca4;
        }
        if (uVar8 < 0x7531) {
          uVar10 = *(undefined2 *)(iVar2 + 0x18a);
          goto LAB_08007ca4;
        }
        if (uVar8 < 0x9c41) {
          uVar10 = *(undefined2 *)(iVar2 + 0x19e);
          goto LAB_08007ca4;
        }
        if (uVar8 < 0xc351) {
          uVar10 = *(undefined2 *)(iVar2 + 0x1b2);
          goto LAB_08007ca4;
        }
        if (uVar8 < 0xea61) {
          uVar10 = *(undefined2 *)(iVar2 + 0x1c6);
          goto LAB_08007ca4;
        }
      }
      *puVar3 = (ushort)uVar8;
      goto LAB_08007ca8;
    }
    *(undefined2 *)(puVar1 + 0x12) = *(undefined2 *)(iVar2 + 0x12a);
LAB_08007cb0:
    iVar9 = GPIO_ReadInputDataBit(0x40010c00/*GPIOB*/,4);
    if (iVar9 != 0) goto LAB_08007cba;
    uVar8 = (uint)*(ushort *)(puVar1 + 0x12) *
            (uint)*(ushort *)(puVar1 + 0xc) * *(int *)(puVar1 + 0x18) * 10;
LAB_08007d02:
    unaff_r7 = ((uint)*(ushort *)(puVar1 + 0xe) * (uVar8 / 10000)) /
               ((uint)*(ushort *)(iVar2 + 0x2a) * 100);
  }
  if (*(short *)(iVar2 + 0x20) == 4) {
    if (*(short *)(puVar1 + 0x14) == 0) {
      uVar8 = (uint)*(ushort *)(puVar1 + 0x12) *
              (uint)*(ushort *)(puVar1 + 0xc) * *(int *)(puVar1 + 0x18) * 10;
    }
    else {
      iVar9 = GPIO_ReadInputDataBit(0x40010c00/*GPIOB*/,4);
      if (iVar9 != 1) {
        if (*(short *)(iVar2 + 0x1ec) == 0) {
          unaff_r7 = 0;
        }
        goto LAB_08007d70;
      }
      uVar8 = (uint)*(ushort *)(puVar1 + 0x12) *
              (uint)*(ushort *)(puVar1 + 0xc) * *(int *)(puVar1 + 0x18);
    }
    unaff_r7 = ((uint)*(ushort *)(puVar1 + 2) * (uVar8 / 10000)) /
               ((uint)*(ushort *)(iVar2 + 0x2a) * 100);
  }
LAB_08007d70:
  iVar9 = 0x20000654;
  if (*0x20000032 == 0x6f) goto LAB_08007eee;
  if (((*(int *)(0x20000654 + 4) != 1) || (*(uint *)(0x20000654 + 0x14) == 0)) ||
     (*(uint *)(0x20000654 + 8) <= (uint)*(ushort *)(iVar2 + 0x2c))) {
    *(undefined4 *)(puVar1 + 0x18) = 0;
    *(undefined2 *)(puVar1 + 8) = 0;
    *(undefined2 *)(puVar1 + 10) = 0;
    unaff_r7 = 0;
    *(undefined2 *)(puVar1 + 0xe) = 0;
    DAC_SetChannel2Data(0,0);
    goto LAB_08007eee;
  }
  if (*(uint *)(0x20000654 + 0x14) < 0x157d) {
    if (*(short *)(puVar1 + 6) == 0) {
      *(undefined2 *)(puVar1 + 6) = 1;
      unaff_r7 = 0;
      *(undefined2 *)(puVar1 + 0xe) = 0;
      DAC_SetChannel2Data(0,0);
    }
    else {
      uVar8 = *(uint *)(puVar1 + 0x18);
      if ((uVar8 < param_1) &&
         (sVar5 = *(short *)(puVar1 + 4), *(short *)(puVar1 + 4) = sVar5 + 1, sVar5 != 0)) {
        *(undefined2 *)(puVar1 + 4) = 0;
        uVar11 = (uint)*(ushort *)(iVar2 + 0x1a);
        if ((uVar11 < uVar8) && ((uVar8 < param_1 - uVar11 || (uVar11 + param_1 < uVar8)))) {
          uVar8 = uVar8 + uVar11;
        }
        else {
          uVar8 = uVar8 + 1;
        }
        *(uint *)(puVar1 + 0x18) = uVar8;
      }
      if ((param_1 < uVar8) &&
         (sVar5 = *(short *)(puVar1 + 4), *(short *)(puVar1 + 4) = sVar5 + 1, sVar5 != 0)) {
        *(undefined2 *)(puVar1 + 4) = 0;
        goto LAB_08007e4a;
      }
    }
  }
  else {
    *(undefined2 *)(puVar1 + 10) = 0;
    *(undefined2 *)(puVar1 + 6) = 0;
    uVar8 = *(uint *)(puVar1 + 0x18);
    if ((uVar8 < param_1) &&
       (sVar5 = *(short *)(puVar1 + 4), *(short *)(puVar1 + 4) = sVar5 + 1, sVar5 != 0)) {
      *(undefined2 *)(puVar1 + 4) = 0;
      uVar11 = (uint)*(ushort *)(iVar2 + 0x1a);
      if ((uVar11 < uVar8) && ((uVar8 < param_1 - uVar11 || (uVar11 + param_1 < uVar8)))) {
        uVar8 = uVar8 + uVar11;
      }
      else {
        uVar8 = uVar8 + 1;
      }
      *(uint *)(puVar1 + 0x18) = uVar8;
    }
    if ((param_1 < uVar8) &&
       (sVar5 = *(short *)(puVar1 + 4), *(short *)(puVar1 + 4) = sVar5 + 1, sVar5 != 0)) {
      *(undefined2 *)(puVar1 + 4) = 0;
LAB_08007e4a:
      uVar11 = (uint)*(ushort *)(iVar2 + 0x1c);
      if ((uVar11 < uVar8) && ((uVar8 < param_1 - uVar11 || (uVar11 + param_1 < uVar8)))) {
        iVar7 = -uVar11;
      }
      else {
        iVar7 = -1;
      }
      *(uint *)(puVar1 + 0x18) = uVar8 + iVar7;
    }
  }
  if (*(uint *)(iVar9 + 0x14) < 0x157d) {
    *(undefined2 *)(puVar1 + 8) = 0;
    sVar5 = *(short *)(puVar1 + 10);
    if (sVar5 != 0) goto LAB_08007ea2;
    uVar6 = *(ushort *)(puVar1 + 0xe);
    if (*(ushort *)(iVar2 + 0x18) <= uVar6) {
      *(undefined2 *)(puVar1 + 10) = 1;
      goto LAB_08007eee;
    }
    sVar5 = *(short *)(puVar1 + 4);
    *(short *)(puVar1 + 4) = sVar5 + 1;
    if (sVar5 == 0) goto LAB_08007eee;
    *(undefined2 *)(puVar1 + 4) = 0;
LAB_08007ec6:
    sVar5 = uVar6 + 1;
  }
  else {
    sVar5 = *(short *)(puVar1 + 8);
    if (sVar5 == 0) {
      uVar6 = *(ushort *)(puVar1 + 0xe);
      if (*(ushort *)(iVar2 + 0x18) <= uVar6) {
        *(undefined2 *)(puVar1 + 8) = 1;
        goto LAB_08007eee;
      }
      sVar5 = *(short *)(puVar1 + 4);
      *(short *)(puVar1 + 4) = sVar5 + 1;
      if (sVar5 == 0) goto LAB_08007eee;
      *(undefined2 *)(puVar1 + 4) = 0;
      goto LAB_08007ec6;
    }
LAB_08007ea2:
    if (sVar5 != 1) goto LAB_08007eee;
    sVar5 = *(short *)(puVar1 + 2);
  }
  *(short *)(puVar1 + 0xe) = sVar5;
LAB_08007eee:
  if ((*(uint *)(puVar1 + 0x18) == param_1) && (*(short *)(puVar1 + 0xe) == *(short *)(puVar1 + 2)))
  {
    *puVar1 = 1;
  }
  else {
    *puVar1 = 0;
  }
  uVar6 = *(ushort *)(puVar1 + 2);
  if (uVar6 <= unaff_r7) {
    unaff_r7 = (uint)uVar6;
  }
  uVar12 = (ushort)unaff_r7;
  if ((*(short *)(iVar2 + 0x20) == 0) && (*(int *)(0x20000654 + 0x10) == 0)) {
    uVar12 = uVar6;
  }
  DAC_SetChannel2Data(0,uVar12);
  return;
}


// ===== set_voltage_dac @ 08007f2c (size 68)

/* DAC ch1 */

void set_voltage_dac(int param_1)

{
  short sVar1;
  ushort uVar2;
  uint uVar3;
  uint uVar4;
  
  uVar4 = (uint)*(ushort *)(0x200001e8 + 2);
  uVar3 = (uVar4 * ((uint)(0x186a0 * param_1) / (uint)*(ushort *)(0x200001e8 + 0x10))) /
          ((uint)*(ushort *)(0x20000398 + 0x2a) * 100);
  if (uVar4 <= uVar3) {
    uVar3 = uVar4;
  }
  sVar1 = *(short *)(0x20000398 + 0x20);
  uVar2 = (ushort)uVar3;
  if ((((sVar1 == 1) || (sVar1 == 2)) || (sVar1 == 3)) && (*(int *)(0x20000654 + 0x10) == 0)) {
    uVar2 = *(ushort *)(0x200001e8 + 2);
  }
  DAC_SetChannel1Data(0,uVar2);
  return;
}


// ===== lcd_write @ 08007f80 (size 178)

/* HD44780 4-bit on GPIOE (dead code: GPIOE clock never enabled) */

void lcd_write(int param_1,uint param_2)

{
  undefined4 *puVar1;
  undefined4 *puVar2;
  
  puVar1 = 0x40011810;
  puVar2 = 0x40011810 + 1;
  if (param_1 == 0) {
    *puVar2 = 0x1000;
  }
  else {
    *0x40011810 = 0x1000;
  }
  *puVar1 = 0x2000;
  if ((int)(param_2 << 0x1b) < 0) {
    *puVar1 = 0x100;
  }
  else {
    *puVar2 = 0x100;
  }
  if ((int)(param_2 << 0x1a) < 0) {
    *puVar1 = 0x200;
  }
  else {
    *puVar2 = 0x200;
  }
  if ((int)(param_2 << 0x19) < 0) {
    *puVar1 = 0x400;
  }
  else {
    *puVar2 = 0x400;
  }
  if ((int)(param_2 << 0x18) < 0) {
    *puVar1 = 0x800;
  }
  else {
    *puVar2 = 0x800;
  }
  delay_us(0x19);
  *puVar2 = 0x2000;
  delay_us(0x19);
  *puVar1 = 0x2000;
  if ((param_2 & 1) == 0) {
    *puVar2 = 0x100;
  }
  else {
    *puVar1 = 0x100;
  }
  if ((int)(param_2 << 0x1e) < 0) {
    *puVar1 = 0x200;
  }
  else {
    *puVar2 = 0x200;
  }
  if ((int)(param_2 << 0x1d) < 0) {
    *puVar1 = 0x400;
  }
  else {
    *puVar2 = 0x400;
  }
  if ((int)(param_2 << 0x1c) < 0) {
    *puVar1 = 0x800;
  }
  else {
    *puVar2 = 0x800;
  }
  delay_us(0x19);
  *puVar2 = 0x2000;
  delay_us(0x19);
  return;
}


