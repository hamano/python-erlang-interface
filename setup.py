#!/usr/bin/env python

from distutils.core import setup, Extension

setup(name = "PyErl",
      version = "0.2.2",
      description = "Erlang Interface for Python",
      long_description = '''
PyErl is Erlang Interface for Python. which help you integrate
programs written in Python and Erlang.
''',
      author = "Tsukasa Hamano",
      author_email = "code@cuspy.org",
      url="http://github.com/hamano/python-erlang-interface",
      download_url="http://pypi.python.org/pypi/PyErl/",
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
