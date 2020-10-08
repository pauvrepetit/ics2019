#include <am.h>
#include <amdev.h>
#include <nemu.h>

#define KEYDOWN_MASK 0x8000
#define I8042_DATA_PORT 0x60

size_t __am_input_read(uintptr_t reg, void *buf, size_t size) {
  switch (reg) {
    case _DEVREG_INPUT_KBD: {
      _DEV_INPUT_KBD_t *kbd = (_DEV_INPUT_KBD_t *)buf;
      uint32_t sig = inl(I8042_DATA_PORT);
      kbd->keydown = (sig & KEYDOWN_MASK) ? 1 : 0;
      kbd->keycode = (sig & KEYDOWN_MASK) ? (sig | KEYDOWN_MASK) : _KEY_NONE;
      return sizeof(_DEV_INPUT_KBD_t);
    }
  }
  return 0;
}
