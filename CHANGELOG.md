# Changelog

All notable changes to this mod are documented in this file. Entries follow
[Conventional Commits](https://www.conventionalcommits.org/en/v1.0.0/), and
versions follow [Semantic Versioning](https://semver.org/) with the project
rule from [AGENTS.md](AGENTS.md): breaking changes bump MINOR, never MAJOR.

## [0.2.0] - 2026-11-05

### Features

- fps: add an optional FPS counter in the rain color and character size,
  placed in a chosen corner of the primary monitor's work area

## [0.1.0] - 2026-11-05

### Features

- wallpaper: play a digital rain animation behind the desktop icons, drawn
  with Direct3D 11 into a click-through `WorkerW` child window
- rain: port the original canvas animation to persistent render textures,
  with three layers, trail fading, blur and a blinking overlay
- settings: add rain color, speed, character size, style and font settings
- power: pause while windows cover every monitor, while the session is locked
  and, optionally, on battery power
- recovery: reattach when Explorer recreates the desktop host and recreate
  the renderer after a lost graphics device

### Documentation

- readme: describe the mod and credit third-party authors and licenses
- agents: add commit, versioning, changelog, style and licensing rules
