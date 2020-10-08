#include <klib.h>
#include <amdev.h>

#include <nemu.h>

#define MIN(a,b)	((a)<(b)?(a):(b))

uint32_t uptime() {
  _DEV_TIMER_UPTIME_t uptime;
  _io_read(_DEV_TIMER, _DEVREG_TIMER_UPTIME, &uptime, sizeof(uptime));
  return uptime.lo;
}

void get_timeofday(void *rtc) {
  _io_read(_DEV_TIMER, _DEVREG_TIMER_DATE, rtc, sizeof(_DEV_TIMER_DATE_t));
}

int read_key() {
  _DEV_INPUT_KBD_t key;
  _io_read(_DEV_INPUT, _DEVREG_INPUT_KBD, &key, sizeof(_DEV_INPUT_KBD_t));
  int ret = key.keycode;
  if (key.keydown) ret |= 0x8000;
  return ret;
}

void draw_rect(uint32_t *pixels, int x, int y, int w, int h) {
  _DEV_VIDEO_FBCTL_t ctl = (_DEV_VIDEO_FBCTL_t) {
    .pixels = pixels,
    .x = x, .y = y, .w = w, .h = h,
    .sync = 0,
  };
  // memcpy((void *)(uintptr_t)FB_ADDR, (void *)pixels, sizeof(uint32_t) * w * h);
  int width = screen_width();
  int height = screen_height();
  printf("draw x is %d, y is %d, w is %d, h is %d\n", x, y, w, h);
  for(int i = 0; i < 0xffffff; i++);
  for (int i = y; (i-y) < h && i < height; i++) {
    memcpy((uint32_t *)(uintptr_t)FB_ADDR + i * width + x, pixels, sizeof(uint32_t) * MIN(w, width-x));
    pixels += w;
  }
  printf("drawing\n");
  _io_write(_DEV_VIDEO, _DEVREG_VIDEO_FBCTL, &ctl, sizeof(ctl));
}

void draw_sync() {
  _DEV_VIDEO_FBCTL_t ctl;
  ctl.pixels = (uint32_t *)(uintptr_t)FB_ADDR;
  ctl.x = 0;
  ctl.y = 0;
  ctl.w = screen_width();
  ctl.h = screen_height();
  // ctl.pixels = NULL;
  // ctl.x = ctl.y = ctl.w = ctl.h = 0;
  ctl.sync = 1;
  _io_write(_DEV_VIDEO, _DEVREG_VIDEO_FBCTL, &ctl, sizeof(ctl));
}

int screen_width() {
  _DEV_VIDEO_INFO_t info;
  _io_read(_DEV_VIDEO, _DEVREG_VIDEO_INFO, &info, sizeof(info));
  return info.width;
}

int screen_height() {
  _DEV_VIDEO_INFO_t info;
  _io_read(_DEV_VIDEO, _DEVREG_VIDEO_INFO, &info, sizeof(info));
  return info.height;
}
