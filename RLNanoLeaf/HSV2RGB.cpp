#include <iostream>
#include <cmath>  // For NAN
#include "pch.h"
#include <map>
#include <nlohmann/json.hpp>
#include "RLNanoLeaf.h"
#include "IMGUI/imgui_internal.h"
#include "IMGUI/imgui_searchablecombo.h"
#include "IMGUI/imgui_stdlib.h"


RLNanoLeaf::HSV RLNanoLeaf::rgb2hsv(const RGB& in) {
    HSV out;
    LOG("Converting RGB to HSV - Input RGB: R: {}, G: {}, B: {}", in.r, in.g, in.b);

    double r = in.r / 255.0;
    double g = in.g / 255.0;
    double b = in.b / 255.0;

    double min = std::min({ r, g, b });
    double max = std::max({ r, g, b });
    double delta = max - min;

    // Value (brightness)
    out.v = static_cast<int>(max * 100);

    if (delta < 0.00001) {
        out.s = 0;
        out.h = 0;  // Hue undefined
        return out;
    }

    // Saturation
    out.s = static_cast<int>((delta / max) * 100);

    // Hue calculation
    if (r >= max)
        out.h = static_cast<int>(60.0 * (g - b) / delta);  // Between yellow & magenta
    else if (g >= max)
        out.h = static_cast<int>(60.0 * (2.0 + (b - r) / delta));  // Between cyan & yellow
    else
        out.h = static_cast<int>(60.0 * (4.0 + (r - g) / delta));  // Between magenta & cyan

    if (out.h < 0)
        out.h += 360;

    return out;
}
