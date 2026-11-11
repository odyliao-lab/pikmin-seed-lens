#pragma once
#include <string>
namespace seed_extended {
struct InsightContext { std::string session; void *inventory{}; };
using ContextReader=InsightContext(*)();
void start(const char *game_data_directory,ContextReader reader=nullptr);
}
