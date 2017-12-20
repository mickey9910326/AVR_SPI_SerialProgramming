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
void isp_program_flash(/* arguments */);
uint8_t isp_set_param(uint8_t id, uint8_t data);
uint8_t isp_get_param(uint8_t id, uint8_t* data);
uint8_t isp_load_flash(uint8_t isHigh, uint8_t addr_a, uint8_t addr_b, uint8_t data);
uint8_t isp_write_flash(uint8_t addr_a, uint8_t addr_b);

typedef union ADDRESS {
    uint32_t ui32;
    uint8_t ui8[4];
}Addres_t;
