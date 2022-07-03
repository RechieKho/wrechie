# Getting started

## Compiling `wrechie`
To get the source of `wrechie`,
```sh
git clone https://github.com/RechieKho/wrechie.git
```
And the source will be in the folder `wrechie/` located in where you execute the `git` command.

Then, compile it.
```sh
cd wrechie/
scons
```
And you should have `wrechie` living inside of `dist/` folder.


## Your first wren script
Here is a very basic hello world in [wren].
```wren
# hello_world.wren
System.print("Hello world!")
```
Save it in `hello_world.wren` and run the script by calling `wrechie hello_world.wren`. 

> **NOTE**: If you haven't put `wrechie` onto your `PATH` environment variable, then you should give the full path of `wrechie` to the terminal, such as `path/to/wrechie hello_world.wren`

And you run your first script! Yay! It is *advisable* to read about the [wren] scripting language before going any futher as we won't be discussing about about to code in [wren].

[wren]: https://wren.io/