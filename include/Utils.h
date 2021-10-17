#pragma once

#include <glm/glm.hpp>

half3 PcxDecodeColor(uint data)
{
    half r = (data      ) & 0xff;
    half g = (data >>  8) & 0xff;
    half b = (data >> 16) & 0xff;
    half a = (data >> 24) & 0xff;
    return half3(r, g, b) * a * PCX_MAX_BRIGHTNESS / (255 * 255);
}