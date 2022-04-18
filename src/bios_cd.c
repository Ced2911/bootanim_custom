// wrapper arround cd bios function
#include "bios_cd.h"
#include <stdint.h>

int bios_cd_is_present()
{
    // init cd
    int (*bios_cd_init)(int arg) = (void *)BIOS_CD_INIT_ADDR;
    int (*bios_cd_get_status)(void *) = (void *)BIOS_CD_GET_STATUS_ADDR;

    uint8_t stat[16];

    int init = bios_cd_init(1);
    int status = bios_cd_get_status(stat);

    /*
    uint32_t *dbg = (uint32_t *)(0x0600ff00);
    uint32_t *s = stat;
    dbg[0] = *s++;
    dbg[1] = *s++;
    dbg[2] = *s++;
    dbg[3] = *s++;
    dbg[4] = stat;
    dbg[5] = init;
    dbg[6] = status;
    dbg[7] = stat[0] & 0xF;
    */

    switch (stat[0] & 0xF)
    {
    case CD_STATUS_BUSY:
        return 0;
    case CD_STATUS_OPEN:
        return 0;
    case CD_STATUS_NODISC:
        return 0;
    default:
        break;
    }

    return 1;
}
