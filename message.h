#ifndef MESSAGE_H
# define MESSAGE_H
// error
# define ERR_TOOMANYCHANNELS(user, channel) "405 " + user + " " + channel + " :You have joined too many channels"
# define ERR_NONICKNAMEGIVEN(user) "431 " + user + " :Nickname not given"
# define ERR_NICKNAMEINUSE(user) "433 " + user + " " + user + " :Nickname is already in use"
# define ERR_NEEDMOREPARAMS(source, command) "461 " + source + " " + command + " :Not enough parameters"
# define ERR_ALREADYREGISTRED(source) "462 " + source + " :You may not register"
# define ERR_PASSWDMISMATCH(user) "464 " + user + " :Password incorrect"
# define ERR_NOORIGIN(user) "465 " + user + " :No origin specified"
# define ERR_CHANNELISFULL(user, channel) "471 " + user + " " + channel + " :Cannot join channel (+1)"
# define ERR_INVITEONLYCHAN(user, channel) "473 " + user + " " + channel + " :Cannot join channel (+i)"
# define ERR_BANNEDFROMCHAN(user, channel) "474 " + user + " " + channel + " :Cannot join channel (+b)"
# define ERR_BADCHANNELKEY(user, channel) "475 " + user + " " + channel + " :Cannot join channel (+k)"
# define ERR_QUIT(user, message) "ERROR :Closing link: (" + user + ") [Quit: " + message + "]"
# define ERR_NOSUCHSERVER(user, server) "402 " + user + " " + server + " :No such server"
# define ERR_NOSUCHCHANNEL(user, channel) "403 " + user + " " + channel + " :No such channel"
# define ERR_CHANOPRIVSNEEDED(user, channel) "482 " + user + " " + channel + " :You must be a channel operator"
# define ERR_CHANOPRIVSNEEDED2(user, channel) "482 " + user + " " + channel + " :You must be a channel half-operator"
# define ERR_CHANOPRIVSNEEDEDMODE(user, channel, mode) "482 " + user + " " + channel + " :You must have channel op access or above to set channel mode " + mode
# define ERR_USERNOTINCHANNEL(user, nick, channel) "441 " + user + " " + nick + " " + channel + " :They are not on that channel"
# define ERR_NOTONCHANNEL(user, channel) "442 " + user + " " + channel + " :You're not on that channel!"
# define ERR_CANNOTSENDTOCHAN(user, channel) "404 " + user + " " + channel + " :Cannot send to channel (no external messages)"
# define ERR_NOSUCHNICK(user, nick) "401 " + user + " " + nick + " :No such nick"
# define ERR_UNKNOWNMODE(user, mode) "472 " + user + " " + mode + " :is unknown mode char to me"
# define ERR_NOOPPARAM(user, channel, mode, modename, param) "696 " + user + " " + channel + " " + mode + " * :You must specify a parameter for the " + modename + " mode. Syntax: <" + param + ">."
# define ERR_LONGPWD(user, channel) ":" + user + " " + channel + " :Too long password"
# define ERR_USERONCHANNEL(user, nick, channel) "443 " + user + " " + nick + " " + channel + " :is already on channel"
// numeric
# define RPL_WELCOME(user) "001 " + user + " :Welcome to the happyirc network " + user + "!"
# define RPL_TOPIC(user, channel, topic) "332 " + user + " " + channel + " " + topic
# define RPL_TOPICWHOTIME(user, channel, nick, setat) "333 " + user + " " + channel + " " + nick + " " + setat
# define RPL_WHOREPLY(client, channel, user, host, server, nick, opt, real) "352 " + client + " " + channel + " " + user + " " + host + " " + server + " " + nick + " " + opt + " " + ":0 " + real
# define RPL_ENDOFWHO(user, name) "315 " + user + " " + name + " :End of /WHO list"
# define RPL_NAMREPLY(user, symbol, channel, users) "353 " + user + " " + symbol + " " + channel + " :" + users
# define RPL_ENDOFNAMES(user, channel) "366 " + user + " " + channel + " :End of /NAMES list."
# define RPL_LISTSTART(user) "321 " + user + " Channel :Users Name"
# define RPL_LIST(user, channel, visible, mode, topic) "322 " + user + " " + channel + " " + visible + " :" + mode + " " + topic
# define RPL_LISTEND(user) "323 " + user + ":End of /LIST"
# define RPL_NOTOPIC(user, channel) "331 " + user + " " + channel + " :No topic is set"
# define RPL_INVITING(user, nick, channel) "341 " + user + " " + nick + " :" + channel
# define RPL_CHANNELMODEIS(user, channel, modes, params) "324 " + user + " " + channel + " " + modes + params
# define RPL_CHANNELCREATETIME(user, channel, date) "329 " + user + " " + channel + " :" + date
# define RPL_ENDOFBANLIST(user, channel) "368 " + user + " " + channel + " :End of channel ban list"
// command
# define RPL_QUIT(user, message) ":" + user + " QUIT :Quit: " + message
# define RPL_PONG(user, ping) ":" + user + " PONG :" + ping
# define RPL_JOIN(user, channel) ":" + user + " JOIN :" + channel
# define RPL_PRIVMSG(user, target, msg) ":" + user + " PRIVMSG " + target + msg
# define RPL_MY_TOPIC(user, channel, topic) ":" + user + " TOPIC " + channel + " " + topic
# define RPL_PART(user, channel) ":" + user + " PART " + " :" + channel
# define RPL_KICK(user, channel, nick) ":" + user + " KICK " + channel + " " + nick + " :"
# define RPL_INVITE(user, nick, channel) ":" + user + " INVITE " + nick + " :" + channel
# define RPL_MODE(user, channel, modes, params) ":" + user + " MODE " + channel + " " + modes + params
# define RPL_NICK(before, after) ":" + before + " NICK :" + after

#endif  // MESSAGE_H