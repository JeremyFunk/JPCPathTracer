#pragma once
#include <string>
#include <vector>

namespace jpctracer::utility
{
std::vector<std::string> split(const std::string &str,
                               const std::string &delim);
}