#pragma once

#define DISP_HEIGHT 320                      // px
#define DISP_WIDTH 240                       // px
#define DISP_PIXEL_CLOCK_HZ 40 * 1000 * 1000 // 40MHz
#define DISP_CMD_BITS 8
#define DISP_PARAM_BITS 8

void ui_start(void);
void ui_init(void);