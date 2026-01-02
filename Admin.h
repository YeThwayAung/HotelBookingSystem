#ifndef ADMIN_H
#define ADMIN_H

#include <string>
using namespace std;

class Admin {
private:
    string username;
    string password;

public:
    Admin(string u = "", string p = "");
    bool login(const string& u, const string& p) const;
};

#endif