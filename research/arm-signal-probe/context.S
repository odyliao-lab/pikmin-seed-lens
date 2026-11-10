// Own disposable App instructions only. Nothing here resumes a signal context.
.text
.p2align 2
.macro case_function name, instruction, second=0
.global \name
.hidden \name
.type \name,%function
\name:
    stp x19,x20,[sp,#-16]!
    mov x19,#0x534c
    mov x20,#0x1357
.global \name\()_fault
.hidden \name\()_fault
\name\()_fault:
    .inst \instruction
.if \second
    .inst \second
.endif
    ldp x19,x20,[sp],#16
    ret
.size \name,.-\name
.endm
case_function own_udf,0
case_function own_sevl,0xd50320bf
case_function own_pair,0xd50320bf,0xd503205f
case_function own_wfe,0xd503205f
.section .note.GNU-stack,"",%progbits
