# PKMud Source (PKsrc)

This directory contains the source code for the PKMud server, based on the Merc DikuMUD codebase with C++ extensions.

## Prerequisites

- **C++ Compiler**: `g++` (supports modern GCC versions, e.g. GCC 13+)
- **Make**: GNU Make
- **Libraries**:
  - `libcrypt` (e.g. `libcrypt-dev` on Debian/Ubuntu)
  - Standard C/C++ math and runtime libraries (`-lm`, `libstdc++`)

## Building

To build the executable:

```bash
cd PKsrc
make
```

To clean and perform a fresh rebuild:

```bash
make clean && make
```

The build produces the binary `PKexecutable`.

## Running

By default, the server can be started with an optional port number (must be > 1024, default is 4000):

```bash
cd PKArea
../PKsrc/PKexecutable [port #]
```

Example:
```bash
cd PKArea
../PKsrc/PKexecutable 4000
```

Startup scripts are also available in this directory:
- `PKstartup`
- `PKstart2`

## Recent Modernization & Compatibility Fixes

1. **twitcurl Dependency Removed**:
   - The obsolete and uninstalled `twitcurl` library has been decoupled from `twitterAnnounce.h` and `twitterAnnounce.cpp`.
   - The `twitterAnnounce` class interface has been retained as a stub so game logic callers (`comm.c`, `update.c`) operate without modification.
   - `-ltwitcurl` removed from linker flags in `Makefile`.

2. **Pointer Comparison Fix**:
   - Corrected pointer checks against `'\0'` in `social.c` to compare with `NULL` instead, resolving modern C++ `-fpermissive` compilation errors.

3. **Modern GCC (g++ 13) Compiler and Linker Flags**:
   - Added `-Wno-narrowing` and `-Wno-write-strings` to `C_FLAGS` in `Makefile` to handle legacy Telnet protocol character constants and string literal assignments under modern C++ standards.
   - Reordered `$(L_FLAGS)` to appear after object files `$(O_FILES)` in the `Makefile` link command, allowing GNU `ld` to properly resolve symbols from `libcrypt`.
