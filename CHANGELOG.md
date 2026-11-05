# Changelog

All notable changes to this mod are documented in this file. Entries follow
[Conventional Commits](https://www.conventionalcommits.org/en/v1.0.0/), and
versions follow [Semantic Versioning](https://semver.org/) with the project
rule from [AGENTS.md](AGENTS.md): breaking changes bump MINOR, never MAJOR.

## [0.8.0] - 2026-11-05

### Features

- color: accept several rain colors separated by commas or semicolons, which
  change linearly from one to the next and back to the first
- color: add a color change time setting, 60 seconds by default

## [0.7.0] - 2026-11-05

### Features

- rss: add an RSS feeds setting. Its comma or semicolon separated, optionally
  quoted RSS or Atom links are fetched in the background and refreshed every
  15 minutes, and the columns spell their headlines
- rss: show one error line per unavailable feed at the bottom center, at the
  size of the FPS counter
- atlas: add the feed characters to the glyph atlas, drawn with fallback
  fonts for scripts the chosen font lacks

## [0.6.0] - 2026-11-05

### Features

- direction: add a direction setting, so the characters fly to the top,
  left, right or bottom (original) while staying upright

## [0.5.0] - 2026-11-05

### Features

- fps: show the frame limit in parentheses in the FPS counter

## [0.4.0] - 2026-11-05

### Features

- fps: add a frame limit from 24 FPS up to the highest monitor refresh rate.
  The rain keeps its speed at any limit, and the overlay's blur fades run at
  the limit

## [0.3.0] - 2026-11-05

### Features

- size: set the character size from 1% to 100% of 72 pt for the largest
  characters, with the smaller layers in proportion
- atlas: rasterize the glyph atlas at the drawn size, so large characters
  stay sharp, and store it in a single channel

### BREAKING CHANGES

- size: the `fontScale` setting (50% to 400% of the original) is replaced by
  `fontSize`. A saved `fontScale` value is ignored and the size returns to
  the default 13%, the original size

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
