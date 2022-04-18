#include <iapetus.h>
#include <string.h>
#include "bios_cd.h"
#include "vdpregs.h"
#include "sega.h"

void per_handler(void);

#define page_t uint32_t

#define PLAN_A_OFFSET (0x008000)
#define PLAN_B_OFFSET (0x004000)

#define PLAN_A ((volatile uint32_t *)(VDP2_RAM + PLAN_A_OFFSET))
#define PLAN_B ((volatile uint32_t *)(VDP2_RAM + PLAN_B_OFFSET))

#define VDP2_MAP_ADDR(offset) ((offset >> 14))

#define VDP2_SCRN_PND_CP_NUM(address) (((uint32_t)(address)) >> 5)
#define VDP2_SCRN_PND_MODE_0_PAL_NUM(cram_address) ((((uint32_t)(cram_address)) >> 5) & 0x007F)
#define VDP2_SCRN_PND_MODE_1_PAL_NUM(cram_address) ((((uint32_t)(cram_address)) >> 5) & 0x007F)
#define VDP2_SCRN_PND_MODE_2_PAL_NUM(cram_address) ((((uint32_t)(cram_address)) >> 6) & 0x007F)

#define VDP2_SCRN_PND_PAL_NUM(cram_mode, cram_address)           \
    __CONCAT(VDP2_SCRN_PND_MODE_, __CONCAT(cram_mode, _PAL_NUM)) \
    (cram_address)

#define VDP2_SCRN_PND_CONFIG_8(cram_mode, cpd_addr, pal_addr, vf, hf, pr, cc) \
    ((((vf)&0x01) << 31) |                                                    \
     (((hf)&0x01) << 30) |                                                    \
     (((pr)&0x01) << 29) |                                                    \
     (((cc)&0x01) << 28) |                                                    \
     ((VDP2_SCRN_PND_PAL_NUM(cram_mode, pal_addr) & 0x007F) << 16) |          \
     (VDP2_SCRN_PND_CP_NUM(cpd_addr) & 0x7FFF))

static uint16_t MD_TO_SS_PALETTE(uint16_t cv)
{
    static const uint8_t col_level[] = {0, 6, 10, 14, 18, 21, 25, 31};

    uint8_t r = (cv & 0x00E) >> 1;
    uint8_t g = (cv & 0x0E0) >> 5;
    uint8_t b = (cv & 0xE00) >> 9;

    return 0x8000 | (col_level[b] << 10) | (col_level[g] << 5) | col_level[r];
}

static void _setTileMap(const uint8_t *tilemap, page_t *pages)
{
    const uint32_t screen_pal_adr = 0;
    const uint32_t screen_cpd_adr = 0;

    const uint16_t v = *((uint16_t *)tilemap);

    const uint16_t tile_idx = v & 0x7FF;
    const uint8_t pal_idx = (v >> 13) & 0x3;

    const uint32_t pal_adr = (uint32_t)(screen_pal_adr) + (pal_idx * 32 * 2);
    const uint32_t cpd_adr = (uint32_t)(screen_cpd_adr) + (tile_idx << 5);

    const uint8_t y_flip = (v & 0x1000) != 0;
    const uint8_t x_flip = (v & 0x0800) != 0;

    const uint8_t prio = (v >> 15) ? 1 : 0;

    const uint32_t pnd = VDP2_SCRN_PND_CONFIG_8(1, cpd_adr, pal_adr, y_flip, x_flip, prio, 0);

    *pages = pnd;
    // duplicate the tile a 2nd time
    pages[1 << 11] = pnd;
}

void CopyTilemap(const uint8_t *tilemap, size_t plan, uint8_t x, uint8_t y, size_t width, size_t height)
{
    const uint32_t page_width = 64;
    const int is_bg = ((plan >> 13) == 7);

    page_t *pages = is_bg ? (page_t *)PLAN_B : (page_t *)PLAN_A;

    for (uint32_t page_y = 0; page_y < height; page_y++)
    {
        for (uint32_t page_x = 0; page_x < width; page_x++)
        {
            uint16_t page_idx = (x + page_x) + (page_width * (page_y + y));
            _setTileMap(tilemap, &pages[page_idx]);
            tilemap += 2;
        }
    }
}

#define VRAM_FG 0xC000 // Foreground nametable
#define VRAM_BG 0xE000 // Fackground nametable
#define SPEED_T(x) (x >> 0)

static uint16_t bootanim_finished = 0;
static uint16_t time = 0;

void sega_logo_vblank_interrupt()
{
    volatile uint16_t *cram = (volatile uint16_t *)VDP2_CRAM;
    per_handler();
    if (SPEED_T(time) > 48)
    {
        int s = (SPEED_T(time) - 48) >> 2;
        int pal_offset = s * 6;
        if (pal_offset > 18)
            pal_offset = 18;

        {
            volatile uint16_t *pal = &cram[0];
            uint16_t *from = (uint16_t *)(pal_sega_b);

            for (int i = 0; i < 5; i++)
            {
                pal[i + 2] = MD_TO_SS_PALETTE(from[i + pal_offset]);
            }

            for (int i = 1; i < 16; i++)
            {
                pal[i + (32 * 1)] = MD_TO_SS_PALETTE(from[5 + pal_offset]);
                pal[i + (32 * 2)] = MD_TO_SS_PALETTE(from[5 + pal_offset]);
                pal[i + (32 * 3)] = MD_TO_SS_PALETTE(from[5 + pal_offset]);
            }
        }
    }

    if (SPEED_T(time) < 48)
    {
        volatile uint16_t *pal = &cram[0];
        uint16_t *from = (uint16_t *)pal_sega_a;
        int pal_offset = 16 + (SPEED_T(time) % 48);
        int max = pal_offset + 6;
        if (max > 64)
            max = 64;

        for (int i = pal_offset; i < max; i++)
        {
            int bank = (i & 0x30) << 1;
            if ((i & 0xF))
            {
                pal[(i & 0xf) + bank] = MD_TO_SS_PALETTE(*from++);
            }
        }
    }

    time++;

    // reset
    if (SPEED_T(time) > 128)
    {
        bootanim_finished = 1;
        // loop
        if (0)
        {
            time = 0;
            for (int i = 0; i < 256; i++)
            {
                cram[i] = 0xFFFF;
            }
        }
    }
}

void sega_logo_init()
{
    // Copy tiles
    uint16_t *dst = (uint16_t *)VDP2_RAM;
    memcpy(dst, sega_logo_chr, sizeof(sega_logo_chr));

    CopyTilemap(&map_sega[0x0180], VRAM_FG, 0, 0, 40, 28);
    CopyTilemap(&map_sega[0x0000], VRAM_BG, 8, 10, 24, 8);

    // init palette
    volatile uint16_t *cram = (volatile uint16_t *)VDP2_CRAM;

    for (int i = 0; i < 256; i++)
    {
        cram[i] = 0xFFFF;
    }

    bootanim_finished = 0;
    time = 0;

    uint32_t mask = interrupt_get_level_mask();
    interrupt_set_level_mask(0xF);
    bios_set_scu_interrupt(0x41, sega_logo_vblank_interrupt);
    interrupt_set_level_mask(mask);
}

void vdp_setup()
{
    // setup ram access
    {
        VDP2_REG_CYCA0L = 0x450F;
        VDP2_REG_CYCA0U = 0xFFFF;
        VDP2_REG_CYCA1L = 0x1FFF;
        VDP2_REG_CYCA1U = 0xFFFF;
        VDP2_REG_CYCB0L = 0xFFFF;
        VDP2_REG_CYCB0U = 0xFFFF;
        VDP2_REG_CYCB1L = 0xFFFF;
        VDP2_REG_CYCB1U = 0xFFFF;
    }

    // setup screen scroll regs
    {
        // Screen Scroll Value Registers: No scroll
        VDP2_REG_SCXIN1 = 0;
        VDP2_REG_SCXDN1 = 0;
        VDP2_REG_SCYIN1 = 0;
        VDP2_REG_SCYDN1 = 0;

        // Coordinate Increment Registers: 1.0/1.0
        VDP2_REG_ZMXIN1 = 1;
        VDP2_REG_ZMXDN1 = 0;
        VDP2_REG_ZMYIN1 = 1;
        VDP2_REG_ZMYDN1 = 0;

        // Screen Scroll Value Registers: No scroll
        VDP2_REG_SCXIN0 = 0;
        VDP2_REG_SCXDN0 = 0;
        VDP2_REG_SCYIN0 = 0;
        VDP2_REG_SCYDN0 = 0;

        // Coordinate Increment Registers: 1.0/1.0
        VDP2_REG_ZMXIN0 = 1;
        VDP2_REG_ZMXDN0 = 0;
        VDP2_REG_ZMYIN0 = 1;
        VDP2_REG_ZMYDN0 = 0;

        // Pattern Name Control Register - 2 Words
        VDP2_REG_PNCN1 = 0;
        VDP2_REG_PNCN0 = 0;
        VDP2_REG_PLSZ = 0;
    }

    // configure map
    {
        static const chctla_t chctla = {
            .part.n0chcn = BG_16COLOR,
            .part.n0chsz = 2, // 1x1

            .part.n1chcn = BG_16COLOR,
            .part.n1chsz = 2, // 1x1
        };
        uint8_t n0map = VDP2_MAP_ADDR(PLAN_A_OFFSET) & 0xFF;
        uint8_t n1map = VDP2_MAP_ADDR(PLAN_B_OFFSET) & 0xFF;

        VDP2_REG_CHCTLA = chctla.all;
        VDP2_REG_MPABN0 = (n0map << 24) | (n0map << 16) | (n0map << 8) | n0map;
        VDP2_REG_MPABN1 = (n1map << 24) | (n1map << 16) | (n1map << 8) | n1map;
    }

    // priority
    VDP2_REG_PRINA = 0x0303;

    // Enable Screen
    VDP2_REG_BGON = 0x0003;

    // display enable
    VDP2_REG_TVMD |= 0x8000;
}

void system_init()
{
    interrupt_set_level_mask(0xF);

    for (int i = 0; i < 0x80; i++)
        bios_set_sh2_interrupt(i, 0);

    for (int i = 0x40; i < 0x60; i++)
        bios_set_scu_interrupt(i, 0);

    // Make sure all interrupts have been called
    bios_change_scu_interrupt_mask(0, 0);
    bios_change_scu_interrupt_mask(0xFFFFFFFF, 0xFFFFFFFF);

    vdp_init(RES_320x224);
    per_init();

    vdp_setup();

    // If DSP is running, stop it
    // if (dsp_is_exec())
    //    dsp_stop();

    if (interrupt_get_level_mask() > 0x7)
        interrupt_set_level_mask(0x7);
}

int main()
{
    system_init();
    sega_logo_init();

    // hack for yabause
    *(volatile uint32_t *)(0x0600029c) = 0x1904;
    *(volatile uint32_t *)(0x060002cc) = 0x1912;
    *(volatile uint32_t *)(0x06000288) = 0x18a8;
    *(volatile uint32_t *)(0x060002d4) = 0x18fe;

    int cd_present = -1;
    int card_present = -1;

    card_present = card_load();
    if (card_present != 0 && bios_cd_is_present())
    {
        cd_present = cd_load();
    }

    while (bootanim_finished == 0)
    {
    };
    interrupt_set_level_mask(0xF);
    while (1)
    {

        if (card_present == 0)
            card_boot();
        else if (cd_present == 0)
            cd_boot();
        // failed to boot card or cd
        bios_run_cd_player();
    }
    return 0;
}
