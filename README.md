# Layomap

Layomap is a simple command line program which automatically apply predefined layout files to an orienteering source map file.

The point of the project is to automate the progress of applying paper layouts (like A4 with a border and map title and information) to raw map files. The program allows to apply multiple layouts, which means a source map can cover several forests (or sprint areas) close to each other and layouted files for distribution can be created from the source map.

This will also make it possible to implement a sort of "continous integration" workflow, like with software, where the source map can be version controlled in GIT and every time someone commits a new version, the finished layouted files are automatically generated.

This repo is a fork of the [OpenOrienteering Mapper](https://www.openorienteering.org/apps/mapper/) project, and uses [its code](https://github.com/OpenOrienteering/mapper) to import/export maps in .xmap, .omap and .ocd format and to perform operations based on layout files.

All of the code for the project is in the /layomap directory. The build.sh script can be used to build the program for linux. Also follow the instructions of Mapper's install guide in INSTALL.md.
For now I have just tried to compile this on Ubuntu Linux (20.04.6 through WSL) but since Mapper is crossplatform it should be possible on Windows and MacOS too.

## Issues, improvements, comments & discussions

All done here in GitHub ([Issues](https://github.com/SebRM/layomap/issues) and [discussions](https://github.com/SebRM/layomap/discussions)), or contact the original author (SebRM) on email [sebrm@live.dk](mailto:sebrm@live.dk).

## Download

The program can be manually compiled along with all of OO Mapper by following the instructions in INSTALL.md. The latest version is available in a [Docker image](https://github.com/SebRM/layomap/pkgs/container/layomap-image).

## Usage

The program is compiled to an executable (see above) which can be run without any arguments:

`layomap`

This will look for some special files and folders in the current directory:

- The source map file, either `sourcemap.xmap` or `sourcemap.omap` (in that order).
- The layout file folder. By default this is `layouts` which contain layout files ending in `.layout.xmap` or `.layout.omap`.
- The output folder, `output` by default, which will be populated with folders for each layout map containing exported files in omap and ocd formats.

These files and folders can also be provided explicitly with arguments:

- `-h,` `--help` Displays this help.
- `-v`, `--version` Displays version information.
- `-s`, `--source <source>` Path to source file in XML (.omap/.xmap) format. If
  not provided, the program will look for sourcemap
  files in this order:
  'sourcemap.xmap', 'sourcemap.omap'
- `-l`, `--layouts <layouts>` Path to layouts directory. Defaults to 'layouts'.
- `-o`, `--output <output>` Path to output directory. Defaults to 'output'.

### Layout files

Layout files are special files for Layomap and are picked up from the provided (or default) layouts folder. A Layout file must end with `.layout.omap` or .`layout.xmap`.

The layout map can contain any symbols and objects you like; typically a title, info about the map, some logos and so on.

Your sourcemap will be imported into the layout map, which means all the layout symbols will be above the source map symbols.

A layout map contains (for now) 2 special objects, defined by their symbol number:

- `900.0` is considered a **border object**. There shall be at most 1 object with symbol 900.0! It must be a line object, and it must end where it started.

  Every object in the source map (not the layout map) outside of the border object will be cutout, after which the border object is removed.

- `900.1` is considered a **frame object**. There shall be at most 1 object with symbol 900.1! It must be a line object, and it must end where it started.

  Every object in both the source map and the layout map that are outside the frame object will be cutout and the frame stays.

## The future

Currently there is one critical bug which needs to be fixed. When the map is exported to .ocd format (any version), for some reason the text changes alignment, position and line height. I have looked into the Mapper sourcecode (where it works in 'Save as...') but cannot find out what to do differently.

Also, it takes ages to compile because I am no good at cmake and compiler stuff so it compiles all of OO Mapper every time. This is possibly optimizable.

Regarding new features, I would like to make something so the date on the map is automatically updated, and perhaps make special fields so one can change definitions of special symbols, cartographer credits and such without doing it in several layout files. I'd also perhaps like PDF and other exports, and more customization of the program with a config file. But all that is far into the horizon. Let me know if you have any ideas.

## License

Layomap is, just like OpenOrienteering Mapper, licensed under the [GNU GENERAL PUBLIC LICENSE Version 3](https://www.gnu.org/licenses/gpl.html).

## Credits etc

This project (for now) is made entirely by Sebastian R Maltby (SebRM on GitHub) who had basically no prior C++ experience but thought it was a fun project that would be of some use.

This is of course a very small project, and someone with more experience with C++ and the Mapper project could do it in a fraction of the time.

The Mapper project is awesome and I want to thank them for making this possible so easily thanks to good code and good documentation (and open source), and of course also for a great program which I have used for many hours to make orienteering maps.

If anyone have any questions about this or want something different or want to help, you're always welcome to contact me on email [sebrm@live.dk](mailto:sebrm@live.dk).
