\# Lama verifying iterative bytecode interpreter

Requires \[lamac](https://github.com/PLTools/Lama/) runtime.

\## Build

```shell
$ ./gradlew installDist
```

\## Run

```shell
$ ./build/install/Lama/dist/Lama <source\_file>
```

\## Tests

```shell
$ ./run\_tests.sh
```

\## Performance

| Interpreter                        | Time |
|------------------------------------|------|
| Recursive source-level interpreter | 5.46 |
| Recursive bytecode interpreter     | 1.29 |
| Iterative bytecode interpreter (no verifier)     | 2.39 |
| Iterative bytecode interpreter (with verifier)     | 2.17 |
| Graal Truffle interpreter     | 3.32 |
