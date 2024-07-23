# httpc - simple http/1.1 serializer / deserializer

## Warning

This library was made quickly - it lacks proper testing, and should not be used in any serious task.

For normal usage (see test directory) valgrind doesn't report any memory leaks, but be on guard.

## Building & Including in Your project

For now library consists only of `./source/httpc.c` and `./include/httpc.h`, so You can easily add it to Your project, just by adding httpc.c to your sources list.
```bash
$~ gcc your_source.c ... ./httpc/source/httpc.c -Ihttpc/include -o your_exe
```

You can also easily add this library to Your project via cmake:
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

Just one other thing - I'm using this library in simple http server, so only functions relevant to this usage are implemented. Other functionality will come in the future.

## Todo

- [ ] use some testing framework
- [ ] fragment httpc.c to multiple source files
- [ ] add missing request and response methods
- [ ] modify API for better readability
- [ ] cleanup the ugly serialization code
