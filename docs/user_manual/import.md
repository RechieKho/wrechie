# Importing stuff
Importing makes your project modular. There are two type of import in `wrechie`:

- Relative import
- Package import


### Relative import
Relative import means import another wren script by specifying the path relative to the current wren script.

Here is our example directory:
```
src\
    - main.wren
    - module.wren
```

To import `module.wren` from `main.wren`, in `main.wren`, you just need to type this out:
```wren
import "./module" for Something
```

the path given to the import statment must start with either `./` or `../`. The path given should be in unix-style path, which mean using `/` instead of `\` for directory seperator (even if you are in windows). The extension (`.wren`) is automatically appended.


### Package import
Package import means importing a package.
For now, you can only import built-in packages like `path`.

To import a package, just type out the package's name. For instance, to import `path`:
```wren
import "path" for Path
```

## Write your own build-in package
To write your own build-in package, you'll need to know how to [compile `wrechie` from source](./getting_started.md#compiling-wrechie).

Get into `wrechie` source code. Then, you'll be overwhelmed by lots of folders. But here, we'll be only focusing on the `wrenfiles/` folder. Every single [wren] script in this folder will be packed into `wrechie` as built-in package.

**The rules:**

- The name of the [wren] script will be the package name, so *choose it wisely*.
- *Relative imports are only for people who know what they are doing* as it won't work as intended.

> **What happens to Relative import here?** Well, all the script in `wrenfiles/` will get packed into `wrechie` and they are not living in a directory. When you do relative import in built-in packages, it will instead *import scripts relative to the script that imports the built-in package*

Here we'll write a `Greeter` class that greet us.
```wren
# greeter.wren
class Greeter{
    sayHello(){
        System.print("Hello world")
    }
}
```
Save it in `greeter.wren`. Now, you just need to compile `wrechie` again and it will be packed into `wrechie` automatically. 

To test whether is it working, let's write a test script.
```wren
# test.wren
import "greeter" for Greeter
Greeter.sayHello()
```
Save it in `test.wren`. Now, run the script and it should work.
```sh
$ ./dist/wrechie test.wren
Hello world
```

[wren]: https://wren.io/