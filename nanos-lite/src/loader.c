#include "proc.h"
#include <elf.h>

#ifdef __ISA_AM_NATIVE__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif

size_t ramdisk_read(void *buf, size_t offset, size_t len);

static uintptr_t loader(PCB *pcb, const char *filename) {
  // TODO();
  Elf_Ehdr elf_header;
  ramdisk_read((void *)&elf_header, 0, sizeof(Elf_Ehdr));
  uint32_t phoff = elf_header.e_phoff;
  uint32_t phsize = elf_header.e_phentsize;
  uint32_t phcount = elf_header.e_phnum;
  Elf_Phdr elf_ph_header;
  for (int i = 0; i < phcount; i++) {
    ramdisk_read((void *)&elf_ph_header, phoff + i * phsize, phsize);
    if (elf_ph_header.p_type == PT_LOAD) {
      // 需要加载
      memset((void *)(elf_ph_header.p_vaddr), 0, elf_ph_header.p_memsz);
      ramdisk_read((void *)(elf_ph_header.p_vaddr), elf_ph_header.p_offset, elf_ph_header.p_filesz);
    }
  }
  return elf_header.e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %x", entry);
  ((void(*)())entry) ();
}

void context_kload(PCB *pcb, void *entry) {
  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->cp = _kcontext(stack, entry, NULL);
}

void context_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);

  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->cp = _ucontext(&pcb->as, stack, stack, (void *)entry, NULL);
}
