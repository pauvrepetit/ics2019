#ifndef __ARCH_H__
#define __ARCH_H__

struct _Context {
  struct _AddressSpace *as;
  uintptr_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
  int irq;
  uintptr_t eip, cs;
  
  union {
    struct {
      uintptr_t CF      :1;
      uintptr_t pad1    :5;
      uintptr_t ZF      :1;
      uintptr_t SF      :1;
      uintptr_t pad2    :1;
      uintptr_t IF      :1;
      uintptr_t pad3    :1;
      uintptr_t OF      :1;
      uintptr_t pad4   :20;
    };

    uintptr_t eflags;
  };
};

#define GPR1 eax
#define GPR2 ebx
#define GPR3 ecx
#define GPR4 edx
#define GPRx eax

#endif
