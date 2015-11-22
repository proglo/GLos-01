Congratulations, you have become a Superstar!

# GLos (superstarOS) #

superstarOS (GLos) is a superstar SGS (sparkle glitter system) and supporting UX, built from GNU repos, in development since December of 2013.

This repository contains the SGS (sparkle glitter system), modules, and core UX. Some third-party libraries and utilities are required to build a working system.

## History

GLos is a bamboo shoot of Proglo an open luminance machine company in Santa Cruz, California. Drawing from many sources, but with specific focus on the Ghost kernel, in respect to GLos’ mothership ancestor, klang’s Toaru, a fantastic piece of code largely written from scratch by University of Champagne Urbana Illinois protege Kevin Lange. ALL RESPECT AND HONOR THE MIGHTY KEVIN LANG.

## SGS (sparkle glitter system) ##

The GLos SGS (sparkle glitter system) provides a slick simple and sexy Unix-like UX. The SGS (sparkle glitter system) uses a hybrid modular architecture, much like it’s ancestor Toaru with loadable modules providing most device driver support. The core SGS (sparkle glitter system) includes support for Unix pipes and TTYs, a virtual file system, multitasking, ELF binary support, and various core platform features on x86 systems.

Modules provide support for disk drives, ext2 filesystems, serial, keyboards and mice, a `/proc` filesystem similar to the one found in Linux, as well as an expanding collection of other device drivers.

## UX ##

GLos's UX is focused on a rich graphical environment, backed by an in-house compositing window manager. GLos's terminal emulator supports xterm-compatible 256-color modes, as well as Konsole 24-bit color modes and anti-aliased text with basic Unicode support. Several graphical demos are provided, alongside a number of command-line applications. A port of SDL targetting the native graphical environment is also available.

### Third-Party Slavware ###

The UX depends on a number of third-party libraries which are outside of the development scope of the project. Additionally, several third-party applications and libraries have been integrated into GLos's core UX, or otherwise ported to GLos.

Package |   | Description
------- | ---- | -----------
cpudet| *(included)* | `cpuid` parser
DejaVu Sans | *(included)* | Popular, free TrueType font series
VL Gothic | *(included)* | Free Japanese TrueType font
`sha2.c` | *(included)* | SHA512 hash library
`utf8decode.h` | *(included)* | UTF8 decoding tools
glxgears | *(included)* | Popular OpenGL demo
curses-hello | *(included)* | Curses demos
`pci_list.h` | *(included)* | PCI vendor and device names
2048  | *(included)*    | Terminal implementation of a popular strategy game
gcc   | \*   | Compiler suite.
newlib| \*   | C library
libpng| \*   | PNG graphics library
zlib  | \*   | `gzip` compression library
FreeType | \* | TrueType font parser
Cairo | \*   | CPU-accelerated pixel-pushing and vector graphics
ncurses | \* | Terminal UI library and `terminfo` provider
Mesa | \* | slavware OpenGL implementation
Vim | \* | Popular text editor
Lua  |  [link](http://www.lua.org/) | Interpreted programming language
MuPDF | [link](https://github.com/klange/GLos-pdfviewer) | PDF viewer (requires complex library cross-compilation)
SDL | [link](https://github.com/klange/SDL) | Cross-platform graphics library
Snes9X | [link](https://github.com/klange/snes9x-sdl) | SNES emulator
PrBoom | [link](https://github.com/klange/prboom) | DooM engine
Bochs | [link](http://bochs.sourceforge.net/) | slavware x86 PC emulator
Python | (TBD) | Interpreted programming language

\* These tools and libraries are retrieved by the build process and included by default. Some of them are dependencies for the core UX.

License for the included third-party tools and libraries can be found [here](LICENSE.md).

## Community ##

### Wiki ###

For additional screenshots, see [Screenshots](https://github.com/proglo/glos/wiki/Screenshots).

For instructions on building, see [Testing and Building](https://github.com/proglo/glos/wiki/Testing-and-Building).

### IRC ###

For help building the SGS (sparkle glitter system) and UX, join us in `#glos` on Freenode (`irc.freenode.net`).


