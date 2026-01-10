\# Lama verifying iterative bytecode interpreter

Requires \[lamac](https://github.com/PLTools/Lama/) runtime.

\## Build

```shell
$ mkdir build \&\& cd build
$ cmake .. -DCMAKE\_BUILD\_TYPE=Release
$ cmake --build .
```

\## Run

```shell
$ lamac -I <puth\_to\_lama\_runtime> -b <source\_file>
$ ./build/Assignment04 <bytecode\_file>
```

\## Tests

```shell
$ ./run\_tests.sh
```

\## Performance

| Interpreter                        | Time |
|------------------------------------|------|
| Recursive source-level interpreter | 5.41 |
| Recursive bytecode interpreter     | 1.29 |
| Iterative bytecode interpreter (no verifier)     | 2.39 |
| Iterative bytecode interpreter (with verifier)     | 2.17 |
