//
// Created by toor on 10.12.2020.
//
#include <uv.h>
#include <iostream>
#include <vector>

class User;

using namespace std;

#ifndef UV_ECHO_CHANNEL_H
#define UV_ECHO_CHANNEL_H


class Channel {
public:
    string name;
    Channel(string name);
    void addUser(User *user);
    void removeUser(User *user);
    vector<string> getUsers();
    vector<User *> users;
};


#endif //UV_ECHO_CHANNEL_H
