# Research overlay only. Never edits the pinned upstream checkout.
# Caller has checked the complete dependency commit/status before this include.
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_LIST_DIR}/bounded_wait.inc"
  "${CMAKE_CURRENT_LIST_DIR}/bounded_stop.inc"
  "${CMAKE_CURRENT_LIST_DIR}/bounded_function.inc")
function(seed_replace_once old new)
  string(FIND "${TRACE_TEXT}" "${old}" pos)
  if(pos LESS 0)
    message(FATAL_ERROR "Bounded trace anchor missing")
  endif()
  string(LENGTH "${old}" old_len)
  math(EXPR next "${pos}+${old_len}")
  string(SUBSTRING "${TRACE_TEXT}" ${next} -1 rest)
  string(FIND "${rest}" "${old}" duplicate)
  if(NOT duplicate LESS 0)
    message(FATAL_ERROR "Bounded trace anchor ambiguous")
  endif()
  string(SUBSTRING "${TRACE_TEXT}" 0 ${pos} before)
  set(TRACE_TEXT "${before}${new}${rest}" PARENT_SCOPE)
endfunction()
function(seed_replace_method begin end content)
  string(FIND "${TRACE_TEXT}" "${begin}" start)
  string(FIND "${TRACE_TEXT}" "${end}" finish)
  if(start LESS 0 OR finish LESS_EQUAL start)
    message(FATAL_ERROR "Bounded trace method boundary missing")
  endif()
  math(EXPR size "${finish}-${start}")
  string(SUBSTRING "${TRACE_TEXT}" ${start} ${size} old)
  seed_replace_once("${old}" "${content}\n\n")
  set(TRACE_TEXT "${TRACE_TEXT}" PARENT_SCOPE)
endfunction()
file(READ "${TRACE}" TRACE_TEXT)
seed_replace_once("#include \"KittyTrace.hpp\"" "#include \"KittyTrace.hpp\"\n#include <chrono>\n#include <algorithm>\n#include <fstream>\n#include \"${CMAKE_CURRENT_LIST_DIR}/x64_call_context.h\"")
file(READ "${CMAKE_CURRENT_LIST_DIR}/bounded_wait.inc" replacement)
seed_replace_method("pid_t KittyTraceMgr::wait(" "bool KittyTraceMgr::waitSyscall()" "${replacement}")
file(READ "${CMAKE_CURRENT_LIST_DIR}/bounded_stop.inc" replacement)
seed_replace_method("bool KittyTraceMgr::stop()" "bool KittyTraceMgr::cont(" "${replacement}")
# Syscall wait needs one deadline shared by entry, exit, and incidental signals.
seed_replace_once("    while (phase < 2)" [=[
    if (_remoteCallTimeout <= 0 || _remoteCallTimeout > 30000)
        return failure_return(KT_RP_CALL_WAIT_FAILED);
    const auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(_remoteCallTimeout);
    while (phase < 2)]=])
seed_replace_once("        pid_t wp = wait(&status, __WALL);" [=[
        const auto left = std::chrono::duration_cast<std::chrono::milliseconds>(deadline - std::chrono::steady_clock::now()).count();
        pid_t wp = left <= 0 ? 0 : wait(&status, __WALL, int(left));
        if (wp == 0) {
            if (!stop()) return {KT_RP_CALL_NOT_STOPPED, {0}};
            return failure_return(KT_RP_CALL_TIMEOUT);
        }]=])
# Before restoring after any syscall failure, prove it is still our stopped
# tracee. A wait error is not proof that SETREGS or POKETEXT can succeed.
seed_replace_once([=[        if (_autoRestoreRegs)
            setRegs(&backup_regs);

        if (wrote_syscall)
            pokeMem(exec_addr, backup_code.data(), backup_code.size());
        return {s, {0}};]=] [=[        if (!stop()) return {KT_RP_CALL_NOT_STOPPED, {0}};
        bool restored = true;
        if (wrote_syscall) restored = pokeMem(exec_addr, backup_code.data(), backup_code.size());
        if (_autoRestoreRegs) restored = setRegs(&backup_regs) && restored;
        return {restored ? s : KT_RP_CALL_REGS_FAILED, {0}};]=])
seed_replace_once([=[    if (wrote_syscall && !pokeMem(exec_addr, backup_code.data(), backup_code.size()))
        KITTY_LOGW("%s: Failed to restore code at %p!", ctx.c_str(), (void *)exec_addr);

    // Restore regs
    if (_autoRestoreRegs)
        setRegs(&backup_regs);]=] [=[    bool restored = true;
    if (wrote_syscall) restored = pokeMem(exec_addr, backup_code.data(), backup_code.size());
    if (_autoRestoreRegs) restored = setRegs(&backup_regs) && restored;
    if (!restored) return {KT_RP_CALL_REGS_FAILED, {0}};]=])
seed_replace_once([=[    if (!getRegs(&return_regs))
        return failure_return(KT_RP_CALL_REGS_FAILED);

    kitty_rp_call_t result = {KT_RP_CALL_SUCCESS, {static_cast<intptr_t>(return_regs.KT_REG_RET)}};]=] [=[    if (!getRegs(&return_regs))
        return failure_return(KT_RP_CALL_REGS_FAILED);
    // Linux internal restart statuses are not completed syscalls. Suppressed
    // async signals can expose these at ptrace syscall-exit stops. Never replay.
    const auto syscall_value = static_cast<intptr_t>(return_regs.KT_REG_RET);
    if (syscall_value <= -512 && syscall_value >= -516)
        return failure_return(KT_RP_CALL_FAILED);
    kitty_rp_call_t result = {KT_RP_CALL_SUCCESS, {syscall_value}};]=])
file(READ "${CMAKE_CURRENT_LIST_DIR}/bounded_function.inc" replacement)
seed_replace_method("kitty_rp_call_t KittyTraceMgr::_callFunctionFrom(" "kitty_rp_call_t KittyTraceMgr::_callSyscall(" "${replacement}")
# Product syscall failures cannot fall through injector cleanup and attempt
# another remote call with uncertain state. Fixtures retain restore testing.
seed_replace_once("        if (!stop()) return {KT_RP_CALL_NOT_STOPPED, {0}};\n        bool restored = true;" [=[
#ifdef SEED_GAME_TRACE
        puts("SUPERVISED_SYSCALL_FAILURE_UNKNOWN");_exit(88);
#endif
        if (!stop()) return {KT_RP_CALL_NOT_STOPPED, {0}};
        bool restored = true;]=])
seed_replace_once("    if (!restored) return {KT_RP_CALL_REGS_FAILED, {0}};" [=[
    if (!restored) {
#ifdef SEED_GAME_TRACE
        puts("SUPERVISED_SYSCALL_RESTORE_UNKNOWN");_exit(89);
#endif
        return {KT_RP_CALL_REGS_FAILED, {0}};
    }]=])
# Production never suppresses an unrelated signal or continues after an
# uncertain syscall. The supervised worker exits; its target is pinned/armed.
seed_replace_once("        // group-stop (seized) / job-control signals: resume, drop the signal." [=[
#ifdef SEED_GAME_TRACE
        puts("SUPERVISED_SYSCALL_UNEXPECTED_STOP");_exit(87);
#endif
        // group-stop (seized) / job-control signals: resume, drop the signal.]=])
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/KittyTrace-bounded.cpp" "${TRACE_TEXT}")
