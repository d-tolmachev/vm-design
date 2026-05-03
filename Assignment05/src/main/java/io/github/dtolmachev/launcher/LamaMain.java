package io.github.dtolmachev.launcher;

import org.graalvm.polyglot.Context;
import org.graalvm.polyglot.PolyglotException;
import org.graalvm.polyglot.Source;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.PrintStream;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.HashMap;
import java.util.Map;
import java.util.Objects;

public final class LamaMain {
    private static final String LAMA = "lama";

    public static void main(String[] args) throws IOException {
        Map<String, String> options = new HashMap<>();
        Path file = null;
        boolean launcherOutput = true;
        for (String arg : args) {
            if (launcherOutput && arg.equals("--")) {
                launcherOutput = false;
            } else if (launcherOutput && arg.startsWith("--")) {
                int index = arg.indexOf("=");
                String key = index != -1 ? arg.substring(2, index) : arg.substring(2);
                String value = index != -1 ? arg.substring(index + 1) : "true";
                options.put(key, value);
            } else if (Objects.isNull(file)) {
                file = Path.of(arg);
            } else {
                throw new IllegalArgumentException("Too many arguments");
            }
        }
        InputStream in = Objects.nonNull(file) ? Files.newInputStream(file) : System.in;
        String name = Objects.nonNull(file) ? file.getFileName().toString() : "<stdin>";
        Source source = Source.newBuilder(LAMA, new BufferedReader(new InputStreamReader(in)), name).build();
        if (Objects.nonNull(file)) {
            in.close();
        }
        System.exit(executeSource(source, System.in, System.out, options));
    }

    @SuppressWarnings({"CallToPrintStackTrace", "SameParameterValue"})
    private static int executeSource(Source source, InputStream in, PrintStream out, Map<String, String> options) {
        try (Context context = Context.newBuilder(LAMA).in(in).out(out).options(options).allowAllAccess(true).build()) {
            context.eval(source);
            return 0;
        } catch (PolyglotException e) {
            if (e.isInternalError()) {
                e.printStackTrace();
            } else {
                System.err.println(e.getMessage());
            }
            return 1;
        }
    }
}
