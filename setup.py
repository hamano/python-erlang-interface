#!/usr/bin/env python

from distutils.core import setup, Extension

setup(name = "PyErl",
      version = "0.1.10",
      description = "Erlang Interface for Python",
      author = "Tsukasa Hamano",
      author_email = "code@cuspy.org",
      url="http://github.com/hamano/python-erlang-interface",
      license="MPL",
      ext_modules = [
        Extension(
            "pyerl",
            ["pyerl.c", "eterm.c"],
            libraries=["erl_interface", "ei"]
            )
        ],
      classifiers = [
        'Programming Language :: Python',
        'Programming Language :: Erlang',
        'Topic :: Software Development :: Libraries :: Python Modules',
        ],
      )
