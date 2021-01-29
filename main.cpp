#include <cstring>
#include <uv.h>
#include <iostream>
#include <vector>
#include "User.h"

using namespace std;

uv_loop_t *loop;
sockaddr_in addr;

vector<User *> clients;
vector<Channel *> channels;

void echo_write(uv_write_t *req, int status) {
    if (status) {
        cout << "Write error " <<  uv_strerror(status) << endl;
    }
    delete req;
}

// 1. Find first $ charater in command
// 2. Find second $ character in command
// 3. Return substring from between $ marks
// http://www.cplusplus.com/reference/string/string/
string get_first_attribute(string command){
    int pos1, pos2;
    pos1 = command.find("$");
    if(pos1==string::npos) {
        return "";
    }
    pos2 = command.find("$",pos1+1);
    if(pos2==string::npos) {
        return "";
    }
    if(pos2-pos1-1>16)
        pos2 = pos1+17;
    return command.substr(pos1+1, pos2-pos1-1);
}

string get_message(string command){
    int pos1, posMessageStart, posMessageEnd;
    pos1 = command.find("$");
    if(pos1==string::npos) {
        return "";
    }
    posMessageStart = command.find("$",pos1+1);
    if(posMessageStart == string::npos) {
        return "";
    }
    posMessageEnd = command.find("$", posMessageStart + 1);
    if (posMessageEnd ==string::npos) {
        return "";
    }
    int messageLength = posMessageEnd - posMessageStart;
    return command.substr(posMessageStart + 1, messageLength - 1);
}

void sendResponse(User *destination, string response, uv_write_t *req) {
    uv_buf_t wrbuf = uv_buf_init((char *) response.c_str(), response.size());
    uv_write(req, (uv_stream_t *) destination, &wrbuf, 1, echo_write);
}

void sendCommandToAll(string command, uv_write_t *req) {
    for (int i = 0; i < clients.size(); i++) {
        sendResponse(clients[i], command, req);
    }
}

void sendCommandTo(vector<User *> destination, string command, uv_write_t *req) {
    for (auto u : destination) {
        sendResponse(u, command, req);
    }
}

void command_parser(User *client, string command) {
    auto *req = new uv_write_t;
    string response;
    if(command.find("SETNICKNAME")==0) {
        string nick = get_first_attribute(command);
        if(nick.length()>0) {
            response.append("Nick successfully set\n\r");
            sendResponse(client, response, req);
            client->nickname = nick;
        } else {
            response.append("Nick is to short\n\r");
            sendResponse(client, response, req);
        }
    } else if(command.find("LISTUSERS")==0) {
        response.append("USERS\n");
        for(int i=0;i<clients.size();i++) {
            response.append(clients.at(i)->nickname);
            response.append("( ");
            for(auto a: clients.at(i)->getChannels()){
                response.append("#").append(a).append(" ");
            }
            response.append(")\n");
        }
        sendResponse(client, response, req);
    } else if(command.find("LISTCHANNELS")==0) {
        response.append("CHANNELS\n");
            for(int i=0;i<channels.size();i++) {
                response.append(channels.at(i)->name);
                response.append("\n");
            }
        sendResponse(client, response, req);
} else if(command.find("CREATECHANNEL")==0) {
        string channelName = get_first_attribute(command);
        if(channelName.length()>0) {
            response.append("Channel successfully created\n\r");
            sendResponse(client, response, req);
            Channel *channel = new Channel(channelName);
            channels.push_back(channel);
        } else {
            response.append("Name is to short\n\r");
            sendResponse(client, response, req);
        }
    } else if(command.find("JOINCHANNEL")==0) {
        string channelName = get_first_attribute(command);
        if(channelName.length()==0) {
            response.append("Name is to short\n\r");
        } else {
            response.append("Channel can't be find\n\r");
            for (int i = 0; i < channels.size(); i++) {
                if (channelName == channels.at(i)->name) {
                    channels.at(i)->addUser(client);
                    client->joinChannel(channels.at(i));
                    response.assign("User ");
                    response.append(client->nickname);
                    response.append(" joined channel ");
                    response.append(channelName);
                    response.append("\n\r");
                    for (int j = 0; j < channels.at(i)->users.size(); j++) {
                        if(client!=channels.at(i)->users.at(j)) {
                            sendResponse(channels.at(i)->users.at(j), response, req);
                        }
                    }
                    response.clear();
                    response = "JOINCHANNEL\n";
                    response.append(channelName).append("\0");
                    sendResponse(client, response, req);
                    return;
                }
            }
            sendResponse(client, response, req);
        }
    } else if(command.find("LEAVECHANNEL")==0) {
        response.append("Channel successfully left\n\r");
        string channelName = get_first_attribute(command);
        if(channelName.length()==0) {
            response.append("Name is to short\n\r");
        } else if(client->isOnChannel(channelName)){
            for(int i=0;i<channels.size();i++){
                if(channels.at(i)->name==channelName){
                    client->leaveChannel(channels.at(i));
                    channels.at(i)->removeUser(client);
                }
            }
        }
        sendResponse(client, response, req);
    } else if(command.find("REMOVECHANNEL")==0) {
        response.append("Channel successfully removed\n\r");
        string channelName = get_first_attribute(command);
        if(channelName.length()==0) {
            response.append("Name is to short\n\r");
        } else if(client->isOnChannel(channelName)){
            for(int i=0;i<channels.size();i++){
                if(channels.at(i)->name==channelName){
                    for(int j=0;j<channels.at(i)->users.size();j++) {
                        channels.at(i)->users.at(j)->leaveChannel(channels.at(i));
                    }
                    delete channels.at(i);
                    channels.erase(channels.begin()+i);
                }
            }
        }
        sendResponse(client, response, req);
    } else if(command.find("SENDTOCHANNEL")==0) {
        string channelName = get_first_attribute(command);
        string message = get_message(command);
        if(channelName.length()==0) {
            response.append("Name is to short\n\r");
        } else if(client->isOnChannel(channelName)){
            response.append("Channel can't be find\n\r");
            for (int i = 0; i < channels.size(); i++) {
                if (channelName == channels.at(i)->name) {
                    response.assign(client->nickname);
                    response.append("@");
                    response.append(channelName);
                    response.append(": ");
                    response.append(message);
                    response.append("\n");
                    for (int j = 0; j < channels.at(i)->users.size(); j++) {
                        if(channels.at(i)->users.at(j)!=client) { //?
                            sendResponse(channels.at(i)->users.at(j), response, req);
                        }
                    }
                    return;
                }
            }
        } else {
            response.append("You are not a member of this channel\n\r");
        }
        sendResponse(client, response, req);
    } else if(command.find("SENDTOUSER")==0) {
        string user = get_first_attribute(command);
        string message = get_message(command);
        for(int i=0;i<clients.size();i++) {
            if(clients.at(i)->nickname==user) {
                response.assign(client->nickname);
                response.append(": ");
                response.append(message);
                response.append("\n");
                sendResponse(client, response, req);
            }
        }
    } else {
// TODO: Send to client list of all available commands
    }
}

void remove_from_clients_list(uv_tcp_t *client){
    for(int i=0;i<clients.size();i++) {
       if(clients.at(i)==client) {
           cout << "Remove client from list" << endl;
           clients.erase(clients.begin()+i);
           break;
       }
    }
}

void alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) {
    buf->base = new char[suggested_size];
    buf->len = suggested_size;
}

void echo_read(uv_stream_t *client, ssize_t nread, const uv_buf_t *buf) {
    if (nread < 0) {
        if (nread != UV_EOF) {
            cout << "Read error " << uv_err_name(nread) << endl;
            remove_from_clients_list((uv_tcp_t *)client);
            uv_close((uv_handle_t*) client, nullptr);
        }
    } else if (nread > 0) {
//        auto *req = new uv_write_t;
//        uv_buf_t wrbuf = uv_buf_init(buf->base, nread);
        command_parser((User *)client, buf->base);
//        for(int i=0;i<clients.size();i++) {
//            uv_write(req, (uv_stream_t*) clients.at(i), &wrbuf, 1, echo_write);
//        }
    }

    if (buf->base) {
        delete buf->base;
    }
}

void on_new_connection(uv_stream_t *server, int status) {
    if (status < 0) {
        cout << "New connection error " <<  uv_strerror(status);
        return;
    }

    auto *client = new User;
    clients.push_back(client);
    cout << "Number of clients: " << clients.size() << endl;
    uv_tcp_init(loop, client);
    if (uv_accept(server, (uv_stream_t*) client) == 0) {
        uv_read_start((uv_stream_t*)client, alloc_buffer, echo_read);
    } else {
        remove_from_clients_list(client);
        uv_close((uv_handle_t*) client, nullptr);
    }
}

int main() {
    loop = uv_default_loop();

    uv_tcp_t server;
    uv_tcp_init(loop, &server);

    uv_ip4_addr("0.0.0.0", 7000, &addr);

    uv_tcp_bind(&server, (const sockaddr*)&addr, 0);
    int r = uv_listen((uv_stream_t*)&server, 128, on_new_connection);
    if (r) {
        cout << "Listen error " << uv_strerror(r);
        return 1;
    }
    return uv_run(loop, UV_RUN_DEFAULT);
}

