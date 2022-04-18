#pragma once


#define BIOS_CD_GET_STATUS_ADDR     (0x2cb8)
#define BIOS_CD_INIT_ADDR           (0x1f02)


//iapetus copy/paste
#define CD_STATUS_BUSY             0x00
#define CD_STATUS_PAUSE            0x01
#define CD_STATUS_STANDBY          0x02
#define CD_STATUS_PLAY             0x03
#define CD_STATUS_SEEK             0x04
#define CD_STATUS_SCAN             0x05
#define CD_STATUS_OPEN             0x06
#define CD_STATUS_NODISC           0x07
#define CD_STATUS_RETRY            0x08
#define CD_STATUS_ERROR            0x09
#define CD_STATUS_FATAL            0x0a
#define CD_STATUS_PERIODIC         0x20
#define CD_STATUS_TRANSFER         0x40
#define CD_STATUS_WAIT             0x80
#define CD_STATUS_REJECT           0xff

