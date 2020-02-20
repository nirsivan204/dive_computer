/**
 * @file scuba.c
 *
 * @brief Holds common logic for the dive computer capstone project.
 */
#include <stdint.h>
#include "scuba.h"

/**< Respiratory minute volume = 1200 centiLitres / minute */
#define RMV     (1200)

 /**< Respiratory half second volume = 10 centiLitres / half_second */
#define RHSV    (RMV / 120)

/**
 * This computes how much gas is consumed in a half second at a certain depth.
 *
 * @param[in] depth_in_mm the depth in millimeters
 * @return The number of centilitres in gas.
 */
uint32_t gas_rate_in_cl(uint32_t depth_in_mm)
{
    uint32_t  depth_in_m = depth_in_mm / 1000;

    /* 10m of water = 1 bar = 100 centibar */
    uint32_t ambient_pressure_in_cb = 100 + (10 * depth_in_m);

    /* Gas consumed at STP = RHSV * ambient pressure / standard pressure */
    return ((RHSV * ambient_pressure_in_cb) / 100);
}

/**
 * This computes how much gas at STP it would take to surface from the
 * current depth, assuming no decompression stops and an ascent rate
 * of ASCENT_RATE_LIMIT.
 *
 * @param[in] depth_in_mm The current depth in meters
 * @return The number of centilitres of gas at STP required to make
 * it to the surface.
 */
uint32_t gas_to_surface_in_cl(uint32_t depth_in_mm)
{
    uint32_t    gas = 0;
    uint16_t    halfsecs_to_ascend_1m = (2 * 60) / ASCENT_RATE_LIMIT;;
    uint32_t    ambient_pressure_in_cb;     /* Ambient pressure in centiBar */

    for (uint32_t depth_in_m = depth_in_mm / 1000;
            depth_in_m > 0; depth_in_m--)
    {
        ambient_pressure_in_cb = 100 + (depth_in_m * 100 / 10);
        gas += (RHSV * halfsecs_to_ascend_1m * ambient_pressure_in_cb) / 100;
    }

    return (gas);
}

