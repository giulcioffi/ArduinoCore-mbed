#include "Nicla_System.h"

IS31FL3194 leds;
BQ25120A pmic;

void RgbOn(uint8_t color)
{
  leds.reset();
  leds.init();
  leds.powerUp();

  leds.ledSet(color);
  leds.powerDown();
}

void RgbOff()
{
  leds.powerDown();
}


  /*
      LDO reg:
      |   B7   |  B6   |  B5   |  B4   |  B3   |  B2   | B1  | B0  |
      | EN_LDO | LDO_4 | LDO_3 | LDO_2 | LDO_1 | LDO_0 |  X  |  X  |

      Conversion function:
      LDO = 0.8V + LDO_CODE * 100mV

      - for LDO = 3.3V:
          - set LCO_CODE = 25 (0x19)
          - shift to lef by 2 positions: (0x19 << 2) = 0x64
          - set EN_LDO: 0xE4
      - for LDO = 1.8V:
          - set LCO_CODE = 10 (0x0A)
          - shift to lef by 2 positions: (0x0A << 2) = 0x28
          - set EN_LDO: 0xA8
  */

bool enable3V3LDO()
{
  uint8_t ldo_reg = 0xE4;
  pmic.writeByte(BQ25120A_ADDRESS, BQ25120A_LDO_CTRL, ldo_reg);
  if (pmic.readByte(BQ25120A_ADDRESS, BQ25120A_LDO_CTRL) != ldo_reg) {
    return false;
  }
  return true;
}

bool enable1V8LDO()
{
  uint8_t ldo_reg = 0xA8;
  pmic.writeByte(BQ25120A_ADDRESS, BQ25120A_LDO_CTRL, ldo_reg);
  if (pmic.readByte(BQ25120A_ADDRESS, BQ25120A_LDO_CTRL) != ldo_reg) {
    return false;
  }
  return true;
}

bool disableLDO()
{
  uint8_t ldo_reg = pmic.readByte(BQ25120A_ADDRESS, BQ25120A_LDO_CTRL);
  ldo_reg &= 0x7F;
  pmic.writeByte(BQ25120A_ADDRESS, BQ25120A_LDO_CTRL, ldo_reg);
  if (pmic.readByte(BQ25120A_ADDRESS, BQ25120A_LDO_CTRL) != ldo_reg) {
    return false;
  }
  return true;
}