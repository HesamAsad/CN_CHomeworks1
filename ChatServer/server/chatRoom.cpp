#include "chatRoom.h"
#include <cstdint>
#include <utility>
#include <vector>
#include <algorithm> 

using namespace std;

int ChatRoom::connect(string name) {
    users[next_user_id] = User{name: name};
    return next_user_id++;
}

void ChatRoom::disconnect(int user_id) {
    users.erase(user_id);
}

vector<int> ChatRoom::list() {
    vector<int> ret;
    for (auto &user : users)
        ret.push_back(user.first);
    return ret;
}

User ChatRoom::info(int user_id) {
    return users.at(user_id);
}

void ChatRoom::send(int from_id, int to_id, string message) {
    auto& user = users[to_id];
    user.messages.push(make_pair(from_id, message));
}

pair<int, string> ChatRoom::receive(int user_id) {
    auto& user = users[user_id];
    if (user.messages.empty())
        return make_pair(0, "");
    auto message = user.messages.front();
    user.messages.pop();
    return message;
}
