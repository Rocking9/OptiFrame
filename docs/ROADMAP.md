# OptiFrame Roadmap

## Milestone 0 — Reliable baseline (current)

- [x] C++17 / CMake project skeleton
- [x] Invoke FFmpeg without passing user-provided paths through a shell
- [x] Configurable H.264 CRF and preset
- [x] Report file-size change and runtime
- [x] Basic argument validation tests
- [ ] Verify on Windows and Linux with real test videos
- [ ] Record baseline results for a small, documented set of clips

## Milestone 1 — Quality measurement

- [ ] Add a test harness for repeatable encodes
- [ ] Calculate SSIM and PSNR using a documented method
- [ ] Record output size, encoding time, and quality metrics in CSV
- [ ] Explain metric limitations; never equate PSNR alone with perceived quality

## Milestone 2 — Baseline comparisons

- [ ] Sweep a limited CRF/preset grid on the same input clips
- [ ] Produce a comparison table and size-quality curve
- [ ] Establish constraints such as a minimum acceptable quality or maximum encode time

## Milestone 3 — Optimization

- [ ] Define the optimization objective and trade-offs explicitly
- [ ] Choose settings from measured results rather than hard-coded claims
- [ ] Evaluate recommendations on clips not used to tune the strategy

## Milestone 4 — Interface and optional intelligence

- [ ] Consider a GUI only after the CLI is stable
- [ ] Evaluate scene/content features if they have a measurable benefit
- [ ] Consider ONNX Runtime only if a trained model solves a clearly defined problem
