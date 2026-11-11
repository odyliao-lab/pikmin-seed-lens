#pragma once
// Self-only runner; exercise the exact candidate runtime dispatcher as well as
// the shared adapter. The target explicitly defines SEED_API31_CANDIDATE.
#include "../../app/bridge_dispatch.h"
namespace seed_probe_api31 { using Platform=seed_bridge::PlatformForRuntime; }
