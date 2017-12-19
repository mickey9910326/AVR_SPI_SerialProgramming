#include "revlib/ASA/ASA_spi.h"
#include "revlib/ASA/ASA_general.h"
#include "revlib/rev_serial.h"
#include "command.h"
#include "stk500.h"

uint8_t serial_get() {
    while((UCSR0A&(1<<RXC0))==0)
        ;
	return UDR0;
}

uint8_t serial_put(uint8_t data) {
    while((UCSR0A&(1<<UDRE0))==0)
        ;
    UDR0 = data;
    return 0;
}

#define STATUS_START  0
#define STATUS_SEQ    1
#define STATUS_MSG1  2
#define STATUS_MSG2  3
#define STATUS_TOKEN  4
#define STATUS_DATA   5
#define STATUS_CHKSUM 6

#define STK_START 0x1B
#define STK_SEQ   0x01
#define STK_TOKEN 0x0E

Addres_t CurAddres;
uint8_t sequence = 0;

struct cmd {
    uint16_t bytes;
    uint8_t data[256];
} GetCmd, ResCmd;

uint8_t get_cmd() {
    uint8_t ch;
    uint8_t chksum = 0;

    ch = serial_get();
    chksum ^= ch;
    if (ch != STK_START)
        return 1;

    ch = serial_get();
    chksum ^= ch;
    if (ch != sequence+1)
        return 2;
    sequence++;

    ch = serial_get();
    chksum ^= ch;
    GetCmd.bytes = ch;

    ch = serial_get();
    chksum ^= ch;
    GetCmd.bytes = (GetCmd.bytes<<8) + ch;

    ch = serial_get();
    chksum ^= ch;
    if (ch != STK_TOKEN)
        return 3;

    for (uint16_t i = 0; i < GetCmd.bytes; i++) {
        ch = serial_get();
        chksum ^= ch;
        GetCmd.data[i] = ch;
    }

    ch = serial_get();
    if (ch != chksum)
        return 4;
    return 0;
}

uint8_t put_cmd(struct cmd* c) {
    uint8_t chksum=0;
    uint8_t ch;

    ch = STK_START;
    chksum ^= ch;
    serial_put(ch);

    ch = sequence;
    chksum ^= ch;
    serial_put(ch);

    ch = c->bytes>>8;
    chksum ^= ch;
    serial_put(ch);

    ch = c->bytes&0xFF;
    chksum ^= ch;
    serial_put(ch);

    ch = STK_TOKEN;
    chksum ^= ch;
    serial_put(ch);

    for (uint16_t i = 0; i < c->bytes; i++) {
        ch = c->data[i];
        chksum ^= ch;
        serial_put(ch);
    }
    serial_put(chksum);
    return 0;
}

int main() {
    rev_serial_init();
    uint8_t get_cmd_res;
    uint8_t spires[4];
    while (1) {
        get_cmd_res = get_cmd();
        if (get_cmd_res) {
            /* TODO error out*/
        }
        switch (GetCmd.data[0]) {
            case CMD_SIGN_ON: {
                ResCmd.bytes = 11;
                ResCmd.data[0] = CMD_SIGN_ON;
                ResCmd.data[1] = STATUS_CMD_OK;
                ResCmd.data[2] = 8;
                ResCmd.data[3] = 'A';
                ResCmd.data[4] = 'V';
                ResCmd.data[5] = 'R';
                ResCmd.data[6] = 'I';
                ResCmd.data[7] = 'S';
                ResCmd.data[8] = 'P';
                ResCmd.data[9] = '_';
                ResCmd.data[10] = '2';
                put_cmd(&ResCmd);
                break;
            }
            case CMD_SET_PARAMETER: {
                uint8_t id = GetCmd.data[1];
                uint8_t value = GetCmd.data[2];
                uint8_t res = isp_set_param(id,value);
                if(res) {
                    ResCmd.bytes = 2;
                    ResCmd.data[0] = CMD_SET_PARAMETER;
                    ResCmd.data[1] = STATUS_CMD_FAILED;
                    put_cmd(&ResCmd);
                } else {
                    ResCmd.bytes = 2;
                    ResCmd.data[0] = CMD_SET_PARAMETER;
                    ResCmd.data[1] = STATUS_CMD_OK;
                    put_cmd(&ResCmd);
                }
                break;
            }
            case CMD_GET_PARAMETER: {
                uint8_t id = GetCmd.data[1];
                uint8_t value = 1;
                uint8_t res = isp_get_param(id, &value);
                if(res) {
                    ResCmd.bytes = 2;
                    ResCmd.data[0] = CMD_GET_PARAMETER;
                    ResCmd.data[1] = STATUS_CMD_FAILED;
                    put_cmd(&ResCmd);
                } else {
                    ResCmd.bytes = 3;
                    ResCmd.data[0] = CMD_GET_PARAMETER;
                    ResCmd.data[1] = STATUS_CMD_OK;
                    ResCmd.data[2] = value;
                    put_cmd(&ResCmd);
                }
                break;
            }
            case CMD_OSCCAL: {
                // NOTE not implement
                break;
            }
            case CMD_SET_DEVICE_PARAMETERS: {
                // NOTE not implement
                break;
            }
            case CMD_LOAD_ADDRESS: {
                // MSB first
                CurAddres.ui8[0] = GetCmd.data[1];
                CurAddres.ui8[1] = GetCmd.data[2];
                CurAddres.ui8[2] = GetCmd.data[3];
                CurAddres.ui8[3] = GetCmd.data[4];
                ResCmd.bytes = 2;
                ResCmd.data[0] = CMD_LOAD_ADDRESS;
                ResCmd.data[1] = STATUS_CMD_OK;
                put_cmd(&ResCmd);
                break;
            }
            case CMD_FIRMWARE_UPGRADE: {
                // NOTE not implement
                break;
            }
            // ***[ STK ISP command constants ]******************************
            case CMD_ENTER_PROGMODE_ISP: {
                // 0 Command ID
                // 1 timeout
                // 2 stabDelay
                // 3 cmdexeDelay
                // 4 synchLoops
                // 5 byteDelay
                // 6 pollValue
                // 7 pollIndex
                // 8 cmd1
                // 9 cmd2
                // 10 cmd3
                // 11 cmd4
                isp_enter_progmode();
                spires[0] = spi_swap(GetCmd.data[ 8]);
                spires[1] = spi_swap(GetCmd.data[ 9]);
                spires[2] = spi_swap(GetCmd.data[10]);
                spires[3] = spi_swap(GetCmd.data[11]);
                ResCmd.data[0] = CMD_ENTER_PROGMODE_ISP;
                // if (GetCmd.data[6]!=spires[GetCmd.data[7]]) {
                //     ResCmd.bytes = 2;
                //     ResCmd.data[1] = STATUS_CMD_FAILED;
                //     put_cmd(&ResCmd);
                // } else if (0) {
                //     // TODO timeout
                //     ResCmd.data[1] = STATUS_CMD_TOUT;
                // } else {
                    ResCmd.bytes = 2;
                    ResCmd.data[1] = STATUS_CMD_OK;
                    put_cmd(&ResCmd);
                // }
                break;
            }
            case CMD_LEAVE_PROGMODE_ISP: {
                // 0 Command ID
                // 1 preDelay
                // 2 postDelay
                _delay_ms(GetCmd.data[1]);
                isp_leave_progmode();
                _delay_ms(GetCmd.data[2]);
                ResCmd.bytes = 2;
                ResCmd.data[0] = CMD_LEAVE_PROGMODE_ISP;
                ResCmd.data[1] = STATUS_CMD_OK;
                put_cmd(&ResCmd);
                break;
            }
            case CMD_CHIP_ERASE_ISP: {
                // 0 Command ID
                // 1 eraseDelay
                // 2 pollMethod
                // 3 cmd1
                // 4 cmd2
                // 5 cmd3
                // 6 cmd4

                spi_swap(GetCmd.data[3]);
                spi_swap(GetCmd.data[4]);
                spi_swap(GetCmd.data[5]);
                spi_swap(GetCmd.data[6]);
                _delay_ms(GetCmd.data[1]);

                ResCmd.data[0] = CMD_CHIP_ERASE_ISP;
                if (GetCmd.data[6]!=spires[GetCmd.data[7]]) {
                    ResCmd.bytes = 2;
                    ResCmd.data[1] = STATUS_CMD_FAILED;
                    put_cmd(&ResCmd);
                } else {
                    ResCmd.bytes = 2;
                    ResCmd.data[1] = STATUS_CMD_OK;
                    put_cmd(&ResCmd);
                }

                break;
            }
            case CMD_PROGRAM_FLASH_ISP: {
                // 0 Command ID
                // 1 NumBytes
                // 2 NumBytes
                // 3 mode
                // 4 delay
                // 5 cmd1
                // 6 cmd2
                // 7 cmd3
                // 8 cmd2
                // 9 poll1
                // 10 poll2
                // 11 Data (N bytes)

                break;
            }
            case CMD_READ_FLASH_ISP: {

                break;
            }
            case CMD_PROGRAM_EEPROM_ISP: {

                break;
            }
            case CMD_READ_EEPROM_ISP: {

                break;
            }
            case CMD_PROGRAM_FUSE_ISP:
            case CMD_PROGRAM_LOCK_ISP: {

                break;
            }
            case CMD_READ_FUSE_ISP:
            case CMD_READ_LOCK_ISP:
            case CMD_READ_SIGNATURE_ISP:
            case CMD_READ_OSCCAL_ISP: {
                // 0 Command ID
                // 1 RetAddr
                // 2 cmd1
                // 3 cmd2
                // 4 cmd3
                // 5 cmd4
                spi_swap(GetCmd.data[2]);
                spi_swap(GetCmd.data[3]);
                spi_swap(GetCmd.data[4]);
                uint8_t res = spi_swap(GetCmd.data[5]);
                ResCmd.bytes = 4;
                ResCmd.data[0] = CMD_READ_OSCCAL_ISP;
                ResCmd.data[1] = STATUS_CMD_OK;
                ResCmd.data[2] = res;
                ResCmd.data[3] = STATUS_CMD_OK;
                put_cmd(&ResCmd);
                break;
            }
            case CMD_SPI_MULTI: {
                // 0 Command ID
                // 1 NumTx
                // 2 NumRx
                // 3 RxStartAddr
                // 4 cmd1
                // 5 cmd2
                // 6 cmd3
                // 7 cmd4
                spires[0] = spi_swap(GetCmd.data[4]);
                spires[1] = spi_swap(GetCmd.data[5]);
                spires[2] = spi_swap(GetCmd.data[6]);
                spires[3] = spi_swap(GetCmd.data[7]);
                ResCmd.bytes = 7;
                ResCmd.data[0] = CMD_SPI_MULTI;
                ResCmd.data[1] = STATUS_CMD_OK;
                ResCmd.data[2] = spires[0];
                ResCmd.data[3] = spires[1];
                ResCmd.data[4] = spires[2];
                ResCmd.data[5] = spires[3];
                ResCmd.data[6] = STATUS_CMD_OK;
                put_cmd(&ResCmd);
                break;
            }

        }
    }
    return 0;
}
