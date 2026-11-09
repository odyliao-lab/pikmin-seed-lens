// Explicit Linux HOST research only; not Android code or a product dependency.
#if !defined(__linux__) || !defined(__x86_64__) || defined(__ANDROID__)
#error Linux x86_64 host research only.
#endif
    .text
    .macro SAVE_WRAPPER index
    .p2align 4
    .global seed_mmu_save_\index
    .protected seed_mmu_save_\index
    .type seed_mmu_save_\index,@function
seed_mmu_save_\index:
    .cfi_startproc
    cmpq $0, seed_mmu_originals+8*\index(%rip)
    je .Lmissing\index
    subq $536, %rsp
    .cfi_def_cfa_offset 544
    .irp reg,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15
    vmovdqu %ymm\reg, 32*\reg(%rsp)
    .endr
    stmxcsr 512(%rsp)
    call *seed_mmu_originals+8*\index(%rip)
    ldmxcsr 512(%rsp)
    .irp reg,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15
    vmovdqu 32*\reg(%rsp), %ymm\reg
    .endr
    addq $536, %rsp
    .cfi_def_cfa_offset 8
    ret
.Lmissing\index:
    ud2 // Uninitialized helper use must be fatal, never fabricated success.
    .cfi_endproc
    .size seed_mmu_save_\index,.-seed_mmu_save_\index
    .endm
    .irp index,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
    SAVE_WRAPPER \index
    .endr
    .section .data.rel.ro,"aw",@progbits
    .p2align 3
    .global seed_mmu_wrappers
    .protected seed_mmu_wrappers
    .type seed_mmu_wrappers,@object
seed_mmu_wrappers:
    .irp index,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
    .quad seed_mmu_save_\index
    .endr
    .size seed_mmu_wrappers,.-seed_mmu_wrappers
    .bss
    .p2align 3
    .global seed_mmu_originals
    .protected seed_mmu_originals
    .type seed_mmu_originals,@object
seed_mmu_originals:
    .zero 256
    .size seed_mmu_originals,.-seed_mmu_originals
    .section .note.GNU-stack,"",@progbits
