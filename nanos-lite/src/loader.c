#include "proc.h"
#include <elf.h>
#include "fs.h"

#ifdef __ISA_AM_NATIVE__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif

size_t ramdisk_read(void *buf, size_t offset, size_t len);
void __am_get_cur_as(_Context *c);
void __am_switch(_Context *c);

static uintptr_t loader(PCB *pcb, const char *filename) {
  int fd = fs_open(filename, 0, 0);
  Elf_Ehdr elf_header;
  fs_read(fd, (void *)&elf_header, sizeof(Elf_Ehdr));

  printf("entry is %d\n", elf_header.e_entry);

  uint32_t phoff = elf_header.e_phoff;
  uint32_t phsize = elf_header.e_phentsize;
  uint32_t phcount = elf_header.e_phnum;
  Elf_Phdr elf_ph_header;
  for (int i = 0; i < phcount; i++) {
    fs_lseek(fd, phoff + i * phsize, SEEK_SET);
    fs_read(fd, (void *)&elf_ph_header, phsize);
    if (elf_ph_header.p_type == PT_LOAD) {
      // 需要加载
      // 从文件fd的p_offset处开始的p_filesz个字节的数据
      // 加载到内存中地址p_vaddr处开始的p_memsz字节的范围内
      int block_count = elf_ph_header.p_memsz / PGSIZE;
      if (elf_ph_header.p_memsz % PGSIZE) block_count++;
      int j = 0;
      uint32_t *paddr;
      for(j = 0; j < block_count - 1; j++) {
        paddr = _map(&pcb->as, (void *)(elf_ph_header.p_vaddr + j * PGSIZE), 0, 0);
        fs_lseek(fd, elf_ph_header.p_offset + j * PGSIZE, SEEK_SET);
        fs_read(fd, (void *)paddr, PGSIZE);
      }

      paddr = _map(&pcb->as, (void *)(elf_ph_header.p_vaddr + j * PGSIZE), 0, 0);
      fs_lseek(fd, elf_ph_header.p_offset + j * PGSIZE, SEEK_SET);
      fs_read(fd, (void *)paddr, elf_ph_header.p_filesz - j * PGSIZE);

      // memset((void *)(elf_ph_header.p_vaddr), 0, elf_ph_header.p_memsz);
      // fs_lseek(fd, elf_ph_header.p_offset, SEEK_SET);
      // fs_read(fd, (void *)(elf_ph_header.p_vaddr), elf_ph_header.p_filesz);
    }
  }
  printf("entry is %d\n", elf_header.e_entry);
  return elf_header.e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %d", entry);
  ((void(*)())entry) ();
}

void context_kload(PCB *pcb, void *entry) {
  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->cp = _kcontext(stack, entry, NULL);
}

void context_uload(PCB *pcb, const char *filename) {
  _protect(&pcb->as);
  uintptr_t entry = loader(pcb, filename);
  printf("loader finish, entry is %d\n", entry);

  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->cp = _ucontext(&pcb->as, stack, stack, (void *)entry, NULL);
}
