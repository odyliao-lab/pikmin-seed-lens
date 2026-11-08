# Pinned upstream stays untouched. ARM64 replaces both complete call methods;
# it never patches target executable memory or suppresses unknown signals.
include("${CMAKE_CURRENT_LIST_DIR}/trace-replace.cmake")
file(READ "${TRACE}" TRACE_TEXT)
seed_replace_once("#include \"KittyTrace.hpp\"" "#include \"KittyTrace.hpp\"\n#include <chrono>\n#include <algorithm>\n#include <sys/syscall.h>\n#include \"${CMAKE_CURRENT_LIST_DIR}/../app/arm64_context.h\"")
file(READ "${CMAKE_CURRENT_LIST_DIR}/bounded_wait.inc" replacement)
seed_replace_method("pid_t KittyTraceMgr::wait(" "bool KittyTraceMgr::waitSyscall()" "${replacement}")
file(READ "${CMAKE_CURRENT_LIST_DIR}/bounded_stop.inc" replacement)
seed_replace_method("bool KittyTraceMgr::stop()" "bool KittyTraceMgr::cont(" "${replacement}")
file(READ "${CMAKE_CURRENT_LIST_DIR}/arm64_calls.inc" replacement)
seed_replace_method("kitty_rp_call_t KittyTraceMgr::_callFunctionFrom(" "KT_BP_RESULT KittyTraceMgr::setSoftBreakpointAndWait(" "${replacement}")
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_LIST_DIR}/bounded_wait.inc" "${CMAKE_CURRENT_LIST_DIR}/bounded_stop.inc"
  "${CMAKE_CURRENT_LIST_DIR}/arm64_calls.inc")
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/KittyTrace-arm64.cpp" "${TRACE_TEXT}")
