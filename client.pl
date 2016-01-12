#!/usr/bin/perl

use strict;
use warnings;

use Socket;

die "No LAUNCHD_TEST_SOCK available" unless $ENV{LAUNCHD_TEST_SOCK};

my $sockaddr = pack_sockaddr_un($ENV{LAUNCHD_TEST_SOCK});

socket SOCKET, PF_UNIX, SOCK_STREAM, 0
  or die "socket: $!\n";
connect SOCKET, $sockaddr
  or die "connect: $!\n";

print while <SOCKET>;
