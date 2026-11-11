// Own Linux HOST process: verify vector/MXCSR, arguments, return and alignment.
// An extra argument deliberately exercises the uninitialized-wrapper refusal.
#if !defined(__linux__) || !defined(__x86_64__) || defined(__ANDROID__)
#error Linux x86_64 host research only.
#endif
    .text
    .global _start
_start:
    cmpq $1,(%rsp)
    jne missing
    lea clobber(%rip),%rax
    movq %rax,seed_mmu_originals(%rip)
    vxorps %ymm0,%ymm0,%ymm0
    vcmpps $0,%ymm0,%ymm0,%ymm0
    .irp reg,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15
    vmovaps %ymm0,%ymm\reg
    .endr
    ldmxcsr default_csr(%rip)
    movq $1,%rdi
    movq $2,%rsi
    movq $3,%rdx
    movq $4,%rcx
    movq $5,%r8
    call seed_mmu_save_0
    cmpq $55,%rax
    jne fail
    .irp reg,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15
    vmovdqu %ymm\reg, vector_output+32*\reg(%rip)
    .endr
    stmxcsr csr_output(%rip)
    cmpl $0x1f80,csr_output(%rip)
    jne fail
    lea vector_output(%rip),%rdi
    movl $64,%ecx
check:
    cmpq $-1,(%rdi)
    jne fail
    addq $8,%rdi
    loop check
    movl $1,%eax
    movl $1,%edi
    lea pass(%rip),%rsi
    movl $(pass_end-pass),%edx
    syscall
    xorl %edi,%edi
    jmp exit
missing:
    call seed_mmu_save_1
fail:
    movl $1,%edi
exit:
    movl $60,%eax
    syscall
clobber:
    vzeroall
    ldmxcsr changed_csr(%rip)
    movq %rsp,%rax
    andl $15,%eax
    cmpl $8,%eax
    jne bad_alignment
    lea (%rdi,%rsi,2),%rax
    imulq $3,%rdx,%r9
    addq %r9,%rax
    imulq $4,%rcx,%r9
    addq %r9,%rax
    imulq $5,%r8,%r9
    addq %r9,%rax
    ret
bad_alignment:
    movq $-1,%rax
    ret
    .section .rodata
    .p2align 2
default_csr: .long 0x1f80
changed_csr: .long 0x3f80
pass: .ascii "SEED_MMU_VECTOR_ABI_PASS\n"
pass_end:
    .bss
    .p2align 5
vector_output: .zero 512
csr_output: .zero 4
    .section .note.GNU-stack,"",@progbits
