#include "fs.h"

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  ReadFn read;
  WriteFn write;
  size_t open_offset;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB};

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t serial_write(const void *buf, size_t offset, size_t len);
size_t events_read(void *buf, size_t offset, size_t len);
size_t dispinfo_read(void *buf, size_t offset, size_t len);
size_t fb_write(const void *buf, size_t offset, size_t len);
size_t fbsync_write(const void *buf, size_t offset, size_t len);

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  {"stdin", 0, 0, invalid_read, invalid_write},
  {"stdout", 0, 0, invalid_read, serial_write},
  {"stderr", 0, 0, invalid_read, serial_write},
  {"/dev/events", 0, 0, events_read, invalid_write},
  {"/dev/fb", 0, 0, invalid_read, fb_write},
  {"/proc/dispinfo", 0, 0, dispinfo_read, invalid_write},
  {"/dev/fbsync", 0, 0, invalid_read, fbsync_write},
#include "files.h"
};

#define NR_FILES (sizeof(file_table) / sizeof(file_table[0]))

size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t ramdisk_write(const void *buf, size_t offset, size_t len);

int fs_open(const char *pathname, int flags, int mode) {
  printf("open file %s\n", pathname);
  for (int i = 0; i < NR_FILES; i++) {
    printf("compare with %d %s\n", i, file_table[i].name);
    if (strcmp(pathname, file_table[i].name) == 0) {
      file_table[i].open_offset = 0;
      return i;
    }
  }
  assert(1 == 0);
}

size_t fs_text_read(int fd, void *buf, size_t len) {
  Finfo *f = file_table + fd;
  if (f->open_offset + len >= f->size) {
    len = f->size - f->open_offset;
  }
  len = ramdisk_read(buf, f->disk_offset + f->open_offset, len);
  f->open_offset += len;
  return len;
}

size_t fs_read(int fd, void *buf, size_t len) {
  printf("read file %d\n", fd);
  if (file_table[fd].read) {
    return file_table[fd].read(buf, 0, len);
  } else {
    return fs_text_read(fd, buf, len);
  }
}

size_t fs_text_write(int fd, const void *buf, size_t len) {
  if (fd == FD_STDIN) {
    return 0;
  } else if (fd == FD_STDOUT || fd == FD_STDERR) {
    int count = 0;
    for (count = 0; count < len; count++) {
      _putc(((const char *)buf)[count]);
    }
    return count;
  }
  Finfo *f = file_table + fd;
  if (f->open_offset + len >= f->size) {
    len = f->size - f->open_offset;
  }
  len = ramdisk_write(buf, f->disk_offset + f->open_offset, len);
  f->open_offset += len;
  return len;
}

size_t fs_write(int fd, const void *buf, size_t len) {
  printf("write file %d\n", fd);
  if (file_table[fd].write) {
    return file_table[fd].write(buf, 0, len);
  } else {
    printf("write text file %d\n", fd);
    return fs_text_write(fd, buf, len);
  }
}

size_t fs_lseek(int fd, size_t offset, int whence) {
  switch (whence) {
  case SEEK_SET:
    if (offset <= file_table[fd].size) {
      file_table[fd].open_offset = offset;
    } else {
      return -1;
    }
    break;
  case SEEK_CUR:
    if (offset + file_table[fd].open_offset <= file_table[fd].size) {
      file_table[fd].open_offset += offset;
    } else {
      return -1;
    }
    break;
  case SEEK_END:
    if (offset + file_table[fd].size <= file_table[fd].size) {
      file_table[fd].open_offset = file_table[fd].size + offset;
    } else {
      return -1;
    }
    break;
  default:
    break;
  }
  return file_table[fd].open_offset;
}
int fs_close(int fd) {
  file_table[fd].open_offset = 0;
  return 0;
}

void init_fs() {
  // TODO: initialize the size of /dev/fb
  int fd = fs_open("/proc/dispinfo", 0, 0);
  int video_info[2];
  fs_read(fd, (void *)video_info, 8);
  int fb_fd = fs_open("/dev/fb", 0, 0);
  file_table[fb_fd].size = video_info[0] * video_info[1];

  int fbsync_fd = fs_open("/dev/fbsync", 0, 0);
  file_table[fbsync_fd].size = file_table[fb_fd].size;


  fs_close(fd);
  fs_close(fb_fd);
  fs_close(fbsync_fd);
}
