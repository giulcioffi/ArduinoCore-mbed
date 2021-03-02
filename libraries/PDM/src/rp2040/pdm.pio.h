// -------------------------------------------------- //
// This file is autogenerated by pioasm; do not edit! //
// -------------------------------------------------- //

#if !PICO_NO_HARDWARE
#include "hardware/pio.h"
#endif

// ------- //
// pdm_pio //
// ------- //

#define pdm_pio_wrap_target 0
#define pdm_pio_wrap 3

static const uint16_t pdm_pio_program_instructions[] = {
            //     .wrap_target
    0x9060, //  0: push   iffull block    side 1     
    0xb042, //  1: nop                    side 1     
    0x4001, //  2: in     pins, 1         side 0     
    0xa042, //  3: nop                    side 0     
            //     .wrap
};

#if !PICO_NO_HARDWARE
static const struct pio_program pdm_pio_program = {
    .instructions = pdm_pio_program_instructions,
    .length = 4,
    .origin = -1,
};

static inline pio_sm_config pdm_pio_program_get_default_config(uint offset) {
    pio_sm_config c = pio_get_default_sm_config();
    sm_config_set_wrap(&c, offset + pdm_pio_wrap_target, offset + pdm_pio_wrap);
    sm_config_set_sideset(&c, 1, false, false);
    return c;
}

#include "hardware/gpio.h"
static inline void pdm_pio_program_init(PIO pio, uint sm, uint offset, uint clkPin, uint dataPin, float clkDiv) {
  pio_sm_config c = pdm_pio_program_get_default_config(offset);
  sm_config_set_sideset(&c, 1, false, false);
  sm_config_set_in_shift(&c, false, true, 8);
  sm_config_set_in_pins(&c, dataPin);
  sm_config_set_sideset_pins(&c, clkPin);
  sm_config_set_clkdiv(&c, clkDiv);
  pio_sm_set_consecutive_pindirs(pio, sm, dataPin, 1, false);
  pio_sm_set_consecutive_pindirs(pio, sm, clkPin, 1, true);
  pio_sm_set_pins_with_mask(pio, sm, 0, (1u << clkPin) );
  pio_gpio_init(pio, dataPin);
  pio_gpio_init(pio, clkPin);
  pio_sm_init(pio, sm, offset, &c);
  pio_sm_set_enabled(pio, sm, true);
}

#endif

