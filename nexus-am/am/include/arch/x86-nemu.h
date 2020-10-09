#ifndef __ARCH_H__
#define __ARCH_H__

struct _Context {

  // uintptr_t esp;
  struct _AddressSpace *as;
  uintptr_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
  int irq;
  uintptr_t eip, cs, eflags;


  // uintptr_t eflags, cs, eip;
  // int irq;
  // uintptr_t eax, ecx, edx, ebx, temp, ebp, esi, edi;
  // struct _AddressSpace *as;
  // uintptr_t esp;

  // uintptr_t esi, ebx, eax, eip, edx, eflags, ecx, cs, esp, edi, ebp;
  // struct _AddressSpace *as;
  // int irq;
};

#define GPR1 eax
#define GPR2 eip
#define GPR3 eip
#define GPR4 eip
#define GPRx eip

#endif
