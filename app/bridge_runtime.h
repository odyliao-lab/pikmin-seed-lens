#pragma once
#include "bridge_policy.h"
#include <string>
#include <android/api-level.h>
#include <sys/system_properties.h>
namespace seed_bridge {
inline Backend runtime_backend(){
    char sdk[PROP_VALUE_MAX]{},bridge[PROP_VALUE_MAX]{};
    int api=android_get_device_api_level();
    __system_property_get("ro.build.version.sdk",sdk);
    // Exact text as well as native API: no atoi prefix, API32 or spoof fallback.
    if(std::string_view(sdk)!=std::to_string(api))return Backend::Unsupported;
    __system_property_get("ro.dalvik.vm.native.bridge",bridge);
    return backend_for(api,bridge,api31Candidate);
}
}
