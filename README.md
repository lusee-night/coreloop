# Test status

![Automated tests now](https://github.com/lusee-night/coreloop/actions/workflows/uncrater_tests.yml/badge.svg)

# coreloop
Core loop of the LuSEE-Night uC flight software.

# To compile

To compile, make sure you have cmake installed. To make the build environment
please issues

```
cmake -B build
```

To build or rebuild the code, use

```
cmake --build build
```

Alternatively, VS code should pick up the configuration files in .vscode. You can compile/run withing the VS code environemnt.
Pressing Ctrl+Shift+P and then issuing `Cmake:Build` command will rebuild the codebase.

# versions

The version history (changelog) is found [here](documentation/CHANGELOG.md).

Note that each version needs to be compiled with `LuSEE_MiV` of the same version.

## Note on versions 0x308, 0x309:

In general, the version commited and tagged in github contains a matching version in `coreloop.h`. Version 0x308 is an exception, since it is 0x307 + documentation and code comments. Version 0x309 is also an exception, since it fixes a single bug. So coreloop.h version in this case still says 0x307.

# Code documentation

Can be found [here](documentation/coreloop_intro.md).





