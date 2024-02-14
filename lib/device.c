#include "device.h"
#include "log.h"

void DEV_GPIO_Init() {
    DEV_GPIO_Mode(LCD_CS, 1);
    DEV_GPIO_Mode(LCD_RST, 1);
    DEV_GPIO_Mode(LCD_DC, 1);
    DEV_GPIO_Mode(LCD_BL, 1);

    DEV_Digital_Write(LCD_CS, 1);
    DEV_Digital_Write(LCD_BL, 1);
}

/******************************************************************************
function:	Module Initialize, the library and initialize the pins, SPI protocol
parameter:
Info:
******************************************************************************/
uint8_t DEV_ModuleInit(void) {
    if (!bcm2835_init()) {
        printf("bcm2835 init failed  !!! \r\n");
        return 1;
    } else {
        printf("bcm2835 init success !!! \r\n");
    }
    DEV_GPIO_Init();

    // Start SPI
    bcm2835_spi_begin();
    bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);
    bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);
    bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_8);
    bcm2835_spi_chipSelect(BCM2835_SPI_CS0);
    bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, LOW);

    return 0;
}

/******************************************************************************
function:	Module exits, closes SPI and BCM2835 library
parameter:
Info:
******************************************************************************/
void DEV_ModuleExit(void) {
    bcm2835_spi_end();
    bcm2835_close();
}

void DEV_GPIO_Mode(uint16_t Pin, uint16_t Mode) {
    if (Mode == 0 || Mode == BCM2835_GPIO_FSEL_INPT) {
        bcm2835_gpio_fsel(Pin, BCM2835_GPIO_FSEL_INPT);
    } else {
        bcm2835_gpio_fsel(Pin, BCM2835_GPIO_FSEL_OUTP);
    }
}

void DEV_Digital_Write(uint16_t Pin, uint8_t Value) { bcm2835_gpio_write(Pin, Value); }

uint8_t DEV_Digital_Read(uint16_t Pin) { return bcm2835_gpio_lev(Pin); }

void DEV_SPI_WriteByte(uint8_t Value) { bcm2835_spi_transfer(Value); }

void delay_ms(unsigned int ms) { bcm2835_delay(ms); }
