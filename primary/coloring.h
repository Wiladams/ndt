#pragma once


#include "blend2d.h"

namespace coloring {
    // RGBA_Doubles
    // 380 <= wl <= 780 nanometers
    //
    BLRgba  ColorRGBAFromWavelength(double wl, double gamma = 1.0)
    {
        BLRgba t = { 0,0,0,1 };

        if (wl >= 380.0 && wl <= 440.0) {
            t.r = -1.0 * (wl - 440.0) / (440.0 - 380.0);
            t.b = 1.0;
        }
        else if (wl >= 440.0 && wl <= 490.0) {
            t.g = (wl - 440.0) / (490.0 - 440.0);
            t.b = 1.0;
        }
        else if (wl >= 490.0 && wl <= 510.0) {
            t.g = 1.0;
            t.b = -1.0 * (wl - 510.0) / (510.0 - 490.0);
        }
        else if (wl >= 510.0 && wl <= 580.0) {
            t.r = (wl - 510.0) / (580.0 - 510.0);
            t.g = 1.0;
        }
        else if (wl >= 580.0 && wl <= 645.0) {
            t.r = 1.0;
            t.g = -1.0 * (wl - 645.0) / (645.0 - 580.0);
        }
        else if (wl >= 645.0 && wl <= 780.0) {
            t.r = 1.0;
        }

        auto s = 1.0;
        if (wl > 700.0) {
            s = 0.3 + 0.7 * (780.0 - wl) / (780.0 - 700.0);
        }
        else if (wl < 420.0) {
            s = 0.3 + 0.7 * (wl - 380.0) / (420.0 - 380.0);
        }

        t.r = pow(t.r * s, gamma);
        t.g = pow(t.g * s, gamma);
        t.b = pow(t.b * s, gamma);

        return t;
    }



    /*
    function factorAdjust(comp, Factor, IntensityMax, Gamma)
    if (comp == 0.0)then
    return 0;
    else
    return IntensityMax * pow(comp * Factor, Gamma);
    end
    end

    function getColorFromWaveLength(Wavelength, Gamma)
    local IntensityMax = 255;
    local Blue;
    local Green;
    local Red;
    local Factor;

    if (Wavelength >= 350 && Wavelength <= 439) then
    Red = -(Wavelength - 440) / (440 - 350);
    Green = 0.0;
    Blue = 1.0;
    elseif(Wavelength >= 440 && Wavelength <= 489) then
    Red = 0.0;
    Green = (Wavelength - 440) / (490 - 440);
    Blue = 1.0;
    elseif(Wavelength >= 490 && Wavelength <= 509) then
    Red = 0.0;
    Green = 1.0;
    Blue = -(Wavelength - 510) / (510 - 490);
    elseif(Wavelength >= 510 && Wavelength <= 579)then
    Red = (Wavelength - 510) / (580 - 510);
    Green = 1.0;
    Blue = 0.0;
    elseif(Wavelength >= 580 && Wavelength <= 644) then
    Red = 1.0;
    Green = -(Wavelength - 645) / (645 - 580);
    Blue = 0.0;
    elseif(Wavelength >= 645 && Wavelength <= 780) then
    Red = 1.0;
    Green = 0.0;
    Blue = 0.0;
    else
    Red = 0.0;
    Green = 0.0;
    Blue = 0.0;
    end

    if (Wavelength >= 350 && Wavelength <= 419) then
    Factor = 0.3 + 0.7 * (Wavelength - 350) / (420 - 350);
    elseif(Wavelength >= 420 && Wavelength <= 700) then
    Factor = 1.0;
    elseif(Wavelength >= 701 && Wavelength <= 780) then
    Factor = 0.3 + 0.7 * (780 - Wavelength) / (780 - 700);
    else
    Factor = 0.0;
    end

    local R = factorAdjust(Red, Factor, IntensityMax, Gamma);
    local G = factorAdjust(Green, Factor, IntensityMax, Gamma);
    local B = factorAdjust(Blue, Factor, IntensityMax, Gamma);

    return R, G, B
    end
    */
}
