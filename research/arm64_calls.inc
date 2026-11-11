// Every failure ends the supervised worker. No upstream remote cleanup can run
// after a refused/unknown call. Only the parent owns the durable outcome.
kitty_rp_call_t KittyTraceMgr::_callSyscall(long number,int nargs,...){
    extern bool seed_game_trace_admitted(int);
    if(!seed_game_trace_admitted(_pid)||nargs<0||nargs>6){puts("ARM64_SYSCALL_NOT_ADMITTED");_exit(80);}
    std::array<uintptr_t,6> args{};va_list vl;va_start(vl,nargs);
    for(int i=0;i<nargs;++i)args[i]=va_arg(vl,uintptr_t);va_end(vl);
    seed_arm64::Context context(*this);auto result=context.syscall(number,args,_remoteCallTimeout);
    if(result.state!=seed_arm64::State::Returned||!result.restored){
        printf("ARM64_SYSCALL_UNKNOWN state=%d signal=%d\n",int(result.state),result.signal);_exit(81);
    }
    return {KT_RP_CALL_SUCCESS,{result.value}};
}
kitty_rp_call_t KittyTraceMgr::_callFunctionFrom(uintptr_t caller,uintptr_t function,int nargs,...){
    extern bool seed_game_trace_admitted(int);
    if(!seed_game_trace_admitted(_pid)||nargs<0||nargs>8){puts("ARM64_FUNCTION_NOT_ADMITTED");_exit(82);}
    std::array<uintptr_t,8> args{};va_list vl;va_start(vl,nargs);
    for(int i=0;i<nargs;++i)args[i]=va_arg(vl,uintptr_t);va_end(vl);
    // Scratch stack is dedicated and guarded. Unknown paths never issue munmap
    // or any other cleanup in a half-completed remote context.
    size_t page=static_cast<size_t>(sysconf(_SC_PAGESIZE)),size=1024*1024;
    auto alloc=callSyscall(SYS_mmap,uintptr_t(0),uintptr_t(size+2*page),uintptr_t(PROT_NONE),
        uintptr_t(MAP_PRIVATE|MAP_ANONYMOUS),uintptr_t(-1),uintptr_t(0));
    if(alloc.result.val<=0){puts("ARM64_STACK_MAP_FAILED");_exit(83);}
    uintptr_t base=static_cast<uintptr_t>(alloc.result.val);
    if(callSyscall(SYS_mprotect,base+page,uintptr_t(size),uintptr_t(PROT_READ|PROT_WRITE)).result.val){
        puts("ARM64_STACK_PROTECT_FAILED");_exit(84);
    }
    seed_arm64::Context context(*this);
    auto result=context.function(function,args,base+page,size,_remoteCallTimeout,caller);
    if(result.state!=seed_arm64::State::Returned||!result.restored){
        printf("ARM64_FUNCTION_UNKNOWN state=%d signal=%d\n",int(result.state),result.signal);_exit(85);
    }
    if(callSyscall(SYS_munmap,base,uintptr_t(size+2*page)).result.val){puts("ARM64_STACK_RELEASE_FAILED");_exit(86);}
    return {KT_RP_CALL_SUCCESS,{result.value}};
}
