/**
 * @file  app_config.c
 * @brief Persistent configuration in the last-but-four flash page (0x0800F000).
 *
 * Layout is identical to the original firmware, so the factory calibration
 * already stored on a unit is reused as-is.
 */
#include "app.h"
#include "stm32f1xx_hal.h"
#include <string.h>

uint16_t g_cfg[CFG_WORDS] __attribute__((aligned(4)));

void cfg_load(void)
{
    const uint16_t *flash = (const uint16_t *)CFG_FLASH_ADDR;
    memcpy(g_cfg, flash, CFG_BYTES);

    if (g_cfg[0] != CFG_MAGIC_VALUE || g_cfg[1] != 0u) {
        /* blank or foreign page: fall back to the captured factory defaults */
        memcpy(g_cfg, g_cfg_defaults, CFG_BYTES);
        g_cfg[0] = CFG_MAGIC_VALUE;
        g_cfg[1] = 0u;
    }

    /* restore saved setpoints (only when inside the limits) */
    if (CFG(CFG_SAVED_I) <= CFG(CFG_IMAX)) g_st.set_i = CFG(CFG_SAVED_I);
    if (CFG(CFG_SAVED_V) <= CFG(CFG_VMAX)) g_st.set_v = CFG(CFG_SAVED_V);
}

void cfg_save(void)
{
    FLASH_EraseInitTypeDef erase = {
        .TypeErase   = FLASH_TYPEERASE_PAGES,
        .PageAddress = CFG_FLASH_ADDR,
        .NbPages     = 1,
    };
    uint32_t page_err = 0;

    g_cfg[0] = CFG_MAGIC_VALUE;
    g_cfg[1] = 0u;

    HAL_FLASH_Unlock();
    if (HAL_FLASHEx_Erase(&erase, &page_err) == HAL_OK) {
        for (uint32_t n = 0; n < CFG_WORDS; n++) {
            HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,
                              CFG_FLASH_ADDR + 2u * n, g_cfg[n]);
        }
    }
    HAL_FLASH_Lock();
    stream_event(EVT_CFG_SAVE, 0);
}
