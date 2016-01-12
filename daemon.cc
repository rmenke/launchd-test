#include "daemon.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <errno.h>
#include <fcntl.h>
#include <launch.h>
#include <stdlib.h>
#include <sysexits.h>

#include <sys/socket.h>

static inline void Exit(int status, const std::string &msg) {
  std::cerr << getprogname() << ": " << msg << std::endl;
  exit(status);
}

static inline void Exit(int status, int code) {
  Exit(status, strerror(code));
}

static inline std::vector<int> LaunchActivateSocket(const std::string &name) {
  int *fds;
  size_t cnt;

  if (int result = launch_activate_socket(name.c_str(), &fds, &cnt)) {
    Exit(EX_OSERR, result);
  }

  std::vector<int> result { fds, fds + cnt };
  free(fds);
  return result;
}

int main(int argc, const char **argv) {
  int socket = LaunchActivateSocket("com.the-wabe.launchd-test").at(0);

  int flags = fcntl(socket, F_GETFL, 0);
  if (flags < 0) {
    Exit(EX_IOERR, errno);
  }

  std::cerr << "flags = " << flags << std::endl;

  if (fcntl(socket, F_SETFL, O_NONBLOCK | flags) < 0) {
    Exit(EX_IOERR, errno);
  }

  flags = fcntl(socket, F_GETFL, 0);
  if (flags < 0) {
    Exit(EX_IOERR, errno);
  }

  std::cerr << "flags = " << flags << std::endl;

  sockaddr_storage addr;
  socklen_t addrlen = sizeof(addr);

  while (true) {
    std::cerr << "Accepting connection..." << std::endl;

    int connection = accept(socket, reinterpret_cast<sockaddr *>(&addr), &addrlen);

    std::cerr << "Accepted." << std::endl;

    if (connection < 0) {
      if (errno == EWOULDBLOCK) {
	std::cerr << "Exiting normally" << std::endl;
        exit(0);
      }
      else {
        Exit(EX_IOERR, errno);
      }
    }

    std::string filename { "/dev/fd/" };
    filename += std::to_string(connection);

    std::ofstream file { filename };

    file << "Hello, client" << std::endl;
    file << "Goodbye, client" << std::endl;

    close(connection);
  }
}
