# Task: sbrk + malloc for processes

## Steps

- [x] Investigate memory model, syscall table, ELF loader, SDK
- [x] Diagnose panic (user-space statics unusable in flat model)
- [x] Kernel: `sys_sbrk(0)` returns heap base (`heap_start`)
- [x] Kernel: `elf_to_proc` reserves `USER_HEAP_RESERVED` bytes after the image
- [x] SDK: rewrite `compiler/sdk/malloc.c` as static-free allocator
- [x] SDK: bump `USER_HEAP_RESERVED` to 1 MiB in `compiler/sdk/lib/memory.h`
- [x] SDK: fill `compiler/sdk/lib/malloc.h` (shared guard with memory.h)
- [x] Verify include chains (`-Isdk/lib`) and syscall index consistency
- [ ] Build & test with `compiler/examples/simple_heap.c` (needs arm-none-eabi-gcc)

