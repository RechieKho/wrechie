# Thirdparty libraries / code

Please write down the information regarding the third party source you are adding. Example:
```md
## libexample
- Upstream: https:// github.com/libexample/libexample
- Version: 0.0.1 (%GIT_HASH%, 2022) 
- License: BSD

The compiled static libraries for each platform is stored in its corresponding folder, for instance, static library for linux is in `linux/` folder.
```

Please keep categories (`##` level) listed alphabetically and matching their respective folder names. Use two empty lines to seperate categories for readability.


## wren
- Upstream: https://github.com/wren-lang/wren
- Version: 0.4.0 (4a18fc489f9ea3d253b20dd40f4cdad0d6bb40eb, 2022)
- License: MIT

extracted files: `src/vm/`, `src/optional/`, `src/include/*`


## fmt
- Upstream: https://github.com/fmtlib/fmt
- Version: 8.1.1 (b6f4ceaed0a0a24ccf575fab6c56dd50ccf6f1a9, 2022)
- License: custom

extracted files: `src/format.cc`, `src/os.cc` and `include/fmt/`


## cxxopts
- Upstream: https://github.com/jarro2783/cxxopts
- Version: 3.0.0 (c74846a891b3cc3bfa992d588b1295f528d43039, 2021)
- License: MIT

It is a header-only library. Extract all the header files.


## dylib
- Upstream: https://github.com/martin-olivier/dylib
- Version: 2.0.0 (657cd9f583dbaf5513075a6a0c0a46ebe09cefcd, 2022)
- License: MIT

It is a header-only library. Extract all the header files.


## cpppath
- Upstream: https://github.com/tdegeus/cpppath
- Version: git (4b55179030f324f58f6f5d7041b0109a4ba1586a, 2020)
- License: MIT

It is a header-only library.
Add `inline` to `all_equal` function on line `414` to fix multiple definition error.


## cjson
- Upstream: https://github.com/DaveGamble/cJSON
- Version: 1.7.15 (d348621ca93571343a56862df7de4ff3bc9b5667, 2021)
- License: MIT

