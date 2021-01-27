//
// Created by toor on 10.12.2020.
//

#include "User.h"

vector<string> User::getChannels() {
    vector<string> ret;
    for(int i=0;i<this->channels.size(); i++) {
        ret.push_back(this->channels.at(i)->name);
    }
    return ret;
}

string User::leaveChannel(Channel *channel) {
    for(int i=0;i<this->channels.size(); i++){
        if(this->channels.at(i) == channel) {
            this->channels.erase(channels.begin() + i);
            return "Group left";
        }
    }
    return "Group is not existent";
}

void User::joinChannel(Channel *channel) {
    this->channels.push_back(channel);
}

bool User::isOnChannel(string channel) {
    for(auto a: this->channels){
       if(a->name==channel)
           return true;
    }
    return false;
}

User::User() {
    this->nickname = "anonymous";
    this->nickname.append(to_string(this->socket));
}
