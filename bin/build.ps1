if ((g++ -dumpmachine).Contains('64')) {
    g++ vimdows.cpp -o vimdows64.dll -shared -lgdi32 -O3 -s -static
} else {
    g++ vimdows.cpp -o vimdows32.dll -shared -lgdi32 -O3 -s -static -std=gnu++0x
}

