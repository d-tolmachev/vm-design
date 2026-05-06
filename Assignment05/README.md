\# Lama Graal Truffle interpreter

For NativeImage generation requires [GraalVM](https://github.com/oracle/graal)

\## Build

```shell
$ ./gradlew installDist
$ ./gradlew nativeCompile
```

\## Run

```shell
$ ./build/install/Assignment05/bin/Assignment05 <source\_file>
$ ./build/native/nativeCompile/Assignment05 <source\_file>
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
| Graal Truffle interpreter (GraalVM)     | 1.01 |
| Graal Truffle interpreter (NativeImage)     | 1.41 |
