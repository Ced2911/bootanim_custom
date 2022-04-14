#pragma once
#include <stdint.h>

typedef union
{
    struct
    {
        uint8_t unused1 : 2;
        uint8_t n1chcn : 2;
        uint8_t n1bmsz : 2;
        uint8_t n1bmenab : 1;
        uint8_t n1chsz : 1;
        uint8_t unused2 : 1;
        uint8_t n0chcn : 3;
        uint8_t n0bmsz : 2;
        uint8_t n0bmenab : 1;
        uint8_t n0chsz : 1;
    } part;
    uint16_t all;
} chctla_t;

typedef union
{
    struct
    {
        uint8_t unused1 : 2;
        uint8_t n0mpb : 6;
        uint8_t unused2 : 2;
        uint8_t n0mpa : 6;
    } part;
    uint16_t all;
} mpabn0_t;

typedef union
{
    struct
    {
        uint8_t unused1 : 2;
        uint8_t n0mpd : 6;
        uint8_t unused2 : 2;
        uint8_t n0mpc : 6;
    } part;
    uint16_t all;
} mpcdn0_t;

typedef union
{
    struct
    {
        uint8_t unused1 : 2;
        uint8_t n1mpb : 6;
        uint8_t unused2 : 2;
        uint8_t n1mpa : 6;
    } part;
    uint16_t all;
} mpabn1_t;

