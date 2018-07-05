# Mist

Mist is a 2D game engine inspired by Unreal.

It is written primarily in Lua, with some functionality, like OS interaction and the basics of graphics subsystem, implemented in C++11.

The facilities of the engine may also be used for other purposes. For example, 'img' library uses them for high-precision image processing.

The engine is still very early in development. The codebase is rather unstable, so there's also a large amount of code in the project that is either obsolete, or not adapted to the changed interfaces.

## Building

To build the current version of the engine on Windows, just having Visual Studio 2017 should be enough. Open `vs/mist-solution.sln`, select a configuration and issue a 'Build solution' command.

For small changes that don't involve adding/removing source files or changing reflected interfaces, just rebuilding the solution should suffice. Otherwise, the internal build system must be invoked.

To invoke Mist's own build system, a Lua 5.1 interpreter is required. It is used to generate VS project files with the actual source file list and to generate reflection files. It may also be used to produce the binaries by invoking GCC or MSVC toolchains through the command line.

The main script `tool/build.lua` expects to be called from the project's root in this fashion:

```
mist&gt; lua <i>configuration</i> tool/build.lua <i>target</i>
```

Similar to `make`, if a proper production of the <i>target</i> requires other files to be produced beforehand, the build system will take care of that.

Possible <i>target</i>s are:

* `all` builds everything to the latest version.

* `vsproj` produces the VS project files according to the current structure of the source files, and additionally builds `reflection`. After this step is done, the produced solution may be used again to build the latest version.
  VS project files are located in the directory `vs`.

* `reflection` produces the files used in the reflection system, incuding .r.cpp files which are build and linked along with the source files to provide the required wrappers to the C++ interface.

* <i>filename</i> is used to produce specific files.
  `o//client-main.exe` produces the main executable. `o//` here stands for the output directory, which has the format of `output/bin-{toolchain}-{platform}-{profile}/`. For example, one of the configuration produces `output/bin-gcc-win64-debug/client-main.exe`.
  `b//{path-to-cpp}.o` produces the object file. Similarly, `b//` is mapped to the intermediate build directory `build/{toolchain}-{platform}-{profile}/`. In the same example, `src/utils/path.cpp` is compiled into `build/gcc-win64-debug/src/utils/path.o`, which may be referenced as `b//src/utils/path.o`.

The list of the source files with their place in the build system is located in `sources.lua`.

The statements in <i>configuration</i> are used to tune the behavior of the build system. Technically, this is achieved by running short Lua chunks before the main script that set certain global variables; therefore, the general syntax for them is `-e {name}={value}`, where `-e` causes the Lua interpreter to run the next argument as a script.

* `-e toolchain='gcc'` and `-e toolchain='cl'` determine which commands are used to compile and link the binaries. `gcc` causes the system to use `g++` commands with the GCC argument syntax, and `cl` causes the use of `cl` and `link` commands with the MSVC syntax.
  The build system assumes these commands are readily available through the PATH environment variable.
  The default value is `gcc`.

* `-e platform='win32'` and `-e platform='win64'` tells the system the target platform.
  The build system assumes the provided toolchain actually corresponds to the declared platform.
  The default value is `win64`.

* `-e profile='debug'` and `-e profile='release'` determines the use of certain compiler flags, as well as the presence of `MIST_DEBUG` macro in the source files.
  The default value is `debug`.

* `-e printgraph=true` causes the system to print out the constructed dependency graph without building anything.

* `-e dryrun=true` causes the system to print out the compiler commands instead of actually issuing them. This command does not affect the production of `reflection` and `vsproj` targets, as they are implemented through Lua procedures rather than standalone commands.

For most uses, the build system should automatically detect the changes in source files and rebuild the targets that depend on them, including the dependencies introduced by `#include` directives. However, is is also possible to treat every target as stale by passing `-e force=true` in the <i>configuration</i>, which causes the system to produce every requested target anew regardless of the current state.

`build` directory holds intermediate files, while `output/bin-*` directories hold the resulting binaries. These can be deleted at any time. Both the VS project build process, as well as the internal Mist build system will create and populate them again if necessary.

`reflection` directory holds files to support the reflection mechanism. Only the internal build system can be used to create and populate it, as the VS projects are not set up to rebuild reflection support files.
