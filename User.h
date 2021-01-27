//
// Created by toor on 10.12.2020.
//
#include <uv.h>
#include <iostream>
#include <vector>
#include "Channel.h"

using namespace std;

#ifndef UV_ECHO_USER_H
#define UV_ECHO_USER_H


class User: public uv_tcp_t {
public:
    string nickname;
    User();
    void joinChannel(Channel *channel);
    string leaveChannel(Channel *channel);
    vector<string> getChannels();
    bool isOnChannel(string channel);
private:
    vector<Channel *> channels;
};


#endif //UV_ECHO_USER_H
