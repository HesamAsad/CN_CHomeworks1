#pragma once

#include <string>
#include <queue>
#include <map>

struct User {
    std::string name;
    std::queue<std::pair<int, std::string>> messages;
};
