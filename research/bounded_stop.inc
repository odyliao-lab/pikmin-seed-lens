bool KittyTraceMgr::stop()
{
    if (!_attached || !_seized || _pid <= 0 || _remoteCallTimeout <= 0) return false;
    user_regs_struct regs{};
    // A successful ptrace read proves this caller owns an already stopped task.
    iovec io{&regs,sizeof(regs)};
    if (ptrace(PTRACE_GETREGSET, _pid, NT_PRSTATUS, &io) == 0 && io.iov_len == sizeof(regs)) return true;
    if (ptrace(PTRACE_INTERRUPT, _pid, nullptr, nullptr) == -1L) return false;
    int status = 0;
    auto got = wait(&status, __WALL, std::min(_remoteCallTimeout, 2000));
    if (got != _pid) return false;
    if (WIFEXITED(status) || WIFSIGNALED(status)) {_attached = false;return false;}
    return WIFSTOPPED(status) && getRegs(&regs);
}
