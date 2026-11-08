#pragma once
#include "load_journal.h"
#include <string>
int seed_x64_load(int pid,const std::string& start,const std::string& host,seed_root::LoadAttempt& attempt);
// Called only by the bounded trace implementation inside the supervised worker.
bool seed_game_trace_admitted(int tid);
