#include "User.h"

void User::decrease_size(int size_) {
    size -= size_;
}

std::string User::get_username() { return username; }

std::string User::get_password() { return password; }

bool User::is_admin() { return admin; }

int User::get_size() { return size; }