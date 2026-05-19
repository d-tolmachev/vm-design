# Lama verifying iterative bytecode interpreter

Requires [lamac](https://github.com/PLTools/Lama/) runtime.

## Build

```shell
$ mkdir build && cd build
$ cmake .. -DCMAKE_BUILD_TYPE=Release
$ cmake --build .
```

## Run

```shell
$ lamac -I <puth_to_lama_runtime> -b <source_file>
$ ./build/Assignment04 <bytecode_file>
```

## Tests

```shell
$ ./run_tests.sh
```

## Performance

| Interpreter                        | Time |
|------------------------------------|------|
| Recursive source-level interpreter | 5.46 |
| Recursive bytecode interpreter     | 1.29 |
| Iterative bytecode interpreter (no verifier)     | 2.39 |
| Iterative bytecode interpreter (with verifier)     | 2.17 |
