#include "common.h"
#include <amdev.h>

size_t serial_write(const void *buf, size_t offset, size_t len) {
  int count = 0;
  for (count = 0; count < len; count++) {
    _putc(((const char *)buf)[count]);
  }
  return count;
}

#define NAME(key) \
  [_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [_KEY_NONE] = "NONE",
  _KEYS(NAME)
};

size_t events_read(void *buf, size_t offset, size_t len) {
  _DEV_INPUT_KBD_t kbd_event;
  _io_read(_DEV_INPUT, _DEVREG_INPUT_KBD, &kbd_event, sizeof(_DEV_INPUT_KBD_t));
  if (kbd_event.keycode == _KEY_NONE) {
    // 没有键盘事件，那么我们返回一个时钟事件，时钟是必然存在的
    _DEV_TIMER_UPTIME_t time_event;
    _io_read(_DEV_TIMER, _DEVREG_TIMER_UPTIME, &time_event, sizeof(_DEV_TIMER_DATE_t));
    sprintf((char *)buf, "t %d\n", time_event.lo);
  } else {
    // 有键盘事件，我们输出键盘事件
    if (kbd_event.keydown == 1) {
      // 按下按键
      sprintf((char *)buf, "kd %s\n", keyname[kbd_event.keycode & 0x7fff]);
    } else {
      sprintf((char *)buf, "ku %s\n", keyname[kbd_event.keycode & 0x7fff]);
    }
    // printf("hit kbd %s\n", keyname[kbd_event.keycode & 0x7fff]);
  }
  return strlen((char *)buf);
}

static char dispinfo[128] __attribute__((used)) = {};

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  _DEV_VIDEO_INFO_t video_info;
  _io_read(_DEV_VIDEO, _DEVREG_VIDEO_INFO, (void *)&video_info, sizeof(_DEV_VIDEO_INFO_t));
  static char dispinfo[128];
  sprintf(dispinfo, "WIDTH : %d\nHEIGHT : %d\n", video_info.width, video_info.height);
  int length = strlen(dispinfo);
  if (offset >= length) {
    ((char *)buf)[0] = 0;
    return -1;
  } else {
    strcpy((char *)buf, dispinfo+offset);
    return length - offset;
  }
}

#define MIN(a,b) ((a) < (b) ? (a) : (b))

size_t fb_write(const void *buf, size_t offset, size_t len) {
  offset /= 4;
  int pixels = len / 4;
  _DEV_VIDEO_INFO_t video_info;
  _io_read(_DEV_VIDEO, _DEVREG_VIDEO_INFO, (void *)&video_info, sizeof(_DEV_VIDEO_INFO_t));
  draw_rect((uint32_t *)buf, offset % video_info.width, offset / video_info.width, MIN(video_info.width, pixels), pixels / video_info.width + 1);
  // draw_sync();
  return len;
}

size_t fbsync_write(const void *buf, size_t offset, size_t len) {
  draw_sync();
  return 0;
}

uint32_t get_display_size() {
  _DEV_VIDEO_INFO_t video_info;
  _io_read(_DEV_VIDEO, _DEVREG_VIDEO_INFO, (void *)&video_info, sizeof(_DEV_VIDEO_INFO_t));
  return video_info.height * video_info.width * 16;
}

void init_device() {
  Log("Initializing devices...");
  _ioe_init();

  // TODO: print the string to array `dispinfo` with the format
  // described in the Navy-apps convention
  _DEV_VIDEO_INFO_t video_info;
  _io_read(_DEV_VIDEO, _DEVREG_VIDEO_INFO, (void *)&video_info, sizeof(_DEV_VIDEO_INFO_t));
  Log("Video Height: %d", video_info.height);
  Log("Video Width:  %d", video_info.width);
}
