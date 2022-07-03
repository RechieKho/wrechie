# `Path`
`Path`, a builtin class packed with common pathname manipulation.

To use this class,
```wren
import "path" for Path
```

## `Path.sep`
Directory seperator. On unix system, it is `/` while on windows, it is `\`.

## `Path.dirname(path)`
Returns the directory name of pathname `path`. For instance, the result of `Path.dirname("/cow/data.txt")` is `"/cow"`.

## `Path.filename(path)`
Returns the filename of pathname `path`, including its extension. For instance, the result of `Path.filename("/cow/data.txt")` is `"data.txt"`.

## `Path.filebase(path)`
Returns the filename of pathname `path` **without its extension**. For instance, the result of `Path.filebase("/cow/data.txt")` is `"data"`.

## `Path.splitext(path)`
Returns a list consisting the root and the extension of pathname `path`. For instance, the result of `Path.splitext("/cow/data.txt")` is `["/cow/data", ".txt"]`.

## `Path.ext(path)`
Returns the extension of pathname `path`. For instance, the result of `Path.splitext("/cow/data.txt")` is `".txt"`.

The result is same as `Path.splitext(path)[0]`.

## `Path.split(path)`
Returns a list consisting the component of pathname `path`. For instance, the result of `Path.split("/cow/data.txt")` is `["cow", "data.txt"]`.

## `Path.join(list)`
Returns a pathname joined from a list of path `list`. For instance, the result of `Path.join(["/cow","data.txt"])` is `"/cow/data.txt"`.

Elements of the `list` must be string.

> **NOTE**: `path != Path.join(Path.split(path))`. This is because `Path.split()` do strip off the leading `/`.

## `Path.select(path, begin, [end])`
Returns a new pathname in which it is sliced from pathname `path` starts from `begin` to `end`. 

Examples:
```wren
Path.select("/path/to/foo/bar.txt", 2) # foo/bar.txt
Path.select("/path/to/foo/bar.txt", 2, 3) # foo/
```

## `Path.normpath(path)`
Returns a normalized pathname from pathname `path`.

Examples:
```wren
Path.normpath("/path/../foo/bar.txt") # /foo/bar.txt
Path.select("/path/to/./bar.txt") # /path/to/bar.txt
```

## `Path.commonprefix(list)`
Returns the common prefix of pathnames in `list`. 

Examples:
```wren
Path.commonprefix([
    "/path/to/id=000/file.txt", 
    "/path/to/id=001/file.txt"
]) # /path/to/id=00
```

> **NOTE**: It might give invalid pathname. To obtain a valid path, use [`Path.commondirname(list)`](#pathcommondirnamelist) instead.

## `Path.commondirname(list)`
Returns the common directory of pathnames in `list`.

Examples:
```wren
Path.commondirname([
    "/path/to/id=000/file.txt", 
    "/path/to/id=001/file.txt"
]) # /path/to
```

## `Path.exists(path)`
Returns `true` if the pathname `path` exists or not, else `false`.