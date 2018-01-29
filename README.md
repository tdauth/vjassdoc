# vjassdoc
This program is a simple JASS and vJass documentation generation tool.
It has been written by Tamino Dauth <tamino@cdauth.de>.
The program can parse multiple JASS and vJass file and generate HTML files from it.
It does also support creating an SQLite database.
The parser has been written manually and does not rely on tools like Bison or Boost.Spirit.
It stores the declarations into objects of different classes.

## Usage
The following call of the progarm parses the file `mycode.j` which can contain JASS and vJass code.
It generates a HTML documentation into the directory `./out`.
It sorts parsed identifiers alphabetically and creates one sub page per declaration.
```
./vjassdoc -lgsva -T "My API Documentation" -I . -D ./out mycode.j
```

## Automatic Build with TravisCI
[![Build Status](https://travis-ci.org/tdauth/vjassdoc.svg?branch=master)](https://travis-ci.org/tdauth/vjassdoc)

## Manual Build
For compilation you have to use CMake.
The following CMake options are supported:
* DEBUG: Enables the debug mode.
* GUI: Creates a KDE4-based GUI for the program.
* SQLITE: Adds SQLite support. This allows to create an SQLite databse with the parsed information.
* DOC: Generates an API documentation with Doxygen.

### Dependencies
* gettext
* Boost.Filesystem
* Optional: SQLite (for database support)
* Optional: Doxygen (for the API documentation)
* Optional: KDE4 (for the GUI)
