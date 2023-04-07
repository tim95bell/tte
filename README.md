
# TTE

Text Editor

## Dependencies

- CMake >= 3.15

## Setup

### Recursively clone submodules

```Shell
git submodule update --init --recursive
```

### Add Fonts

Put ttf fonts in `resources/fonts`

### Build SDL2

```Shell
./scripts/build_sdl2.sh
```

### Create Build

- MacOS might add `-GXcode`
```Shell
cmake -Bbuild/tte
```

## Build

```Shell
cmake --build build/tte
```

## Run

```Shell
./build/tte/modules/ui/Debug/tte_ui
```

## Run Unit Tests

```Shell
./build/tte/modules/engine/test/Debug/tte_engine_tests
```
