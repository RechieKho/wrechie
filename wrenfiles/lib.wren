class Lib {
    /* load c shared library into memory.
    [name] - the name of the library
    [dir]  - the directory where the library lives, empty string will search system's library path
    */
    foreign static load(name, dir)

    /* unload shared medule from memory.
    [name] - the name of the library

    NOTE: Dangerous method that causes SEGFAULT
    */
    // foreign static unload(name) 
}