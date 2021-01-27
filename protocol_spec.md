# CHAT Protocol specificiation

## Main rules

* Text based protocol
* All communication is defined by commands
* Each command could get arguments with its constant number typical for command
* Delimiter for nickname or channel name argument is $ sign
* Listening port of server is 7000

## Command list

* __SETNICKNAME__ _$nickname of user$_
* __LISTUSERS__
* __CREATECHANNEL__ _$name of channel$_
* __JOINCHANNEL__ _$name of channel$_
* __LEAVECHANNEL__ _$name of channel$_
* __REMOVECHANNEL__ _$name of channel$_
* __SENDTOCHANNEL__ _$name of channel$_ Body of the message
* __SENDTOUSER__ _$nickname of user$_ Body of the message 
