pid_t KittyTraceMgr::wait(int *status, int options, int timeout_ms) const
{
    if (!_attached || !status || timeout_ms <= 0 || timeout_ms > 30000) {
        errno = EINVAL;
        return -1;
    }
    const auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(timeout_ms);
    for (;;) {
        // EINTR must consume the same wall-clock budget; never restart it.
        pid_t got = waitpid(_pid, status, options | __WALL | WNOHANG);
        if (got > 0 || (got < 0 && errno != EINTR)) return got;
        if (std::chrono::steady_clock::now() >= deadline) return 0;
        usleep(1000);
    }
}
