// Own disposable App instructions only. Only the explicit resume_* cases may
// request a one-shot handler round trip past their own known UDF label.
.text
.p2align 2
.macro case_function name, instruction, second=0, marker=0x534c, canary=0
.global \name
.hidden \name
.type \name,%function
\name:
    stp x19,x20,[sp,#-16]!
    mov x19,#\marker
    mov x20,#0x1357
.global \name\()_fault
.hidden \name\()_fault
\name\()_fault:
    .inst \instruction
.if \second
    .inst \second
.endif
.if \canary
    mov x0,#0x5eed
.endif
    ldp x19,x20,[sp],#16
    ret
.size \name,.-\name
.endm
case_function own_udf,0
case_function own_sevl,0xd50320bf
case_function own_pair,0xd50320bf,0xd503205f
case_function own_wfe,0xd503205f
case_function own_resume_udf,0,0,0x534c,1
case_function own_resume_bad,0,0,0x534d,1
.section .note.GNU-stack,"",%progbits
