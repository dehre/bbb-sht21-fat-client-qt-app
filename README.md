Build:

```sh
mkdir build
cd build
qmake .. CONFIG+=debug # or `CONFIG+=release`
make -j8 TARGET=app    # or `TARGET=someNameForTheExecutable`
```

Run:

```sh
./build/app
```
