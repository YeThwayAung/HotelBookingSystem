#include "Admin.h"

Admin::Admin(string u, string p) : username(u), password(p) {}

bool Admin::login(const string& u, const string& p) const {
    return username == u && password == p;
}