#include "stk500.h"
#include "command.h"
#include "revlib/ASA/ASA_spi.h"
#include "revlib/ASA/ASA_general.h"

void isp_enter_progmode() {
    ASA_ID_init();
    ASA_SPIM_cs_init();
    ASA_ID_set(1);
    ASA_SPIM_cs(1);
    _delay_ms(10);
    spi_master_init();
    spi_set_frequency(devider_64);
    spi_en(1);
}

void isp_leave_progmode() {
    ASA_SPIM_cs(0);
    spi_en(0);
}

void isp_erise_chip(uint8_t delay) {
    spi_swap(0xAC);
    spi_swap(0x80);
    spi_swap(0x00);
    spi_swap(0x00);
    // Delay to ensure that the erase of the device is finished
    // Value would be received from PC software in STK500 protocol
    // XML path: /AVRPART/ICE_SETTINGS/STK500_2/IspChipErase/
    // Value can also find as "XML: eraseDelay" in the chip XML file
    _delay_ms(delay);
}

void isp_program_flash(/* arguments */) {

}
struct param {
    char vtarget;
    char vadjust;
    char pscale;
    char cmatch;
    char duration;
    char rst_polarity;
    char init;
};
struct param PARAM = {
    STK500_VTARGET,
    STK500_VADJUST,
    STK500_OSC_PSCALE,
    STK500_OSC_CMATCH,
    PARAM_SCK_DURATION,
    STK500_RESET_POLARITY,
    STK500_CONTROLLER_INIT
};

uint8_t isp_set_param(uint8_t id, uint8_t data) {
    switch (id) {
        // only writable parameter
        case PARAM_VTARGET: { PARAM.vtarget = data; break; }
        case PARAM_VADJUST: { PARAM.vadjust = data; break; }
        case PARAM_OSC_PSCALE: { PARAM.pscale = data; break; }
        case PARAM_OSC_CMATCH: { PARAM.cmatch = data; break; }
        case PARAM_SCK_DURATION: { PARAM.cmatch = data; break; }
        case PARAM_RESET_POLARITY: { PARAM.rst_polarity = data; break; }
        case PARAM_CONTROLLER_INIT: { PARAM.init = data; break; }
        default: { return 1; }
    }
    return 0;
}

uint8_t isp_get_param(uint8_t id, uint8_t* data) {
    switch (id) {
        // only readable parameter
        case PARAM_BUILD_NUMBER_LOW:   *data = STK500_BUILD_NUMBER_HIGH; break;
        case PARAM_BUILD_NUMBER_HIGH:  *data = STK500_BUILD_NUMBER_HIGH; break;
        case PARAM_HW_VER:             *data = STK500_HW_VER; break;
        case PARAM_SW_MAJOR:           *data = STK500_SW_MAJOR; break;
        case PARAM_SW_MINOR:           *data = STK500_SW_MINOR; break;
        case PARAM_VTARGET:            *data = PARAM.vtarget; break;
        case PARAM_VADJUST:            *data = PARAM.vadjust; break;
        case PARAM_OSC_PSCALE:         *data = PARAM.pscale; break;
        case PARAM_OSC_CMATCH:         *data = PARAM.cmatch; break;
        case PARAM_SCK_DURATION:       *data = STK500_SCK_DURATION; break;
        case PARAM_TOPCARD_DETECT:     *data = STK500_TOPCARD_DETECT; break;
        case PARAM_DATA:               *data = STK500_DATA; break;
        case PARAM_CONTROLLER_INIT:    *data = PARAM.init; break;
        default: return 1;
    }
    return 0;

}
