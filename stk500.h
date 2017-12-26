#ifndef __STK500_H__
#define __STK500_H__

/**
 * @file stk500.h
 * @author LiYu
 * @date 2017.12.26
 * @brief STK500 腳位定義、及硬體相關函式。
 *
 * 若要更改reset腳位，可更改 "STK 500 pin defines" 區域內定義。
 * "STK parameter constants" 區域，定義硬體資訊，如韌體版本、驅動電壓、SPI周期等，可
 * 參考 AVR068.pdf (AVR068: STK500 Communication Protocol)。
 *
 */

// *****************[ STK pin defines ]************************************
// 可更改 RST 以配合燒錄卡
#define RST       PF4
#define RST_PORT  PORTF
#define RST_DDR   DDRF
#define RST_PIN   PINF
#define RST_SHIFT RST
#define RST_MASK  (1<<RST)

// 低電位致能
// PIN腳經過ASABU有反向，填1；沒反向，填0
#define RST_EN  1

// *****************[ STK parameter constants ]***************************
#define STK500_BUILD_NUMBER_LOW              0
#define STK500_BUILD_NUMBER_HIGH             1
#define STK500_HW_VER                        2
#define STK500_SW_MAJOR                      2
#define STK500_SW_MINOR                      4
#define STK500_VTARGET                       50
#define STK500_VADJUST                       50
#define STK500_OSC_PSCALE                    2
#define STK500_OSC_CMATCH                    1
#define STK500_SCK_DURATION                  2
// default spi freq is 11059200/64 = 172800
#define STK500_TOPCARD_DETECT                0xAA
#define STK500_STATUS                        0x00
#define STK500_DATA                          0x00
#define STK500_RESET_POLARITY                1
#define STK500_CONTROLLER_INIT               0x00

#include <inttypes.h>

void isp_enter_progmode();
void isp_leave_progmode();
void isp_erise_chip(uint8_t delay);
void isp_program_flash();
uint8_t isp_set_param(uint8_t id, uint8_t data);
uint8_t isp_get_param(uint8_t id, uint8_t* data);
uint8_t isp_load_flash(uint8_t isHigh, uint8_t addr_a, uint8_t addr_b, uint8_t data);
uint8_t isp_write_flash(uint8_t addr_a, uint8_t addr_b);
uint8_t isp_write_eeprom(uint8_t addr_a, uint8_t addr_b, uint8_t data);

/**
 * @brief 4 bytes type of int, can access as uint8_t[4] or uint32_t.
 */
typedef union ADDRESS {
    uint32_t ui32;
    uint8_t ui8[4];
}Addres_t;

#endif // __STK500_H__
