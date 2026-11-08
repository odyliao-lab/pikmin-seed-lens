// Default: disposable probe only. Product builds additionally require the
// supervised worker's exact pinned game/TID and explicit fail-stop consent.
// UNKNOWN is never called rollback: the supervisor contains the pinned process.
kitty_rp_call_t KittyTraceMgr::_callFunctionFrom(uintptr_t callerAddress, uintptr_t functionAddress, int nargs, ...)
{
#ifdef SEED_GAME_TRACE
    extern bool seed_game_trace_admitted(int);
    bool exact=seed_game_trace_admitted(_pid);
#else
    std::ifstream command("/proc/"+std::to_string(_pid)+"/cmdline");
    std::string cmd((std::istreambuf_iterator<char>(command)),{});
    const std::string probe="dev.ody.seedlensprobe";
    // Android pads argv[0] storage with NUL bytes. Permit only those zeros;
    // colon subprocesses, arguments, suffixes and truncated names still refuse.
    bool exact=cmd.size()>probe.size() && cmd.starts_with(probe) && cmd.find_first_not_of('\0',probe.size())==cmd.npos;
#endif
    if(!exact || !_attached || nargs<0 || nargs>6 ||
       !_autoRestoreRegs || _remoteCallTimeout<1 || _remoteCallTimeout>2000) {
        puts("RESEARCH_FUNCTION_REFUSED_IDENTITY_OR_CONTRACT");_exit(80);
    }
    // Caller identity influences Android's linker namespace. Preserve the caller
    // supplied by the injector, but accept only a currently readable NX mapping.
    if(callerAddress){
        std::ifstream maps("/proc/"+std::to_string(_pid)+"/maps");std::string line;bool nx=false;
        while(std::getline(maps,line)){unsigned long long lo=0,hi=0;char perms[5]{};
            if(sscanf(line.c_str(),"%llx-%llx %4s",&lo,&hi,perms)==3 && callerAddress>=lo && callerAddress<hi)
                nx=perms[0]=='r'&&perms[2]!='x';
        }
        if(!nx){puts("RESEARCH_FUNCTION_REFUSED_CALLER");_exit(81);}
    }
    user_regs_struct before{};
    if(!getRegs(&before) || static_cast<int64_t>(before.orig_rax)!=-1){
        puts("RESEARCH_FUNCTION_REFUSED_KERNEL_CONTEXT");_exit(82);
    }
    std::array<uintptr_t,6> args{};va_list vl;va_start(vl,nargs);
    for(int i=0;i<nargs;i++)args[i]=va_arg(vl,uintptr_t);va_end(vl);
    size_t page=static_cast<size_t>(sysconf(_SC_PAGESIZE)),size=1024*1024;
    auto alloc=callSyscall(SYS_mmap,uintptr_t(0),uintptr_t(size+2*page),uintptr_t(PROT_NONE),
        uintptr_t(MAP_PRIVATE|MAP_ANONYMOUS),uintptr_t(-1),uintptr_t(0));
    if(alloc.status!=KT_RP_CALL_SUCCESS || alloc.result.val<=0){puts("RESEARCH_STACK_MAP_UNKNOWN");_exit(83);}
    auto base=static_cast<uintptr_t>(alloc.result.val);
    auto protect=callSyscall(SYS_mprotect,base+page,uintptr_t(size),uintptr_t(PROT_READ|PROT_WRITE));
    if(protect.status!=KT_RP_CALL_SUCCESS || protect.result.val!=0){puts("RESEARCH_STACK_PROTECT_UNKNOWN");_exit(84);}
    seed_research::X64CallContext context(*this);
    auto result=context.call(functionAddress,args,base+page,size,_remoteCallTimeout,callerAddress);
    if(result.state!=seed_research::CallState::Returned || !result.cpuRestored){
        printf("RESEARCH_FUNCTION_UNKNOWN state=%d restored=%d signal=%d\n",int(result.state),result.cpuRestored,result.signal);
        _exit(85);
    }
    auto freed=callSyscall(SYS_munmap,base,uintptr_t(size+2*page));
    user_regs_struct after{};
    if(freed.status!=KT_RP_CALL_SUCCESS || freed.result.val!=0 || !getRegs(&after) || memcmp(&before,&after,sizeof(before))){
        puts("RESEARCH_STACK_RELEASE_UNKNOWN");_exit(86);
    }
    puts("RESEARCH_FUNCTION_RETURNED_CPU_RESTORED");
    return {KT_RP_CALL_SUCCESS,{static_cast<intptr_t>(result.value)}};
}
