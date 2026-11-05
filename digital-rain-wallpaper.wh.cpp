// ==WindhawkMod==
// @id              digital-rain-wallpaper
// @name            Digital Rain Wallpaper
// @description     Plays a digital rain animation as the desktop background, rendered with Direct3D 11 behind the desktop icons
// @version         0.8.0
// @author          radiocity
// @github          https://github.com/radiocity
// @license         GPL-3.0
// @include         explorer.exe
// @compilerOptions -ld3d11 -ld3dcompiler_47 -ldxgi -ldwmapi -lgdi32 -lwtsapi32 -lole32 -loleaut32 -lmsxml6
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Digital Rain Wallpaper

Plays a digital rain animation of falling Latin and Japanese characters as
your desktop background.

The animation is drawn with Direct3D 11 into a window behind the desktop
icons, so icons, right-click menus and drag and drop keep working. One
animation spans all monitors.

## Features

- Three rain layers with different sizes, speeds, opacities and blur, plus a
  blinking overlay of scattered characters.
- Adjustable color, speed and character size.
- Simple style with a single sharp rain layer.
- Pauses while windows cover every monitor, while the session is locked and,
  optionally, on battery power.
- Optional FPS counter in any corner of the primary monitor.
- Optional RSS or Atom feeds: the columns spell the feed headlines.

## Settings

- **Rain color:** `#RGB` or `#RRGGBB`, green by default. Several colors
  separated by commas or semicolons, like `#00FF00, #00CCFF; #FF2266`, change
  linearly from one to the next and back to the first.
- **Color change time:** seconds each change from one color to the next
  takes, 60 by default.
- **Speed:** 25% to 400% of the original speed.
- **Character size:** size of the largest characters, from 1% to 100% of
  72 pt. Smaller layers keep their proportions. 13% is the original size. The
  display DPI scales it further.
- **Frame limit:** maximum frames per second, from 24 up to the highest
  refresh rate of your monitors, which the animation spans. 0 uses that
  refresh rate. The rain keeps its speed at any limit. Higher limits make the
  overlay's blur fades smoother.
- **Direction:** the characters fly to the top, left, right or bottom
  (original). The characters stay upright.
- **Style:** layered (original look) or simple.
- **Font:** font used for the characters. Characters the font can't draw are
  skipped. The default, MS Gothic, covers the whole set.
- **Show FPS counter:** shows the frames presented per second and, in
  parentheses, the frame limit, in the rain color and the size of the front
  rain layer.
- **FPS counter position:** top left, top right, bottom left or bottom right
  of the primary monitor's work area.
- **RSS feeds:** RSS or Atom feed links, separated by commas or semicolons
  and optionally in quotes, for example
  `https://www.alarabiya.net/feed/rss2/ar.xml; "https://feeds.feedburner.com/TheHackersNews"`.
  Each column spells a headline from the feeds, then the next one. Feeds
  refresh every 15 minutes. A feed that can't be read shows an error at the
  bottom center, at the size of the FPS counter. Empty plays the original
  characters.

## Credits

- The rain animation is a C++ and HLSL translation of
  [Matrix Falling Code Effect](https://github.com/andresz74/matrix) by Andres
  Zenteno, MIT License.
- The desktop window setup is adapted from
  [Live Video Wallpaper](https://github.com/sysakshay/wh-live-wallpaper) by
  AKS HAY, GPL-3.0.

This mod is licensed under GPL-3.0.
*/
// ==/WindhawkModReadme==

// clang-format off
// ==WindhawkModSettings==
/*
- color: "#00FF00"
  $name: Rain color
  $description: Color of the falling characters, as #RGB or #RRGGBB. Several colors separated by commas or semicolons change linearly from one to the next, for example #00FF00, #00CCFF; #FF2266.
- colorSeconds: 60
  $name: Color change time (seconds)
  $description: Seconds each change from one rain color to the next takes, from 1 to 3600. Used with several colors.
- speed: 100
  $name: Speed (%)
  $description: Animation speed relative to the original, from 25 to 400.
- fontSize: 13
  $name: Character size (%)
  $description: Size of the largest characters, from 1 to 100 percent of 72 pt. Smaller layers keep their proportions. 13 is the original size. The display DPI scales it further.
- frameLimit: 60
  $name: Frame limit (FPS)
  $description: Maximum frames per second, from 24 up to the highest refresh rate of your monitors. 0 uses that refresh rate. The rain keeps its speed at any limit.
- rssFeeds: ""
  $name: RSS feeds
  $description: RSS or Atom feed links, separated by commas or semicolons, optionally in quotes. The columns spell the feed headlines. Empty plays the original characters.
- direction: bottom
  $name: Direction
  $description: Direction the characters fly in. The characters stay upright.
  $options:
  - top: Top
  - left: Left
  - right: Right
  - bottom: Bottom (original)
- style: layered
  $name: Style
  $options:
  - layered: Layered (three rain layers, blur and a blinking overlay)
  - simple: Simple (one sharp rain layer)
- fontName: MS Gothic
  $name: Font
  $description: Font for the characters. Characters the font can't draw are skipped.
- pauseWhenCovered: true
  $name: Pause when the desktop is covered
  $description: Pause while maximized or fullscreen windows cover every monitor.
- pauseOnBattery: false
  $name: Pause on battery power
- showFps: false
  $name: Show FPS counter
  $description: Shows the frames presented per second and, in parentheses, the frame limit, in the rain color and character size.
- fpsPosition: topRight
  $name: FPS counter position
  $description: Corner of the primary monitor's work area, so the taskbar doesn't hide the counter.
  $options:
  - topLeft: Top left
  - topRight: Top right
  - bottomLeft: Bottom left
  - bottomRight: Bottom right
*/
// ==/WindhawkModSettings==
// clang-format on

// Digital Rain Wallpaper
// Copyright (C) 2026 radiocity
//
// This program is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation, either version 3 of the License, or (at your option)
// any later version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
// more details: <https://www.gnu.org/licenses/>.
//
// Third-party code:
//
// - The desktop host discovery, the window styles and the thread layout are
//   adapted from Live Video Wallpaper by AKS HAY
//   (https://github.com/sysakshay/wh-live-wallpaper), licensed under GPL-3.0.
//   Modified on 2026-11-05: rewritten around a Direct3D 11 animation.
//
// - The rain animation, its character set and its layer parameters are
//   translated from JavaScript to C++ and HLSL from Matrix Falling Code
//   Effect by Andres Zenteno (https://github.com/andresz74/matrix), licensed
//   under the MIT License:
//
//   Copyright (c) 2024 Andres Zenteno
//
//   Permission is hereby granted, free of charge, to any person obtaining a
//   copy of this software and associated documentation files (the
//   "Software"), to deal in the Software without restriction, including
//   without limitation the rights to use, copy, modify, merge, publish,
//   distribute, sublicense, and/or sell copies of the Software, and to permit
//   persons to whom the Software is furnished to do so, subject to the
//   following conditions:
//
//   The above copyright notice and this permission notice shall be included
//   in all copies or substantial portions of the Software.
//
//   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
//   OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
//   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN
//   NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
//   DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
//   OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
//   USE OR OTHER DEALINGS IN THE SOFTWARE.

#include <windows.h>

#include <d3d11.h>
#include <d3dcompiler.h>
#include <dwmapi.h>
#include <dxgi1_2.h>
#include <wrl/client.h>
#include <wtsapi32.h>
#include <ole2.h>
#include <msxml6.h>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <cwchar>
#include <iterator>
#include <memory>
#include <mutex>
#include <optional>
#include <unordered_map>
#include <random>
#include <string>
#include <vector>

using Microsoft::WRL::ComPtr;

namespace {

// Character set of the original animation.
constexpr wchar_t kLatinCharacters[] =
    L"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890"
    L"!@#$%^&*()-_=+[]{}|;:',.<>?/`~"
    L"¡™£¢∞§¶•ªº–≠œ∑´®†¥¨ˆøπ“‘åß∂ƒ©˙∆˚¬Ω≈ç√∫˜µ≤≥÷";
constexpr wchar_t kJapaneseCharacters[] =
    L"アイウエオカキクケコサシスセソタチツテト"
    L"ナニヌネノハヒフヘホマミムメモヤユヨラリ"
    L"ルレロワヲンガギグゲゴザジズゼゾダヂヅデ"
    L"ドバビブベボパピプペポ"
    L"あいうえおかきくけこさしすせそたちつてと"
    L"なにぬねのはひふへほまみむめもやゆよらり"
    L"るれろわをん一二三四五六七八九十零";

// Parameters of one rain canvas in the original animation.
struct RainLayerConfig {
  float speed_factor;
  float font_size;  // CSS pixels.
  float delay_factor;
  float fps;
  float opacity;  // CSS opacity of the canvas.
  bool blurred;   // CSS blur() on the canvas.
};

constexpr RainLayerConfig kRainLayers[] = {
    {0.9f, 8.0f, 1.0f, 30.0f, 1.0f, false},
    {0.6f, 12.0f, 6.0f, 30.0f, 0.4f, false},
    {0.8f, 12.0f, 4.0f, 24.0f, 0.4f, true},
};

constexpr float kTrailFadeAlpha = 0.05f;
constexpr float kDropResetChance = 0.025f;
constexpr float kOverlayFontSize = 12.0f;
constexpr int kOverlayGlyphCount = 10;
constexpr double kOverlayBlinkMs = 400.0;
constexpr float kOverlayOpacity = 0.4f;
constexpr float kOverlayGlyphAlpha = 0.8f;
constexpr double kOverlayBlurToggleMs = 3000.0;
constexpr double kBlurTransitionMs = 500.0;
constexpr float kBlurRadius = 4.0f;  // CSS pixels.
// Keeps 3 sigma within the 12-tap kernel radius of PsBlur.
constexpr float kMaxBlurSigma = 4.0f;

// The largest characters at 100% of the size setting: 72 pt is 96 CSS px.
constexpr float kMaxFontSize = 96.0f;
constexpr float kLargestFontSize = [] {
  float largest = kOverlayFontSize;
  for (const RainLayerConfig& layer : kRainLayers) {
    largest = std::max(largest, layer.font_size);
  }
  return largest;
}();

// The atlas em follows the largest characters, so big sizes stay sharp.
constexpr int kMinAtlasEm = 48;
constexpr int kMaxAtlasEm = 240;
constexpr int kMaxAtlasWidth = 4096;
constexpr int kMaxAtlasHeight = 8192;

// Fonts for feed characters the chosen font lacks, tried in order: Latin,
// Cyrillic, Greek, Arabic and Hebrew; Korean; Japanese; Chinese; Indic;
// Thai; symbols.
constexpr const wchar_t* kFallbackFonts[] = {
    L"Segoe UI",    L"Malgun Gothic", L"Yu Gothic UI",   L"Microsoft YaHei UI",
    L"Nirmala UI",  L"Leelawadee UI", L"Segoe UI Symbol"};
constexpr float kAtlasMaxLod = 3.0f;

constexpr wchar_t kDefaultFontName[] = L"MS Gothic";
constexpr wchar_t kWindowClassName[] = L"DigitalRainWallpaper";
constexpr UINT kMsgReloadSettings = WM_APP + 1;
constexpr UINT kMsgFeedsUpdated = WM_APP + 2;

constexpr DWORD kFeedRefreshMs = 15 * 60 * 1000;
constexpr DWORD kFeedRetryMs = 60 * 1000;
constexpr ULONGLONG kFeedTimeoutMs = 20 * 1000;
constexpr size_t kMaxTitlesPerFeed = 50;
constexpr size_t kMaxTitleLength = 200;
constexpr size_t kMaxFeedErrorLines = 3;
// Empty cells between two headlines in a column.
constexpr int kTitleGap = 3;
constexpr wchar_t kFeedUserAgent[] =
    L"Mozilla/5.0 (Windows NT 10.0; Win64; x64) DigitalRainWallpaper";
constexpr UINT_PTR kHostTimerId = 1;
constexpr UINT_PTR kPauseTimerId = 2;
constexpr UINT kHostCheckIntervalMs = 1000;
constexpr UINT kPauseCheckIntervalMs = 500;
constexpr DWORD kHostWaitIntervalMs = 1000;
constexpr DWORD kRetryDelayMs = 5000;
constexpr double kMaxIdleMs = 1000.0;
constexpr int kMinFrameLimit = 24;
constexpr int kDefaultRefreshRate = 60;
// Steps a layer may catch up in one frame before it skips ahead instead.
constexpr int kMaxCatchUpSteps = 4;
// Steps due within this time run now, to merge nearly coinciding frames.
constexpr double kEarlyStepMs = 1.0;
constexpr double kFpsWindowMs = 1000.0;
constexpr float kFpsBackdropAlpha = 0.6f;

// Undocumented Progman message that creates the WorkerW window behind the
// desktop icons.
constexpr UINT kSpawnWorkerWMessage = 0x052C;

constexpr char kShaderSource[] = R"hlsl(
cbuffer Constants : register(b0) {
  float2 viewport_size;
  float2 blur_step;
  float opacity;
  float blur_sigma;
  float2 padding;
};

Texture2D source_texture : register(t0);
SamplerState linear_sampler : register(s0);

struct GlyphInput {
  float4 rect : RECT;
  float4 uv_rect : UVRECT;
  float4 color : COLOR;
  uint vertex_id : SV_VertexID;
};

struct GlyphVaryings {
  float4 position : SV_Position;
  float2 uv : TEXCOORD0;
  float4 color : COLOR;
};

GlyphVaryings VsGlyph(GlyphInput input) {
  float2 corner = float2(input.vertex_id & 1, input.vertex_id >> 1);
  float2 pixel = input.rect.xy + corner * input.rect.zw;
  GlyphVaryings result;
  result.position =
      float4(pixel / viewport_size * float2(2, -2) + float2(-1, 1), 0, 1);
  result.uv = input.uv_rect.xy + corner * input.uv_rect.zw;
  result.color = input.color;
  return result;
}

float4 PsGlyph(GlyphVaryings input) : SV_Target {
  float coverage = source_texture.Sample(linear_sampler, input.uv).r;
  return float4(input.color.rgb, input.color.a * coverage);
}

struct FullscreenVaryings {
  float4 position : SV_Position;
  float2 uv : TEXCOORD0;
};

FullscreenVaryings VsFullscreen(uint vertex_id : SV_VertexID) {
  FullscreenVaryings result;
  result.uv = float2((vertex_id << 1) & 2, vertex_id & 2);
  result.position = float4(result.uv * float2(2, -2) + float2(-1, 1), 0, 1);
  return result;
}

float4 PsComposite(FullscreenVaryings input) : SV_Target {
  return source_texture.Sample(linear_sampler, input.uv) * opacity;
}

float4 PsBlur(FullscreenVaryings input) : SV_Target {
  float4 sum = 0;
  float weight_sum = 0;
  [unroll] for (int i = -12; i <= 12; ++i) {
    float weight = exp(-(i * i) / (2 * blur_sigma * blur_sigma));
    sum += source_texture.Sample(linear_sampler, input.uv + blur_step * i) *
           weight;
    weight_sum += weight;
  }
  return sum / weight_sum;
}
)hlsl";

HMODULE module_instance = nullptr;
HANDLE stop_event = nullptr;
HANDLE queue_ready_event = nullptr;
HANDLE render_thread = nullptr;
DWORD render_thread_id = 0;

struct Color {
  float r, g, b, a;
};

// One textured quad, drawn as an instance of a 4-vertex triangle strip.
struct GlyphInstance {
  float x, y, width, height;
  float u, v, uv_width, uv_height;
  Color color;
};

struct alignas(16) ShaderConstants {
  float viewport_size[2];
  float blur_step[2];
  float opacity;
  float blur_sigma;
  float padding[2];
};

struct RenderTexture {
  ComPtr<ID3D11RenderTargetView> target;
  ComPtr<ID3D11ShaderResourceView> view;
  int width = 0;
  int height = 0;
};

enum class FpsPosition { kTopLeft, kTopRight, kBottomLeft, kBottomRight };

// Where the characters fly to.
enum class Direction { kTop, kLeft, kRight, kBottom };

struct Settings {
  // One color, or several the rain moves through.
  std::vector<Color> colors = {{0.0f, 1.0f, 0.0f, 1.0f}};
  double color_transition_ms = 60000.0;
  float speed = 1.0f;
  // Requested limit; 0 means the highest monitor refresh rate.
  int frame_limit = kDefaultRefreshRate;
  float font_scale = 1.0f;
  bool layered = true;
  std::wstring font_name = kDefaultFontName;
  bool pause_when_covered = true;
  bool pause_on_battery = false;
  std::wstring rss_feeds;
  bool show_fps = false;
  FpsPosition fps_position = FpsPosition::kTopRight;
  Direction direction = Direction::kBottom;
};

// Headlines read from the feeds, and one message per feed that failed.
struct FeedContent {
  std::vector<std::wstring> titles;
  std::vector<std::wstring> errors;
};

enum class FrameResult { kIdle, kPresented, kDeviceLost };

enum class RunResult { kStopped, kHostLost };

bool Failed(HRESULT result, const wchar_t* operation) {
  if (FAILED(result)) {
    Wh_Log(L"%s failed: 0x%08lX", operation,
           static_cast<unsigned long>(result));
    return true;
  }
  return false;
}

double NowMs() {
  static const double frequency = [] {
    LARGE_INTEGER value;
    QueryPerformanceFrequency(&value);
    return static_cast<double>(value.QuadPart);
  }();
  LARGE_INTEGER counter;
  QueryPerformanceCounter(&counter);
  return static_cast<double>(counter.QuadPart) * 1000.0 / frequency;
}

int HexDigitValue(wchar_t c) {
  if (c >= L'0' && c <= L'9') return c - L'0';
  if (c >= L'a' && c <= L'f') return c - L'a' + 10;
  if (c >= L'A' && c <= L'F') return c - L'A' + 10;
  return -1;
}

// Parses "#RGB" or "#RRGGBB". The "#" is optional.
std::optional<Color> ParseHexColor(const wchar_t* text) {
  while (*text == L' ') ++text;
  if (*text == L'#') ++text;
  size_t length = wcslen(text);
  while (length > 0 && text[length - 1] == L' ') --length;
  if (length != 3 && length != 6) return std::nullopt;

  int digits[6];
  for (size_t i = 0; i < length; ++i) {
    digits[i] = HexDigitValue(text[i]);
    if (digits[i] < 0) return std::nullopt;
  }
  float channels[3];
  for (int i = 0; i < 3; ++i) {
    int value = length == 3 ? digits[i] * 17
                            : digits[i * 2] * 16 + digits[i * 2 + 1];
    channels[i] = value / 255.0f;
  }
  return Color{channels[0], channels[1], channels[2], 1.0f};
}

// Parses colors separated by commas or semicolons. Invalid ones are skipped.
std::vector<Color> ParseHexColors(const std::wstring& text) {
  std::vector<Color> colors;
  size_t begin = 0;
  while (begin <= text.size()) {
    size_t end = text.find_first_of(L",;", begin);
    if (end == std::wstring::npos) end = text.size();
    const std::wstring entry = text.substr(begin, end - begin);
    if (entry.find_first_not_of(L" \t") != std::wstring::npos) {
      if (std::optional<Color> color = ParseHexColor(entry.c_str())) {
        colors.push_back(*color);
      } else {
        Wh_Log(L"Invalid color \"%s\" skipped", entry.c_str());
      }
    }
    begin = end + 1;
  }
  return colors;
}

// Moves linearly from each color to the next, transition_ms per change, and
// from the last back to the first.
Color CycleColor(const std::vector<Color>& colors, double elapsed_ms,
                 double transition_ms) {
  if (colors.size() == 1) return colors.front();
  const double position =
      std::fmod(std::max(0.0, elapsed_ms) / transition_ms,
                static_cast<double>(colors.size()));
  const size_t index = static_cast<size_t>(position) % colors.size();
  const float t = static_cast<float>(position - std::floor(position));
  const Color& from = colors[index];
  const Color& to = colors[(index + 1) % colors.size()];
  return {from.r + (to.r - from.r) * t, from.g + (to.g - from.g) * t,
          from.b + (to.b - from.b) * t, 1.0f};
}

Settings LoadSettings() {
  Settings settings;

  PCWSTR color = Wh_GetStringSetting(L"color");
  std::vector<Color> colors = ParseHexColors(color);
  if (!colors.empty()) {
    settings.colors = std::move(colors);
  } else {
    Wh_Log(L"No valid color in \"%s\", using the default", color);
  }
  Wh_FreeStringSetting(color);
  settings.color_transition_ms =
      std::clamp(Wh_GetIntSetting(L"colorSeconds"), 1, 3600) * 1000.0;

  settings.speed = std::clamp(Wh_GetIntSetting(L"speed"), 25, 400) / 100.0f;
  settings.frame_limit = std::max(0, Wh_GetIntSetting(L"frameLimit"));

  // Percent of kMaxFontSize for the largest characters.
  const int font_size = std::clamp(Wh_GetIntSetting(L"fontSize"), 1, 100);
  settings.font_scale = font_size / 100.0f * kMaxFontSize / kLargestFontSize;

  PCWSTR style = Wh_GetStringSetting(L"style");
  settings.layered = wcscmp(style, L"simple") != 0;
  Wh_FreeStringSetting(style);

  PCWSTR font_name = Wh_GetStringSetting(L"fontName");
  if (*font_name) {
    settings.font_name.assign(
        font_name, std::min(wcslen(font_name), size_t{LF_FACESIZE - 1}));
  }
  Wh_FreeStringSetting(font_name);

  settings.pause_when_covered = Wh_GetIntSetting(L"pauseWhenCovered") != 0;
  settings.pause_on_battery = Wh_GetIntSetting(L"pauseOnBattery") != 0;

  PCWSTR rss_feeds = Wh_GetStringSetting(L"rssFeeds");
  settings.rss_feeds = rss_feeds;
  Wh_FreeStringSetting(rss_feeds);
  settings.show_fps = Wh_GetIntSetting(L"showFps") != 0;

  PCWSTR fps_position = Wh_GetStringSetting(L"fpsPosition");
  if (wcscmp(fps_position, L"topLeft") == 0) {
    settings.fps_position = FpsPosition::kTopLeft;
  } else if (wcscmp(fps_position, L"bottomLeft") == 0) {
    settings.fps_position = FpsPosition::kBottomLeft;
  } else if (wcscmp(fps_position, L"bottomRight") == 0) {
    settings.fps_position = FpsPosition::kBottomRight;
  }
  Wh_FreeStringSetting(fps_position);

  PCWSTR direction = Wh_GetStringSetting(L"direction");
  if (wcscmp(direction, L"top") == 0) {
    settings.direction = Direction::kTop;
  } else if (wcscmp(direction, L"left") == 0) {
    settings.direction = Direction::kLeft;
  } else if (wcscmp(direction, L"right") == 0) {
    settings.direction = Direction::kRight;
  }
  Wh_FreeStringSetting(direction);
  return settings;
}

bool CreateRenderTexture(ID3D11Device* device, int width, int height,
                         RenderTexture* texture) {
  D3D11_TEXTURE2D_DESC desc = {};
  desc.Width = width;
  desc.Height = height;
  desc.MipLevels = 1;
  desc.ArraySize = 1;
  desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
  desc.SampleDesc.Count = 1;
  desc.Usage = D3D11_USAGE_DEFAULT;
  desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

  ComPtr<ID3D11Texture2D> resource;
  *texture = {};
  if (Failed(device->CreateTexture2D(&desc, nullptr, &resource),
             L"CreateTexture2D") ||
      Failed(device->CreateRenderTargetView(resource.Get(), nullptr,
                                            &texture->target),
             L"CreateRenderTargetView") ||
      Failed(device->CreateShaderResourceView(resource.Get(), nullptr,
                                              &texture->view),
             L"CreateShaderResourceView")) {
    *texture = {};
    return false;
  }
  texture->width = width;
  texture->height = height;
  return true;
}

ComPtr<ID3DBlob> CompileShader(const char* entry_point, const char* target) {
  ComPtr<ID3DBlob> code;
  ComPtr<ID3DBlob> errors;
  HRESULT result = D3DCompile(kShaderSource, sizeof(kShaderSource) - 1,
                              "digital-rain", nullptr, nullptr, entry_point,
                              target, D3DCOMPILE_OPTIMIZATION_LEVEL3, 0,
                              &code, &errors);
  if (FAILED(result)) {
    Wh_Log(L"Compiling %S failed: 0x%08lX %S", entry_point,
           static_cast<unsigned long>(result),
           errors ? static_cast<const char*>(errors->GetBufferPointer())
                  : "");
    return nullptr;
  }
  return code;
}

HFONT CreateAtlasFont(const wchar_t* name, int em, DWORD pitch_and_family) {
  return CreateFontW(-em, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                     DEFAULT_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS,
                     ANTIALIASED_QUALITY, pitch_and_family, name);
}

// Texture with every character rasterized by GDI, one cell per character.
// Cell 0 is solid white, for drawing filled rectangles.
class GlyphAtlas {
 public:
  // Rasterizes the original characters and extra_characters with an em of
  // `em` pixels, or smaller when they don't fit in one texture.
  // requested_em is for internal use.
  bool Build(ID3D11Device* device, ID3D11DeviceContext* context,
             const std::wstring& font_name, int em,
             const std::wstring& extra_characters, int requested_em = 0);

  int glyph_count() const { return glyph_count_; }
  // The original characters are the first glyphs.
  int base_glyph_count() const { return base_glyph_count_; }
  int requested_em() const { return requested_em_; }
  const std::wstring& font_name() const { return font_name_; }
  const std::wstring& extra_characters() const { return extra_characters_; }
  // Characters of the glyphs, in glyph order.
  const std::wstring& characters() const { return characters_; }
  ID3D11ShaderResourceView* view() const { return view_.Get(); }

  float AdvancePx(int glyph, float font_px) const {
    return advances_[glyph] * font_px / em_;
  }
  float AscentPx(float font_px) const { return ascent_ * font_px / em_; }
  float DescentPx(float font_px) const { return descent_ * font_px / em_; }

  // Places a glyph like canvas fillText(): x is the left edge and baseline
  // the alphabetic baseline, in pixels.
  GlyphInstance MakeGlyph(int glyph, float x, float baseline, float font_px,
                          const Color& color) const;
  GlyphInstance MakeSolid(float x, float y, float width, float height,
                          const Color& color) const;

 private:
  ComPtr<ID3D11ShaderResourceView> view_;
  std::wstring font_name_;
  std::wstring extra_characters_;
  int requested_em_ = 0;
  int base_glyph_count_ = 0;
  int em_ = 1;
  int padding_ = 0;
  int columns_ = 1;
  int glyph_count_ = 0;
  std::wstring characters_;
  std::vector<float> advances_;  // Atlas pixels.
  float cell_width_ = 0.0f;
  float cell_height_ = 0.0f;
  float ascent_ = 0.0f;
  float descent_ = 0.0f;
  float texture_width_ = 1.0f;
  float texture_height_ = 1.0f;
};

bool GlyphAtlas::Build(ID3D11Device* device, ID3D11DeviceContext* context,
                       const std::wstring& font_name, int em,
                       const std::wstring& extra_characters,
                       int requested_em) {
  const std::wstring base_characters =
      std::wstring(kLatinCharacters) + kJapaneseCharacters;

  HDC dc = CreateCompatibleDC(nullptr);
  if (!dc) return false;

  // The chosen font draws the original characters. Feed characters it lacks
  // come from the first fallback font that has them.
  std::vector<HFONT> fonts = {
      CreateAtlasFont(font_name.c_str(), em, FIXED_PITCH | FF_MODERN)};
  if (!extra_characters.empty()) {
    for (const wchar_t* fallback : kFallbackFonts) {
      fonts.push_back(CreateAtlasFont(fallback, em, FF_DONTCARE));
    }
  }
  const auto delete_fonts = [&fonts] {
    for (HFONT font : fonts) DeleteObject(font);
  };
  HGDIOBJ old_font = SelectObject(dc, fonts[0]);
  wchar_t face_name[LF_FACESIZE] = {};
  GetTextFaceW(dc, LF_FACESIZE, face_name);

  struct AtlasGlyph {
    wchar_t character;
    size_t font;
  };
  std::vector<AtlasGlyph> glyphs;
  std::wstring characters;

  // Drops the original characters the font has no glyph for.
  std::vector<WORD> indices(base_characters.size());
  if (GetGlyphIndicesW(dc, base_characters.c_str(),
                       static_cast<int>(base_characters.size()),
                       indices.data(),
                       GGI_MARK_NONEXISTING_GLYPHS) != GDI_ERROR) {
    for (size_t i = 0; i < base_characters.size(); ++i) {
      if (indices[i] != 0xFFFF) glyphs.push_back({base_characters[i], 0});
    }
  }
  if (glyphs.empty()) {
    for (wchar_t character : base_characters) {
      glyphs.push_back({character, 0});
    }
  }
  const size_t base_glyph_count = glyphs.size();
  for (const AtlasGlyph& glyph : glyphs) characters += glyph.character;

  for (wchar_t character : extra_characters) {
    if (characters.find(character) != std::wstring::npos) continue;
    for (size_t font = 0; font < fonts.size(); ++font) {
      SelectObject(dc, fonts[font]);
      WORD index = 0xFFFF;
      if (GetGlyphIndicesW(dc, &character, 1, &index,
                           GGI_MARK_NONEXISTING_GLYPHS) == 1 &&
          index != 0xFFFF) {
        glyphs.push_back({character, font});
        characters += character;
        break;
      }
    }
  }

  // One baseline for every font, low enough for the tallest of them.
  int ascent = 0;
  int descent = 0;
  std::vector<bool> font_used(fonts.size(), false);
  for (const AtlasGlyph& glyph : glyphs) font_used[glyph.font] = true;
  for (size_t font = 0; font < fonts.size(); ++font) {
    if (!font_used[font]) continue;
    SelectObject(dc, fonts[font]);
    TEXTMETRICW metrics = {};
    GetTextMetricsW(dc, &metrics);
    ascent = std::max(ascent, static_cast<int>(metrics.tmAscent));
    descent = std::max(descent, static_cast<int>(metrics.tmDescent));
  }

  std::vector<float> advances;
  advances.reserve(glyphs.size());
  for (const AtlasGlyph& glyph : glyphs) {
    SelectObject(dc, fonts[glyph.font]);
    INT advance = 0;
    if (!GetCharWidth32W(dc, glyph.character, glyph.character, &advance)) {
      advance = em / 2;
    }
    advances.push_back(static_cast<float>(advance));
  }

  // The padding keeps neighboring cells apart down to mip kAtlasMaxLod.
  const int padding = std::max(4, em / 12);
  const int cell_width = em + 2 * padding;
  const int cell_height = ascent + descent + 2 * padding;
  const int columns = std::max(1, kMaxAtlasWidth / cell_width);
  const int cell_count = static_cast<int>(glyphs.size()) + 1;
  const int rows = (cell_count + columns - 1) / columns;
  const int width = columns * cell_width;
  const int height = rows * cell_height;

  // Many feed characters at a large size don't fit; a smaller em does.
  if (height > kMaxAtlasHeight && em > kMinAtlasEm / 2) {
    SelectObject(dc, old_font);
    delete_fonts();
    DeleteDC(dc);
    return Build(device, context, font_name, em * 3 / 4, extra_characters,
                 requested_em ? requested_em : em);
  }

  BITMAPINFO info = {};
  info.bmiHeader.biSize = sizeof(info.bmiHeader);
  info.bmiHeader.biWidth = width;
  info.bmiHeader.biHeight = -height;
  info.bmiHeader.biPlanes = 1;
  info.bmiHeader.biBitCount = 32;
  info.bmiHeader.biCompression = BI_RGB;
  void* bits = nullptr;
  HBITMAP bitmap =
      CreateDIBSection(dc, &info, DIB_RGB_COLORS, &bits, nullptr, 0);
  if (!bitmap) {
    SelectObject(dc, old_font);
    delete_fonts();
    DeleteDC(dc);
    return false;
  }
  HGDIOBJ old_bitmap = SelectObject(dc, bitmap);

  RECT solid = {0, 0, cell_width, cell_height};
  FillRect(dc, &solid, static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH)));
  SetTextColor(dc, RGB(255, 255, 255));
  SetBkMode(dc, TRANSPARENT);
  SetTextAlign(dc, TA_LEFT | TA_BASELINE | TA_NOUPDATECP);
  for (size_t i = 0; i < glyphs.size(); ++i) {
    const int cell = static_cast<int>(i) + 1;
    SelectObject(dc, fonts[glyphs[i].font]);
    TextOutW(dc, (cell % columns) * cell_width + padding,
             (cell / columns) * cell_height + padding + ascent,
             &glyphs[i].character, 1);
  }
  GdiFlush();

  // GDI draws grayscale coverage, which becomes the single texture channel.
  std::vector<uint8_t> pixels(static_cast<size_t>(width) * height);
  const uint32_t* source = static_cast<const uint32_t*>(bits);
  for (size_t i = 0; i < pixels.size(); ++i) {
    pixels[i] = static_cast<uint8_t>(std::max({(source[i] >> 16) & 0xFF,
                                               (source[i] >> 8) & 0xFF,
                                               source[i] & 0xFF}));
  }

  SelectObject(dc, old_bitmap);
  DeleteObject(bitmap);
  SelectObject(dc, old_font);
  delete_fonts();
  DeleteDC(dc);

  D3D11_TEXTURE2D_DESC desc = {};
  desc.Width = width;
  desc.Height = height;
  desc.MipLevels = 0;
  desc.ArraySize = 1;
  desc.Format = DXGI_FORMAT_R8_UNORM;
  desc.SampleDesc.Count = 1;
  desc.Usage = D3D11_USAGE_DEFAULT;
  desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
  desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

  ComPtr<ID3D11Texture2D> texture;
  ComPtr<ID3D11ShaderResourceView> view;
  if (Failed(device->CreateTexture2D(&desc, nullptr, &texture),
             L"CreateTexture2D") ||
      Failed(device->CreateShaderResourceView(texture.Get(), nullptr, &view),
             L"CreateShaderResourceView")) {
    return false;
  }
  context->UpdateSubresource(texture.Get(), 0, nullptr, pixels.data(), width,
                             0);
  context->GenerateMips(view.Get());

  view_ = view;
  font_name_ = font_name;
  requested_em_ = requested_em ? requested_em : em;
  extra_characters_ = extra_characters;
  em_ = em;
  padding_ = padding;
  columns_ = columns;
  glyph_count_ = static_cast<int>(glyphs.size());
  base_glyph_count_ = static_cast<int>(base_glyph_count);
  characters_ = characters;
  advances_ = std::move(advances);
  cell_width_ = static_cast<float>(cell_width);
  cell_height_ = static_cast<float>(cell_height);
  ascent_ = static_cast<float>(ascent);
  descent_ = static_cast<float>(descent);
  texture_width_ = static_cast<float>(width);
  texture_height_ = static_cast<float>(height);
  Wh_Log(L"Glyph atlas: font \"%s\", em %d, %d characters (%d original), "
         L"%dx%d",
         face_name, em_, glyph_count_, base_glyph_count_, width, height);
  return true;
}

GlyphInstance GlyphAtlas::MakeGlyph(int glyph, float x, float baseline,
                                    float font_px, const Color& color) const {
  const int cell = glyph + 1;
  const float scale = font_px / em_;
  GlyphInstance instance;
  instance.x = x - padding_ * scale;
  instance.y = baseline - (padding_ + ascent_) * scale;
  instance.width = cell_width_ * scale;
  instance.height = cell_height_ * scale;
  instance.u = (cell % columns_) * cell_width_ / texture_width_;
  instance.v = (cell / columns_) * cell_height_ / texture_height_;
  instance.uv_width = cell_width_ / texture_width_;
  instance.uv_height = cell_height_ / texture_height_;
  instance.color = color;
  return instance;
}

GlyphInstance GlyphAtlas::MakeSolid(float x, float y, float width,
                                    float height, const Color& color) const {
  return {x,
          y,
          width,
          height,
          0.5f * cell_width_ / texture_width_,
          0.5f * cell_height_ / texture_height_,
          0.0f,
          0.0f,
          color};
}

// Direct3D 11 port of the canvas animation. Every canvas of the original is
// a persistent render texture that keeps the previous frames, like a canvas
// does, and the textures are composited with their CSS opacity and blur.
class RainRenderer {
 public:
  RainRenderer() = default;
  RainRenderer(const RainRenderer&) = delete;
  RainRenderer& operator=(const RainRenderer&) = delete;
  ~RainRenderer();

  bool Initialize(HWND window, int width, int height, float dpi_scale,
                  const Settings& settings, double now_ms);
  bool Resize(int width, int height, float dpi_scale, double now_ms);
  bool ApplySettings(const Settings& settings, double now_ms);

  // Area the FPS counter is placed in, in window pixels. Resize() resets it
  // to the whole window.
  void SetCounterArea(const RECT& area) { counter_area_ = area; }

  // Caps the presented frames per second. The animation keeps its speed:
  // steps that come due between frames are drawn together.
  void SetFrameLimit(int fps) {
    frame_limit_ = fps;
    frame_interval_ms_ = 1000.0 / fps;
  }

  // With titles, the columns spell them instead of random characters. The
  // errors show at the bottom center.
  bool SetFeedContent(const FeedContent& content);

  // Restarts the frame clocks, so a long pause doesn't cause a burst of
  // catch-up steps.
  void Resume(double now_ms);

  // Steps every animation that is due and presents the result.
  FrameResult Advance(double now_ms);

  // Time of the next animation step.
  double NextFrameMs(double now_ms) const;

 private:
  struct RainLayer {
    RainLayerConfig config;
    RenderTexture texture;
    float font_px = 0.0f;
    std::vector<int> drops;
    std::vector<float> delays;
    // With feed titles: the title each column spells and the position of
    // its drop in it. Position -1 is the row above the canvas.
    std::vector<int> column_titles;
    std::vector<int> column_offsets;
    double last_step_ms = 0.0;
  };

  bool CreateDevice();
  void BindSharedState();
  bool EnsureAtlas();
  bool CreateTargets();
  bool ResetAnimation(double now_ms);
  double StepIntervalMs(const RainLayer& layer) const;
  float BlurAmount(double now_ms) const;
  float Random() { return distribution_(random_engine_); }
  bool IsHorizontal() const {
    return settings_.direction == Direction::kLeft ||
           settings_.direction == Direction::kRight;
  }
  int RandomGlyph();

  void StepLayer(RainLayer& layer);
  void BlinkOverlay();
  void Composite(double now_ms);
  void CompositeOverlay(double now_ms);
  void DrawFpsCounter();
  void DrawFeedErrors();
  // Text in the glyphs of the atlas. Missing characters, like the space,
  // only advance.
  float MeasureText(const std::wstring& text, float font_px) const;
  void AppendText(const std::wstring& text, float left, float top,
                  float font_px);
  void AppendTextBackdrop(float left, float top, float width, float font_px);
  void MapTitleGlyphs();
  int NextTitle();
  void AssignTitle(RainLayer& layer, size_t column);
  void CountPresentedFrame(double now_ms);
  void Blur(ID3D11ShaderResourceView* source);

  void SetTarget(ID3D11RenderTargetView* target, int width, int height);
  void UploadConstants();
  bool EnsureInstanceCapacity(size_t count);
  void DrawGlyphs(const std::vector<GlyphInstance>& glyphs,
                  ID3D11BlendState* blend);
  void DrawFullscreen(ID3D11PixelShader* shader,
                      ID3D11ShaderResourceView* source,
                      ID3D11BlendState* blend);

  HWND window_ = nullptr;
  int width_ = 0;
  int height_ = 0;
  float dpi_scale_ = 1.0f;
  Settings settings_;

  ComPtr<ID3D11Device> device_;
  ComPtr<ID3D11DeviceContext> context_;
  ComPtr<IDXGISwapChain1> swap_chain_;
  ComPtr<ID3D11RenderTargetView> back_buffer_;
  ComPtr<ID3D11VertexShader> glyph_vertex_shader_;
  ComPtr<ID3D11VertexShader> fullscreen_vertex_shader_;
  ComPtr<ID3D11PixelShader> glyph_pixel_shader_;
  ComPtr<ID3D11PixelShader> composite_pixel_shader_;
  ComPtr<ID3D11PixelShader> blur_pixel_shader_;
  ComPtr<ID3D11InputLayout> glyph_layout_;
  ComPtr<ID3D11Buffer> instance_buffer_;
  size_t instance_capacity_ = 0;
  ComPtr<ID3D11Buffer> constant_buffer_;
  ShaderConstants constants_ = {};
  ComPtr<ID3D11SamplerState> sampler_;
  ComPtr<ID3D11RasterizerState> rasterizer_;
  // Canvas source-over with straight alpha colors.
  ComPtr<ID3D11BlendState> straight_blend_;
  // Source-over for compositing premultiplied canvas textures.
  ComPtr<ID3D11BlendState> premultiplied_blend_;
  GlyphAtlas atlas_;

  std::vector<RainLayer> layers_;
  RenderTexture overlay_texture_;
  RenderTexture blur_horizontal_;
  RenderTexture blur_vertical_;
  std::vector<GlyphInstance> glyphs_;
  std::vector<GlyphInstance> overlay_glyphs_;
  double last_blink_ms_ = 0.0;
  bool overlay_blurred_ = true;
  double blur_toggle_ms_ = 0.0;
  double next_blur_toggle_ms_ = 0.0;
  float presented_blur_amount_ = -1.0f;

  RECT counter_area_ = {};
  int fps_ = -1;  // -1 until the first measurement.
  int fps_frames_ = 0;
  double fps_window_start_ms_ = 0.0;

  std::vector<std::wstring> feed_titles_;
  std::vector<std::wstring> feed_errors_;
  // Unique characters of the titles, for the atlas.
  std::wstring feed_characters_;
  // Glyph of every atlas character.
  std::unordered_map<wchar_t, int> glyph_lookup_;
  // Glyphs of each title; -1 for characters with no glyph.
  std::vector<std::vector<int>> title_glyphs_;
  int next_title_ = 0;
  bool content_changed_ = false;

  // The rain color now, on its way through settings_.colors since
  // color_start_ms_.
  Color current_color_ = {0.0f, 1.0f, 0.0f, 1.0f};
  double color_start_ms_ = 0.0;

  int frame_limit_ = kDefaultRefreshRate;
  double frame_interval_ms_ = 1000.0 / kDefaultRefreshRate;
  double last_present_ms_ = 0.0;

  std::mt19937 random_engine_{std::random_device{}()};
  std::uniform_real_distribution<float> distribution_{0.0f, 1.0f};
};

RainRenderer::~RainRenderer() {
  if (context_) context_->ClearState();
}

bool RainRenderer::Initialize(HWND window, int width, int height,
                              float dpi_scale, const Settings& settings,
                              double now_ms) {
  window_ = window;
  width_ = width;
  height_ = height;
  dpi_scale_ = dpi_scale;
  settings_ = settings;
  counter_area_ = {0, 0, width_, height_};
  color_start_ms_ = now_ms;
  current_color_ = settings_.colors.front();
  return CreateDevice() && EnsureAtlas() && CreateTargets() &&
         ResetAnimation(now_ms);
}

bool RainRenderer::EnsureAtlas() {
  const int em = std::clamp(
      static_cast<int>(kLargestFontSize * dpi_scale_ * settings_.font_scale +
                       0.5f),
      kMinAtlasEm, kMaxAtlasEm);
  if (atlas_.view() && atlas_.requested_em() == em &&
      atlas_.font_name() == settings_.font_name &&
      atlas_.extra_characters() == feed_characters_) {
    return true;
  }
  if (!atlas_.Build(device_.Get(), context_.Get(), settings_.font_name, em,
                    feed_characters_)) {
    return false;
  }
  // A new atlas renumbers the glyphs.
  MapTitleGlyphs();
  return true;
}

bool RainRenderer::SetFeedContent(const FeedContent& content) {
  feed_titles_ = content.titles;
  feed_errors_ = content.errors;
  std::wstring characters;
  for (const std::wstring& title : feed_titles_) {
    for (wchar_t character : title) {
      if (character != L' ' &&
          characters.find(character) == std::wstring::npos) {
        characters += character;
      }
    }
  }
  std::sort(characters.begin(), characters.end());
  feed_characters_ = characters;
  if (!EnsureAtlas()) return false;
  MapTitleGlyphs();

  next_title_ = 0;
  for (RainLayer& layer : layers_) {
    for (size_t column = 0; column < layer.drops.size(); ++column) {
      AssignTitle(layer, column);
    }
  }
  content_changed_ = true;
  return true;
}

void RainRenderer::MapTitleGlyphs() {
  glyph_lookup_.clear();
  const std::wstring& characters = atlas_.characters();
  for (size_t i = 0; i < characters.size(); ++i) {
    glyph_lookup_.emplace(characters[i], static_cast<int>(i));
  }
  title_glyphs_.clear();
  for (const std::wstring& title : feed_titles_) {
    std::vector<int>& title_glyphs = title_glyphs_.emplace_back();
    for (wchar_t character : title) {
      const auto found = glyph_lookup_.find(character);
      title_glyphs.push_back(found == glyph_lookup_.end() ? -1
                                                          : found->second);
    }
  }
}

// Hands out the titles in turn, so neighboring columns spell different ones.
int RainRenderer::NextTitle() {
  if (title_glyphs_.empty()) return -1;
  const int title = next_title_;
  next_title_ = (next_title_ + 1) % static_cast<int>(title_glyphs_.size());
  return title;
}

void RainRenderer::AssignTitle(RainLayer& layer, size_t column) {
  layer.column_titles[column] = NextTitle();
  layer.column_offsets[column] = -1;
}

bool RainRenderer::CreateDevice() {
  const D3D_FEATURE_LEVEL feature_levels[] = {
      D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0};
  if (Failed(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
                               D3D11_CREATE_DEVICE_BGRA_SUPPORT,
                               feature_levels, std::size(feature_levels),
                               D3D11_SDK_VERSION, &device_, nullptr,
                               &context_),
             L"D3D11CreateDevice")) {
    return false;
  }

  ComPtr<IDXGIDevice> dxgi_device;
  ComPtr<IDXGIAdapter> adapter;
  ComPtr<IDXGIFactory2> factory;
  if (Failed(device_.As(&dxgi_device), L"QueryInterface(IDXGIDevice)") ||
      Failed(dxgi_device->GetAdapter(&adapter), L"GetAdapter") ||
      Failed(adapter->GetParent(IID_PPV_ARGS(&factory)), L"GetParent")) {
    return false;
  }

  DXGI_SWAP_CHAIN_DESC1 swap_chain_desc = {};
  swap_chain_desc.Width = width_;
  swap_chain_desc.Height = height_;
  swap_chain_desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
  swap_chain_desc.SampleDesc.Count = 1;
  swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  swap_chain_desc.BufferCount = 2;
  swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
  swap_chain_desc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
  if (Failed(factory->CreateSwapChainForHwnd(device_.Get(), window_,
                                             &swap_chain_desc, nullptr,
                                             nullptr, &swap_chain_),
             L"CreateSwapChainForHwnd")) {
    return false;
  }
  factory->MakeWindowAssociation(
      window_, DXGI_MWA_NO_ALT_ENTER | DXGI_MWA_NO_WINDOW_CHANGES);

  ComPtr<ID3DBlob> glyph_vs = CompileShader("VsGlyph", "vs_4_0");
  ComPtr<ID3DBlob> fullscreen_vs = CompileShader("VsFullscreen", "vs_4_0");
  ComPtr<ID3DBlob> glyph_ps = CompileShader("PsGlyph", "ps_4_0");
  ComPtr<ID3DBlob> composite_ps = CompileShader("PsComposite", "ps_4_0");
  ComPtr<ID3DBlob> blur_ps = CompileShader("PsBlur", "ps_4_0");
  if (!glyph_vs || !fullscreen_vs || !glyph_ps || !composite_ps || !blur_ps) {
    return false;
  }

  const D3D11_INPUT_ELEMENT_DESC glyph_elements[] = {
      {"RECT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0,
       D3D11_INPUT_PER_INSTANCE_DATA, 1},
      {"UVRECT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16,
       D3D11_INPUT_PER_INSTANCE_DATA, 1},
      {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32,
       D3D11_INPUT_PER_INSTANCE_DATA, 1},
  };

  D3D11_BUFFER_DESC constants_desc = {};
  constants_desc.ByteWidth = sizeof(ShaderConstants);
  constants_desc.Usage = D3D11_USAGE_DEFAULT;
  constants_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

  D3D11_SAMPLER_DESC sampler_desc = {};
  sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
  sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
  sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
  sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
  sampler_desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
  // Deeper mips would bleed neighboring atlas cells into each other.
  sampler_desc.MaxLOD = kAtlasMaxLod;

  D3D11_RASTERIZER_DESC rasterizer_desc = {};
  rasterizer_desc.FillMode = D3D11_FILL_SOLID;
  rasterizer_desc.CullMode = D3D11_CULL_NONE;
  rasterizer_desc.DepthClipEnable = TRUE;

  D3D11_BLEND_DESC blend_desc = {};
  D3D11_RENDER_TARGET_BLEND_DESC& blend = blend_desc.RenderTarget[0];
  blend.BlendEnable = TRUE;
  blend.SrcBlend = D3D11_BLEND_SRC_ALPHA;
  blend.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
  blend.BlendOp = D3D11_BLEND_OP_ADD;
  blend.SrcBlendAlpha = D3D11_BLEND_ONE;
  blend.DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
  blend.BlendOpAlpha = D3D11_BLEND_OP_ADD;
  blend.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
  D3D11_BLEND_DESC premultiplied_desc = blend_desc;
  premultiplied_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;

  if (Failed(device_->CreateVertexShader(glyph_vs->GetBufferPointer(),
                                         glyph_vs->GetBufferSize(), nullptr,
                                         &glyph_vertex_shader_),
             L"CreateVertexShader") ||
      Failed(device_->CreateVertexShader(fullscreen_vs->GetBufferPointer(),
                                         fullscreen_vs->GetBufferSize(),
                                         nullptr, &fullscreen_vertex_shader_),
             L"CreateVertexShader") ||
      Failed(device_->CreatePixelShader(glyph_ps->GetBufferPointer(),
                                        glyph_ps->GetBufferSize(), nullptr,
                                        &glyph_pixel_shader_),
             L"CreatePixelShader") ||
      Failed(device_->CreatePixelShader(composite_ps->GetBufferPointer(),
                                        composite_ps->GetBufferSize(),
                                        nullptr, &composite_pixel_shader_),
             L"CreatePixelShader") ||
      Failed(device_->CreatePixelShader(blur_ps->GetBufferPointer(),
                                        blur_ps->GetBufferSize(), nullptr,
                                        &blur_pixel_shader_),
             L"CreatePixelShader") ||
      Failed(device_->CreateInputLayout(
                 glyph_elements, std::size(glyph_elements),
                 glyph_vs->GetBufferPointer(), glyph_vs->GetBufferSize(),
                 &glyph_layout_),
             L"CreateInputLayout") ||
      Failed(device_->CreateBuffer(&constants_desc, nullptr,
                                   &constant_buffer_),
             L"CreateBuffer") ||
      Failed(device_->CreateSamplerState(&sampler_desc, &sampler_),
             L"CreateSamplerState") ||
      Failed(device_->CreateRasterizerState(&rasterizer_desc, &rasterizer_),
             L"CreateRasterizerState") ||
      Failed(device_->CreateBlendState(&blend_desc, &straight_blend_),
             L"CreateBlendState") ||
      Failed(device_->CreateBlendState(&premultiplied_desc,
                                       &premultiplied_blend_),
             L"CreateBlendState")) {
    return false;
  }

  BindSharedState();
  return true;
}

void RainRenderer::BindSharedState() {
  ID3D11Buffer* constant_buffer = constant_buffer_.Get();
  ID3D11SamplerState* sampler = sampler_.Get();
  context_->VSSetConstantBuffers(0, 1, &constant_buffer);
  context_->PSSetConstantBuffers(0, 1, &constant_buffer);
  context_->PSSetSamplers(0, 1, &sampler);
  context_->RSSetState(rasterizer_.Get());
}

bool RainRenderer::CreateTargets() {
  back_buffer_.Reset();
  ComPtr<ID3D11Texture2D> buffer;
  if (Failed(swap_chain_->GetBuffer(0, IID_PPV_ARGS(&buffer)),
             L"GetBuffer") ||
      Failed(device_->CreateRenderTargetView(buffer.Get(), nullptr,
                                             &back_buffer_),
             L"CreateRenderTargetView")) {
    return false;
  }

  // The blurred passes run at half resolution.
  const int half_width = std::max(1, (width_ + 1) / 2);
  const int half_height = std::max(1, (height_ + 1) / 2);
  constants_.viewport_size[0] = static_cast<float>(width_);
  constants_.viewport_size[1] = static_cast<float>(height_);
  return CreateRenderTexture(device_.Get(), half_width, half_height,
                             &overlay_texture_) &&
         CreateRenderTexture(device_.Get(), half_width, half_height,
                             &blur_horizontal_) &&
         CreateRenderTexture(device_.Get(), half_width, half_height,
                             &blur_vertical_);
}

bool RainRenderer::Resize(int width, int height, float dpi_scale,
                          double now_ms) {
  if (width == width_ && height == height_ && dpi_scale == dpi_scale_) {
    return true;
  }
  width_ = width;
  height_ = height;
  dpi_scale_ = dpi_scale;
  counter_area_ = {0, 0, width_, height_};

  // ResizeBuffers needs every reference to the old buffers released.
  layers_.clear();
  back_buffer_.Reset();
  context_->ClearState();
  if (Failed(swap_chain_->ResizeBuffers(0, width_, height_,
                                        DXGI_FORMAT_UNKNOWN, 0),
             L"ResizeBuffers")) {
    return false;
  }
  BindSharedState();
  return EnsureAtlas() && CreateTargets() && ResetAnimation(now_ms);
}

bool RainRenderer::ApplySettings(const Settings& settings, double now_ms) {
  const bool layout_changed = settings.font_name != settings_.font_name ||
                              settings.layered != settings_.layered ||
                              settings.direction != settings_.direction ||
                              settings.font_scale != settings_.font_scale;
  // New colors start over from the first one.
  const bool colors_changed =
      settings.colors.size() != settings_.colors.size() ||
      !std::equal(settings.colors.begin(), settings.colors.end(),
                  settings_.colors.begin(),
                  [](const Color& a, const Color& b) {
                    return a.r == b.r && a.g == b.g && a.b == b.b;
                  });
  settings_ = settings;
  if (colors_changed) {
    color_start_ms_ = now_ms;
    current_color_ = settings_.colors.front();
  }
  return EnsureAtlas() && (!layout_changed || ResetAnimation(now_ms));
}

// Matches the canvas state after a resize: empty canvases and every drop at
// the top.
bool RainRenderer::ResetAnimation(double now_ms) {
  const float transparent[4] = {};
  const size_t layer_count = settings_.layered ? std::size(kRainLayers) : 1;
  layers_.clear();
  for (size_t i = 0; i < layer_count; ++i) {
    RainLayer& layer = layers_.emplace_back();
    layer.config = kRainLayers[i];
    if (!settings_.layered) {
      layer.config.opacity = 1.0f;
      layer.config.blurred = false;
    }
    layer.font_px = std::max(
        2.0f, layer.config.font_size * dpi_scale_ * settings_.font_scale);
    // Columns span the screen across the direction of flight.
    const int across = IsHorizontal() ? height_ : width_;
    const int columns = std::max(1, static_cast<int>(across / layer.font_px));
    layer.drops.assign(columns, 0);
    layer.delays.assign(columns, 0.0f);
    layer.column_titles.assign(columns, -1);
    layer.column_offsets.assign(columns, -1);
    for (int column = 0; column < columns; ++column) {
      AssignTitle(layer, column);
    }
    if (!CreateRenderTexture(device_.Get(), width_, height_,
                             &layer.texture)) {
      return false;
    }
    context_->ClearRenderTargetView(layer.texture.target.Get(), transparent);
    layer.last_step_ms = now_ms - StepIntervalMs(layer);
  }

  overlay_glyphs_.clear();
  context_->ClearRenderTargetView(overlay_texture_.target.Get(), transparent);
  last_blink_ms_ = now_ms - kOverlayBlinkMs;
  overlay_blurred_ = true;
  blur_toggle_ms_ = now_ms - kBlurTransitionMs;
  next_blur_toggle_ms_ = now_ms + kOverlayBlurToggleMs;
  presented_blur_amount_ = -1.0f;
  fps_frames_ = 0;
  fps_window_start_ms_ = now_ms;
  return true;
}

double RainRenderer::StepIntervalMs(const RainLayer& layer) const {
  return 1000.0 / (layer.config.fps * settings_.speed);
}

float RainRenderer::BlurAmount(double now_ms) const {
  const float progress = std::clamp(
      static_cast<float>((now_ms - blur_toggle_ms_) / kBlurTransitionMs),
      0.0f, 1.0f);
  // CSS ease-in-out.
  const float eased = progress * progress * (3.0f - 2.0f * progress);
  return overlay_blurred_ ? eased : 1.0f - eased;
}

// The rain keeps the original characters; feed characters only appear in
// titles.
int RainRenderer::RandomGlyph() {
  const int count = atlas_.base_glyph_count();
  return std::min(static_cast<int>(Random() * count), count - 1);
}

void RainRenderer::Resume(double now_ms) {
  for (RainLayer& layer : layers_) {
    layer.last_step_ms = now_ms - StepIntervalMs(layer);
  }
  last_blink_ms_ = now_ms - kOverlayBlinkMs;
  next_blur_toggle_ms_ = now_ms + kOverlayBlurToggleMs;
  fps_frames_ = 0;
  fps_window_start_ms_ = now_ms;
}

FrameResult RainRenderer::Advance(double now_ms) {
  if (now_ms + kEarlyStepMs < last_present_ms_ + frame_interval_ms_) {
    return FrameResult::kIdle;
  }

  current_color_ = CycleColor(settings_.colors, now_ms - color_start_ms_,
                              settings_.color_transition_ms);
  bool dirty = false;
  for (RainLayer& layer : layers_) {
    const double interval = StepIntervalMs(layer);
    // Below the layer's own rate, one frame draws several steps.
    for (int step = 0;
         step < kMaxCatchUpSteps &&
         now_ms + kEarlyStepMs >= layer.last_step_ms + interval;
         ++step) {
      StepLayer(layer);
      layer.last_step_ms += interval;
      dirty = true;
    }
    if (now_ms - layer.last_step_ms > interval) layer.last_step_ms = now_ms;
  }

  if (settings_.layered) {
    if (now_ms + kEarlyStepMs >= last_blink_ms_ + kOverlayBlinkMs) {
      BlinkOverlay();
      last_blink_ms_ = now_ms;
      dirty = true;
    }
    if (now_ms + kEarlyStepMs >= next_blur_toggle_ms_) {
      overlay_blurred_ = !overlay_blurred_;
      blur_toggle_ms_ = now_ms;
      next_blur_toggle_ms_ = now_ms + kOverlayBlurToggleMs;
    }
    if (BlurAmount(now_ms) != presented_blur_amount_) dirty = true;
  }

  if (content_changed_) {
    content_changed_ = false;
    dirty = true;
  }

  if (!dirty) return FrameResult::kIdle;
  Composite(now_ms);
  const HRESULT result = swap_chain_->Present(0, 0);
  if (result == DXGI_ERROR_DEVICE_REMOVED ||
      result == DXGI_ERROR_DEVICE_RESET) {
    Wh_Log(L"Direct3D device lost: 0x%08lX",
           static_cast<unsigned long>(result));
    return FrameResult::kDeviceLost;
  }
  last_present_ms_ = now_ms;
  CountPresentedFrame(now_ms);
  return FrameResult::kPresented;
}

void RainRenderer::CountPresentedFrame(double now_ms) {
  ++fps_frames_;
  const double elapsed_ms = now_ms - fps_window_start_ms_;
  if (elapsed_ms < kFpsWindowMs) return;
  fps_ = static_cast<int>(fps_frames_ * 1000.0 / elapsed_ms + 0.5);
  fps_frames_ = 0;
  fps_window_start_ms_ = now_ms;
}

double RainRenderer::NextFrameMs(double now_ms) const {
  double next_ms = now_ms + kMaxIdleMs;
  for (const RainLayer& layer : layers_) {
    next_ms = std::min(next_ms, layer.last_step_ms + StepIntervalMs(layer));
  }
  if (settings_.layered) {
    next_ms = std::min(
        {next_ms, last_blink_ms_ + kOverlayBlinkMs, next_blur_toggle_ms_});
    // The blur fade animates at the frame limit.
    if (now_ms - blur_toggle_ms_ < kBlurTransitionMs) {
      next_ms = std::min(next_ms, now_ms + frame_interval_ms_);
    }
  }
  return std::max(next_ms, last_present_ms_ + frame_interval_ms_);
}

// One frame of matrixRain() in the original.
void RainRenderer::StepLayer(RainLayer& layer) {
  const RainLayerConfig& config = layer.config;
  const float font_px = layer.font_px;

  glyphs_.clear();
  // Fades the previous frames, which leaves trails behind the drops.
  glyphs_.push_back(atlas_.MakeSolid(0.0f, 0.0f, width_, height_,
                                     {0.0f, 0.0f, 0.0f, kTrailFadeAlpha}));
  // The original falls down; the other directions mirror or turn the path,
  // and the glyphs stay upright.
  const float along_limit = IsHorizontal() ? width_ : height_;
  const bool spell_titles = !title_glyphs_.empty();
  for (size_t i = 0; i < layer.drops.size(); ++i) {
    const float across = i * font_px;
    const float along = layer.drops[i] * font_px;

    // Titles leave their spaces and the gap between them empty.
    int glyph = -1;
    if (!spell_titles) {
      glyph = RandomGlyph();
    } else if (layer.column_titles[i] >= 0) {
      const std::vector<int>& title = title_glyphs_[layer.column_titles[i]];
      const int offset = layer.column_offsets[i];
      if (offset >= 0 && offset < static_cast<int>(title.size())) {
        glyph = title[offset];
      }
    }

    // The original also draws past the canvas; those glyphs are skipped.
    if (glyph >= 0 && along - 2.0f * font_px < along_limit) {
      float x = across;
      float baseline = along;
      switch (settings_.direction) {
        case Direction::kBottom:
          break;
        case Direction::kTop:
          baseline = height_ - along + font_px;
          break;
        case Direction::kRight:
          x = along - font_px;
          baseline = across + font_px;
          break;
        case Direction::kLeft:
          x = width_ - along;
          baseline = across + font_px;
          break;
      }
      glyphs_.push_back(
          atlas_.MakeGlyph(glyph, x, baseline, font_px, current_color_));
    }

    if (layer.delays[i] <= 0.0f) {
      ++layer.drops[i];
      layer.delays[i] = Random() * (config.delay_factor / config.speed_factor);
      // A column moves on to the next title after a gap.
      if (spell_titles && layer.column_titles[i] >= 0 &&
          ++layer.column_offsets[i] >=
              static_cast<int>(title_glyphs_[layer.column_titles[i]].size()) +
                  kTitleGap) {
        layer.column_titles[i] = NextTitle();
        layer.column_offsets[i] = 0;
      }
    } else {
      layer.delays[i] -= 1.0f;
    }

    if (along > along_limit && Random() > 1.0f - kDropResetChance) {
      layer.drops[i] = 0;
      if (spell_titles) AssignTitle(layer, i);
    }
  }

  SetTarget(layer.texture.target.Get(), width_, height_);
  DrawGlyphs(glyphs_, straight_blend_.Get());
}

// One blink of matrixOverlay() in the original.
void RainRenderer::BlinkOverlay() {
  const float font_px = kOverlayFontSize * dpi_scale_ * settings_.font_scale;
  const Color color = {1.0f, 1.0f, 1.0f, kOverlayGlyphAlpha};
  overlay_glyphs_.clear();
  for (int i = 0; i < kOverlayGlyphCount; ++i) {
    const int glyph = RandomGlyph();
    const float x = Random() * width_;
    const float y = Random() * height_;
    overlay_glyphs_.push_back(atlas_.MakeGlyph(glyph, x, y, font_px, color));
  }

  const float transparent[4] = {};
  context_->ClearRenderTargetView(overlay_texture_.target.Get(), transparent);
  SetTarget(overlay_texture_.target.Get(), overlay_texture_.width,
            overlay_texture_.height);
  DrawGlyphs(overlay_glyphs_, straight_blend_.Get());
}

void RainRenderer::Composite(double now_ms) {
  const float black[4] = {0.0f, 0.0f, 0.0f, 1.0f};
  context_->ClearRenderTargetView(back_buffer_.Get(), black);

  for (const RainLayer& layer : layers_) {
    ID3D11ShaderResourceView* source = layer.texture.view.Get();
    if (layer.config.blurred) {
      Blur(source);
      source = blur_vertical_.view.Get();
    }
    SetTarget(back_buffer_.Get(), width_, height_);
    constants_.opacity = layer.config.opacity;
    DrawFullscreen(composite_pixel_shader_.Get(), source,
                   premultiplied_blend_.Get());
  }

  if (settings_.layered) CompositeOverlay(now_ms);
  DrawFpsCounter();
  DrawFeedErrors();
}

void RainRenderer::CompositeOverlay(double now_ms) {
  // The overlay's blur fades in and out, so the sharp and the blurred
  // versions are cross-faded.
  const float blur_amount = BlurAmount(now_ms);
  if (blur_amount < 1.0f) {
    glyphs_ = overlay_glyphs_;
    for (GlyphInstance& glyph : glyphs_) {
      glyph.color.a *= kOverlayOpacity * (1.0f - blur_amount);
    }
    SetTarget(back_buffer_.Get(), width_, height_);
    DrawGlyphs(glyphs_, straight_blend_.Get());
  }
  if (blur_amount > 0.0f) {
    Blur(overlay_texture_.view.Get());
    SetTarget(back_buffer_.Get(), width_, height_);
    constants_.opacity = kOverlayOpacity * blur_amount;
    DrawFullscreen(composite_pixel_shader_.Get(), blur_vertical_.view.Get(),
                   premultiplied_blend_.Get());
  }
  presented_blur_amount_ = blur_amount;
}

// Draws "FPS: <n> (<limit>)" with the glyphs, color and size of the front
// rain layer, on a translucent backdrop that keeps it readable over the rain.
void RainRenderer::DrawFpsCounter() {
  if (!settings_.show_fps || layers_.empty()) return;
  const float font_px = layers_.front().font_px;
  const std::wstring text =
      L"FPS: " + (fps_ < 0 ? std::wstring(L"--") : std::to_wstring(fps_)) +
      L" (" + std::to_wstring(frame_limit_) + L")";

  const float text_width = MeasureText(text, font_px);
  const float text_height =
      atlas_.AscentPx(font_px) + atlas_.DescentPx(font_px);
  const float margin = font_px;
  const bool right = settings_.fps_position == FpsPosition::kTopRight ||
                     settings_.fps_position == FpsPosition::kBottomRight;
  const bool bottom = settings_.fps_position == FpsPosition::kBottomLeft ||
                      settings_.fps_position == FpsPosition::kBottomRight;
  const float left = right ? counter_area_.right - margin - text_width
                           : counter_area_.left + margin;
  const float top = bottom ? counter_area_.bottom - margin - text_height
                           : counter_area_.top + margin;

  glyphs_.clear();
  AppendTextBackdrop(left, top, text_width, font_px);
  AppendText(text, left, top, font_px);
  SetTarget(back_buffer_.Get(), width_, height_);
  DrawGlyphs(glyphs_, straight_blend_.Get());
}

// Draws one line per failed feed at the bottom center of the counter area,
// at the size of the FPS counter.
void RainRenderer::DrawFeedErrors() {
  if (feed_errors_.empty() || layers_.empty()) return;
  const float font_px = layers_.front().font_px;
  std::vector<std::wstring> lines(
      feed_errors_.begin(),
      feed_errors_.begin() +
          std::min(feed_errors_.size(), kMaxFeedErrorLines));
  if (feed_errors_.size() > kMaxFeedErrorLines) {
    lines.back() = L"+" +
                   std::to_wstring(feed_errors_.size() - lines.size() + 1) +
                   L" RSS feeds unavailable";
  }

  const float text_height =
      atlas_.AscentPx(font_px) + atlas_.DescentPx(font_px);
  // The backdrops of neighboring lines touch.
  const float line_step = text_height + font_px;
  const float center = (counter_area_.left + counter_area_.right) / 2.0f;
  glyphs_.clear();
  for (size_t i = 0; i < lines.size(); ++i) {
    const std::wstring& line = lines[lines.size() - 1 - i];
    const float width = MeasureText(line, font_px);
    const float left = center - width / 2.0f;
    const float top =
        counter_area_.bottom - font_px - text_height - i * line_step;
    AppendTextBackdrop(left, top, width, font_px);
    AppendText(line, left, top, font_px);
  }
  SetTarget(back_buffer_.Get(), width_, height_);
  DrawGlyphs(glyphs_, straight_blend_.Get());
}

float RainRenderer::MeasureText(const std::wstring& text,
                                float font_px) const {
  float width = 0.0f;
  for (wchar_t character : text) {
    const auto found = glyph_lookup_.find(character);
    width += found == glyph_lookup_.end()
                 ? 0.5f * font_px
                 : atlas_.AdvancePx(found->second, font_px);
  }
  return width;
}

void RainRenderer::AppendText(const std::wstring& text, float left,
                              float top, float font_px) {
  const float baseline = top + atlas_.AscentPx(font_px);
  float x = left;
  for (wchar_t character : text) {
    const auto found = glyph_lookup_.find(character);
    if (found == glyph_lookup_.end()) {
      x += 0.5f * font_px;
      continue;
    }
    glyphs_.push_back(atlas_.MakeGlyph(found->second, x, baseline, font_px,
                                       current_color_));
    x += atlas_.AdvancePx(found->second, font_px);
  }
}

// A translucent box that keeps text readable over the rain.
void RainRenderer::AppendTextBackdrop(float left, float top, float width,
                                      float font_px) {
  const float padding = 0.5f * font_px;
  const float height = atlas_.AscentPx(font_px) + atlas_.DescentPx(font_px);
  glyphs_.push_back(atlas_.MakeSolid(left - padding, top - padding,
                                     width + 2.0f * padding,
                                     height + 2.0f * padding,
                                     {0.0f, 0.0f, 0.0f, kFpsBackdropAlpha}));
}

// Separable Gaussian blur into blur_vertical_.
void RainRenderer::Blur(ID3D11ShaderResourceView* source) {
  // CSS blur() takes the standard deviation. Half resolution halves it.
  constants_.blur_sigma =
      std::clamp(kBlurRadius * dpi_scale_ / 2.0f, 0.5f, kMaxBlurSigma);

  SetTarget(blur_horizontal_.target.Get(), blur_horizontal_.width,
            blur_horizontal_.height);
  constants_.blur_step[0] = 1.0f / blur_horizontal_.width;
  constants_.blur_step[1] = 0.0f;
  DrawFullscreen(blur_pixel_shader_.Get(), source, nullptr);

  SetTarget(blur_vertical_.target.Get(), blur_vertical_.width,
            blur_vertical_.height);
  constants_.blur_step[0] = 0.0f;
  constants_.blur_step[1] = 1.0f / blur_vertical_.height;
  DrawFullscreen(blur_pixel_shader_.Get(), blur_horizontal_.view.Get(),
                 nullptr);
}

void RainRenderer::SetTarget(ID3D11RenderTargetView* target, int width,
                             int height) {
  context_->OMSetRenderTargets(1, &target, nullptr);
  const D3D11_VIEWPORT viewport = {
      0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height),
      0.0f, 1.0f};
  context_->RSSetViewports(1, &viewport);
}

void RainRenderer::UploadConstants() {
  context_->UpdateSubresource(constant_buffer_.Get(), 0, nullptr, &constants_,
                              0, 0);
}

bool RainRenderer::EnsureInstanceCapacity(size_t count) {
  if (count <= instance_capacity_) return true;
  const size_t capacity = std::max(count, instance_capacity_ * 2);
  D3D11_BUFFER_DESC desc = {};
  desc.ByteWidth = static_cast<UINT>(capacity * sizeof(GlyphInstance));
  desc.Usage = D3D11_USAGE_DYNAMIC;
  desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
  desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
  instance_buffer_.Reset();
  instance_capacity_ = 0;
  if (Failed(device_->CreateBuffer(&desc, nullptr, &instance_buffer_),
             L"CreateBuffer")) {
    return false;
  }
  instance_capacity_ = capacity;
  return true;
}

void RainRenderer::DrawGlyphs(const std::vector<GlyphInstance>& glyphs,
                              ID3D11BlendState* blend) {
  if (glyphs.empty() || !EnsureInstanceCapacity(glyphs.size())) return;
  D3D11_MAPPED_SUBRESOURCE mapped;
  if (FAILED(context_->Map(instance_buffer_.Get(), 0,
                           D3D11_MAP_WRITE_DISCARD, 0, &mapped))) {
    return;
  }
  memcpy(mapped.pData, glyphs.data(), glyphs.size() * sizeof(GlyphInstance));
  context_->Unmap(instance_buffer_.Get(), 0);
  UploadConstants();

  ID3D11Buffer* buffer = instance_buffer_.Get();
  const UINT stride = sizeof(GlyphInstance);
  const UINT offset = 0;
  ID3D11ShaderResourceView* atlas = atlas_.view();
  context_->IASetInputLayout(glyph_layout_.Get());
  context_->IASetVertexBuffers(0, 1, &buffer, &stride, &offset);
  context_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
  context_->VSSetShader(glyph_vertex_shader_.Get(), nullptr, 0);
  context_->PSSetShader(glyph_pixel_shader_.Get(), nullptr, 0);
  context_->PSSetShaderResources(0, 1, &atlas);
  context_->OMSetBlendState(blend, nullptr, 0xFFFFFFFF);
  context_->DrawInstanced(4, static_cast<UINT>(glyphs.size()), 0, 0);
}

void RainRenderer::DrawFullscreen(ID3D11PixelShader* shader,
                                  ID3D11ShaderResourceView* source,
                                  ID3D11BlendState* blend) {
  UploadConstants();
  context_->IASetInputLayout(nullptr);
  context_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
  context_->VSSetShader(fullscreen_vertex_shader_.Get(), nullptr, 0);
  context_->PSSetShader(shader, nullptr, 0);
  context_->PSSetShaderResources(0, 1, &source);
  context_->OMSetBlendState(blend, nullptr, 0xFFFFFFFF);
  context_->Draw(3, 0);
  // The source may be a render target in the next pass.
  ID3D11ShaderResourceView* no_view = nullptr;
  context_->PSSetShaderResources(0, 1, &no_view);
}

BOOL CALLBACK FindIconsHostSibling(HWND window, LPARAM param) {
  if (!FindWindowExW(window, nullptr, L"SHELLDLL_DefView", nullptr)) {
    return TRUE;
  }
  *reinterpret_cast<HWND*>(param) =
      FindWindowExW(nullptr, window, L"WorkerW", nullptr);
  return FALSE;
}

// Finds the window behind the desktop icons that the wallpaper is drawn in.
// Adapted from Live Video Wallpaper by AKS HAY (GPL-3.0).
HWND FindWallpaperHost(bool spawn_worker) {
  HWND progman = FindWindowW(L"Progman", nullptr);
  if (!progman) return nullptr;
  if (spawn_worker) {
    DWORD_PTR result = 0;
    SendMessageTimeoutW(progman, kSpawnWorkerWMessage, 0, 0, SMTO_NORMAL,
                        1000, &result);
  }

  // Classic layout: the top-level WorkerW behind the window with the icons.
  HWND host = nullptr;
  EnumWindows(FindIconsHostSibling, reinterpret_cast<LPARAM>(&host));
  if (host) return host;

  // Windows 11 24H2 layout: a WorkerW child of Progman, below the icons.
  HWND worker = nullptr;
  while ((worker = FindWindowExW(progman, worker, L"WorkerW", nullptr))) {
    if (!FindWindowExW(worker, nullptr, L"SHELLDLL_DefView", nullptr)) {
      return worker;
    }
  }
  return nullptr;
}

// Waits until this Explorer process owns the desktop and the wallpaper host
// exists. Returns nullptr when the mod stops or another process owns the
// desktop.
HWND WaitForWallpaperHost() {
  for (;;) {
    HWND shell = GetShellWindow();
    if (shell) {
      DWORD process_id = 0;
      GetWindowThreadProcessId(shell, &process_id);
      if (process_id != GetCurrentProcessId()) {
        Wh_Log(L"Another Explorer process owns the desktop");
        return nullptr;
      }
      if (HWND host = FindWallpaperHost(true)) return host;
    }
    if (WaitForSingleObject(stop_event, kHostWaitIntervalMs) ==
        WAIT_OBJECT_0) {
      return nullptr;
    }
  }
}

BOOL CALLBACK CollectMonitorRect(HMONITOR, HDC, LPRECT rect, LPARAM param) {
  reinterpret_cast<std::vector<RECT>*>(param)->push_back(*rect);
  return TRUE;
}

bool IsDesktopIconsWindow(HWND window) {
  wchar_t class_name[16];
  if (!GetClassNameW(window, class_name, std::size(class_name))) return false;
  if (wcscmp(class_name, L"Progman") == 0) return true;
  return wcscmp(class_name, L"WorkerW") == 0 &&
         FindWindowExW(window, nullptr, L"SHELLDLL_DefView", nullptr);
}

bool ContainsRect(const RECT& outer, const RECT& inner) {
  return outer.left <= inner.left && outer.top <= inner.top &&
         outer.right >= inner.right && outer.bottom >= inner.bottom;
}

// Returns true when visible windows cover every monitor, so the wallpaper
// can't be seen.
bool IsDesktopCovered() {
  std::vector<RECT> monitors;
  EnumDisplayMonitors(nullptr, nullptr, CollectMonitorRect,
                      reinterpret_cast<LPARAM>(&monitors));
  std::vector<bool> covered(monitors.size(), false);
  size_t remaining = monitors.size();

  for (HWND window = GetTopWindow(nullptr); window && remaining > 0;
       window = GetWindow(window, GW_HWNDNEXT)) {
    if (IsDesktopIconsWindow(window)) break;
    if (!IsWindowVisible(window) || IsIconic(window)) continue;
    const LONG ex_style = GetWindowLongW(window, GWL_EXSTYLE);
    if (ex_style & WS_EX_TRANSPARENT) continue;
    if ((ex_style & (WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE)) &&
        !(ex_style & WS_EX_APPWINDOW)) {
      continue;
    }
    DWORD cloaked = 0;
    if (SUCCEEDED(DwmGetWindowAttribute(window, DWMWA_CLOAKED, &cloaked,
                                        sizeof(cloaked))) &&
        cloaked) {
      continue;
    }

    RECT bounds;
    if (FAILED(DwmGetWindowAttribute(window, DWMWA_EXTENDED_FRAME_BOUNDS,
                                     &bounds, sizeof(bounds))) &&
        !GetWindowRect(window, &bounds)) {
      continue;
    }
    // A maximized window leaves only the taskbar uncovered.
    if (IsZoomed(window)) {
      MONITORINFO info = {};
      info.cbSize = sizeof(info);
      if (GetMonitorInfoW(MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST),
                          &info)) {
        bounds = info.rcMonitor;
      }
    }
    for (size_t i = 0; i < monitors.size(); ++i) {
      if (!covered[i] && ContainsRect(bounds, monitors[i])) {
        covered[i] = true;
        --remaining;
      }
    }
  }
  return !monitors.empty() && remaining == 0;
}

BOOL CALLBACK CollectMonitorRefreshRate(HMONITOR monitor, HDC, LPRECT,
                                        LPARAM param) {
  MONITORINFOEXW info = {};
  info.cbSize = sizeof(info);
  DEVMODEW mode = {};
  mode.dmSize = sizeof(mode);
  if (GetMonitorInfoW(monitor, &info) &&
      EnumDisplaySettingsW(info.szDevice, ENUM_CURRENT_SETTINGS, &mode) &&
      mode.dmDisplayFrequency > 1) {
    int& rate = *reinterpret_cast<int*>(param);
    rate = std::max(rate, static_cast<int>(mode.dmDisplayFrequency));
  }
  return TRUE;
}

// Returns the highest refresh rate of all monitors, which the animation
// spans. Values 0 and 1 from EnumDisplaySettings mean the hardware default
// and are ignored.
int MaxMonitorRefreshRate() {
  int rate = 0;
  EnumDisplayMonitors(nullptr, nullptr, CollectMonitorRefreshRate,
                      reinterpret_cast<LPARAM>(&rate));
  return rate > 0 ? rate : kDefaultRefreshRate;
}

// Resolves the requested frame limit to [kMinFrameLimit, the highest monitor
// refresh rate].
int EffectiveFrameLimit(int requested) {
  const int monitor_max = MaxMonitorRefreshRate();
  const int limit = requested <= 0 ? monitor_max : requested;
  return std::max(kMinFrameLimit, std::min(limit, monitor_max));
}

bool IsOnBattery() {
  SYSTEM_POWER_STATUS status;
  return GetSystemPowerStatus(&status) && status.ACLineStatus == 0;
}

std::wstring TrimSpaces(const std::wstring& text) {
  const size_t begin = text.find_first_not_of(L" \t\r\n");
  if (begin == std::wstring::npos) return L"";
  const size_t end = text.find_last_not_of(L" \t\r\n");
  return text.substr(begin, end - begin + 1);
}

// Splits the setting into links. Links are separated by commas or
// semicolons, and may be quoted with "", '' or curly quotes.
std::vector<std::wstring> ParseFeedLinks(const std::wstring& text) {
  std::vector<std::wstring> links;
  size_t i = 0;
  while (i < text.size()) {
    while (i < text.size() &&
           (iswspace(text[i]) || text[i] == L',' || text[i] == L';')) {
      ++i;
    }
    if (i >= text.size()) break;

    std::wstring link;
    const wchar_t open = text[i];
    if (open == L'"' || open == L'\'' || open == L'“') {
      const wchar_t close = open == L'“' ? L'”' : open;
      const size_t end = text.find(close, i + 1);
      link = text.substr(i + 1, end == std::wstring::npos
                                    ? std::wstring::npos
                                    : end - i - 1);
      i = end == std::wstring::npos ? text.size() : end + 1;
    } else {
      const size_t end = text.find_first_of(L",;", i);
      link = text.substr(
          i, end == std::wstring::npos ? std::wstring::npos : end - i);
      i = end == std::wstring::npos ? text.size() : end;
    }
    link = TrimSpaces(link);
    if (!link.empty()) links.push_back(link);
  }
  return links;
}

// Collapses whitespace and drops control and surrogate characters, which
// the atlas can't hold.
std::wstring NormalizeTitle(const wchar_t* text, size_t length) {
  std::wstring title;
  bool pending_space = false;
  for (size_t i = 0; i < length && title.size() < kMaxTitleLength; ++i) {
    const wchar_t character = text[i];
    if (iswspace(character)) {
      pending_space = !title.empty();
      continue;
    }
    if (iswcntrl(character) || (character >= 0xD800 && character <= 0xDFFF)) {
      continue;
    }
    if (pending_space) title += L' ';
    pending_space = false;
    title += character;
  }
  return title;
}

std::wstring FeedErrorReason(HRESULT result) {
  switch (result) {
    case static_cast<HRESULT>(0x80072EE7):
      return L"host not found";
    case static_cast<HRESULT>(0x80072EE2):
      return L"timed out";
    case static_cast<HRESULT>(0x80072EFD):
    case static_cast<HRESULT>(0x80072EFE):
      return L"cannot connect";
    case static_cast<HRESULT>(0x80072F06):
    case static_cast<HRESULT>(0x80072F7D):
    case static_cast<HRESULT>(0x80072F8F):
      return L"secure connection failed";
  }
  wchar_t reason[32];
  swprintf(reason, std::size(reason), L"error 0x%08lX",
           static_cast<unsigned long>(result));
  return reason;
}

// Owns a BSTR for one call.
class ScopedBstr {
 public:
  explicit ScopedBstr(const wchar_t* text) : value_(SysAllocString(text)) {}
  ScopedBstr(const ScopedBstr&) = delete;
  ScopedBstr& operator=(const ScopedBstr&) = delete;
  ~ScopedBstr() { SysFreeString(value_); }
  BSTR get() const { return value_; }

 private:
  BSTR value_;
};

// Downloads the feeds on its own thread, so slow servers never stall the
// animation, and posts kMsgFeedsUpdated to the render thread with each
// result.
class FeedFetcher {
 public:
  explicit FeedFetcher(DWORD notify_thread_id)
      : notify_thread_id_(notify_thread_id) {}
  FeedFetcher(const FeedFetcher&) = delete;
  FeedFetcher& operator=(const FeedFetcher&) = delete;
  ~FeedFetcher();

  bool Start();
  // Fetches the links now if they changed, then on a schedule.
  void SetLinks(const std::vector<std::wstring>& links);
  FeedContent TakeContent();

 private:
  static DWORD WINAPI ThreadProc(void* param);
  void Run();
  bool IsStopping() const {
    return WaitForSingleObject(stop_event_, 0) == WAIT_OBJECT_0;
  }
  // Returns false and sets error when the feed can't be read. A stop
  // returns false with an empty error.
  bool FetchFeed(const std::wstring& link, std::vector<std::wstring>* titles,
                 std::wstring* error);

  const DWORD notify_thread_id_;
  HANDLE thread_ = nullptr;
  HANDLE stop_event_ = nullptr;
  HANDLE wake_event_ = nullptr;

  std::mutex mutex_;
  std::vector<std::wstring> links_;  // Guarded by mutex_.
  bool links_set_ = false;           // Guarded by mutex_.
  FeedContent content_;              // Guarded by mutex_.
};

FeedFetcher::~FeedFetcher() {
  if (thread_) {
    SetEvent(stop_event_);
    WaitForSingleObject(thread_, INFINITE);
    CloseHandle(thread_);
  }
  if (wake_event_) CloseHandle(wake_event_);
  if (stop_event_) CloseHandle(stop_event_);
}

bool FeedFetcher::Start() {
  stop_event_ = CreateEventW(nullptr, TRUE, FALSE, nullptr);
  wake_event_ = CreateEventW(nullptr, FALSE, FALSE, nullptr);
  if (!stop_event_ || !wake_event_) return false;
  thread_ = CreateThread(nullptr, 0, ThreadProc, this, 0, nullptr);
  return thread_ != nullptr;
}

void FeedFetcher::SetLinks(const std::vector<std::wstring>& links) {
  {
    std::lock_guard<std::mutex> lock(mutex_);
    if (links_set_ && links == links_) return;
    links_ = links;
    links_set_ = true;
  }
  SetEvent(wake_event_);
}

FeedContent FeedFetcher::TakeContent() {
  std::lock_guard<std::mutex> lock(mutex_);
  return content_;
}

DWORD WINAPI FeedFetcher::ThreadProc(void* param) {
  static_cast<FeedFetcher*>(param)->Run();
  return 0;
}

void FeedFetcher::Run() {
  const bool com_initialized =
      SUCCEEDED(CoInitializeEx(nullptr, COINIT_MULTITHREADED));
  const HANDLE handles[] = {stop_event_, wake_event_};
  DWORD wait_ms = INFINITE;
  for (;;) {
    const DWORD wait = WaitForMultipleObjects(std::size(handles), handles,
                                              FALSE, wait_ms);
    if (wait != WAIT_OBJECT_0 + 1 && wait != WAIT_TIMEOUT) break;

    std::vector<std::wstring> links;
    {
      std::lock_guard<std::mutex> lock(mutex_);
      links = links_;
    }
    FeedContent content;
    for (const std::wstring& link : links) {
      std::vector<std::wstring> titles;
      std::wstring error;
      if (FetchFeed(link, &titles, &error)) {
        Wh_Log(L"Feed %s: %d titles", link.c_str(),
               static_cast<int>(titles.size()));
        content.titles.insert(content.titles.end(), titles.begin(),
                              titles.end());
      } else if (!error.empty()) {
        Wh_Log(L"Feed %s: %s", link.c_str(), error.c_str());
        content.errors.push_back(L"RSS feed unavailable: " + link + L" (" +
                                 error + L")");
      }
      if (IsStopping()) break;
    }
    if (IsStopping()) break;

    wait_ms = links.empty()            ? INFINITE
              : content.errors.empty() ? kFeedRefreshMs
                                       : kFeedRetryMs;
    {
      std::lock_guard<std::mutex> lock(mutex_);
      content_ = std::move(content);
    }
    PostThreadMessageW(notify_thread_id_, kMsgFeedsUpdated, 0, 0);
  }
  if (com_initialized) CoUninitialize();
}

bool FeedFetcher::FetchFeed(const std::wstring& link,
                            std::vector<std::wstring>* titles,
                            std::wstring* error) {
  if (link.compare(0, 7, L"http://") != 0 &&
      link.compare(0, 8, L"https://") != 0) {
    *error = L"not an http or https link";
    return false;
  }

  ComPtr<IServerXMLHTTPRequest> request;
  HRESULT result =
      CoCreateInstance(CLSID_ServerXMLHTTP60, nullptr, CLSCTX_INPROC_SERVER,
                       IID_PPV_ARGS(&request));
  if (FAILED(result)) {
    *error = L"MSXML 6 unavailable";
    return false;
  }

  // An asynchronous request can be abandoned when the mod unloads.
  VARIANT async;
  VariantInit(&async);
  async.vt = VT_BOOL;
  async.boolVal = VARIANT_TRUE;
  VARIANT empty;
  VariantInit(&empty);
  request->setTimeouts(10000, 10000, 10000, 15000);
  result = request->open(ScopedBstr(L"GET").get(),
                         ScopedBstr(link.c_str()).get(), async, empty, empty);
  if (SUCCEEDED(result)) {
    request->setRequestHeader(ScopedBstr(L"User-Agent").get(),
                              ScopedBstr(kFeedUserAgent).get());
    result = request->send(empty);
  }
  if (FAILED(result)) {
    *error = FeedErrorReason(result);
    return false;
  }

  const ULONGLONG deadline = GetTickCount64() + kFeedTimeoutMs;
  for (;;) {
    VARIANT timeout;
    VariantInit(&timeout);
    timeout.vt = VT_I4;
    timeout.lVal = 1;  // Seconds.
    VARIANT_BOOL done = VARIANT_FALSE;
    result = request->waitForResponse(timeout, &done);
    if (FAILED(result)) {
      *error = FeedErrorReason(result);
      return false;
    }
    if (done) break;
    if (IsStopping()) {
      request->abort();
      return false;
    }
    if (GetTickCount64() > deadline) {
      request->abort();
      *error = L"timed out";
      return false;
    }
  }

  long status = 0;
  result = request->get_status(&status);
  if (FAILED(result)) {
    *error = FeedErrorReason(result);
    return false;
  }
  if (status != 200) {
    *error = L"HTTP " + std::to_wstring(status);
    return false;
  }

  // Loading the raw bytes lets MSXML detect the encoding from the XML.
  VARIANT body;
  VariantInit(&body);
  ComPtr<IXMLDOMDocument2> document;
  VARIANT_BOOL loaded = VARIANT_FALSE;
  if (SUCCEEDED(request->get_responseBody(&body)) &&
      SUCCEEDED(CoCreateInstance(CLSID_DOMDocument60, nullptr,
                                 CLSCTX_INPROC_SERVER,
                                 IID_PPV_ARGS(&document)))) {
    document->put_async(VARIANT_FALSE);
    document->put_validateOnParse(VARIANT_FALSE);
    document->put_resolveExternals(VARIANT_FALSE);
    document->load(body, &loaded);
  }
  VariantClear(&body);
  if (!loaded) {
    *error = L"not a valid feed";
    return false;
  }

  // RSS items and Atom entries, in any namespace.
  ComPtr<IXMLDOMNodeList> nodes;
  long length = 0;
  if (FAILED(document->selectNodes(
          ScopedBstr(L"//*[local-name()='item']/*[local-name()='title'] | "
                     L"//*[local-name()='entry']/*[local-name()='title']")
              .get(),
          &nodes)) ||
      FAILED(nodes->get_length(&length))) {
    *error = L"not a valid feed";
    return false;
  }
  for (long i = 0; i < length && titles->size() < kMaxTitlesPerFeed; ++i) {
    ComPtr<IXMLDOMNode> node;
    BSTR text = nullptr;
    if (FAILED(nodes->get_item(i, &node)) || !node ||
        FAILED(node->get_text(&text))) {
      continue;
    }
    std::wstring title = NormalizeTitle(text, SysStringLen(text));
    SysFreeString(text);
    if (!title.empty()) titles->push_back(std::move(title));
  }
  if (titles->empty()) {
    *error = L"no titles";
    return false;
  }
  return true;
}

// Click-through child window of the wallpaper host, driven by a message loop
// on the render thread.
class DesktopWallpaper {
 public:
  DesktopWallpaper() = default;
  DesktopWallpaper(const DesktopWallpaper&) = delete;
  DesktopWallpaper& operator=(const DesktopWallpaper&) = delete;
  ~DesktopWallpaper();

  bool Create(HWND host);
  RunResult Run();

 private:
  static LRESULT CALLBACK WindowProc(HWND window, UINT message, WPARAM wparam,
                                     LPARAM lparam);
  LRESULT HandleMessage(HWND window, UINT message, WPARAM wparam,
                        LPARAM lparam);

  bool CreateRenderer();
  void RenderFrame();
  void ScheduleFrame(double delay_ms);
  void ReloadSettings();
  void CheckHost();
  void UpdatePauseState();
  SIZE HostSize() const;
  float DpiScale() const;
  RECT CounterArea() const;
  void ApplyFrameLimit();
  void ApplyFeedContent();

  HWND host_ = nullptr;
  HWND window_ = nullptr;
  HANDLE frame_timer_ = nullptr;
  bool class_registered_ = false;
  bool session_registered_ = false;
  bool destroying_ = false;
  bool window_lost_ = false;
  bool shown_ = false;
  bool paused_ = false;
  bool session_locked_ = false;
  int width_ = 0;
  int height_ = 0;
  float dpi_scale_ = 1.0f;
  int frame_limit_ = 0;
  Settings settings_;
  std::unique_ptr<RainRenderer> renderer_;
  std::unique_ptr<FeedFetcher> feed_fetcher_;
  // Kept for renderers recreated after a lost device.
  FeedContent feed_content_;
};

DesktopWallpaper::~DesktopWallpaper() {
  destroying_ = true;
  // Waits for a download in progress to abort, about a second at most.
  feed_fetcher_.reset();
  renderer_.reset();
  if (window_) {
    if (session_registered_) WTSUnRegisterSessionNotification(window_);
    DestroyWindow(window_);
  }
  if (class_registered_) UnregisterClassW(kWindowClassName, module_instance);
  if (frame_timer_) CloseHandle(frame_timer_);
  // Repaints the wallpaper that the window covered.
  if (host_ && IsWindow(host_)) {
    RedrawWindow(host_, nullptr, nullptr,
                 RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN);
  }
}

bool DesktopWallpaper::Create(HWND host) {
  host_ = host;
  settings_ = LoadSettings();

  // A high resolution timer keeps the 24 and 30 FPS layers on time.
  frame_timer_ = CreateWaitableTimerExW(
      nullptr, nullptr, CREATE_WAITABLE_TIMER_HIGH_RESOLUTION,
      TIMER_ALL_ACCESS);
  if (!frame_timer_) {
    frame_timer_ =
        CreateWaitableTimerExW(nullptr, nullptr, 0, TIMER_ALL_ACCESS);
  }
  if (!frame_timer_) {
    Wh_Log(L"CreateWaitableTimerExW failed: %lu", GetLastError());
    return false;
  }

  WNDCLASSEXW window_class = {};
  window_class.cbSize = sizeof(window_class);
  window_class.lpfnWndProc = WindowProc;
  window_class.hInstance = module_instance;
  window_class.lpszClassName = kWindowClassName;
  if (!RegisterClassExW(&window_class) &&
      GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {
    Wh_Log(L"RegisterClassExW failed: %lu", GetLastError());
    return false;
  }
  class_registered_ = true;

  const SIZE size = HostSize();
  // Same click-through styles as Live Video Wallpaper, so the desktop icons
  // and menus get all input.
  window_ = CreateWindowExW(
      WS_EX_NOACTIVATE | WS_EX_TRANSPARENT | WS_EX_NOPARENTNOTIFY,
      kWindowClassName, L"Digital Rain", WS_CHILD | WS_DISABLED |
      WS_CLIPSIBLINGS, 0, 0, size.cx, size.cy, host_, nullptr,
      module_instance, this);
  if (!window_) {
    Wh_Log(L"CreateWindowExW failed: %lu", GetLastError());
    return false;
  }
  width_ = size.cx;
  height_ = size.cy;
  dpi_scale_ = DpiScale();
  SetWindowPos(window_, HWND_BOTTOM, 0, 0, 0, 0,
               SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
  session_registered_ =
      WTSRegisterSessionNotification(window_, NOTIFY_FOR_THIS_SESSION);
  SetTimer(window_, kHostTimerId, kHostCheckIntervalMs, nullptr);
  SetTimer(window_, kPauseTimerId, kPauseCheckIntervalMs, nullptr);

  if (!CreateRenderer()) return false;
  Wh_Log(L"Attached to host 0x%p, %dx%d, DPI scale %.2f", host_, width_,
         height_, dpi_scale_);

  feed_fetcher_ = std::make_unique<FeedFetcher>(GetCurrentThreadId());
  if (feed_fetcher_->Start()) {
    feed_fetcher_->SetLinks(ParseFeedLinks(settings_.rss_feeds));
  } else {
    Wh_Log(L"Starting the feed thread failed: %lu", GetLastError());
    feed_fetcher_.reset();
  }
  ScheduleFrame(0.0);
  UpdatePauseState();
  return true;
}

RunResult DesktopWallpaper::Run() {
  const HANDLE handles[] = {stop_event, frame_timer_};
  for (;;) {
    const DWORD wait = MsgWaitForMultipleObjectsEx(
        std::size(handles), handles, kHostCheckIntervalMs, QS_ALLINPUT,
        MWMO_INPUTAVAILABLE);
    if (wait == WAIT_OBJECT_0) return RunResult::kStopped;
    if (wait == WAIT_FAILED) {
      Wh_Log(L"MsgWaitForMultipleObjectsEx failed: %lu", GetLastError());
      return RunResult::kStopped;
    }
    if (wait == WAIT_OBJECT_0 + 1) RenderFrame();

    MSG message;
    while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
      if (message.message == WM_QUIT) return RunResult::kStopped;
      if (!message.hwnd && message.message == kMsgReloadSettings) {
        ReloadSettings();
        continue;
      }
      if (!message.hwnd && message.message == kMsgFeedsUpdated) {
        ApplyFeedContent();
        continue;
      }
      TranslateMessage(&message);
      DispatchMessageW(&message);
    }
    // Explorer destroys our window with its parent, for example when it
    // restarts the desktop.
    if (window_lost_ || !window_ || !IsWindow(window_)) {
      return RunResult::kHostLost;
    }
  }
}

LRESULT CALLBACK DesktopWallpaper::WindowProc(HWND window, UINT message,
                                              WPARAM wparam, LPARAM lparam) {
  if (message == WM_NCCREATE) {
    const auto* create = reinterpret_cast<const CREATESTRUCTW*>(lparam);
    SetWindowLongPtrW(window, GWLP_USERDATA,
                      reinterpret_cast<LONG_PTR>(create->lpCreateParams));
  }
  auto* self = reinterpret_cast<DesktopWallpaper*>(
      GetWindowLongPtrW(window, GWLP_USERDATA));
  if (!self) return DefWindowProcW(window, message, wparam, lparam);
  if (message == WM_NCDESTROY) {
    SetWindowLongPtrW(window, GWLP_USERDATA, 0);
    self->window_ = nullptr;
    if (!self->destroying_) self->window_lost_ = true;
    return DefWindowProcW(window, message, wparam, lparam);
  }
  return self->HandleMessage(window, message, wparam, lparam);
}

LRESULT DesktopWallpaper::HandleMessage(HWND window, UINT message,
                                        WPARAM wparam, LPARAM lparam) {
  switch (message) {
    case WM_NCHITTEST:
      return HTTRANSPARENT;
    case WM_MOUSEACTIVATE:
      return MA_NOACTIVATEANDEAT;
    case WM_ERASEBKGND:
      return 1;
    case WM_PAINT:
      ValidateRect(window, nullptr);
      return 0;
    case WM_TIMER:
      if (wparam == kHostTimerId) {
        CheckHost();
      } else if (wparam == kPauseTimerId) {
        UpdatePauseState();
      }
      return 0;
    case WM_WTSSESSION_CHANGE:
      if (wparam == WTS_SESSION_LOCK) {
        session_locked_ = true;
      } else if (wparam == WTS_SESSION_UNLOCK) {
        session_locked_ = false;
      }
      UpdatePauseState();
      return 0;
  }
  return DefWindowProcW(window, message, wparam, lparam);
}

bool DesktopWallpaper::CreateRenderer() {
  // A window can have only one flip model swap chain, so the old renderer
  // goes first.
  renderer_.reset();
  auto renderer = std::make_unique<RainRenderer>();
  if (!renderer->Initialize(window_, width_, height_, dpi_scale_, settings_,
                            NowMs())) {
    return false;
  }
  renderer_ = std::move(renderer);
  renderer_->SetCounterArea(CounterArea());
  ApplyFrameLimit();
  if (!feed_content_.titles.empty() || !feed_content_.errors.empty()) {
    renderer_->SetFeedContent(feed_content_);
  }
  return true;
}

void DesktopWallpaper::ApplyFeedContent() {
  if (!feed_fetcher_) return;
  feed_content_ = feed_fetcher_->TakeContent();
  Wh_Log(L"Feeds: %d titles, %d errors",
         static_cast<int>(feed_content_.titles.size()),
         static_cast<int>(feed_content_.errors.size()));
  if (renderer_ && !renderer_->SetFeedContent(feed_content_)) {
    renderer_.reset();
  }
  if (!paused_) ScheduleFrame(0.0);
}

void DesktopWallpaper::RenderFrame() {
  if (paused_) return;
  if (!renderer_ && !CreateRenderer()) {
    ScheduleFrame(kRetryDelayMs);
    return;
  }

  const double now_ms = NowMs();
  const FrameResult result = renderer_->Advance(now_ms);
  if (result == FrameResult::kDeviceLost) {
    renderer_.reset();
    ScheduleFrame(0.0);
    return;
  }
  if (result == FrameResult::kPresented && !shown_) {
    ShowWindow(window_, SW_SHOWNOACTIVATE);
    shown_ = true;
  }
  ScheduleFrame(renderer_->NextFrameMs(now_ms) - NowMs());
}

void DesktopWallpaper::ScheduleFrame(double delay_ms) {
  // Negative due times are relative, in 100 ns units.
  LARGE_INTEGER due_time;
  due_time.QuadPart =
      -std::max<LONGLONG>(1, static_cast<LONGLONG>(delay_ms * 10000.0));
  SetWaitableTimer(frame_timer_, &due_time, 0, nullptr, nullptr, FALSE);
}

void DesktopWallpaper::ReloadSettings() {
  settings_ = LoadSettings();
  if (renderer_ && !renderer_->ApplySettings(settings_, NowMs())) {
    renderer_.reset();
  }
  ApplyFrameLimit();
  if (feed_fetcher_) {
    feed_fetcher_->SetLinks(ParseFeedLinks(settings_.rss_feeds));
  }
  UpdatePauseState();
  if (!paused_) ScheduleFrame(0.0);
}

void DesktopWallpaper::CheckHost() {
  // Explorer can replace the host window, for example on cold boot when the
  // classic WorkerW shows up late.
  HWND host = FindWallpaperHost(false);
  if (host && host != host_) {
    Wh_Log(L"Moving to host 0x%p", host);
    SetParent(window_, host);
    host_ = host;
  }
  SetWindowPos(window_, HWND_BOTTOM, 0, 0, 0, 0,
               SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOREDRAW);

  const SIZE size = HostSize();
  const float dpi_scale = DpiScale();
  const bool resized = size.cx != width_ || size.cy != height_ ||
                       dpi_scale != dpi_scale_;
  if (resized) {
    width_ = size.cx;
    height_ = size.cy;
    dpi_scale_ = dpi_scale;
    Wh_Log(L"Resizing to %dx%d, DPI scale %.2f", width_, height_,
           dpi_scale_);
    SetWindowPos(window_, nullptr, 0, 0, width_, height_,
                 SWP_NOZORDER | SWP_NOACTIVATE);
    if (renderer_ &&
        !renderer_->Resize(width_, height_, dpi_scale_, NowMs())) {
      renderer_.reset();
    }
  }
  // The taskbar can move or resize the work area, and monitors can change
  // their refresh rate, at any time.
  if (renderer_) renderer_->SetCounterArea(CounterArea());
  ApplyFrameLimit();
  if (resized && !paused_) ScheduleFrame(0.0);
}

void DesktopWallpaper::UpdatePauseState() {
  const bool pause = session_locked_ ||
                     (settings_.pause_on_battery && IsOnBattery()) ||
                     (settings_.pause_when_covered && IsDesktopCovered());
  if (pause == paused_) return;
  paused_ = pause;
  Wh_Log(L"%s", paused_ ? L"Paused" : L"Resumed");
  if (paused_) {
    CancelWaitableTimer(frame_timer_);
    return;
  }
  if (renderer_) renderer_->Resume(NowMs());
  ScheduleFrame(0.0);
}

SIZE DesktopWallpaper::HostSize() const {
  RECT rect = {};
  if (GetClientRect(host_, &rect) && rect.right > 0 && rect.bottom > 0) {
    return {rect.right, rect.bottom};
  }
  return {GetSystemMetrics(SM_CXVIRTUALSCREEN),
          GetSystemMetrics(SM_CYVIRTUALSCREEN)};
}

float DesktopWallpaper::DpiScale() const {
  const UINT dpi = window_ ? GetDpiForWindow(window_) : 0;
  return dpi ? dpi / 96.0f : 1.0f;
}

void DesktopWallpaper::ApplyFrameLimit() {
  const int limit = EffectiveFrameLimit(settings_.frame_limit);
  if (limit != frame_limit_) {
    Wh_Log(L"Frame limit: %d FPS", limit);
    frame_limit_ = limit;
  }
  if (renderer_) renderer_->SetFrameLimit(limit);
}

// The primary monitor's work area in window pixels, so the taskbar doesn't
// hide the FPS counter.
RECT DesktopWallpaper::CounterArea() const {
  RECT area = {0, 0, width_, height_};
  MONITORINFO info = {};
  info.cbSize = sizeof(info);
  if (GetMonitorInfoW(MonitorFromPoint({0, 0}, MONITOR_DEFAULTTOPRIMARY),
                      &info)) {
    area = info.rcWork;
    MapWindowPoints(HWND_DESKTOP, window_, reinterpret_cast<POINT*>(&area),
                    2);
  }
  return area;
}

DWORD WINAPI RenderThreadProc(void*) {
  // Creates the message queue, so Wh_ModSettingsChanged can post to it.
  MSG message;
  PeekMessageW(&message, nullptr, WM_USER, WM_USER, PM_NOREMOVE);
  SetEvent(queue_ready_event);

  for (;;) {
    HWND host = WaitForWallpaperHost();
    if (!host) return 0;
    DesktopWallpaper wallpaper;
    if (wallpaper.Create(host)) {
      if (wallpaper.Run() == RunResult::kStopped) return 0;
      Wh_Log(L"Lost the desktop host, attaching again");
    } else if (WaitForSingleObject(stop_event, kRetryDelayMs) ==
               WAIT_OBJECT_0) {
      return 0;
    }
  }
}

}  // namespace

BOOL Wh_ModInit() {
  Wh_Log(L"Init");
  if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                              GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                          reinterpret_cast<LPCWSTR>(&Wh_ModInit),
                          &module_instance)) {
    return FALSE;
  }

  stop_event = CreateEventW(nullptr, TRUE, FALSE, nullptr);
  queue_ready_event = CreateEventW(nullptr, TRUE, FALSE, nullptr);
  if (stop_event && queue_ready_event) {
    render_thread = CreateThread(nullptr, 0, RenderThreadProc, nullptr, 0,
                                 &render_thread_id);
  }
  if (!render_thread) {
    Wh_Log(L"Starting the render thread failed: %lu", GetLastError());
    if (stop_event) CloseHandle(stop_event);
    if (queue_ready_event) CloseHandle(queue_ready_event);
    stop_event = nullptr;
    queue_ready_event = nullptr;
    return FALSE;
  }
  return TRUE;
}

void Wh_ModUninit() {
  Wh_Log(L"Uninit");
  SetEvent(stop_event);
  WaitForSingleObject(render_thread, INFINITE);
  CloseHandle(render_thread);
  CloseHandle(queue_ready_event);
  CloseHandle(stop_event);
  render_thread = nullptr;
  queue_ready_event = nullptr;
  stop_event = nullptr;
}

void Wh_ModSettingsChanged() {
  Wh_Log(L"Settings changed");
  // Before the queue exists, the thread hasn't read the settings yet.
  if (WaitForSingleObject(queue_ready_event, 0) == WAIT_OBJECT_0) {
    PostThreadMessageW(render_thread_id, kMsgReloadSettings, 0, 0);
  }
}
