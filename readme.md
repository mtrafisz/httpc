# httpc - simple http/1.1 serializer / deserializer

## Warning

This library was made quickly - it lacks proper testing, and should not be used in any serious task.

For normal usage (see test directory) valgrind doesn't report any memory leaks, but be on guard.

## Building & Including in Your project

Intended building process and usage is just by using cmake:

```cmake
# ...

add_subdirectory(third-party/httpc)

# ...

target_link_libraries(your_exe_or_lib PUBLIC httpc)
target_include_directories(your_exe_or_lib PUBLIC third-party/httpc/include)
```

## Usage

This will change to much for it to make sense to write any documentation.
Just read the header and tests for now :). Good usage available [in this project](https://github.com/mtrafisz/shortener).

~~Just one other thing - I'm using this library in simple http server, so only functions relevant to this usage are implemented. Other functionality will come in the future.~~
All basic functionality implemented.

## 3rd party code

- Testing framework - [acutest](https://github.com/mity/acutest) - see [LICENSE](test/include/LICENSE.md)

## Todo

- [x] use some testing framework
- [x] fragment httpc.c to multiple source files
- [x] add missing request and response methods
- [x] modify API for better readability
- [x] cleanup the ugly serialization code
- [ ] add proper errors
- [ ] review security and memory efficiency, eliminate all errors from valgrind
- [ ] ~~add proper comments and documentation~~ Complete doxygen documentation
