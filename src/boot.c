#include <stdint.h>
#include <iapetus.h>

static const char sega_hdr[] = "SEGA SEGASATURN ";

static void copy_u32(uint32_t *dst, uint32_t *src, uint32_t len)
{
    for (int i = 0; i < len; i++)
    {
        dst[i] = src[i];
    }
}

// code at 1a62
int card_load()
{
    const uint32_t *hdr = (uint32_t *)sega_hdr;
    uint32_t *card = (uint32_t *)0x22000000;
    uint32_t *dst = (uint32_t *)0x06002000;
    uint32_t *dst_hdr = (uint32_t *)0x06000c00;

    uint32_t *ip_param = (uint32_t *)0x06000264;
    uint32_t *ip_boot_addr = (uint32_t *)0x06000254;

    // check header
    for (int i = 0; i < 4; i++)
    {
        if (hdr[i] != card[i])
            return -1;
    }
    // copy card header
    copy_u32(dst, card, 0x100 / 4);
    // validate size
    uint32_t size = *(uint32_t *)0x060020e0;
    if (size > 0xfff && size < 0x8001)
    {
        // copy executable (ip.bin ?)
        size = (size + 3 >> 2) - 0x40;
        copy_u32((uint32_t *)0x06002100, (uint32_t *)0x22000100, size);

        // copy headers
        copy_u32((uint32_t *)0x060002a0, (uint32_t *)0x060020e0, 8);
        copy_u32((uint32_t *)0x06000c00, (uint32_t *)0x06002000, 0x40);

        *ip_param = 0x22001188;
        *ip_boot_addr = 0x06002100;

        return 0;
    }

    return -1;
}

int card_boot()
{
    void (*launch)() = (void *)0x06000690;
    launch();
    return 0;
}

int cd_load()
{
    // check if cd prsent...
    bios_loadcd_init(); // 1904
    bios_loadcd_read(); // 1912

    return 0;
}

int cd_boot()
{
    bios_loadcd_boot(); // 18a8
    return 0;
}

int cd_boot_backup()
{
    //@todo
    return 0;
}
