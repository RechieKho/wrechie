class Path {
    foreign static sep
    foreign static dirname(path)
    foreign static filename(path)
    foreign static filebase(path)
    foreign static splitext(path)
    foreign static ext(path)
    foreign static split(path)
    foreign static join(list)
    foreign static select(path, begin, end)
    static select(path, begin){ Path.select(path, begin, 0) }
    foreign static normpath(path)
    foreign static commonprefix(list)
    foreign static commondirname(list)
    foreign static exists(path)
    foreign static curdir
}

