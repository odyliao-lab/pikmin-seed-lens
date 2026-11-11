#pragma once
#include "../../app/load_journal.h"
#include <string>
// Each separately built experiment accepts only its pinned self-probe/API tuple.
#ifndef SEED_PROBE_API
#define SEED_PROBE_API 31
#endif
#if defined(SEED_API32_PM_RESEARCH)
static_assert(SEED_PROBE_API==32);
#elif defined(SEED_API34_PM_RESEARCH)
static_assert(SEED_PROBE_API==34);
#else
static_assert(SEED_PROBE_API==31||SEED_PROBE_API==35);
#endif
int seed_probe_pm_load(int pid,const std::string& start,const std::string& host,
                      seed_root::LoadAttempt& attempt,bool timeoutTest);
