# Mist
Mist is a 2D game engine inspired by Unreal.

It is written primarily in Lua, with some functionality, like OS interaction and the basics of graphics subsystem, is implemented in C++11.

The facilities of the engine may also be used for other purposes. For example, 'img' library uses them for high-precision image processing.

The engine is still very early in development. The codebase is rather unstable, so there's also a large amount of code in the project that is either obsolete, or not adapted to the changed interfaces.

## Prerequisites
The engine depends on [LuaJIT](http://luajit.org/luajit.html) and [libpng](http://libpng.org/pub/png/libpng.html). To build them, follow the instructions on their sites.
Note that, under Windows, by default, LuaJIT makefiles produce only the dynamic library. To obtain the static one, pass `-BUILDMODE=static` override.
Once ready, the .a files should be places in the `mist/` directory (along with `client` and `output`) and named `libluajit-ARCH.a` and `libpng-ARCH.a`, with `ARCH` replaced by the target environment's identifier, which are as follows:

| OS            | Processor         | Identifier |
| ------------- | ----------------- | ---------- |
| Windows (x86) | x86 compatible    | `win32`    |
| Windows (x64) | x86-64 compatible | `win64`    |
| Android       | ARM v7            | `arm7a`    |

For example, under 32-bit versions of Windows, the build system will search for `libluajit-win32.a` and `libpng-win32.a`.

## Building
The engine uses a custom LuaJIT script for building. If supported, it may be run by the system's default Lua interpreter. Otherwise, it's possible to build and use the "luaclient" program included there.

The intended compiler is MinGW. The engine uses `std::thread`, which is currently only supported by `threads-posix` branch. You can find it [here](https://sourceforge.net/projects/mingwbuilds/files/host-windows/releases/).

When ready, run the "build.lua" script in the project's root directory.

Once built, only the contents of `output` folder are required and used by the engine. The `build` folder contains intermediate files.

## Building with Visual Studio 2013
So, I finally got around to teaching VS how to deal with all those code generators, and it seems to work.

Again, you will need LuaJIT, libpng and also zlib.

To build LuaJIT, follow the instructions for msvc on their installation page. Once finished, grab `lua51.dll`, `lua51.lib` and `lua.exe` and copy them into the root `mist/` directory. Rename the .lib file to `lua51-ARCH.lib` (see the table above). Copy the dll and rename the copy `lua51-ARCH.dll`. In the end, you should have the `lua.exe`+`lua51.dll` pair to run the code generating scripts and `lua51-ARCH.dll`+`lua51-ARCH.lib` for the engine itself.

Similarly, build libpng with the included solution. To avoid linker warnings when compiling the engine, you can disable link-time and whole program optimization for libpng; though this is not a requirement. After that, you should have `libpng.dll`, `libpng.lib`, `zlib.dll` and `zlib.lib` in the projects' output directory. Copy them to `mist/` and append the corresponding `-ARCH` for each of them.

Now, you should be ready to open `mist/vs/vs.sln` and build the solution. Due to the way file link are made, if you change the active platform or target, you should reload the projects or, alternatively, restart Visual Studio.

Again, intermediate files are put into `build/vs-config-platform` and the resulting binaries are in `output/bin-vs-config-platform`. The appropriate dlls are copied into this folder as well. As its name doesn't really matter, if you wish, you can rename it into just `bin` or whatever strikes your fancy.
