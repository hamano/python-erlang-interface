PyErl
=====

[![PyPI](https://img.shields.io/pypi/v/PyErl.svg)](https://pypi.python.org/pypi/PyErl/)
[![Build Status](https://travis-ci.org/hamano/python-erlang-interface.svg?branch=master)](https://travis-ci.org/hamano/python-erlang-interface)
[![GitHub license](https://img.shields.io/github/license/hamano/python-erlang-interface.svg)]()

PyErl is Erlang Interface for Python. which help you integrate
programs written in Python and Erlang.

DEPENDENCIES
------------

This module requires these other modules and libraries:

 Erlang/OTP R12 or later

In debian:
 apt-get install erlang-dev

INSTALL
-------

 python setup.py build
 python setup.py install

EXAMPLE
-------

* make erlang terms
 import pyerl
 atom = pyerl.mk_atom("hello")
 string = pyerl.mk_string("world")
 int = pyerl.mk_int(-1)
 list = pyerl.mk_list([atom, string, int])
 print list

* call rpc
 # just call pingpong:ping() in node2@localhost
 import pyerl
 host = "localhost"
 name = "node1"
 node = name + "@" + host
 cookie = "TESTCOOKIE"
 pyerl.connect_xinit(host, name, node, "127.0.0.1", cookie, 1)
 pyerl.xconnect("127.0.0.1", "node2")
 atom = pyerl.mk_atom("ping")
 args = pyerl.mk_list([atom]);
 eterm = pyerl.rpc(sock, "pingpong", "ping", args);
 pyerl.close_connection(sock);
 print eterm

AUTHER
------
Tsukasa Hamano <code@cuspy.org>
