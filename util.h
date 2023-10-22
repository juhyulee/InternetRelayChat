#ifndef UTIL_H
# define UTIL_H

# include <fcntl.h>
# include <arpa/inet.h>
# include <stdlib.h>
# include <string.h>
# include <sys/event.h>
# include <sys/socket.h>
# include <sys/time.h>
# include <sys/types.h>
# include <unistd.h>
# include <cstdint>
# include <string>
# include <vector>
# include <map>
# include <set>
# include <iterator>
# include <iostream>
# include <ostream>
# include <sstream>
# include <utility>

# include "message.h"

// for class client
# define CLIENT_CHANNEL_LIMIT 10
# define D_REALNAME "default_realname"


// for class channel
# define CHANNEL_USER_LIMIT 1000



#endif
