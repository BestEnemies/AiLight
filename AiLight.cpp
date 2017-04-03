/*
  AiLight Library

  AiLight is a simple library to control an AiLight that contains the MY9291 LED
  driver and encapsulates the MY9291 LED driver made by Xose Pérez

  Copyright 2017 Sacha Telgenhof (stelgenhof@gmail.com). All rights reserved.

  For the full copyright and license information, please view the LICENSE
  file that was distributed with this source code.
 */

#include "AiLight.hpp"

AiLightClass::AiLightClass(void)
{
    _my9291 = new my9291(MY9291_DI_PIN, MY9291_DCKI_PIN, MY9291_COMMAND_DEFAULT);

    setRGBW(); // Initialise colour channels
}

AiLightClass::AiLightClass(const AiLightClass &obj)
{
    _my9291 = new my9291(MY9291_DI_PIN, MY9291_DCKI_PIN, MY9291_COMMAND_DEFAULT);
    *_my9291 = *obj._my9291;

    setRGBW(); // Initialise colour channels
}

AiLightClass::~AiLightClass(void)
{
    delete _my9291;
}

uint8_t AiLightClass::getBrightness(void)
{
    return _brightness;
}

void AiLightClass::setBrightness(uint16_t level)
{
    _brightness = constrain(level, 0, MY9291_LEVEL_MAX); // Force boundaries

    setRGBW();
    setState(true);
}

bool AiLightClass::getState(void)
{
    return _my9291->getState();
}

void AiLightClass::setState(bool state)
{
    _my9291->setState(state);
}

Color AiLightClass::getColor(void)
{
    return _color;
}

void AiLightClass::setColor(uint8_t red, uint8_t green, uint8_t blue)
{
    _color.red = red;
    _color.green = green;
    _color.blue = blue;

    setRGBW();
    setState(true);
}

void AiLightClass::setWhite(uint8_t white)
{
    _color.white = white;

    setRGBW();
    setState(true);
}

uint16_t AiLightClass::getColorTemperature(void)
{
    return _colortemp;
}

void AiLightClass::setColorTemperature(uint16_t temperature)
{
    _colortemp = temperature; // Save ccolour temperature setting

    // Convert from mired value to relative Kelvin temperature. The temperature
    // must fall between 1000 and 40000 degrees. All calculations require
    // tmpKelvin \ 100, so only do the conversion once
    uint16_t tmpKelvin = constrain(1000000UL / temperature, 1000, 40000) / 100;

    // Perform conversions from colour temperature to RGB values

    // Red
    float red = (tmpKelvin <= 66)
            ? MY9291_LEVEL_MAX
            : 329.698727446 * pow((tmpKelvin - 60), -0.1332047592);

    _color.red = constrain(red, 0, MY9291_LEVEL_MAX); // Force boundaries

    // Green
    float green = (tmpKelvin <= 66)
            ? 99.4708025861 * log(tmpKelvin) - 161.1195681661
            : 288.1221695283 * pow(tmpKelvin, -0.0755148492);

    _color.green = constrain(green, 0, MY9291_LEVEL_MAX); // Force boundaries

    // Blue
    float blue = (tmpKelvin >= 66)
            ? MY9291_LEVEL_MAX
            : ((tmpKelvin <= 19) ? 0
            : 138.5177312231 * log(tmpKelvin - 10) -
            305.0447927307);

    _color.blue = constrain(blue, 0, MY9291_LEVEL_MAX); // Force boundaries

    setRGBW();
    setState(true);
}

void AiLightClass::setRGBW()
{
    _my9291->setColor((my9291_color_t){
        (uint32_t) map(_color.red, 0, MY9291_LEVEL_MAX, 0, _brightness),
        (uint32_t) map(_color.green, 0, MY9291_LEVEL_MAX, 0, _brightness),
        (uint32_t) map(_color.blue, 0, MY9291_LEVEL_MAX, 0, _brightness),
        (uint32_t) map(_color.white, 0, MY9291_LEVEL_MAX, 0, _brightness)
    });
}

AiLightClass AiLight;
