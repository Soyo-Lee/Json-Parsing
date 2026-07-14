# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project overview

JsonParsing is a Windows console application (C++20, Visual Studio) implementing a from-scratch JSON parsing and serialization library — no third-party JSON dependency (e.g. nlohmann/json) is used. The project currently is an empty MSBuild scaffold; source files have not been added yet.

## Build

This is a Visual Studio MSBuild project (no CMake/Makefile). Build via Visual Studio or MSBuild directly:

```
msbuild JsonParsing.slnx /p:Configuration=Debug /p:Platform=x64
msbuild JsonParsing.slnx /p:Configuration=Release /p:Platform=x64
```

Supported platforms: `x64` and `Win32` (x86). Toolset `v145`, language standard `stdcpp20`. `ConfigurationType` is `Application` (console `.exe`), not a static/dynamic library target — the "library" here means an internal JSON module linked into the console app, not a separate `.lib`/`.dll` project.

## Running

In Release, `main()` builds a sample `JsonValue` object tree, saves it to `person.json` (pretty-printed) via `JsonWriter::SaveToFile`, then reparses that file via `JsonParser::ParseFile` and prints selected fields back out — a round-trip demo. In Debug, `main()` is the gtest runner instead (see Tests below).

## Tests

GoogleTest/GoogleMock are pulled in via a vcpkg manifest (`vcpkg.json`, `VcpkgEnableManifest=true` in the vcxproj) — the first Debug build fetches and builds `gtest`/`gmock` automatically (requires network access). `main.cpp` follows the same `#ifdef _DEBUG` split as the sibling `AssemblyCar` project: in Debug, `main()` runs `testing::InitGoogleTest` + `testing::InitGoogleMock` + `RUN_ALL_TESTS()`; in Release, it just prints a placeholder message (the demo app isn't implemented yet). So running `x64/Debug/JsonParsing.exe` *is* the test runner.

Development follows TDD, tracked across `docs/phase1.md` (JsonValue), `docs/phase2.md` (JsonParser), `docs/phase3.md` (JsonWriter + demo) — each phase writes the test file (RED) before the implementation (GREEN).

## Architecture

- `JsonValue.h`/`.cpp` — the value type. Represents null, boolean, number (`double`), string, array, and object via `std::variant<std::monostate, bool, double, std::string, ArrayType, ObjectType>`, where `ArrayType = std::vector<JsonValue>` and `ObjectType = std::vector<std::pair<std::string, JsonValue>>` (insertion order preserved, unlike `std::map`). Type checks (`IsNull`/`IsBoolean`/...), typed accessors (`AsBoolean`/`AsNumber`/`AsString`/`AsArray`/`AsObject`), and `operator[]` for array index / object key access. Accessing the wrong type throws `JsonTypeException`; indexing an object with a missing key throws on the `const` overload but auto-inserts a `Null` entry on the mutable overload (matches `std::map::operator[]`).
- `JsonValueTest.cpp` — gtest cases for construction, type predicates, accessors, indexing, and exception behavior.
- `JsonParser.h`/`.cpp` — recursive-descent parser, JSON text → `JsonValue` tree. `Parse(text)` parses a string; `ParseFile(path)` reads a file then parses it. Handles null/bool/number (incl. negative/decimal/exponent)/string (incl. escapes and `\uXXXX`, BMP only — no surrogate-pair decoding)/array/object, with whitespace skipped between tokens. Any syntax error (empty input, invalid literal, unterminated string, mismatched brackets, trailing garbage, missing file) throws `JsonParseException` with the byte offset where parsing failed.
- `JsonParserTest.cpp` — gtest cases covering each value type, nesting, whitespace tolerance, and the error paths above.
- `JsonWriter.h`/`.cpp` — `JsonValue` tree → JSON text. `Write(value, pretty, indentSize)` serializes to a string (compact by default); `SaveToFile(value, path, pretty, indentSize)` writes it to disk, throwing `JsonWriteException` if the file can't be opened/written. Numbers are formatted via `std::to_chars` (shortest round-trippable representation, so whole numbers print without a trailing `.0`); strings re-escape `"`, `\`, and control characters.
- `JsonWriterTest.cpp` — gtest cases for each value type's output, compact vs. pretty-printed nesting, and round-tripping through `JsonParser` (both in-memory and via a saved file).

Update this section as the parser/writer files are added, following the pattern above (list each file and what it owns).
