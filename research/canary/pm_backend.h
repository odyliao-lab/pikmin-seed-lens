#pragma once
#include "../../app/load_journal.h"
#include <string>
// This separately built experiment accepts only dev.ody.seedlensprobe/API31.
int seed_probe_pm_load(int pid,const std::string& start,const std::string& host,
                      seed_root::LoadAttempt& attempt,bool timeoutTest);
