#!/usr/bin/env python

from distutils.core import setup, Extension
from os import path

search_dirs = [
    "/usr/",
    "/usr/local/",
    "/usr/local/otp/",
    ]

otp_dir = None
for d in search_dirs:
    if path.isfile(d + 'include/erl_interface.h'):
        otp_dir = d

if otp_dir == None:
    print 'Cannot find Erlang/OTP directory.'
    print 'You need to install Erlang/OTP.'
    exit(1)

include_dirs = [otp_dir + 'include/']
library_dirs = [otp_dir + 'lib/']

setup(name = "PyErl",
      version = "0.5",
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
            libraries=["erl_interface", "ei"],
            include_dirs = include_dirs,
            library_dirs = library_dirs,
            )
        ],
      classifiers = [
        'Programming Language :: Python',
        'Programming Language :: Erlang',
        'Topic :: Software Development :: Libraries :: Python Modules',
        ],
      )
