r"""Wrapper for core_loop.h

Generated with:
/home/anze/anaconda3/bin/ctypesgen ../coreloop/core_loop.h

Do not modify this file.
"""

__docformat__ = "restructuredtext"

# Begin preamble for Python

import ctypes
import sys
from ctypes import *  # noqa: F401, F403

_int_types = (ctypes.c_int16, ctypes.c_int32)
if hasattr(ctypes, "c_int64"):
    # Some builds of ctypes apparently do not have ctypes.c_int64
    # defined; it's a pretty good bet that these builds do not
    # have 64-bit pointers.
    _int_types += (ctypes.c_int64,)
for t in _int_types:
    if ctypes.sizeof(t) == ctypes.sizeof(ctypes.c_size_t):
        c_ptrdiff_t = t
del t
del _int_types



class UserString:
    def __init__(self, seq):
        if isinstance(seq, bytes):
            self.data = seq
        elif isinstance(seq, UserString):
            self.data = seq.data[:]
        else:
            self.data = str(seq).encode()

    def __bytes__(self):
        return self.data

    def __str__(self):
        return self.data.decode()

    def __repr__(self):
        return repr(self.data)

    def __int__(self):
        return int(self.data.decode())

    def __long__(self):
        return int(self.data.decode())

    def __float__(self):
        return float(self.data.decode())

    def __complex__(self):
        return complex(self.data.decode())

    def __hash__(self):
        return hash(self.data)

    def __le__(self, string):
        if isinstance(string, UserString):
            return self.data <= string.data
        else:
            return self.data <= string

    def __lt__(self, string):
        if isinstance(string, UserString):
            return self.data < string.data
        else:
            return self.data < string

    def __ge__(self, string):
        if isinstance(string, UserString):
            return self.data >= string.data
        else:
            return self.data >= string

    def __gt__(self, string):
        if isinstance(string, UserString):
            return self.data > string.data
        else:
            return self.data > string

    def __eq__(self, string):
        if isinstance(string, UserString):
            return self.data == string.data
        else:
            return self.data == string

    def __ne__(self, string):
        if isinstance(string, UserString):
            return self.data != string.data
        else:
            return self.data != string

    def __contains__(self, char):
        return char in self.data

    def __len__(self):
        return len(self.data)

    def __getitem__(self, index):
        return self.__class__(self.data[index])

    def __getslice__(self, start, end):
        start = max(start, 0)
        end = max(end, 0)
        return self.__class__(self.data[start:end])

    def __add__(self, other):
        if isinstance(other, UserString):
            return self.__class__(self.data + other.data)
        elif isinstance(other, bytes):
            return self.__class__(self.data + other)
        else:
            return self.__class__(self.data + str(other).encode())

    def __radd__(self, other):
        if isinstance(other, bytes):
            return self.__class__(other + self.data)
        else:
            return self.__class__(str(other).encode() + self.data)

    def __mul__(self, n):
        return self.__class__(self.data * n)

    __rmul__ = __mul__

    def __mod__(self, args):
        return self.__class__(self.data % args)

    # the following methods are defined in alphabetical order:
    def capitalize(self):
        return self.__class__(self.data.capitalize())

    def center(self, width, *args):
        return self.__class__(self.data.center(width, *args))

    def count(self, sub, start=0, end=sys.maxsize):
        return self.data.count(sub, start, end)

    def decode(self, encoding=None, errors=None):  # XXX improve this?
        if encoding:
            if errors:
                return self.__class__(self.data.decode(encoding, errors))
            else:
                return self.__class__(self.data.decode(encoding))
        else:
            return self.__class__(self.data.decode())

    def encode(self, encoding=None, errors=None):  # XXX improve this?
        if encoding:
            if errors:
                return self.__class__(self.data.encode(encoding, errors))
            else:
                return self.__class__(self.data.encode(encoding))
        else:
            return self.__class__(self.data.encode())

    def endswith(self, suffix, start=0, end=sys.maxsize):
        return self.data.endswith(suffix, start, end)

    def expandtabs(self, tabsize=8):
        return self.__class__(self.data.expandtabs(tabsize))

    def find(self, sub, start=0, end=sys.maxsize):
        return self.data.find(sub, start, end)

    def index(self, sub, start=0, end=sys.maxsize):
        return self.data.index(sub, start, end)

    def isalpha(self):
        return self.data.isalpha()

    def isalnum(self):
        return self.data.isalnum()

    def isdecimal(self):
        return self.data.isdecimal()

    def isdigit(self):
        return self.data.isdigit()

    def islower(self):
        return self.data.islower()

    def isnumeric(self):
        return self.data.isnumeric()

    def isspace(self):
        return self.data.isspace()

    def istitle(self):
        return self.data.istitle()

    def isupper(self):
        return self.data.isupper()

    def join(self, seq):
        return self.data.join(seq)

    def ljust(self, width, *args):
        return self.__class__(self.data.ljust(width, *args))

    def lower(self):
        return self.__class__(self.data.lower())

    def lstrip(self, chars=None):
        return self.__class__(self.data.lstrip(chars))

    def partition(self, sep):
        return self.data.partition(sep)

    def replace(self, old, new, maxsplit=-1):
        return self.__class__(self.data.replace(old, new, maxsplit))

    def rfind(self, sub, start=0, end=sys.maxsize):
        return self.data.rfind(sub, start, end)

    def rindex(self, sub, start=0, end=sys.maxsize):
        return self.data.rindex(sub, start, end)

    def rjust(self, width, *args):
        return self.__class__(self.data.rjust(width, *args))

    def rpartition(self, sep):
        return self.data.rpartition(sep)

    def rstrip(self, chars=None):
        return self.__class__(self.data.rstrip(chars))

    def split(self, sep=None, maxsplit=-1):
        return self.data.split(sep, maxsplit)

    def rsplit(self, sep=None, maxsplit=-1):
        return self.data.rsplit(sep, maxsplit)

    def splitlines(self, keepends=0):
        return self.data.splitlines(keepends)

    def startswith(self, prefix, start=0, end=sys.maxsize):
        return self.data.startswith(prefix, start, end)

    def strip(self, chars=None):
        return self.__class__(self.data.strip(chars))

    def swapcase(self):
        return self.__class__(self.data.swapcase())

    def title(self):
        return self.__class__(self.data.title())

    def translate(self, *args):
        return self.__class__(self.data.translate(*args))

    def upper(self):
        return self.__class__(self.data.upper())

    def zfill(self, width):
        return self.__class__(self.data.zfill(width))


class MutableString(UserString):
    """mutable string objects

    Python strings are immutable objects.  This has the advantage, that
    strings may be used as dictionary keys.  If this property isn't needed
    and you insist on changing string values in place instead, you may cheat
    and use MutableString.

    But the purpose of this class is an educational one: to prevent
    people from inventing their own mutable string class derived
    from UserString and than forget thereby to remove (override) the
    __hash__ method inherited from UserString.  This would lead to
    errors that would be very hard to track down.

    A faster and better solution is to rewrite your program using lists."""

    def __init__(self, string=""):
        self.data = string

    def __hash__(self):
        raise TypeError("unhashable type (it is mutable)")

    def __setitem__(self, index, sub):
        if index < 0:
            index += len(self.data)
        if index < 0 or index >= len(self.data):
            raise IndexError
        self.data = self.data[:index] + sub + self.data[index + 1 :]

    def __delitem__(self, index):
        if index < 0:
            index += len(self.data)
        if index < 0 or index >= len(self.data):
            raise IndexError
        self.data = self.data[:index] + self.data[index + 1 :]

    def __setslice__(self, start, end, sub):
        start = max(start, 0)
        end = max(end, 0)
        if isinstance(sub, UserString):
            self.data = self.data[:start] + sub.data + self.data[end:]
        elif isinstance(sub, bytes):
            self.data = self.data[:start] + sub + self.data[end:]
        else:
            self.data = self.data[:start] + str(sub).encode() + self.data[end:]

    def __delslice__(self, start, end):
        start = max(start, 0)
        end = max(end, 0)
        self.data = self.data[:start] + self.data[end:]

    def immutable(self):
        return UserString(self.data)

    def __iadd__(self, other):
        if isinstance(other, UserString):
            self.data += other.data
        elif isinstance(other, bytes):
            self.data += other
        else:
            self.data += str(other).encode()
        return self

    def __imul__(self, n):
        self.data *= n
        return self


class String(MutableString, ctypes.Union):

    _fields_ = [("raw", ctypes.POINTER(ctypes.c_char)), ("data", ctypes.c_char_p)]

    def __init__(self, obj=b""):
        if isinstance(obj, (bytes, UserString)):
            self.data = bytes(obj)
        else:
            self.raw = obj

    def __len__(self):
        return self.data and len(self.data) or 0

    def from_param(cls, obj):
        # Convert None or 0
        if obj is None or obj == 0:
            return cls(ctypes.POINTER(ctypes.c_char)())

        # Convert from String
        elif isinstance(obj, String):
            return obj

        # Convert from bytes
        elif isinstance(obj, bytes):
            return cls(obj)

        # Convert from str
        elif isinstance(obj, str):
            return cls(obj.encode())

        # Convert from c_char_p
        elif isinstance(obj, ctypes.c_char_p):
            return obj

        # Convert from POINTER(ctypes.c_char)
        elif isinstance(obj, ctypes.POINTER(ctypes.c_char)):
            return obj

        # Convert from raw pointer
        elif isinstance(obj, int):
            return cls(ctypes.cast(obj, ctypes.POINTER(ctypes.c_char)))

        # Convert from ctypes.c_char array
        elif isinstance(obj, ctypes.c_char * len(obj)):
            return obj

        # Convert from object
        else:
            return String.from_param(obj._as_parameter_)

    from_param = classmethod(from_param)


def ReturnString(obj, func=None, arguments=None):
    return String.from_param(obj)


# As of ctypes 1.0, ctypes does not support custom error-checking
# functions on callbacks, nor does it support custom datatypes on
# callbacks, so we must ensure that all callbacks return
# primitive datatypes.
#
# Non-primitive return values wrapped with UNCHECKED won't be
# typechecked, and will be converted to ctypes.c_void_p.
def UNCHECKED(type):
    if hasattr(type, "_type_") and isinstance(type._type_, str) and type._type_ != "P":
        return type
    else:
        return ctypes.c_void_p


# ctypes doesn't have direct support for variadic functions, so we have to write
# our own wrapper class
class _variadic_function(object):
    def __init__(self, func, restype, argtypes, errcheck):
        self.func = func
        self.func.restype = restype
        self.argtypes = argtypes
        if errcheck:
            self.func.errcheck = errcheck

    def _as_parameter_(self):
        # So we can pass this variadic function as a function pointer
        return self.func

    def __call__(self, *args):
        fixed_args = []
        i = 0
        for argtype in self.argtypes:
            # Typecheck what we can
            fixed_args.append(argtype.from_param(args[i]))
            i += 1
        return self.func(*fixed_args + list(args[i:]))


def ord_if_char(value):
    """
    Simple helper used for casts to simple builtin types:  if the argument is a
    string type, it will be converted to it's ordinal value.

    This function will raise an exception if the argument is string with more
    than one characters.
    """
    return ord(value) if (isinstance(value, bytes) or isinstance(value, str)) else value

# End preamble

_libs = {}
_libdirs = []

# Begin loader

"""
Load libraries - appropriately for all our supported platforms
"""
# ----------------------------------------------------------------------------
# Copyright (c) 2008 David James
# Copyright (c) 2006-2008 Alex Holkner
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
#  * Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
#  * Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in
#    the documentation and/or other materials provided with the
#    distribution.
#  * Neither the name of pyglet nor the names of its
#    contributors may be used to endorse or promote products
#    derived from this software without specific prior written
#    permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
# FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
# COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
# ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
# ----------------------------------------------------------------------------

import ctypes
import ctypes.util
import glob
import os.path
import platform
import re
import sys


def _environ_path(name):
    """Split an environment variable into a path-like list elements"""
    if name in os.environ:
        return os.environ[name].split(":")
    return []


class LibraryLoader:
    """
    A base class For loading of libraries ;-)
    Subclasses load libraries for specific platforms.
    """

    # library names formatted specifically for platforms
    name_formats = ["%s"]

    class Lookup:
        """Looking up calling conventions for a platform"""

        mode = ctypes.DEFAULT_MODE

        def __init__(self, path):
            super(LibraryLoader.Lookup, self).__init__()
            self.access = dict(cdecl=ctypes.CDLL(path, self.mode))

        def get(self, name, calling_convention="cdecl"):
            """Return the given name according to the selected calling convention"""
            if calling_convention not in self.access:
                raise LookupError(
                    "Unknown calling convention '{}' for function '{}'".format(
                        calling_convention, name
                    )
                )
            return getattr(self.access[calling_convention], name)

        def has(self, name, calling_convention="cdecl"):
            """Return True if this given calling convention finds the given 'name'"""
            if calling_convention not in self.access:
                return False
            return hasattr(self.access[calling_convention], name)

        def __getattr__(self, name):
            return getattr(self.access["cdecl"], name)

    def __init__(self):
        self.other_dirs = []

    def __call__(self, libname):
        """Given the name of a library, load it."""
        paths = self.getpaths(libname)

        for path in paths:
            # noinspection PyBroadException
            try:
                return self.Lookup(path)
            except Exception:  # pylint: disable=broad-except
                pass

        raise ImportError("Could not load %s." % libname)

    def getpaths(self, libname):
        """Return a list of paths where the library might be found."""
        if os.path.isabs(libname):
            yield libname
        else:
            # search through a prioritized series of locations for the library

            # we first search any specific directories identified by user
            for dir_i in self.other_dirs:
                for fmt in self.name_formats:
                    # dir_i should be absolute already
                    yield os.path.join(dir_i, fmt % libname)

            # check if this code is even stored in a physical file
            try:
                this_file = __file__
            except NameError:
                this_file = None

            # then we search the directory where the generated python interface is stored
            if this_file is not None:
                for fmt in self.name_formats:
                    yield os.path.abspath(os.path.join(os.path.dirname(__file__), fmt % libname))

            # now, use the ctypes tools to try to find the library
            for fmt in self.name_formats:
                path = ctypes.util.find_library(fmt % libname)
                if path:
                    yield path

            # then we search all paths identified as platform-specific lib paths
            for path in self.getplatformpaths(libname):
                yield path

            # Finally, we'll try the users current working directory
            for fmt in self.name_formats:
                yield os.path.abspath(os.path.join(os.path.curdir, fmt % libname))

    def getplatformpaths(self, _libname):  # pylint: disable=no-self-use
        """Return all the library paths available in this platform"""
        return []


# Darwin (Mac OS X)


class DarwinLibraryLoader(LibraryLoader):
    """Library loader for MacOS"""

    name_formats = [
        "lib%s.dylib",
        "lib%s.so",
        "lib%s.bundle",
        "%s.dylib",
        "%s.so",
        "%s.bundle",
        "%s",
    ]

    class Lookup(LibraryLoader.Lookup):
        """
        Looking up library files for this platform (Darwin aka MacOS)
        """

        # Darwin requires dlopen to be called with mode RTLD_GLOBAL instead
        # of the default RTLD_LOCAL.  Without this, you end up with
        # libraries not being loadable, resulting in "Symbol not found"
        # errors
        mode = ctypes.RTLD_GLOBAL

    def getplatformpaths(self, libname):
        if os.path.pathsep in libname:
            names = [libname]
        else:
            names = [fmt % libname for fmt in self.name_formats]

        for directory in self.getdirs(libname):
            for name in names:
                yield os.path.join(directory, name)

    @staticmethod
    def getdirs(libname):
        """Implements the dylib search as specified in Apple documentation:

        http://developer.apple.com/documentation/DeveloperTools/Conceptual/
            DynamicLibraries/Articles/DynamicLibraryUsageGuidelines.html

        Before commencing the standard search, the method first checks
        the bundle's ``Frameworks`` directory if the application is running
        within a bundle (OS X .app).
        """

        dyld_fallback_library_path = _environ_path("DYLD_FALLBACK_LIBRARY_PATH")
        if not dyld_fallback_library_path:
            dyld_fallback_library_path = [
                os.path.expanduser("~/lib"),
                "/usr/local/lib",
                "/usr/lib",
            ]

        dirs = []

        if "/" in libname:
            dirs.extend(_environ_path("DYLD_LIBRARY_PATH"))
        else:
            dirs.extend(_environ_path("LD_LIBRARY_PATH"))
            dirs.extend(_environ_path("DYLD_LIBRARY_PATH"))
            dirs.extend(_environ_path("LD_RUN_PATH"))

        if hasattr(sys, "frozen") and getattr(sys, "frozen") == "macosx_app":
            dirs.append(os.path.join(os.environ["RESOURCEPATH"], "..", "Frameworks"))

        dirs.extend(dyld_fallback_library_path)

        return dirs


# Posix


class PosixLibraryLoader(LibraryLoader):
    """Library loader for POSIX-like systems (including Linux)"""

    _ld_so_cache = None

    _include = re.compile(r"^\s*include\s+(?P<pattern>.*)")

    name_formats = ["lib%s.so", "%s.so", "%s"]

    class _Directories(dict):
        """Deal with directories"""

        def __init__(self):
            dict.__init__(self)
            self.order = 0

        def add(self, directory):
            """Add a directory to our current set of directories"""
            if len(directory) > 1:
                directory = directory.rstrip(os.path.sep)
            # only adds and updates order if exists and not already in set
            if not os.path.exists(directory):
                return
            order = self.setdefault(directory, self.order)
            if order == self.order:
                self.order += 1

        def extend(self, directories):
            """Add a list of directories to our set"""
            for a_dir in directories:
                self.add(a_dir)

        def ordered(self):
            """Sort the list of directories"""
            return (i[0] for i in sorted(self.items(), key=lambda d: d[1]))

    def _get_ld_so_conf_dirs(self, conf, dirs):
        """
        Recursive function to help parse all ld.so.conf files, including proper
        handling of the `include` directive.
        """

        try:
            with open(conf) as fileobj:
                for dirname in fileobj:
                    dirname = dirname.strip()
                    if not dirname:
                        continue

                    match = self._include.match(dirname)
                    if not match:
                        dirs.add(dirname)
                    else:
                        for dir2 in glob.glob(match.group("pattern")):
                            self._get_ld_so_conf_dirs(dir2, dirs)
        except IOError:
            pass

    def _create_ld_so_cache(self):
        # Recreate search path followed by ld.so.  This is going to be
        # slow to build, and incorrect (ld.so uses ld.so.cache, which may
        # not be up-to-date).  Used only as fallback for distros without
        # /sbin/ldconfig.
        #
        # We assume the DT_RPATH and DT_RUNPATH binary sections are omitted.

        directories = self._Directories()
        for name in (
            "LD_LIBRARY_PATH",
            "SHLIB_PATH",  # HP-UX
            "LIBPATH",  # OS/2, AIX
            "LIBRARY_PATH",  # BE/OS
        ):
            if name in os.environ:
                directories.extend(os.environ[name].split(os.pathsep))

        self._get_ld_so_conf_dirs("/etc/ld.so.conf", directories)

        bitage = platform.architecture()[0]

        unix_lib_dirs_list = []
        if bitage.startswith("64"):
            # prefer 64 bit if that is our arch
            unix_lib_dirs_list += ["/lib64", "/usr/lib64"]

        # must include standard libs, since those paths are also used by 64 bit
        # installs
        unix_lib_dirs_list += ["/lib", "/usr/lib"]
        if sys.platform.startswith("linux"):
            # Try and support multiarch work in Ubuntu
            # https://wiki.ubuntu.com/MultiarchSpec
            if bitage.startswith("32"):
                # Assume Intel/AMD x86 compat
                unix_lib_dirs_list += ["/lib/i386-linux-gnu", "/usr/lib/i386-linux-gnu"]
            elif bitage.startswith("64"):
                # Assume Intel/AMD x86 compatible
                unix_lib_dirs_list += [
                    "/lib/x86_64-linux-gnu",
                    "/usr/lib/x86_64-linux-gnu",
                ]
            else:
                # guess...
                unix_lib_dirs_list += glob.glob("/lib/*linux-gnu")
        directories.extend(unix_lib_dirs_list)

        cache = {}
        lib_re = re.compile(r"lib(.*)\.s[ol]")
        # ext_re = re.compile(r"\.s[ol]$")
        for our_dir in directories.ordered():
            try:
                for path in glob.glob("%s/*.s[ol]*" % our_dir):
                    file = os.path.basename(path)

                    # Index by filename
                    cache_i = cache.setdefault(file, set())
                    cache_i.add(path)

                    # Index by library name
                    match = lib_re.match(file)
                    if match:
                        library = match.group(1)
                        cache_i = cache.setdefault(library, set())
                        cache_i.add(path)
            except OSError:
                pass

        self._ld_so_cache = cache

    def getplatformpaths(self, libname):
        if self._ld_so_cache is None:
            self._create_ld_so_cache()

        result = self._ld_so_cache.get(libname, set())
        for i in result:
            # we iterate through all found paths for library, since we may have
            # actually found multiple architectures or other library types that
            # may not load
            yield i


# Windows


class WindowsLibraryLoader(LibraryLoader):
    """Library loader for Microsoft Windows"""

    name_formats = ["%s.dll", "lib%s.dll", "%slib.dll", "%s"]

    class Lookup(LibraryLoader.Lookup):
        """Lookup class for Windows libraries..."""

        def __init__(self, path):
            super(WindowsLibraryLoader.Lookup, self).__init__(path)
            self.access["stdcall"] = ctypes.windll.LoadLibrary(path)


# Platform switching

# If your value of sys.platform does not appear in this dict, please contact
# the Ctypesgen maintainers.

loaderclass = {
    "darwin": DarwinLibraryLoader,
    "cygwin": WindowsLibraryLoader,
    "win32": WindowsLibraryLoader,
    "msys": WindowsLibraryLoader,
}

load_library = loaderclass.get(sys.platform, PosixLibraryLoader)()


def add_library_search_dirs(other_dirs):
    """
    Add libraries to search paths.
    If library paths are relative, convert them to absolute with respect to this
    file's directory
    """
    for path in other_dirs:
        if not os.path.isabs(path):
            path = os.path.abspath(path)
        load_library.other_dirs.append(path)


del loaderclass

# End loader

add_library_search_dirs([])

# No libraries

# No modules

__uint8_t = c_ubyte# /usr/include/x86_64-linux-gnu/bits/types.h: 38

__uint16_t = c_ushort# /usr/include/x86_64-linux-gnu/bits/types.h: 40

__uint32_t = c_uint# /usr/include/x86_64-linux-gnu/bits/types.h: 42

__uint64_t = c_ulong# /usr/include/x86_64-linux-gnu/bits/types.h: 45

uint8_t = __uint8_t# /usr/include/x86_64-linux-gnu/bits/stdint-uintn.h: 24

uint16_t = __uint16_t# /usr/include/x86_64-linux-gnu/bits/stdint-uintn.h: 25

uint32_t = __uint32_t# /usr/include/x86_64-linux-gnu/bits/stdint-uintn.h: 26

uint64_t = __uint64_t# /usr/include/x86_64-linux-gnu/bits/stdint-uintn.h: 27

# /home/anze/Dropbox/work/lusee/coreloop/coreloop/spectrometer_interface.h: 34
class struct_ADC_stat(Structure):
    pass

struct_ADC_stat._pack_ = 1
struct_ADC_stat.__slots__ = [
    'min',
    'max',
    'valid_count',
    'invalid_count_max',
    'invalid_count_min',
    'sumv',
    'sumv2',
]
struct_ADC_stat._fields_ = [
    ('min', c_int16),
    ('max', c_int16),
    ('valid_count', uint32_t),
    ('invalid_count_max', uint32_t),
    ('invalid_count_min', uint32_t),
    ('sumv', uint64_t),
    ('sumv2', uint64_t),
]

# /home/anze/Dropbox/work/lusee/coreloop/coreloop/core_loop.h: 133
class struct_core_state(Structure):
    pass

# /home/anze/Dropbox/work/lusee/coreloop/coreloop/core_loop.h: 32
for _lib in _libs.values():
    try:
        state = (struct_core_state).in_dll(_lib, "state")
        break
    except:
        pass

# /home/anze/Dropbox/work/lusee/coreloop/coreloop/core_loop.h: 33
for _lib in _libs.values():
    try:
        avg_counter = (uint16_t).in_dll(_lib, "avg_counter")
        break
    except:
        pass

# /home/anze/Dropbox/work/lusee/coreloop/coreloop/core_loop.h: 34
for _lib in _libs.values():
    try:
        unique_packet_id = (uint32_t).in_dll(_lib, "unique_packet_id")
        break
    except:
        pass

# /home/anze/Dropbox/work/lusee/coreloop/coreloop/core_loop.h: 35
for _lib in _libs.values():
    try:
        leading_zeros_min = (uint8_t * int(16)).in_dll(_lib, "leading_zeros_min")
        break
    except:
        pass

# /home/anze/Dropbox/work/lusee/coreloop/coreloop/core_loop.h: 36
for _lib in _libs.values():
    try:
        leading_zeros_max = (uint8_t * int(16)).in_dll(_lib, "leading_zeros_max")
        break
    except:
        pass

# /home/anze/Dropbox/work/lusee/coreloop/coreloop/core_loop.h: 37
for _lib in _libs.values():
    try:
        housekeeping_request = (uint8_t).in_dll(_lib, "housekeeping_request")
        break
    except:
        pass

# /home/anze/Dropbox/work/lusee/coreloop/coreloop/core_loop.h: 38
for _lib in _libs.values():
    try:
        section_break = (uint32_t).in_dll(_lib, "section_break")
        break
    except:
        pass

# /home/anze/Dropbox/work/lusee/coreloop/coreloop/core_loop.h: 39
for _lib in _libs.values():
    try:
        range_adc = (uint8_t).in_dll(_lib, "range_adc")
        break
    except:
        pass

# /home/anze/Dropbox/work/lusee/coreloop/coreloop/core_loop.h: 39
for _lib in _libs.values():
    try:
        resettle = (uint8_t).in_dll(_lib, "resettle")
        break
    except:
        pass

# /home/anze/Dropbox/work/lusee/coreloop/coreloop/core_loop.h: 39
for _lib in _libs.values():
    try:
        request_waveform = (uint8_t).in_dll(_lib, "request_waveform")
        break
    except:
        pass

# /home/anze/Dropbox/work/lusee/coreloop/coreloop/core_loop.h: 40
for _lib in _libs.values():
    try:
        tick_tock = (c_bool).in_dll(_lib, "tick_tock")
        break
    except:
        pass

# /home/anze/Dropbox/work/lusee/coreloop/coreloop/core_loop.h: 41
for _lib in _libs.values():
    try:
        drop_df = (c_bool).in_dll(_lib, "drop_df")
        break
    except:
        pass

# /home/anze/Dropbox/work/lusee/coreloop/coreloop/core_loop.h: 42
for _lib in _libs.values():
    try:
        soft_reset_flag = (c_bool).in_dll(_lib, "soft_reset_flag")
        break
    except:
        pass

# /home/anze/Dropbox/work/lusee/coreloop/coreloop/core_loop.h: 43
for _lib in _libs.values():
    try:
        heartbeat_packet_count = (uint32_t).in_dll(_lib, "heartbeat_packet_count")
        break
    except:
        pass

# /home/anze/Dropbox/work/lusee/coreloop/coreloop/core_loop.h: 44
for _lib in _libs.values():
    try:
        heartbeat_counter = (uint32_t).in_dll(_lib, "heartbeat_counter")
        break
    except:
        pass

# /home/anze/Dropbox/work/lusee/coreloop/coreloop/core_loop.h: 45
for _lib in _libs.values():
    try:
        resettle_counter = (uint32_t).in_dll(_lib, "resettle_counter")
        break
    except:
        pass

# /home/anze/Dropbox/work/lusee/coreloop/coreloop/core_loop.h: 46
for _lib in _libs.values():
    try:
        flash_store_pointer = (uint16_t).in_dll(_lib, "flash_store_pointer")
        break
    except:
        pass

enum_gain_state = c_int# /home/anze/Dropbox/work/lusee/coreloop/coreloop/core_loop.h: 51

GAIN_LOW = 0# /home/anze/Dropbox/work/lusee/coreloop/coreloop/core_loop.h: 51

GAIN_MED = (GAIN_LOW + 1)# /home/anze/Dropbox/work/lusee/coreloop/coreloop/core_loop.h: 51

GAIN_HIGH = (GAIN_MED + 1)# /home/anze/Dropbox/work/lusee/coreloop/coreloop/core_loop.h: 51

GAIN_DISABLE = (GAIN_HIGH + 1)# /home/anze/Dropbox/work/lusee/coreloop/coreloop/core_loop.h: 51

GAIN_AUTO = (GAIN_DISABLE + 1)# /home/anze/Dropbox/work/lusee/coreloop/coreloop/core_loop.h: 51

enum_output_format = c_int# /home/anze/Dropbox/work/lusee/coreloop/coreloop/core_loop.h: 58

OUTPUT_32BIT = 0# /home/anze/Dropbox/work/lusee/coreloop/coreloop/core_loop.h: 58

OUTPUT_16BIT_UPDATES = (OUTPUT_32BIT + 1)# /home/anze/Dropbox/work/lusee/coreloop/coreloop/core_loop.h: 58

OUTPUT_16BIT_FLOAT1 = (OUTPUT_16BIT_UPDATES + 1)# /home/anze/Dropbox/work/lusee/coreloop/coreloop/core_loop.h: 58

# /home/anze/Dropbox/work/lusee/coreloop/coreloop/core_loop.h: 68
class struct_route_state(Structure):
    pass

struct_route_state._pack_ = 1
struct_route_state.__slots__ = [
    'plus',
    'minus',
]
struct_route_state._fields_ = [
    ('plus', uint8_t),
    ('minus', uint8_t),
]

# /home/anze/Dropbox/work/lusee/coreloop/coreloop/core_loop.h: 74
class struct_sequencer_state(Structure):
    pass

struct_sequencer_state._pack_ = 1
struct_sequencer_state.__slots__ = [
    'gain',
    'gain_auto_min',
    'gain_auto_mult',
    'route',
    'Navg1_shift',
    'Navg2_shift',
    'notch',
    'Navgf',
    'hi_frac',
    'med_frac',
    'bitslice',
    'bitslice_keep_bits',
    'format',
    'reject_ratio',
    'reject_maxbad',
    'tr_start',
    'tr_stop',
    'tr_avg_shift',
]
struct_sequencer_state._fields_ = [
    ('gain', uint8_t * int(4)),
    ('gain_auto_min', uint16_t * int(4)),
    ('gain_auto_mult', uint16_t * int(4)),
    ('route', struct_route_state * int(4)),
    ('Navg1_shift', uint8_t),
    ('Navg2_shift', uint8_t),
    ('notch', uint8_t),
    ('Navgf', uint8_t),
    ('hi_frac', uint8_t),
    ('med_frac', uint8_t),
    ('bitslice', uint8_t * int(16)),
    ('bitslice_keep_bits', uint8_t),
    ('format', uint8_t),
    ('reject_ratio', uint8_t),
    ('reject_maxbad', uint8_t),
    ('tr_start', uint16_t),
    ('tr_stop', uint16_t),
    ('tr_avg_shift', uint16_t),
]

# /home/anze/Dropbox/work/lusee/coreloop/coreloop/core_loop.h: 92
class struct_sequencer_program(Structure):
    pass

struct_sequencer_program._pack_ = 1
struct_sequencer_program.__slots__ = [
    'Nseq',
    'seq',
    'seq_times',
    'sequencer_repeat',
]
struct_sequencer_program._fields_ = [
    ('Nseq', uint8_t),
    ('seq', struct_sequencer_state * int(32)),
    ('seq_times', uint16_t * int(32)),
    ('sequencer_repeat', uint16_t),
]

# /home/anze/Dropbox/work/lusee/coreloop/coreloop/core_loop.h: 101
class struct_core_state_base(Structure):
    pass

struct_core_state_base._pack_ = 1
struct_core_state_base.__slots__ = [
    'time_seconds',
    'time_subseconds',
    'TVS_sensors',
    'errors',
    'corr_products_mask',
    'actual_gain',
    'actual_bitslice',
    'spec_overflow',
    'notch_overflow',
    'ADC_stat',
    'spectrometer_enable',
    'sequencer_counter',
    'sequencer_step',
    'sequencer_substep',
    'rand_state',
    'weight_previous',
    'weight_current',
]
struct_core_state_base._fields_ = [
    ('time_seconds', uint32_t),
    ('time_subseconds', uint16_t),
    ('TVS_sensors', uint16_t * int(4)),
    ('errors', uint32_t),
    ('corr_products_mask', uint16_t),
    ('actual_gain', uint8_t * int(4)),
    ('actual_bitslice', uint8_t * int(16)),
    ('spec_overflow', uint16_t),
    ('notch_overflow', uint16_t),
    ('ADC_stat', struct_ADC_stat * int(4)),
    ('spectrometer_enable', c_bool),
    ('sequencer_counter', uint8_t),
    ('sequencer_step', uint8_t),
    ('sequencer_substep', uint8_t),
    ('rand_state', uint32_t),
    ('weight_previous', uint8_t),
    ('weight_current', uint8_t),
]

# /home/anze/Dropbox/work/lusee/coreloop/coreloop/core_loop.h: 122
class struct_delayed_cdi_sending(Structure):
    pass

struct_delayed_cdi_sending._pack_ = 1
struct_delayed_cdi_sending.__slots__ = [
    'appId',
    'int_counter',
    'format',
    'prod_count',
    'Nfreq',
    'Navgf',
    'packet_id',
]
struct_delayed_cdi_sending._fields_ = [
    ('appId', uint32_t),
    ('int_counter', uint16_t),
    ('format', uint8_t),
    ('prod_count', uint8_t),
    ('Nfreq', uint16_t),
    ('Navgf', uint16_t),
    ('packet_id', uint32_t),
]

struct_core_state._pack_ = 1
struct_core_state.__slots__ = [
    'seq',
    'base',
    'cdi_dispatch',
    'Navg1',
    'Navg2',
    'tr_avg',
    'Navg2_total_shift',
    'Nfreq',
    'gain_auto_max',
    'sequencer_enabled',
    'program',
]
struct_core_state._fields_ = [
    ('seq', struct_sequencer_state),
    ('base', struct_core_state_base),
    ('cdi_dispatch', struct_delayed_cdi_sending),
    ('Navg1', uint16_t),
    ('Navg2', uint16_t),
    ('tr_avg', uint16_t),
    ('Navg2_total_shift', uint8_t),
    ('Nfreq', uint16_t),
    ('gain_auto_max', uint16_t * int(4)),
    ('sequencer_enabled', c_bool),
    ('program', struct_sequencer_program),
]

# /home/anze/Dropbox/work/lusee/coreloop/coreloop/core_loop.h: 146
class struct_saved_core_state(Structure):
    pass

struct_saved_core_state._pack_ = 1
struct_saved_core_state.__slots__ = [
    'in_use',
    'state',
    'CRC',
]
struct_saved_core_state._fields_ = [
    ('in_use', uint32_t),
    ('state', struct_core_state),
    ('CRC', uint32_t),
]

# /home/anze/Dropbox/work/lusee/coreloop/coreloop/core_loop.h: 152
class struct_startup_hello(Structure):
    pass

struct_startup_hello._pack_ = 1
struct_startup_hello.__slots__ = [
    'SW_version',
    'FW_Version',
    'FW_ID',
    'FW_Date',
    'FW_Time',
    'unique_packet_id',
    'time_seconds',
    'time_subseconds',
]
struct_startup_hello._fields_ = [
    ('SW_version', uint32_t),
    ('FW_Version', uint32_t),
    ('FW_ID', uint32_t),
    ('FW_Date', uint32_t),
    ('FW_Time', uint32_t),
    ('unique_packet_id', uint32_t),
    ('time_seconds', uint32_t),
    ('time_subseconds', uint16_t),
]

# /home/anze/Dropbox/work/lusee/coreloop/coreloop/core_loop.h: 164
class struct_meta_data(Structure):
    pass

struct_meta_data._pack_ = 1
struct_meta_data.__slots__ = [
    'version',
    'unique_packet_id',
    'seq',
    'base',
]
struct_meta_data._fields_ = [
    ('version', uint16_t),
    ('unique_packet_id', uint32_t),
    ('seq', struct_sequencer_state),
    ('base', struct_core_state_base),
]

# /home/anze/Dropbox/work/lusee/coreloop/coreloop/core_loop.h: 171
class struct_housekeeping_data_base(Structure):
    pass

struct_housekeeping_data_base._pack_ = 1
struct_housekeeping_data_base.__slots__ = [
    'version',
    'unique_packet_id',
    'errors',
    'housekeeping_type',
]
struct_housekeeping_data_base._fields_ = [
    ('version', uint16_t),
    ('unique_packet_id', uint32_t),
    ('errors', uint32_t),
    ('housekeeping_type', uint16_t),
]

# /home/anze/Dropbox/work/lusee/coreloop/coreloop/core_loop.h: 178
class struct_housekeeping_data_0(Structure):
    pass

struct_housekeeping_data_0._pack_ = 1
struct_housekeeping_data_0.__slots__ = [
    'base',
    'core_state',
]
struct_housekeeping_data_0._fields_ = [
    ('base', struct_housekeeping_data_base),
    ('core_state', struct_core_state),
]

# /home/anze/Dropbox/work/lusee/coreloop/coreloop/core_loop.h: 183
class struct_housekeeping_data_1(Structure):
    pass

struct_housekeeping_data_1._pack_ = 1
struct_housekeeping_data_1.__slots__ = [
    'base',
    'ADC_stat',
    'actual_gain',
]
struct_housekeeping_data_1._fields_ = [
    ('base', struct_housekeeping_data_base),
    ('ADC_stat', struct_ADC_stat * int(4)),
    ('actual_gain', uint8_t * int(4)),
]

# /home/anze/Dropbox/work/lusee/coreloop/coreloop/core_loop.h: 189
class struct_housekeeping_data_99(Structure):
    pass

struct_housekeeping_data_99._pack_ = 1
struct_housekeeping_data_99.__slots__ = [
    'section_break',
]
struct_housekeeping_data_99._fields_ = [
    ('section_break', uint32_t),
]

# /home/anze/Dropbox/work/lusee/coreloop/coreloop/core_loop.h: 195
for _lib in _libs.values():
    try:
        state = (struct_core_state).in_dll(_lib, "state")
        break
    except:
        pass

# /home/anze/Dropbox/work/lusee/coreloop/coreloop/core_loop.h: 196
for _lib in _libs.values():
    try:
        soft_reset_flag = (c_bool).in_dll(_lib, "soft_reset_flag")
        break
    except:
        pass

# /home/anze/Dropbox/work/lusee/coreloop/coreloop/core_loop.h: 199
for _lib in _libs.values():
    if not _lib.has("core_loop", "cdecl"):
        continue
    core_loop = _lib.get("core_loop", "cdecl")
    core_loop.argtypes = []
    core_loop.restype = None
    break

# /home/anze/Dropbox/work/lusee/coreloop/coreloop/core_loop.h: 202
for _lib in _libs.values():
    if not _lib.has("process_cdi", "cdecl"):
        continue
    process_cdi = _lib.get("process_cdi", "cdecl")
    process_cdi.argtypes = []
    process_cdi.restype = c_bool
    break

# /home/anze/Dropbox/work/lusee/coreloop/coreloop/core_loop.h: 208
for _lib in _libs.values():
    if not _lib.has("RFS_stop", "cdecl"):
        continue
    RFS_stop = _lib.get("RFS_stop", "cdecl")
    RFS_stop.argtypes = []
    RFS_stop.restype = None
    break

# /home/anze/Dropbox/work/lusee/coreloop/coreloop/core_loop.h: 209
for _lib in _libs.values():
    if not _lib.has("RFS_start", "cdecl"):
        continue
    RFS_start = _lib.get("RFS_start", "cdecl")
    RFS_start.argtypes = []
    RFS_start.restype = None
    break

# /home/anze/Dropbox/work/lusee/coreloop/coreloop/core_loop.h: 210
for _lib in _libs.values():
    if not _lib.has("restart_spectrometer", "cdecl"):
        continue
    restart_spectrometer = _lib.get("restart_spectrometer", "cdecl")
    restart_spectrometer.argtypes = []
    restart_spectrometer.restype = None
    break

# /home/anze/Dropbox/work/lusee/coreloop/coreloop/core_loop.h: 213
for _lib in _libs.values():
    if not _lib.has("fill_derived", "cdecl"):
        continue
    fill_derived = _lib.get("fill_derived", "cdecl")
    fill_derived.argtypes = []
    fill_derived.restype = None
    break

# /home/anze/Dropbox/work/lusee/coreloop/coreloop/core_loop.h: 218
for _lib in _libs.values():
    if not _lib.has("set_route", "cdecl"):
        continue
    set_route = _lib.get("set_route", "cdecl")
    set_route.argtypes = [uint8_t, uint8_t]
    set_route.restype = None
    break

# /home/anze/Dropbox/work/lusee/coreloop/coreloop/core_loop.h: 221
for _lib in _libs.values():
    if not _lib.has("update_spec_gains", "cdecl"):
        continue
    update_spec_gains = _lib.get("update_spec_gains", "cdecl")
    update_spec_gains.argtypes = []
    update_spec_gains.restype = None
    break

# /home/anze/Dropbox/work/lusee/coreloop/coreloop/core_loop.h: 224
for _lib in _libs.values():
    if not _lib.has("trigger_ADC_stat", "cdecl"):
        continue
    trigger_ADC_stat = _lib.get("trigger_ADC_stat", "cdecl")
    trigger_ADC_stat.argtypes = []
    trigger_ADC_stat.restype = None
    break

# /home/anze/Dropbox/work/lusee/coreloop/coreloop/core_loop.h: 227
for _lib in _libs.values():
    if not _lib.has("reset_errormasks", "cdecl"):
        continue
    reset_errormasks = _lib.get("reset_errormasks", "cdecl")
    reset_errormasks.argtypes = []
    reset_errormasks.restype = None
    break

# /home/anze/Dropbox/work/lusee/coreloop/coreloop/core_loop.h: 230
for _lib in _libs.values():
    if not _lib.has("update_time", "cdecl"):
        continue
    update_time = _lib.get("update_time", "cdecl")
    update_time.argtypes = []
    update_time.restype = None
    break

# /home/anze/Dropbox/work/lusee/coreloop/coreloop/core_loop.h: 233
for _lib in _libs.values():
    if not _lib.has("process_spectrometer", "cdecl"):
        continue
    process_spectrometer = _lib.get("process_spectrometer", "cdecl")
    process_spectrometer.argtypes = []
    process_spectrometer.restype = None
    break

# /home/anze/Dropbox/work/lusee/coreloop/coreloop/core_loop.h: 236
for _lib in _libs.values():
    if not _lib.has("transfer_to_cdi", "cdecl"):
        continue
    transfer_to_cdi = _lib.get("transfer_to_cdi", "cdecl")
    transfer_to_cdi.argtypes = []
    transfer_to_cdi.restype = None
    break

# /home/anze/Dropbox/work/lusee/coreloop/coreloop/core_loop.h: 238
for _lib in _libs.values():
    if not _lib.has("process_delayed_cdi_dispatch", "cdecl"):
        continue
    process_delayed_cdi_dispatch = _lib.get("process_delayed_cdi_dispatch", "cdecl")
    process_delayed_cdi_dispatch.argtypes = []
    process_delayed_cdi_dispatch.restype = c_bool
    break

# /home/anze/Dropbox/work/lusee/coreloop/coreloop/core_loop.h: 241
for _lib in _libs.values():
    if not _lib.has("process_gain_range", "cdecl"):
        continue
    process_gain_range = _lib.get("process_gain_range", "cdecl")
    process_gain_range.argtypes = []
    process_gain_range.restype = None
    break

# /home/anze/Dropbox/work/lusee/coreloop/coreloop/core_loop.h: 242
for _lib in _libs.values():
    if not _lib.has("bitslice_control", "cdecl"):
        continue
    bitslice_control = _lib.get("bitslice_control", "cdecl")
    bitslice_control.argtypes = []
    bitslice_control.restype = c_bool
    break

# /home/anze/Dropbox/work/lusee/coreloop/coreloop/core_loop.h: 245
for _lib in _libs.values():
    if not _lib.has("set_spectrometer_to_sequencer", "cdecl"):
        continue
    set_spectrometer_to_sequencer = _lib.get("set_spectrometer_to_sequencer", "cdecl")
    set_spectrometer_to_sequencer.argtypes = []
    set_spectrometer_to_sequencer.restype = None
    break

# /home/anze/Dropbox/work/lusee/coreloop/coreloop/core_loop.h: 246
for _lib in _libs.values():
    if not _lib.has("default_seq", "cdecl"):
        continue
    default_seq = _lib.get("default_seq", "cdecl")
    default_seq.argtypes = [POINTER(struct_sequencer_state)]
    default_seq.restype = None
    break

# /home/anze/Dropbox/work/lusee/coreloop/coreloop/core_loop.h: 247
for _lib in _libs.values():
    if not _lib.has("advance_sequencer", "cdecl"):
        continue
    advance_sequencer = _lib.get("advance_sequencer", "cdecl")
    advance_sequencer.argtypes = []
    advance_sequencer.restype = None
    break

# /home/anze/Dropbox/work/lusee/coreloop/coreloop/core_loop.h: 250
for _lib in _libs.values():
    if not _lib.has("debug_helper", "cdecl"):
        continue
    debug_helper = _lib.get("debug_helper", "cdecl")
    debug_helper.argtypes = [uint8_t]
    debug_helper.restype = None
    break

# /home/anze/Dropbox/work/lusee/coreloop/coreloop/core_loop.h: 251
for _lib in _libs.values():
    if not _lib.has("cdi_not_implemented", "cdecl"):
        continue
    cdi_not_implemented = _lib.get("cdi_not_implemented", "cdecl")
    cdi_not_implemented.argtypes = [String]
    cdi_not_implemented.restype = None
    break

# /home/anze/Dropbox/work/lusee/coreloop/coreloop/core_loop.h: 255
for _lib in _libs.values():
    if not _lib.has("send_hello_packet", "cdecl"):
        continue
    send_hello_packet = _lib.get("send_hello_packet", "cdecl")
    send_hello_packet.argtypes = []
    send_hello_packet.restype = None
    break

# /home/anze/Dropbox/work/lusee/coreloop/coreloop/core_loop.h: 256
for _lib in _libs.values():
    if not _lib.has("process_hearbeat", "cdecl"):
        continue
    process_hearbeat = _lib.get("process_hearbeat", "cdecl")
    process_hearbeat.argtypes = []
    process_hearbeat.restype = c_bool
    break

# /home/anze/Dropbox/work/lusee/coreloop/coreloop/core_loop.h: 257
for _lib in _libs.values():
    if not _lib.has("process_housekeeping", "cdecl"):
        continue
    process_housekeeping = _lib.get("process_housekeeping", "cdecl")
    process_housekeeping.argtypes = []
    process_housekeeping.restype = c_bool
    break

# /home/anze/Dropbox/work/lusee/coreloop/coreloop/core_loop.h: 266
for _lib in _libs.values():
    if not _lib.has("encode_12plus4", "cdecl"):
        continue
    encode_12plus4 = _lib.get("encode_12plus4", "cdecl")
    encode_12plus4.argtypes = [c_int32]
    encode_12plus4.restype = c_int16
    break

# /home/anze/Dropbox/work/lusee/coreloop/coreloop/core_loop.h: 267
for _lib in _libs.values():
    if not _lib.has("decode_12plus4", "cdecl"):
        continue
    decode_12plus4 = _lib.get("decode_12plus4", "cdecl")
    decode_12plus4.argtypes = [c_int16]
    decode_12plus4.restype = c_int32
    break

# /home/anze/Dropbox/work/lusee/coreloop/coreloop/core_loop.h: 269
for _lib in _libs.values():
    if not _lib.has("CRC", "cdecl"):
        continue
    CRC = _lib.get("CRC", "cdecl")
    CRC.argtypes = [POINTER(None), c_size_t]
    CRC.restype = uint32_t
    break

# /home/anze/Dropbox/work/lusee/coreloop/coreloop/core_loop.h: 9
try:
    VERSION_ID = 0x00000100
except:
    pass

# /home/anze/Dropbox/work/lusee/coreloop/coreloop/core_loop.h: 20
try:
    NSEQ_MAX = 32
except:
    pass

# /home/anze/Dropbox/work/lusee/coreloop/coreloop/core_loop.h: 21
try:
    DISPATCH_DELAY = 6
except:
    pass

# /home/anze/Dropbox/work/lusee/coreloop/coreloop/core_loop.h: 22
try:
    RESETTLE_DELAY = 2
except:
    pass

# /home/anze/Dropbox/work/lusee/coreloop/coreloop/core_loop.h: 23
try:
    HEARTBEAT_DELAY = 1024
except:
    pass

# /home/anze/Dropbox/work/lusee/coreloop/coreloop/core_loop.h: 25
try:
    ADC_STAT_SAMPLES = 8000
except:
    pass

# /home/anze/Dropbox/work/lusee/coreloop/coreloop/core_loop.h: 27
try:
    MAX_STATE_SLOTS = 64
except:
    pass

# /home/anze/Dropbox/work/lusee/coreloop/coreloop/core_loop.h: 29
try:
    PAGES_PER_SLOT = 256
except:
    pass

# /home/anze/Dropbox/work/lusee/coreloop/coreloop/core_loop.h: 262
def MAX(x, y):
    return (x > y) and x or y

# /home/anze/Dropbox/work/lusee/coreloop/coreloop/core_loop.h: 263
def MIN(x, y):
    return (x < y) and x or y

core_state = struct_core_state# /home/anze/Dropbox/work/lusee/coreloop/coreloop/core_loop.h: 133

route_state = struct_route_state# /home/anze/Dropbox/work/lusee/coreloop/coreloop/core_loop.h: 68

sequencer_state = struct_sequencer_state# /home/anze/Dropbox/work/lusee/coreloop/coreloop/core_loop.h: 74

sequencer_program = struct_sequencer_program# /home/anze/Dropbox/work/lusee/coreloop/coreloop/core_loop.h: 92

core_state_base = struct_core_state_base# /home/anze/Dropbox/work/lusee/coreloop/coreloop/core_loop.h: 101

delayed_cdi_sending = struct_delayed_cdi_sending# /home/anze/Dropbox/work/lusee/coreloop/coreloop/core_loop.h: 122

saved_core_state = struct_saved_core_state# /home/anze/Dropbox/work/lusee/coreloop/coreloop/core_loop.h: 146

startup_hello = struct_startup_hello# /home/anze/Dropbox/work/lusee/coreloop/coreloop/core_loop.h: 152

meta_data = struct_meta_data# /home/anze/Dropbox/work/lusee/coreloop/coreloop/core_loop.h: 164

housekeeping_data_base = struct_housekeeping_data_base# /home/anze/Dropbox/work/lusee/coreloop/coreloop/core_loop.h: 171

housekeeping_data_0 = struct_housekeeping_data_0# /home/anze/Dropbox/work/lusee/coreloop/coreloop/core_loop.h: 178

housekeeping_data_1 = struct_housekeeping_data_1# /home/anze/Dropbox/work/lusee/coreloop/coreloop/core_loop.h: 183

housekeeping_data_99 = struct_housekeeping_data_99# /home/anze/Dropbox/work/lusee/coreloop/coreloop/core_loop.h: 189

# No inserted files

# No prefix-stripping

