using System;
using System.IO;
using System.Runtime.InteropServices;


namespace d3m0n_touchscreen
{
    partial class manager
    {
        int MAX_LEN = 3;

        int XPT_START = 0x80;
        int XPT_XPOS = 0x50;
        int XPT_YPOS = 0x10;
        int XPT_8BIT = 0x80;
        int XPT_SER = 0x04;
        int XPT_DEF = 0x03;

        public static void xptGetxy(int chipSelect)
        {
            bcm2835_spi_begin();

            bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);
            bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_1024);
            //bcm2835_spi_chipSelect(BCM2835_SPI_CS1);
            bcm2835_spi_chipSelect(chipSelect);
            //bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS1, LOW);
            bcm2835_spi_setChipSelectPolarity(chipSelect, LOW);

        #if 0
                *xp = xptGetit(XPT_START | XPT_XPOS);
                *yp = xptGetit(XPT_START | XPT_YPOS);
        #endif
                *xp = xptGetit(XPT_START | XPT_XPOS | XPT_SER);
                *yp = xptGetit(XPT_START | XPT_YPOS | XPT_SER);

                bcm2835_spi_end();
        }

        int xptGetit(int cmd)
        {
            char rbuf[MAX_LEN];
            char wbuf[MAX_LEN];

            memset(wbuf, 0, sizeof(rbuf));
            memset(rbuf, 0, sizeof(rbuf));
            wbuf[0] = cmd;
            bcm2835_spi_transfernb(wbuf, rbuf, sizeof(wbuf));
            
            //return((rbuf[1]<<8)+rbuf[2]);
            return((rbuf[1]<<4) + (rbuf[2]>>4));
        }
    }
}