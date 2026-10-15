# OptiFrame

**OptiFrame** is a C++17 project exploring measurable video compression: reduce a video's file size while building toward quality-aware optimization.

This repository starts with a deliberately small baseline. Version `0.1.0` delegates encoding to FFmpeg, exposes a few reproducible settings, and reports file-size change and elapsed time. It **does not yet automatically choose optimal settings or measure perceptual quality**. Those are planned next steps.

## Current features

- C++17 command-line application.
- H.264 video encoding with FFmpeg's `libx264` encoder.
- Configurable CRF, encoding preset, and AAC audio bitrate.
- Reports source size, output size, size reduction/increase, and elapsed time.
- Does not overwrite an existing output file.
- Basic argument-parser tests with CTest.

## Requirements

- A C++17 compiler (MSVC, GCC, or Clang).
- CMake 3.16 or newer.
- FFmpeg installed and available as `ffmpeg` on your `PATH`, with `libx264` enabled.

Check the FFmpeg setup with:

```bash
ffmpeg -hide_banner -encoders
```

The list should include `libx264`.

## Build

Run these commands from the repository root:

```bash
cmake -S . -B build
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```

On a single-configuration generator such as the usual Makefiles or Ninja, `-C Release` in the test command can be omitted.

## Run

Windows with a Visual Studio CMake generator:

```powershell
.\build\Release\optiframe.exe --help
.\build\Release\optiframe.exe compress --input "sample.mp4" --output "sample_compressed.mp4" --crf 23 --preset medium
```

For single-configuration builds, the executable is usually `build/optiframe` (or `build/optiframe.exe` on Windows).

Example with a faster preset:

```bash
optiframe compress --input "sample.mov" --output "sample_compressed.mp4" --crf 26 --preset fast
```

Use a new output filename for each run; OptiFrame intentionally refuses to overwrite an existing output. Lower CRF values generally preserve more detail and produce larger files. Faster presets trade encoding efficiency for speed. Results depend heavily on the source video, so compare several settings on the same short test clip before drawing conclusions.

## CLI options

| Option | Default | Description |
|---|---:|---|
| `--input` | required | Source video path |
| `--output` | required | Output path; must end in `.mp4` and must not already exist |
| `--crf` | `23` | H.264 quality setting from 0 to 51 |
| `--preset` | `medium` | `ultrafast`, `superfast`, `veryfast`, `faster`, `fast`, `medium`, `slow`, `slower`, or `veryslow` |
| `--audio-bitrate-kbps` | `128` | AAC audio bitrate from 32 to 512 kbps |

## Roadmap

1. Establish reproducible test clips and baseline measurements.
2. Add objective quality metrics such as SSIM and PSNR; document their limitations.
3. Compare CRF/preset choices against size, quality, and elapsed time.
4. Build an optimization strategy based on measured trade-offs.
5. Consider a GUI after the core workflow and measurements are reliable.

AI/ONNX-based recommendations are a possible later experiment, not a requirement for the first working release.

## Project structure

See [`docs/ROADMAP.md`](docs/ROADMAP.md) for planned milestones.
