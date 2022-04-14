#include "bootanim.h"
#include "fastlz/fastlz.h"
#include <stdint.h>
#define BOOTANIM_ADDR (0x06010000)
#define ERR_PTR (BOOTANIM_ADDR - 0x20)

void interrupt_set_level_mask(uint32_t imask)
{
    uint32_t sr;

    asm("stc sr,%0"
        : "=r"(sr));
    sr &= 0xFFFFFF0F;
    imask <<= 4;
    sr |= imask;
    asm("ldc %0,sr"
        :
        : "r"(sr));
}

int main()
{
    volatile unsigned int *err_ptr = ((volatile unsigned int *)(ERR_PTR));
    volatile unsigned int *bootanim_decmp = ((volatile unsigned int *)(BOOTANIM_ADDR));

    int d = fastlz_decompress(_workspaces_bootanim_custom_build_bootanim_fz, _workspaces_bootanim_custom_build_bootanim_fz_len, (void *)bootanim_decmp, 32 * 1024);

    err_ptr[0] = d;
    if (d > 0)
    {
        // launch the code
        void (*bootanim)() = (void *)bootanim_decmp;
        bootanim();
    }

    return 0;
}