r"""Wrapper for core_loop.h

Generated with:
/home/anigmetov/code/uncrater/venv/bin/ctypesgen ../coreloop/core_loop.h ../coreloop/calibrator.h

Do not modify this file.
"""

__docformat__ = "restructuredtext"

# Begin preamble for Python v(3, 2)

import ctypes, os, sys
from ctypes import *

_int_types = (c_int16, c_int32)
if hasattr(ctypes, "c_int64"):
    # Some builds of ctypes apparently do not have c_int64
    # defined; it's a pretty good bet that these builds do not
    # have 64-bit pointers.
    _int_types += (c_int64,)
for t in _int_types:
    if sizeof(t) == sizeof(c_size_t):
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

    def __cmp__(self, string):
        if isinstance(string, UserString):
            return cmp(self.data, string.data)
        else:
            return cmp(self.data, string)

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


class String(MutableString, Union):

    _fields_ = [("raw", POINTER(c_char)), ("data", c_char_p)]

    def __init__(self, obj=""):
        if isinstance(obj, (bytes, UserString)):
            self.data = bytes(obj)
        else:
            self.raw = obj

    def __len__(self):
        return self.data and len(self.data) or 0

    def from_param(cls, obj):
        # Convert None or 0
        if obj is None or obj == 0:
            return cls(POINTER(c_char)())

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
        elif isinstance(obj, c_char_p):
            return obj

        # Convert from POINTER(c_char)
        elif isinstance(obj, POINTER(c_char)):
            return obj

        # Convert from raw pointer
        elif isinstance(obj, int):
            return cls(cast(obj, POINTER(c_char)))

        # Convert from c_char array
        elif isinstance(obj, c_char * len(obj)):
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
# typechecked, and will be converted to c_void_p.
def UNCHECKED(type):
    if hasattr(type, "_type_") and isinstance(type._type_, str) and type._type_ != "P":
        return type
    else:
        return c_void_p


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

import os.path, re, sys, glob
import platform
import ctypes
import ctypes.util


def _environ_path(name):
    if name in os.environ:
        return os.environ[name].split(":")
    else:
        return []


class LibraryLoader(object):
    # library names formatted specifically for platforms
    name_formats = ["%s"]

    class Lookup(object):
        mode = ctypes.DEFAULT_MODE

        def __init__(self, path):
            super(LibraryLoader.Lookup, self).__init__()
            self.access = dict(cdecl=ctypes.CDLL(path, self.mode))

        def get(self, name, calling_convention="cdecl"):
            if calling_convention not in self.access:
                raise LookupError(
                    "Unknown calling convention '{}' for function '{}'".format(
                        calling_convention, name
                    )
                )
            return getattr(self.access[calling_convention], name)

        def has(self, name, calling_convention="cdecl"):
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
            try:
                return self.Lookup(path)
            except:
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

            # then we search the directory where the generated python interface is stored
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

    def getplatformpaths(self, libname):
        return []


# Darwin (Mac OS X)


class DarwinLibraryLoader(LibraryLoader):
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
        # Darwin requires dlopen to be called with mode RTLD_GLOBAL instead
        # of the default RTLD_LOCAL.  Without this, you end up with
        # libraries not being loadable, resulting in "Symbol not found"
        # errors
        mode = ctypes.RTLD_GLOBAL

    def getplatformpaths(self, libname):
        if os.path.pathsep in libname:
            names = [libname]
        else:
            names = [format % libname for format in self.name_formats]

        for dir in self.getdirs(libname):
            for name in names:
                yield os.path.join(dir, name)

    def getdirs(self, libname):
        """Implements the dylib search as specified in Apple documentation:

        http://developer.apple.com/documentation/DeveloperTools/Conceptual/
            DynamicLibraries/Articles/DynamicLibraryUsageGuidelines.html

        Before commencing the standard search, the method first checks
        the bundle's ``Frameworks`` directory if the application is running
        within a bundle (OS X .app).
        """

        dyld_fallback_library_path = _environ_path("DYLD_FALLBACK_LIBRARY_PATH")
        if not dyld_fallback_library_path:
            dyld_fallback_library_path = [os.path.expanduser("~/lib"), "/usr/local/lib", "/usr/lib"]

        dirs = []

        if "/" in libname:
            dirs.extend(_environ_path("DYLD_LIBRARY_PATH"))
        else:
            dirs.extend(_environ_path("LD_LIBRARY_PATH"))
            dirs.extend(_environ_path("DYLD_LIBRARY_PATH"))

        if hasattr(sys, "frozen") and sys.frozen == "macosx_app":
            dirs.append(os.path.join(os.environ["RESOURCEPATH"], "..", "Frameworks"))

        dirs.extend(dyld_fallback_library_path)

        return dirs


# Posix


class PosixLibraryLoader(LibraryLoader):
    _ld_so_cache = None

    _include = re.compile(r"^\s*include\s+(?P<pattern>.*)")

    class _Directories(dict):
        def __init__(self):
            self.order = 0

        def add(self, directory):
            if len(directory) > 1:
                directory = directory.rstrip(os.path.sep)
            # only adds and updates order if exists and not already in set
            if not os.path.exists(directory):
                return
            o = self.setdefault(directory, self.order)
            if o == self.order:
                self.order += 1

        def extend(self, directories):
            for d in directories:
                self.add(d)

        def ordered(self):
            return (i[0] for i in sorted(self.items(), key=lambda D: D[1]))

    def _get_ld_so_conf_dirs(self, conf, dirs):
        """
        Recursive funtion to help parse all ld.so.conf files, including proper
        handling of the `include` directive.
        """

        try:
            with open(conf) as f:
                for D in f:
                    D = D.strip()
                    if not D:
                        continue

                    m = self._include.match(D)
                    if not m:
                        dirs.add(D)
                    else:
                        for D2 in glob.glob(m.group("pattern")):
                            self._get_ld_so_conf_dirs(D2, dirs)
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
            "SHLIB_PATH",  # HPUX
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
                # Assume Intel/AMD x86 compat
                unix_lib_dirs_list += ["/lib/x86_64-linux-gnu", "/usr/lib/x86_64-linux-gnu"]
            else:
                # guess...
                unix_lib_dirs_list += glob.glob("/lib/*linux-gnu")
        directories.extend(unix_lib_dirs_list)

        cache = {}
        lib_re = re.compile(r"lib(.*)\.s[ol]")
        ext_re = re.compile(r"\.s[ol]$")
        for dir in directories.ordered():
            try:
                for path in glob.glob("%s/*.s[ol]*" % dir):
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
    name_formats = ["%s.dll", "lib%s.dll", "%slib.dll", "%s"]

    class Lookup(LibraryLoader.Lookup):
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
    for F in other_dirs:
        if not os.path.isabs(F):
            F = os.path.abspath(F)
        load_library.other_dirs.append(F)


del loaderclass

# End loader

add_library_search_dirs([])

# No libraries

# No modules

# /home/anigmetov/code/coreloop/coreloop/spectrometer_interface.h: 47
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
    ('valid_count', c_uint32),
    ('invalid_count_max', c_uint32),
    ('invalid_count_min', c_uint32),
    ('sumv', c_uint64),
    ('sumv2', c_uint64),
]

# /home/anigmetov/code/coreloop/coreloop/calibrator.h: 36
class struct_calibrator_state(Structure):
    pass

struct_calibrator_state._pack_ = 1
struct_calibrator_state.__slots__ = [
    'mode',
    'Navg2',
    'Navg3',
    'drift_guard',
    'drift_step',
    'antenna_mask',
    'notch_index',
    'SNRon',
    'SNRoff',
    'Nsettle',
    'delta_drift_corA',
    'delta_drift_corB',
    'ddrift_guard',
    'gphase_guard',
    'pfb_index',
    'weight_ndx',
    'auto_slice',
    'powertop_slice',
    'delta_powerbot_slice',
    'sum1_slice',
    'sum2_slice',
    'sd2_slice',
    'prod1_slice',
    'prod2_slice',
    'errors',
    'zoom_ch1',
    'zoom_ch2',
    'zoom_Nfft',
    'zoom_prod',
    'zoom_Navg',
]
struct_calibrator_state._fields_ = [
    ('mode', c_uint8),
    ('Navg2', c_uint8),
    ('Navg3', c_uint8),
    ('drift_guard', c_uint8),
    ('drift_step', c_uint8),
    ('antenna_mask', c_uint8),
    ('notch_index', c_uint8),
    ('SNRon', c_uint32),
    ('SNRoff', c_uint32),
    ('Nsettle', c_uint32),
    ('delta_drift_corA', c_uint32),
    ('delta_drift_corB', c_uint32),
    ('ddrift_guard', c_uint32),
    ('gphase_guard', c_uint32),
    ('pfb_index', c_uint16),
    ('weight_ndx', c_uint16),
    ('auto_slice', c_bool),
    ('powertop_slice', c_uint8),
    ('delta_powerbot_slice', c_uint8),
    ('sum1_slice', c_uint8),
    ('sum2_slice', c_uint8),
    ('sd2_slice', c_uint8),
    ('prod1_slice', c_uint8),
    ('prod2_slice', c_uint8),
    ('errors', c_uint32),
    ('zoom_ch1', c_uint8),
    ('zoom_ch2', c_uint8),
    ('zoom_Nfft', c_uint8),
    ('zoom_prod', c_uint8),
    ('zoom_Navg', c_uint8),
]

# /home/anigmetov/code/coreloop/coreloop/calibrator.h: 62
class struct_calibrator_metadata(Structure):
    pass

struct_calibrator_metadata._pack_ = 1
struct_calibrator_metadata.__slots__ = [
    'version',
    'unique_packet_id',
    'time_32',
    'time_16',
    'have_lock',
    'state',
    'SNR_max',
    'SNR_min',
    'drift',
    'error_regs',
]
struct_calibrator_metadata._fields_ = [
    ('version', c_uint16),
    ('unique_packet_id', c_uint32),
    ('time_32', c_uint32),
    ('time_16', c_uint16),
    ('have_lock', c_uint16 * int(4)),
    ('state', struct_calibrator_state),
    ('SNR_max', c_int * int(4)),
    ('SNR_min', c_int * int(4)),
    ('drift', c_int32 * int(1024)),
    ('error_regs', c_uint32 * int(30)),
]

# /home/anigmetov/code/coreloop/coreloop/calibrator.h: 75
class struct_saved_calibrator_weights(Structure):
    pass

struct_saved_calibrator_weights._pack_ = 1
struct_saved_calibrator_weights.__slots__ = [
    'in_use',
    'CRC',
    'weights',
]
struct_saved_calibrator_weights._fields_ = [
    ('in_use', c_uint32),
    ('CRC', c_uint32),
    ('weights', c_uint16 * int(512)),
]

# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 159
class struct_core_state(Structure):
    pass

# /home/anigmetov/code/coreloop/coreloop/calibrator.h: 84
for _lib in _libs.values():
    if not _lib.has("set_calibrator", "cdecl"):
        continue
    set_calibrator = _lib.get("set_calibrator", "cdecl")
    set_calibrator.argtypes = [POINTER(struct_calibrator_state)]
    set_calibrator.restype = None
    break

# /home/anigmetov/code/coreloop/coreloop/calibrator.h: 85
for _lib in _libs.values():
    if not _lib.has("calibrator_set_SNR", "cdecl"):
        continue
    calibrator_set_SNR = _lib.get("calibrator_set_SNR", "cdecl")
    calibrator_set_SNR.argtypes = [POINTER(struct_calibrator_state)]
    calibrator_set_SNR.restype = None
    break

# /home/anigmetov/code/coreloop/coreloop/calibrator.h: 86
for _lib in _libs.values():
    if not _lib.has("calibrator_slice_init", "cdecl"):
        continue
    calibrator_slice_init = _lib.get("calibrator_slice_init", "cdecl")
    calibrator_slice_init.argtypes = [POINTER(struct_calibrator_state)]
    calibrator_slice_init.restype = None
    break

# /home/anigmetov/code/coreloop/coreloop/calibrator.h: 87
for _lib in _libs.values():
    if not _lib.has("calibrator_set_slices", "cdecl"):
        continue
    calibrator_set_slices = _lib.get("calibrator_set_slices", "cdecl")
    calibrator_set_slices.argtypes = [POINTER(struct_calibrator_state)]
    calibrator_set_slices.restype = None
    break

# /home/anigmetov/code/coreloop/coreloop/calibrator.h: 88
for _lib in _libs.values():
    if not _lib.has("process_cal_mode11", "cdecl"):
        continue
    process_cal_mode11 = _lib.get("process_cal_mode11", "cdecl")
    process_cal_mode11.argtypes = [POINTER(struct_core_state)]
    process_cal_mode11.restype = POINTER(struct_calibrator_metadata)
    break

# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 31
for _lib in _libs.values():
    try:
        soft_reset_flag = (c_bool).in_dll(_lib, "soft_reset_flag")
        break
    except:
        pass

# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 33
for _lib in _libs.values():
    try:
        tap_counter = (c_uint64).in_dll(_lib, "tap_counter")
        break
    except:
        pass

# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 35
for _lib in _libs.values():
    try:
        TVS_sensors_avg = (c_uint32 * int(4)).in_dll(_lib, "TVS_sensors_avg")
        break
    except:
        pass

# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 37
for _lib in _libs.values():
    try:
        loop_count_min = (c_uint16).in_dll(_lib, "loop_count_min")
        break
    except:
        pass

# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 37
for _lib in _libs.values():
    try:
        loop_count_max = (c_uint16).in_dll(_lib, "loop_count_max")
        break
    except:
        pass

enum_gain_state = c_int# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 41

GAIN_LOW = 0# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 41

GAIN_MED = (GAIN_LOW + 1)# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 41

GAIN_HIGH = (GAIN_MED + 1)# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 41

GAIN_DISABLE = (GAIN_HIGH + 1)# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 41

GAIN_AUTO = (GAIN_DISABLE + 1)# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 41

enum_output_format = c_int# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 48

OUTPUT_32BIT = 0# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 48

OUTPUT_16BIT_UPDATES = (OUTPUT_32BIT + 1)# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 48

OUTPUT_16BIT_FLOAT1 = (OUTPUT_16BIT_UPDATES + 1)# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 48

OUTPUT_16BIT_10_PLUS_6 = (OUTPUT_16BIT_FLOAT1 + 1)# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 48

OUTPUT_16BIT_4_TO_5 = (OUTPUT_16BIT_10_PLUS_6 + 1)# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 48

OUTPUT_16BIT_SHARED_LZ = (OUTPUT_16BIT_4_TO_5 + 1)# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 48

enum_averaging_mode = c_int# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 57

AVG_INT32 = 0# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 57

AVG_INT_40_BITS = (AVG_INT32 + 1)# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 57

AVG_FLOAT = (AVG_INT_40_BITS + 1)# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 57

# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 64
class struct_route_state(Structure):
    pass

struct_route_state._pack_ = 1
struct_route_state.__slots__ = [
    'plus',
    'minus',
]
struct_route_state._fields_ = [
    ('plus', c_uint8),
    ('minus', c_uint8),
]

# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 68
class struct_time_counters(Structure):
    pass

struct_time_counters._pack_ = 1
struct_time_counters.__slots__ = [
    'heartbeat_counter',
    'resettle_counter',
    'cdi_wait_counter',
    'cdi_dispatch_counter',
]
struct_time_counters._fields_ = [
    ('heartbeat_counter', c_uint64),
    ('resettle_counter', c_uint64),
    ('cdi_wait_counter', c_uint64),
    ('cdi_dispatch_counter', c_uint64),
]

# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 78
class struct_core_state_base(Structure):
    pass

struct_core_state_base._pack_ = 1
struct_core_state_base.__slots__ = [
    'uC_time',
    'time_32',
    'time_16',
    'TVS_sensors',
    'loop_count_min',
    'loop_count_max',
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
    'averaging_mode',
    'errors',
    'corr_products_mask',
    'actual_gain',
    'actual_bitslice',
    'spec_overflow',
    'notch_overflow',
    'ADC_stat',
    'spectrometer_enable',
    'calibrator_enable',
    'rand_state',
    'weight',
    'weight_current',
    'num_bad_min_current',
    'num_bad_max_current',
    'num_bad_min',
    'num_bad_max',
]
struct_core_state_base._fields_ = [
    ('uC_time', c_uint64),
    ('time_32', c_uint32),
    ('time_16', c_uint16),
    ('TVS_sensors', c_uint16 * int(4)),
    ('loop_count_min', c_uint16),
    ('loop_count_max', c_uint16),
    ('gain', c_uint8 * int(4)),
    ('gain_auto_min', c_uint16 * int(4)),
    ('gain_auto_mult', c_uint16 * int(4)),
    ('route', struct_route_state * int(4)),
    ('Navg1_shift', c_uint8),
    ('Navg2_shift', c_uint8),
    ('notch', c_uint8),
    ('Navgf', c_uint8),
    ('hi_frac', c_uint8),
    ('med_frac', c_uint8),
    ('bitslice', c_uint8 * int(16)),
    ('bitslice_keep_bits', c_uint8),
    ('format', c_uint8),
    ('reject_ratio', c_uint8),
    ('reject_maxbad', c_uint8),
    ('tr_start', c_uint16),
    ('tr_stop', c_uint16),
    ('tr_avg_shift', c_uint16),
    ('averaging_mode', c_uint8),
    ('errors', c_uint32),
    ('corr_products_mask', c_uint16),
    ('actual_gain', c_uint8 * int(4)),
    ('actual_bitslice', c_uint8 * int(16)),
    ('spec_overflow', c_uint16),
    ('notch_overflow', c_uint16),
    ('ADC_stat', struct_ADC_stat * int(4)),
    ('spectrometer_enable', c_bool),
    ('calibrator_enable', c_bool),
    ('rand_state', c_uint32),
    ('weight', c_uint16),
    ('weight_current', c_uint16),
    ('num_bad_min_current', c_uint16),
    ('num_bad_max_current', c_uint16),
    ('num_bad_min', c_uint16),
    ('num_bad_max', c_uint16),
]

# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 121
class struct_cdi_stats(Structure):
    pass

struct_cdi_stats._pack_ = 1
struct_cdi_stats.__slots__ = [
    'cdi_total_command_count',
    'cdi_packets_sent',
    'cdi_bytes_sent',
]
struct_cdi_stats._fields_ = [
    ('cdi_total_command_count', c_uint32),
    ('cdi_packets_sent', c_uint32),
    ('cdi_bytes_sent', c_uint64),
]

# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 127
class struct_delayed_cdi_sending(Structure):
    pass

struct_delayed_cdi_sending._pack_ = 1
struct_delayed_cdi_sending.__slots__ = [
    'appId',
    'tr_appId',
    'int_counter',
    'format',
    'prod_count',
    'tr_count',
    'cal_count',
    'Nfreq',
    'Navgf',
    'packet_id',
    'cal_packet_id',
    'cal_appId',
    'cal_size',
    'cal_packet_size',
]
struct_delayed_cdi_sending._fields_ = [
    ('appId', c_uint32),
    ('tr_appId', c_uint32),
    ('int_counter', c_uint16),
    ('format', c_uint8),
    ('prod_count', c_uint8),
    ('tr_count', c_uint8),
    ('cal_count', c_uint8),
    ('Nfreq', c_uint16),
    ('Navgf', c_uint16),
    ('packet_id', c_uint32),
    ('cal_packet_id', c_uint32),
    ('cal_appId', c_uint32),
    ('cal_size', c_uint32),
    ('cal_packet_size', c_uint32),
]

# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 144
class struct_watchdog_config(Structure):
    pass

struct_watchdog_config._pack_ = 1
struct_watchdog_config.__slots__ = [
    'FPGA_max_temp',
    'watchdogs_enabled',
]
struct_watchdog_config._fields_ = [
    ('FPGA_max_temp', c_uint8),
    ('watchdogs_enabled', c_uint8),
]

# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 152
class struct_watchdog_packet(Structure):
    pass

struct_watchdog_packet._pack_ = 1
struct_watchdog_packet.__slots__ = [
    'unique_packet_id',
    'uC_time',
    'tripped',
]
struct_watchdog_packet._fields_ = [
    ('unique_packet_id', c_uint16),
    ('uC_time', c_uint64),
    ('tripped', c_uint8),
]

struct_core_state._pack_ = 1
struct_core_state.__slots__ = [
    'base',
    'cdi_stats',
    'cal',
    'cdi_dispatch',
    'timing',
    'watchdog',
    'cdi_wait_spectra',
    'avg_counter',
    'unique_packet_id',
    'leading_zeros_min',
    'leading_zeros_max',
    'housekeeping_request',
    'range_adc',
    'resettle',
    'request_waveform',
    'request_eos',
    'tick_tock',
    'drop_df',
    'heartbeat_packet_count',
    'flash_slot',
    'cmd_arg_high',
    'cmd_arg_low',
    'cmd_ptr',
    'cmd_end',
    'sequence_upload',
    'loop_depth',
    'loop_start',
    'loop_count',
    'cmd_counter',
    'dispatch_delay',
    'reg_address',
    'reg_value',
    'bitslicer_action_counter',
    'fft_time',
    'fft_computed',
]
struct_core_state._fields_ = [
    ('base', struct_core_state_base),
    ('cdi_stats', struct_cdi_stats),
    ('cal', struct_calibrator_state),
    ('cdi_dispatch', struct_delayed_cdi_sending),
    ('timing', struct_time_counters),
    ('watchdog', struct_watchdog_config),
    ('cdi_wait_spectra', c_uint16),
    ('avg_counter', c_uint16),
    ('unique_packet_id', c_uint32),
    ('leading_zeros_min', c_uint8 * int(16)),
    ('leading_zeros_max', c_uint8 * int(16)),
    ('housekeeping_request', c_uint8),
    ('range_adc', c_uint8),
    ('resettle', c_uint8),
    ('request_waveform', c_uint8),
    ('request_eos', c_uint8),
    ('tick_tock', c_bool),
    ('drop_df', c_bool),
    ('heartbeat_packet_count', c_uint32),
    ('flash_slot', c_int8),
    ('cmd_arg_high', c_uint8 * int(512)),
    ('cmd_arg_low', c_uint8 * int(512)),
    ('cmd_ptr', c_uint16),
    ('cmd_end', c_uint16),
    ('sequence_upload', c_bool),
    ('loop_depth', c_uint8),
    ('loop_start', c_uint16 * int(4)),
    ('loop_count', c_uint8 * int(4)),
    ('cmd_counter', c_uint32),
    ('dispatch_delay', c_uint16),
    ('reg_address', c_uint16),
    ('reg_value', c_int32),
    ('bitslicer_action_counter', c_int8),
    ('fft_time', c_uint32),
    ('fft_computed', c_bool),
]

# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 196
class struct_saved_state(Structure):
    pass

struct_saved_state._pack_ = 1
struct_saved_state.__slots__ = [
    'in_use',
    'cmd_arg_high',
    'cmd_arg_low',
    'cmd_ptr',
    'cmd_end',
    'CRC',
]
struct_saved_state._fields_ = [
    ('in_use', c_uint32),
    ('cmd_arg_high', c_uint8 * int(512)),
    ('cmd_arg_low', c_uint8 * int(512)),
    ('cmd_ptr', c_uint16),
    ('cmd_end', c_uint16),
    ('CRC', c_uint32),
]

# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 203
class struct_state_recover_notification(Structure):
    pass

struct_state_recover_notification._pack_ = 1
struct_state_recover_notification.__slots__ = [
    'slot',
    'size',
]
struct_state_recover_notification._fields_ = [
    ('slot', c_uint32),
    ('size', c_uint32),
]

# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 208
class struct_end_of_sequence(Structure):
    pass

struct_end_of_sequence._pack_ = 1
struct_end_of_sequence.__slots__ = [
    'unique_packet_id',
    'eos_arg',
]
struct_end_of_sequence._fields_ = [
    ('unique_packet_id', c_uint32),
    ('eos_arg', c_uint32),
]

# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 213
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
    'time_32',
    'time_16',
]
struct_startup_hello._fields_ = [
    ('SW_version', c_uint32),
    ('FW_Version', c_uint32),
    ('FW_ID', c_uint32),
    ('FW_Date', c_uint32),
    ('FW_Time', c_uint32),
    ('unique_packet_id', c_uint32),
    ('time_32', c_uint32),
    ('time_16', c_uint16),
]

# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 224
class struct_heartbeat(Structure):
    pass

struct_heartbeat._pack_ = 1
struct_heartbeat.__slots__ = [
    'packet_count',
    'time_32',
    'time_16',
    'TVS_sensors',
    'loop_count_min',
    'loop_count_max',
    'cdi_stats',
    'errors',
    'fft_time',
    'magic',
]
struct_heartbeat._fields_ = [
    ('packet_count', c_uint32),
    ('time_32', c_uint32),
    ('time_16', c_uint16),
    ('TVS_sensors', c_uint16 * int(4)),
    ('loop_count_min', c_uint16),
    ('loop_count_max', c_uint16),
    ('cdi_stats', struct_cdi_stats),
    ('errors', c_uint32),
    ('fft_time', c_uint32),
    ('magic', c_char * int(6)),
]

# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 237
class struct_meta_data(Structure):
    pass

struct_meta_data._pack_ = 1
struct_meta_data.__slots__ = [
    'version',
    'unique_packet_id',
    'base',
]
struct_meta_data._fields_ = [
    ('version', c_uint16),
    ('unique_packet_id', c_uint32),
    ('base', struct_core_state_base),
]

# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 243
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
    ('version', c_uint16),
    ('unique_packet_id', c_uint32),
    ('errors', c_uint32),
    ('housekeeping_type', c_uint16),
]

# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 250
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

# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 255
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
    ('actual_gain', c_uint8 * int(4)),
]

# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 265
for _lib in _libs.values():
    try:
        soft_reset_flag = (c_bool).in_dll(_lib, "soft_reset_flag")
        break
    except:
        pass

# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 268
for _lib in _libs.values():
    if not _lib.has("core_loop", "cdecl"):
        continue
    core_loop = _lib.get("core_loop", "cdecl")
    core_loop.argtypes = [POINTER(struct_core_state)]
    core_loop.restype = None
    break

# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 271
for _lib in _libs.values():
    if not _lib.has("process_cdi", "cdecl"):
        continue
    process_cdi = _lib.get("process_cdi", "cdecl")
    process_cdi.argtypes = [POINTER(struct_core_state)]
    process_cdi.restype = c_bool
    break

# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 275
for _lib in _libs.values():
    if not _lib.has("process_watchdogs", "cdecl"):
        continue
    process_watchdogs = _lib.get("process_watchdogs", "cdecl")
    process_watchdogs.argtypes = [POINTER(struct_core_state)]
    process_watchdogs.restype = c_bool
    break

# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 276
for _lib in _libs.values():
    if not _lib.has("cmd_soft_reset", "cdecl"):
        continue
    cmd_soft_reset = _lib.get("cmd_soft_reset", "cdecl")
    cmd_soft_reset.argtypes = [c_uint8, POINTER(struct_core_state)]
    cmd_soft_reset.restype = None
    break

# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 279
for _lib in _libs.values():
    if not _lib.has("RFS_stop", "cdecl"):
        continue
    RFS_stop = _lib.get("RFS_stop", "cdecl")
    RFS_stop.argtypes = [POINTER(struct_core_state)]
    RFS_stop.restype = None
    break

# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 280
for _lib in _libs.values():
    if not _lib.has("RFS_start", "cdecl"):
        continue
    RFS_start = _lib.get("RFS_start", "cdecl")
    RFS_start.argtypes = [POINTER(struct_core_state)]
    RFS_start.restype = None
    break

# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 281
for _lib in _libs.values():
    if not _lib.has("restart_spectrometer", "cdecl"):
        continue
    restart_spectrometer = _lib.get("restart_spectrometer", "cdecl")
    restart_spectrometer.argtypes = [POINTER(struct_core_state)]
    restart_spectrometer.restype = None
    break

# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 284
for _lib in _libs.values():
    if not _lib.has("get_Navg1", "cdecl"):
        continue
    get_Navg1 = _lib.get("get_Navg1", "cdecl")
    get_Navg1.argtypes = [POINTER(struct_core_state)]
    get_Navg1.restype = c_uint16
    break

# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 285
for _lib in _libs.values():
    if not _lib.has("get_Navg2", "cdecl"):
        continue
    get_Navg2 = _lib.get("get_Navg2", "cdecl")
    get_Navg2.argtypes = [POINTER(struct_core_state)]
    get_Navg2.restype = c_uint16
    break

# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 286
for _lib in _libs.values():
    if not _lib.has("get_Nfreq", "cdecl"):
        continue
    get_Nfreq = _lib.get("get_Nfreq", "cdecl")
    get_Nfreq.argtypes = [POINTER(struct_core_state)]
    get_Nfreq.restype = c_uint16
    break

# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 287
for _lib in _libs.values():
    if not _lib.has("get_tr_avg", "cdecl"):
        continue
    get_tr_avg = _lib.get("get_tr_avg", "cdecl")
    get_tr_avg.argtypes = [POINTER(struct_core_state)]
    get_tr_avg.restype = c_uint16
    break

# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 288
for _lib in _libs.values():
    if not _lib.has("get_gain_auto_max", "cdecl"):
        continue
    get_gain_auto_max = _lib.get("get_gain_auto_max", "cdecl")
    get_gain_auto_max.argtypes = [POINTER(struct_core_state), c_int]
    get_gain_auto_max.restype = c_uint16
    break

# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 289
for _lib in _libs.values():
    if not _lib.has("get_tr_length", "cdecl"):
        continue
    get_tr_length = _lib.get("get_tr_length", "cdecl")
    get_tr_length.argtypes = [POINTER(struct_core_state)]
    get_tr_length.restype = c_uint32
    break

# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 293
for _lib in _libs.values():
    if not _lib.has("set_route", "cdecl"):
        continue
    set_route = _lib.get("set_route", "cdecl")
    set_route.argtypes = [POINTER(struct_core_state), c_uint8, c_uint8]
    set_route.restype = None
    break

# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 296
for _lib in _libs.values():
    if not _lib.has("update_spec_gains", "cdecl"):
        continue
    update_spec_gains = _lib.get("update_spec_gains", "cdecl")
    update_spec_gains.argtypes = [POINTER(struct_core_state)]
    update_spec_gains.restype = None
    break

# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 299
for _lib in _libs.values():
    if not _lib.has("trigger_ADC_stat", "cdecl"):
        continue
    trigger_ADC_stat = _lib.get("trigger_ADC_stat", "cdecl")
    trigger_ADC_stat.argtypes = []
    trigger_ADC_stat.restype = None
    break

# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 302
for _lib in _libs.values():
    if not _lib.has("reset_errormasks", "cdecl"):
        continue
    reset_errormasks = _lib.get("reset_errormasks", "cdecl")
    reset_errormasks.argtypes = [POINTER(struct_core_state)]
    reset_errormasks.restype = None
    break

# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 305
for _lib in _libs.values():
    if not _lib.has("update_time", "cdecl"):
        continue
    update_time = _lib.get("update_time", "cdecl")
    update_time.argtypes = [POINTER(struct_core_state)]
    update_time.restype = None
    break

# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 308
for _lib in _libs.values():
    if not _lib.has("process_spectrometer", "cdecl"):
        continue
    process_spectrometer = _lib.get("process_spectrometer", "cdecl")
    process_spectrometer.argtypes = [POINTER(struct_core_state)]
    process_spectrometer.restype = None
    break

# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 311
for _lib in _libs.values():
    if not _lib.has("transfer_to_cdi", "cdecl"):
        continue
    transfer_to_cdi = _lib.get("transfer_to_cdi", "cdecl")
    transfer_to_cdi.argtypes = [POINTER(struct_core_state)]
    transfer_to_cdi.restype = None
    break

# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 313
for _lib in _libs.values():
    if not _lib.has("process_delayed_cdi_dispatch", "cdecl"):
        continue
    process_delayed_cdi_dispatch = _lib.get("process_delayed_cdi_dispatch", "cdecl")
    process_delayed_cdi_dispatch.argtypes = [POINTER(struct_core_state)]
    process_delayed_cdi_dispatch.restype = c_bool
    break

# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 316
for _lib in _libs.values():
    if not _lib.has("process_gain_range", "cdecl"):
        continue
    process_gain_range = _lib.get("process_gain_range", "cdecl")
    process_gain_range.argtypes = [POINTER(struct_core_state)]
    process_gain_range.restype = None
    break

# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 317
for _lib in _libs.values():
    if not _lib.has("bitslice_control", "cdecl"):
        continue
    bitslice_control = _lib.get("bitslice_control", "cdecl")
    bitslice_control.argtypes = [POINTER(struct_core_state)]
    bitslice_control.restype = c_bool
    break

# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 320
for _lib in _libs.values():
    if not _lib.has("set_spectrometer", "cdecl"):
        continue
    set_spectrometer = _lib.get("set_spectrometer", "cdecl")
    set_spectrometer.argtypes = [POINTER(struct_core_state)]
    set_spectrometer.restype = None
    break

# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 321
for _lib in _libs.values():
    if not _lib.has("default_state", "cdecl"):
        continue
    default_state = _lib.get("default_state", "cdecl")
    default_state.argtypes = [POINTER(struct_core_state_base)]
    default_state.restype = None
    break

# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 324
for _lib in _libs.values():
    if not _lib.has("debug_helper", "cdecl"):
        continue
    debug_helper = _lib.get("debug_helper", "cdecl")
    debug_helper.argtypes = [c_uint8, POINTER(struct_core_state)]
    debug_helper.restype = None
    break

# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 325
for _lib in _libs.values():
    if not _lib.has("cdi_not_implemented", "cdecl"):
        continue
    cdi_not_implemented = _lib.get("cdi_not_implemented", "cdecl")
    cdi_not_implemented.argtypes = [String]
    cdi_not_implemented.restype = None
    break

# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 329
for _lib in _libs.values():
    if not _lib.has("send_hello_packet", "cdecl"):
        continue
    send_hello_packet = _lib.get("send_hello_packet", "cdecl")
    send_hello_packet.argtypes = [POINTER(struct_core_state)]
    send_hello_packet.restype = None
    break

# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 330
for _lib in _libs.values():
    if not _lib.has("process_hearbeat", "cdecl"):
        continue
    process_hearbeat = _lib.get("process_hearbeat", "cdecl")
    process_hearbeat.argtypes = [POINTER(struct_core_state)]
    process_hearbeat.restype = c_bool
    break

# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 331
for _lib in _libs.values():
    if not _lib.has("process_housekeeping", "cdecl"):
        continue
    process_housekeeping = _lib.get("process_housekeeping", "cdecl")
    process_housekeeping.argtypes = [POINTER(struct_core_state)]
    process_housekeeping.restype = c_bool
    break

# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 334
for _lib in _libs.values():
    if not _lib.has("process_eos", "cdecl"):
        continue
    process_eos = _lib.get("process_eos", "cdecl")
    process_eos.argtypes = [POINTER(struct_core_state)]
    process_eos.restype = c_bool
    break

# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 337
for _lib in _libs.values():
    if not _lib.has("cdi_dispatch_uC", "cdecl"):
        continue
    cdi_dispatch_uC = _lib.get("cdi_dispatch_uC", "cdecl")
    cdi_dispatch_uC.argtypes = [POINTER(struct_cdi_stats), c_uint16, c_uint32]
    cdi_dispatch_uC.restype = None
    break

# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 340
for _lib in _libs.values():
    if not _lib.has("delayed_cdi_dispatch_done", "cdecl"):
        continue
    delayed_cdi_dispatch_done = _lib.get("delayed_cdi_dispatch_done", "cdecl")
    delayed_cdi_dispatch_done.argtypes = [POINTER(struct_core_state)]
    delayed_cdi_dispatch_done.restype = c_bool
    break

# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 343
for _lib in _libs.values():
    if not _lib.has("calibrator_default_state", "cdecl"):
        continue
    calibrator_default_state = _lib.get("calibrator_default_state", "cdecl")
    calibrator_default_state.argtypes = [POINTER(struct_calibrator_state)]
    calibrator_default_state.restype = None
    break

# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 344
for _lib in _libs.values():
    if not _lib.has("calib_set_mode", "cdecl"):
        continue
    calib_set_mode = _lib.get("calib_set_mode", "cdecl")
    calib_set_mode.argtypes = [POINTER(struct_core_state), c_uint8]
    calib_set_mode.restype = None
    break

# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 345
for _lib in _libs.values():
    if not _lib.has("process_calibrator", "cdecl"):
        continue
    process_calibrator = _lib.get("process_calibrator", "cdecl")
    process_calibrator.argtypes = [POINTER(struct_core_state)]
    process_calibrator.restype = None
    break

# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 346
for _lib in _libs.values():
    if not _lib.has("dispatch_calibrator_data", "cdecl"):
        continue
    dispatch_calibrator_data = _lib.get("dispatch_calibrator_data", "cdecl")
    dispatch_calibrator_data.argtypes = [POINTER(struct_core_state)]
    dispatch_calibrator_data.restype = None
    break

# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 356
for _lib in _libs.values():
    if not _lib.has("mini_wait", "cdecl"):
        continue
    mini_wait = _lib.get("mini_wait", "cdecl")
    mini_wait.argtypes = [c_uint32]
    mini_wait.restype = None
    break

# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 360
for _lib in _libs.values():
    if not _lib.has("timer_start", "cdecl"):
        continue
    timer_start = _lib.get("timer_start", "cdecl")
    timer_start.argtypes = []
    timer_start.restype = None
    break

# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 363
for _lib in _libs.values():
    if not _lib.has("timer_stop", "cdecl"):
        continue
    timer_stop = _lib.get("timer_stop", "cdecl")
    timer_stop.argtypes = []
    timer_stop.restype = c_uint32
    break

# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 367
for _lib in _libs.values():
    if not _lib.has("encode_12plus4", "cdecl"):
        continue
    encode_12plus4 = _lib.get("encode_12plus4", "cdecl")
    encode_12plus4.argtypes = [c_int32]
    encode_12plus4.restype = c_uint16
    break

# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 368
for _lib in _libs.values():
    if not _lib.has("decode_12plus4", "cdecl"):
        continue
    decode_12plus4 = _lib.get("decode_12plus4", "cdecl")
    decode_12plus4.argtypes = [c_uint16]
    decode_12plus4.restype = c_int32
    break

# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 371
for _lib in _libs.values():
    if not _lib.has("encode_10plus6", "cdecl"):
        continue
    encode_10plus6 = _lib.get("encode_10plus6", "cdecl")
    encode_10plus6.argtypes = [c_int32]
    encode_10plus6.restype = c_uint16
    break

# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 372
for _lib in _libs.values():
    if not _lib.has("decode_10plus6", "cdecl"):
        continue
    decode_10plus6 = _lib.get("decode_10plus6", "cdecl")
    decode_10plus6.argtypes = [c_uint16]
    decode_10plus6.restype = c_int32
    break

# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 378
for _lib in _libs.values():
    if not _lib.has("encode_shared_lz_positive", "cdecl"):
        continue
    encode_shared_lz_positive = _lib.get("encode_shared_lz_positive", "cdecl")
    encode_shared_lz_positive.argtypes = [POINTER(c_uint32), POINTER(c_ubyte), c_int]
    encode_shared_lz_positive.restype = c_int
    break

# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 379
for _lib in _libs.values():
    if not _lib.has("decode_shared_lz_positive", "cdecl"):
        continue
    decode_shared_lz_positive = _lib.get("decode_shared_lz_positive", "cdecl")
    decode_shared_lz_positive.argtypes = [POINTER(c_ubyte), POINTER(c_uint32), c_int]
    decode_shared_lz_positive.restype = None
    break

# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 385
for _lib in _libs.values():
    if not _lib.has("encode_shared_lz_signed", "cdecl"):
        continue
    encode_shared_lz_signed = _lib.get("encode_shared_lz_signed", "cdecl")
    encode_shared_lz_signed.argtypes = [POINTER(c_int32), POINTER(c_ubyte), c_int]
    encode_shared_lz_signed.restype = c_int
    break

# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 386
for _lib in _libs.values():
    if not _lib.has("decode_shared_lz_signed", "cdecl"):
        continue
    decode_shared_lz_signed = _lib.get("decode_shared_lz_signed", "cdecl")
    decode_shared_lz_signed.argtypes = [POINTER(c_ubyte), POINTER(c_int32), c_int]
    decode_shared_lz_signed.restype = None
    break

# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 389
for _lib in _libs.values():
    if not _lib.has("encode_4_into_5", "cdecl"):
        continue
    encode_4_into_5 = _lib.get("encode_4_into_5", "cdecl")
    encode_4_into_5.argtypes = [POINTER(c_int32), POINTER(c_uint16)]
    encode_4_into_5.restype = None
    break

# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 390
for _lib in _libs.values():
    if not _lib.has("decode_5_into_4", "cdecl"):
        continue
    decode_5_into_4 = _lib.get("decode_5_into_4", "cdecl")
    decode_5_into_4.argtypes = [POINTER(c_uint16), POINTER(c_int32)]
    decode_5_into_4.restype = None
    break

# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 393
for _lib in _libs.values():
    if not _lib.has("CRC", "cdecl"):
        continue
    CRC = _lib.get("CRC", "cdecl")
    CRC.argtypes = [POINTER(None), c_size_t]
    CRC.restype = c_uint32
    break

# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 396
for _lib in _libs.values():
    if not _lib.has("fft_precompute_tables", "cdecl"):
        continue
    fft_precompute_tables = _lib.get("fft_precompute_tables", "cdecl")
    fft_precompute_tables.argtypes = []
    fft_precompute_tables.restype = None
    break

# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 397
for _lib in _libs.values():
    if not _lib.has("fft", "cdecl"):
        continue
    fft = _lib.get("fft", "cdecl")
    fft.argtypes = [POINTER(c_uint32), POINTER(c_uint32)]
    fft.restype = None
    break

# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 7
try:
    VERSION_ID = 768
except:
    pass

# /home/anigmetov/code/coreloop/coreloop/calibrator.h: 21
try:
    CAL_MODE_BIT_SLICER_SETTLE = 16
except:
    pass

# /home/anigmetov/code/coreloop/coreloop/calibrator.h: 23
try:
    CAL_MODE_SNR_SETTLE = 32
except:
    pass

# /home/anigmetov/code/coreloop/coreloop/calibrator.h: 25
try:
    CAL_MODE_RUN = 48
except:
    pass

# /home/anigmetov/code/coreloop/coreloop/calibrator.h: 27
try:
    CAL_MODE_BLIND = 64
except:
    pass

# /home/anigmetov/code/coreloop/coreloop/calibrator.h: 29
try:
    CAL_MODE_ZOOM = 80
except:
    pass

# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 19
try:
    DISPATCH_DELAY = 6
except:
    pass

# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 20
try:
    RESETTLE_DELAY = 5
except:
    pass

# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 21
try:
    HEARTBEAT_DELAY = 1024
except:
    pass

# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 22
try:
    CMD_BUFFER_SIZE = 512
except:
    pass

# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 23
try:
    MAX_LOOPS = 4
except:
    pass

# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 24
try:
    ADC_STAT_SAMPLES = 16000
except:
    pass

# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 25
try:
    MAX_STATE_SLOTS = 16
except:
    pass

# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 26
try:
    BITSLICER_MAX_ACTION = 5
except:
    pass

# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 353
def MAX(x, y):
    return (x > y) and x or y

# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 354
def MIN(x, y):
    return (x < y) and x or y

# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 355
def IS_NEG(x):
    return (x < 0) and 1 or 0

calibrator_state = struct_calibrator_state# /home/anigmetov/code/coreloop/coreloop/calibrator.h: 36

calibrator_metadata = struct_calibrator_metadata# /home/anigmetov/code/coreloop/coreloop/calibrator.h: 62

saved_calibrator_weights = struct_saved_calibrator_weights# /home/anigmetov/code/coreloop/coreloop/calibrator.h: 75

core_state = struct_core_state# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 159

route_state = struct_route_state# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 64

time_counters = struct_time_counters# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 68

core_state_base = struct_core_state_base# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 78

cdi_stats = struct_cdi_stats# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 121

delayed_cdi_sending = struct_delayed_cdi_sending# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 127

watchdog_config = struct_watchdog_config# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 144

watchdog_packet = struct_watchdog_packet# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 152

saved_state = struct_saved_state# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 196

state_recover_notification = struct_state_recover_notification# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 203

end_of_sequence = struct_end_of_sequence# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 208

startup_hello = struct_startup_hello# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 213

heartbeat = struct_heartbeat# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 224

meta_data = struct_meta_data# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 237

housekeeping_data_base = struct_housekeeping_data_base# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 243

housekeeping_data_0 = struct_housekeeping_data_0# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 250

housekeeping_data_1 = struct_housekeeping_data_1# /home/anigmetov/code/coreloop/coreloop/core_loop.h: 255

# No inserted files

# No prefix-stripping

