//
// Created by toor on 10.12.2020.
//

#include "Channel.h"
#include "User.h"

Channel::Channel(string name) {
    this->name = name;
}

void Channel::addUser(User *user) {
    this->users.push_back(user);
}

void Channel::removeUser(User *user) {
    for(int i=0;i<this->users.size();i++) {
        if(this->users.at(i)==user) {
            this->users.erase(this->users.begin()+i);
        }
    }
}

vector<string> Channel::getUsers() {
    vector<string> ret;
    for(int i=0;i<this->users.size();i++){
        ret.push_back(this->users.at(i)->nickname);
    }
    return ret;
}

