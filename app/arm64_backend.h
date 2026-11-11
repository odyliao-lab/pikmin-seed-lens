#pragma once
#include "load_journal.h"
int seed_arm64_load(int pid,const std::string& start,const std::string& payload,
                    int api,seed_root::LoadAttempt& attempt);
