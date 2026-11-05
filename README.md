# Digital Rain Wallpaper

A [Windhawk](https://windhawk.net/) mod that plays a digital rain animation as
the Windows desktop background.

Falling Latin and Japanese characters are drawn with Direct3D 11 into a window
behind the desktop icons. Icons, right-click menus and drag and drop keep
working. One animation spans all monitors.

## Features

- Three rain layers with different sizes, speeds, opacities and blur, plus a
  blinking overlay of scattered characters, as in the original web animation.
- Adjustable color, speed and character size, and a simple style with a single
  sharp layer.
- Pauses while windows cover every monitor, while the session is locked and,
  optionally, on battery power.
- Optional FPS counter in any corner of the primary monitor.
- Optional RSS or Atom feeds: the columns spell the feed headlines.
- Recovers when Explorer recreates the desktop or the graphics device is lost.

## Install

Until the mod is in the Windhawk catalog:

1. Open Windhawk and click **Create a new mod**.
2. Replace the template with the contents of
   [digital-rain-wallpaper.wh.cpp](digital-rain-wallpaper.wh.cpp).
3. Click **Compile Mod**, then **Exit editing mode**.

Requires Windows 10 or 11 and a Direct3D 10 capable GPU.

## Settings

| Setting | Default | Description |
| --- | --- | --- |
| Rain color | `#00FF00` | Color of the falling characters, `#RGB` or `#RRGGBB`. |
| Speed | 100% | 25% to 400% of the original speed. |
| Frame limit | 60 FPS | Maximum frames per second, from 24 up to the highest refresh rate of your monitors. `0` uses that refresh rate. The rain keeps its speed at any limit. |
| Character size | 13% | Size of the largest characters, 1% to 100% of 72 pt. Smaller layers keep their proportions. 13% is the original size. The display DPI scales it further. |
| RSS feeds | Empty | RSS or Atom feed links, separated by commas or semicolons and optionally in quotes. Each column spells a headline from the feeds, then the next one. Feeds refresh every 15 minutes. A feed that can't be read shows an error at the bottom center, at the size of the FPS counter. Empty plays the original characters. |
| Direction | Bottom | The characters fly to the top, left, right or bottom (original). They stay upright. |
| Style | Layered | `Layered` is the original look. `Simple` keeps one sharp layer. |
| Font | MS Gothic | Font for the characters. Characters it can't draw are skipped. |
| Pause when the desktop is covered | On | Pause while maximized or fullscreen windows cover every monitor. |
| Pause on battery power | Off | Pause while the computer runs on battery. |
| Show FPS counter | Off | Show the frames presented per second and, in parentheses, the frame limit, in the rain color and the size of the front rain layer. |
| FPS counter position | Top right | Top left, top right, bottom left or bottom right of the primary monitor's work area, clear of the taskbar. |

## How it works

- **Desktop window.** The mod runs inside `explorer.exe`. It creates a
  click-through child window in the `WorkerW` window behind the desktop icons.
  This works with both the classic layout and the Windows 11 24H2 layout.
- **Rendering.** Each canvas of the original animation becomes a persistent
  render texture. Each frame, the texture is darkened by a translucent black
  quad, which leaves trails, and new characters are drawn from a glyph atlas
  rasterized with GDI. The layers are then composited with their opacity, and
  the blurred ones go through a half-resolution Gaussian blur.
- **Timing.** A high-resolution waitable timer drives every layer at its own
  rate (30, 30 and 24 FPS), and a frame is presented only when something
  changed.

## Credits

This mod uses third-party code under these licenses:

- **[Matrix Falling Code Effect](https://github.com/andresz74/matrix)** by
  [Andres Zenteno](https://github.com/andresz74), MIT License. The rain and
  overlay animation, the character set and the layer parameters are translated
  from its JavaScript to C++ and HLSL. The copyright and permission notice are
  kept in the mod source:

  ```text
  Copyright (c) 2024 Andres Zenteno

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
  ```

- **[Live Video Wallpaper](https://github.com/sysakshay/wh-live-wallpaper)**
  by [AKS HAY](https://github.com/sysakshay), GPL-3.0. Our code adapts its
  desktop host discovery (`WorkerW` lookup for the classic and 24H2 layouts),
  its click-through window styles and its render thread layout. Modified on
  2026-11-05: rewritten around a Direct3D 11 animation.

## License

Because it includes GPL-3.0 code, the whole mod is licensed under the
[GNU General Public License v3.0](LICENSE). The MIT-licensed parts remain
available under the MIT License, as stated above.
