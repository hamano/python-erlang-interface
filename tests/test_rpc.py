#!/usr/bin/env python

import unittest
import subprocess
import time
import pyerl

class RPCTest(unittest.TestCase):
    def setUp(self):
        ret = subprocess.call(["erlc", "tests/pingpong.erl"])
        self.assertEqual(ret, 0)
        cmd = 'erl -noshell -setcookie "TESTCOOKIE" -sname node1@localhost -s pingpong start'
        self.proc = subprocess.Popen(cmd, shell=True)

    def tearDown(self):
        ret = subprocess.call(["kill", str(self.proc.pid)])
        pyerl.eterm_release()
        allocated, freed = pyerl.eterm_statistics()
        self.assertEqual(allocated, 0)
        self.assertEqual(freed, 0)
        self.proc.wait()

    def test_rpc(self):
        host = "localhost"
        name = "test"
        node = name + "@" + host
        cookie = "TESTCOOKIE"
        ret = pyerl.connect_xinit(host, name, node, "127.0.0.1", cookie, 1)
        self.assertEqual(ret, 1);
        retry = 0
        while True:
            time.sleep(1)
            sock = pyerl.xconnect("127.0.0.1", "node1")
            if sock > 0: break
            if retry > 3: self.fail()
            retry += 1
        self.assertEqual(sock > 0, True)
        atom = pyerl.mk_atom("ping")
        args = pyerl.mk_list([atom]);
        eterm = pyerl.rpc(sock, "pingpong", "ping", args);
        ret = pyerl.close_connection(sock);
        self.assertEqual(ret, 0);
        self.assertEqual(eterm.type, pyerl.ATOM);
        self.assertEqual(eterm.is_atom(), True);
        self.assertEqual(str(eterm), "pong");

if __name__ == '__main__':
    unittest.main()
