#include "revlib\ASA_DEVICE.h"
#include "revlib\ASA_core_lib.h"
#include "revlib\ASA_SPI.h"
#include "revlib\ASA_DIO.h"
#include "revlib\ASA_STDIO.h"



int main() {
    ASA_STDIO_set();
    printf("Test by LiYu 2017.11.10\n");

    ASA_ID_init();
    ASA_ID_set(1);
    ASA_slave_cs(0);
    _delay_ms(1);
    ASA_slave_cs(1);
    _delay_ms(30);

    ASA_MSTER_spi_set();
    SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR1)|(0<<SPR0);
    // printf("Test by LiYu 2017.11.10\n");

    uint8_t res[4] = {0};
    uint8_t tmp;
    int input ;
    res[0] = M128_SPI_swap(0b10101100);
    res[1] = M128_SPI_swap(0b01010011);
    res[2] = M128_SPI_swap(0b00000000);
    res[3] = M128_SPI_swap(0b11111111);
    printf("1 %02x 2 %02x 3 %02x 4 %02x\n", res[0], res[1], res[2], res[3]);

    // scanf("%d", &input);
    // tmp = input;
    // res[0] = M128_SPI_swap(0b00111000);
    // res[1] = M128_SPI_swap(0b00000000);
    // res[2] = M128_SPI_swap(tmp);
    // res[3] = M128_SPI_swap(0b00000000);
    // printf("1 %02x 2 %02x 3 %02x 4 %02x\n", res[0], res[1], res[2], res[3]);
    // Read Calibration Byte

    res[0] = M128_SPI_swap(0b00110000);
    res[1] = M128_SPI_swap(0b00000000);
    res[2] = M128_SPI_swap(0);
    res[3] = M128_SPI_swap(0b00000000);
    printf("1 %02x 2 %02x 3 %02x 4 %02x\n", res[0], res[1], res[2], res[3]);
    // Read Signature Byte
    res[0] = M128_SPI_swap(0b00110000);
    res[1] = M128_SPI_swap(0b00000000);
    res[2] = M128_SPI_swap(1);
    res[3] = M128_SPI_swap(0b00000000);
    printf("1 %02x 2 %02x 3 %02x 4 %02x\n", res[0], res[1], res[2], res[3]);
    // Read Signature Byte
    res[0] = M128_SPI_swap(0b00110000);
    res[1] = M128_SPI_swap(0b00000000);
    res[2] = M128_SPI_swap(2);
    res[3] = M128_SPI_swap(0b00000000);
    printf("1 %02x 2 %02x 3 %02x 4 %02x\n", res[0], res[1], res[2], res[3]);
    // Read Signature Byte

    return 0;
}
