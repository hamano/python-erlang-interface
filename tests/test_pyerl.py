#!/usr/bin/env python

import unittest
import pyerl

class PyErlTest(unittest.TestCase):
    def setUp(self):
        pyerl.init()

    def tearDown(self):
        pyerl.eterm_release()
        allocated, freed = pyerl.eterm_statistics()
        self.assertEqual(allocated, 0)
        self.assertEqual(freed, 0)

    def test_mk_atom(self):
        eterm = pyerl.mk_atom("atom")
        self.assertEqual(eterm.type, pyerl.ATOM);
        self.assertEqual(eterm.is_atom(), True);
        self.assertEqual(str(eterm), "atom");

    def test_mk_binary(self):
        eterm = pyerl.mk_binary("\x00\x01\x02\x03\x04")
        self.assertEqual(eterm.type, pyerl.BINARY);
        self.assertEqual(eterm.is_binary(), True);
        self.assertEqual(str(eterm), "#Bin<0,1,2,3,4>");

    def test_mk_empty_list(self):
        eterm = pyerl.mk_empty_list()
        self.assertEqual(eterm.type, pyerl.EMPTY_LIST);
        self.assertEqual(eterm.is_empty_list(), True);
        self.assertEqual(eterm.is_list(), True);
        self.assertEqual(eterm.is_nil(), True);
        self.assertEqual(str(eterm), "[]");

    def test_mk_estring(self):
        eterm = pyerl.mk_estring("e\x00string")
        self.assertEqual(eterm.type, pyerl.LIST);
        self.assertEqual(eterm.type, pyerl.CONS);
        self.assertEqual(eterm.is_list(), True);
        self.assertEqual(eterm.is_cons(), True);
        self.assertEqual(str(eterm), "\"e\\x0string\"");

    def test_mk_float(self):
        eterm = pyerl.mk_float(0.000001)
        self.assertEqual(eterm.type, pyerl.FLOAT);
        self.assertEqual(eterm.is_float(), True);
        self.assertEqual(str(eterm), "0.000001");

    def test_mk_int(self):
        eterm = pyerl.mk_int(-1)
        self.assertEqual(eterm.type, pyerl.INTEGER);
        self.assertEqual(eterm.is_integer(), True);
        self.assertEqual(str(eterm), "-1");

    def test_mk_longlong(self):
        eterm = pyerl.mk_longlong(-1)
        self.assertEqual(eterm.type, pyerl.LONGLONG);
        self.assertEqual(eterm.is_longlong(), True);
        self.assertEqual(str(eterm), "-1");

    def test_mk_list(self):
        atom = eterm = pyerl.mk_atom("atom")
        integer = eterm = pyerl.mk_int(-1)
        string = eterm = pyerl.mk_string("string")
        eterm = pyerl.mk_list([atom, integer, string])
        self.assertEqual(eterm.is_list(), True);
        self.assertEqual(eterm.is_cons(), True);
        self.assertEqual(eterm.type, pyerl.LIST);
        self.assertEqual(str(eterm), "[atom, -1, \"string\"]");

    def test_mk_pid(self):
        eterm = pyerl.mk_pid("node", 1, 2, 3)
        self.assertEqual(eterm.type, pyerl.PID);
        self.assertEqual(eterm.is_pid(), True);
        self.assertEqual(str(eterm), "<node.1.2>");

    def test_mk_port(self):
        eterm = pyerl.mk_port("node", 1, 2)
        self.assertEqual(eterm.type, pyerl.PORT);
        self.assertEqual(eterm.is_port(), True);
        self.assertEqual(str(eterm), "#Port<1.2>");

    def test_mk_ref(self):
        eterm = pyerl.mk_ref("node", 1, 2)
        self.assertEqual(eterm.type, pyerl.REF);
        self.assertEqual(eterm.is_ref(), True);
        self.assertEqual(str(eterm), "#Ref<1>");

    def test_mk_long_ref(self):
        eterm = pyerl.mk_long_ref("node", 1, 2, 3, 4)
        self.assertEqual(eterm.type, pyerl.REF);
        self.assertEqual(eterm.is_ref(), True);
        self.assertEqual(str(eterm), "#Ref<3.2.1>");

    def test_mk_string(self):
        eterm = pyerl.mk_string("string")
        self.assertEqual(eterm.type, pyerl.LIST);
        self.assertEqual(eterm.type, pyerl.CONS);
        self.assertEqual(eterm.is_list(), True);
        self.assertEqual(eterm.is_cons(), True);
        self.assertEqual(str(eterm), "\"string\"");

    def test_mk_list(self):
        atom = eterm = pyerl.mk_atom("atom")
        integer = eterm = pyerl.mk_int(-1)
        string = eterm = pyerl.mk_string("string")
        eterm = pyerl.mk_tuple((atom, integer, string))
        self.assertEqual(eterm.is_tuple(), True);
        self.assertEqual(eterm.type, pyerl.TUPLE);
        self.assertEqual(str(eterm), "{atom, -1, \"string\"}");

    def test_mk_uint(self):
        eterm = pyerl.mk_uint(1)
        self.assertEqual(eterm.type, pyerl.U_INTEGER);
        self.assertEqual(eterm.is_unsigned_integer(), True);
        self.assertEqual(str(eterm), "1");

    def test_mk_ulonglong(self):
        eterm = pyerl.mk_ulonglong(1)
        self.assertEqual(eterm.type, pyerl.U_LONGLONG);
        self.assertEqual(eterm.is_unsigned_longlong(), True);
        self.assertEqual(str(eterm), "1");

    def test_mk_var(self):
        eterm = pyerl.mk_var("var")
        self.assertEqual(eterm.type, pyerl.VARIABLE);
        # Cannot convert string, I don't know why.
        # self.assertEqual(str(eterm), "var");

if __name__ == '__main__':
    unittest.main()
