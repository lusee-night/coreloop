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


# Code organization

The directory `coreloop` contains the actual coreloop code that is shared between this repo and the actual flight software repo. Directories `src` and `include` contain the support code that is neccessary to run `coreloop` in a test environment.




