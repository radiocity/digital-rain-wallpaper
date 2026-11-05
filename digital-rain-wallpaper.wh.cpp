// ==WindhawkMod==
// @id              digital-rain-wallpaper
// @name            Digital Rain Wallpaper
// @description     Plays a digital rain animation as the desktop background, rendered with Direct3D 11 behind the desktop icons
// @version         0.1.0
// @author          radiocity
// @github          https://github.com/radiocity
// @license         GPL-3.0
// @include         explorer.exe
// @compilerOptions -ld3d11 -ld3dcompiler_47 -ldxgi -ldwmapi -lgdi32 -lwtsapi32
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

## Settings

- **Rain color:** `#RGB` or `#RRGGBB`, green by default.
- **Speed:** 25% to 400% of the original speed.
- **Character size:** 50% to 400% of the original size, scaled further by the
  display DPI.
- **Style:** layered (original look) or simple.
- **Font:** font used for the characters. Characters the font can't draw are
  skipped. The default, MS Gothic, covers the whole set.

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
  $description: Color of the falling characters, as #RGB or #RRGGBB.
- speed: 100
  $name: Speed (%)
  $description: Animation speed relative to the original, from 25 to 400.
- fontScale: 100
  $name: Character size (%)
  $description: Character size relative to the original, from 50 to 400. The display DPI scales it further.
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

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <cwchar>
#include <iterator>
#include <memory>
#include <optional>
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

constexpr int kAtlasEm = 48;
constexpr int kAtlasPadding = 4;
constexpr int kAtlasColumns = 32;
constexpr float kAtlasMaxLod = 3.0f;

constexpr wchar_t kDefaultFontName[] = L"MS Gothic";
constexpr wchar_t kWindowClassName[] = L"DigitalRainWallpaper";
constexpr UINT kMsgReloadSettings = WM_APP + 1;
constexpr UINT_PTR kHostTimerId = 1;
constexpr UINT_PTR kPauseTimerId = 2;
constexpr UINT kHostCheckIntervalMs = 1000;
constexpr UINT kPauseCheckIntervalMs = 500;
constexpr DWORD kHostWaitIntervalMs = 1000;
constexpr DWORD kRetryDelayMs = 5000;
constexpr double kMaxIdleMs = 1000.0;
constexpr double kTransitionFrameMs = 16.0;
// Steps due within this time run now, to merge nearly coinciding frames.
constexpr double kEarlyStepMs = 1.0;

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

struct Settings {
  Color color = {0.0f, 1.0f, 0.0f, 1.0f};
  float speed = 1.0f;
  float font_scale = 1.0f;
  bool layered = true;
  std::wstring font_name = kDefaultFontName;
  bool pause_when_covered = true;
  bool pause_on_battery = false;
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

Settings LoadSettings() {
  Settings settings;

  PCWSTR color = Wh_GetStringSetting(L"color");
  if (std::optional<Color> parsed = ParseHexColor(color)) {
    settings.color = *parsed;
  } else {
    Wh_Log(L"Invalid color \"%s\", using the default", color);
  }
  Wh_FreeStringSetting(color);

  settings.speed = std::clamp(Wh_GetIntSetting(L"speed"), 25, 400) / 100.0f;
  settings.font_scale =
      std::clamp(Wh_GetIntSetting(L"fontScale"), 50, 400) / 100.0f;

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

// Texture with every character rasterized by GDI, one cell per character.
// Cell 0 is solid white, for drawing filled rectangles.
class GlyphAtlas {
 public:
  bool Build(ID3D11Device* device, ID3D11DeviceContext* context,
             const std::wstring& font_name);

  int glyph_count() const { return glyph_count_; }
  ID3D11ShaderResourceView* view() const { return view_.Get(); }

  // Places a glyph like canvas fillText(): x is the left edge and baseline
  // the alphabetic baseline, in pixels.
  GlyphInstance MakeGlyph(int glyph, float x, float baseline, float font_px,
                          const Color& color) const;
  GlyphInstance MakeSolid(float x, float y, float width, float height,
                          const Color& color) const;

 private:
  ComPtr<ID3D11ShaderResourceView> view_;
  int glyph_count_ = 0;
  float cell_width_ = 0.0f;
  float cell_height_ = 0.0f;
  float ascent_ = 0.0f;
  float texture_width_ = 1.0f;
  float texture_height_ = 1.0f;
};

bool GlyphAtlas::Build(ID3D11Device* device, ID3D11DeviceContext* context,
                       const std::wstring& font_name) {
  const std::wstring candidates =
      std::wstring(kLatinCharacters) + kJapaneseCharacters;

  HDC dc = CreateCompatibleDC(nullptr);
  if (!dc) return false;
  HFONT font = CreateFontW(-kAtlasEm, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                           DEFAULT_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS,
                           ANTIALIASED_QUALITY, FIXED_PITCH | FF_MODERN,
                           font_name.c_str());
  HGDIOBJ old_font = SelectObject(dc, font);

  TEXTMETRICW metrics = {};
  GetTextMetricsW(dc, &metrics);
  wchar_t face_name[LF_FACESIZE] = {};
  GetTextFaceW(dc, LF_FACESIZE, face_name);

  // Drops the characters the font has no glyph for.
  std::wstring glyphs;
  std::vector<WORD> indices(candidates.size());
  if (GetGlyphIndicesW(dc, candidates.c_str(),
                       static_cast<int>(candidates.size()), indices.data(),
                       GGI_MARK_NONEXISTING_GLYPHS) != GDI_ERROR) {
    for (size_t i = 0; i < candidates.size(); ++i) {
      if (indices[i] != 0xFFFF) glyphs += candidates[i];
    }
  }
  if (glyphs.empty()) glyphs = candidates;

  const int cell_width = kAtlasEm + 2 * kAtlasPadding;
  const int cell_height = metrics.tmHeight + 2 * kAtlasPadding;
  const int cell_count = static_cast<int>(glyphs.size()) + 1;
  const int rows = (cell_count + kAtlasColumns - 1) / kAtlasColumns;
  const int width = kAtlasColumns * cell_width;
  const int height = rows * cell_height;

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
    DeleteObject(font);
    DeleteDC(dc);
    return false;
  }
  HGDIOBJ old_bitmap = SelectObject(dc, bitmap);

  RECT solid = {0, 0, cell_width, cell_height};
  FillRect(dc, &solid, static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH)));
  SetTextColor(dc, RGB(255, 255, 255));
  SetBkMode(dc, TRANSPARENT);
  SetTextAlign(dc, TA_LEFT | TA_TOP | TA_NOUPDATECP);
  for (size_t i = 0; i < glyphs.size(); ++i) {
    const int cell = static_cast<int>(i) + 1;
    TextOutW(dc, (cell % kAtlasColumns) * cell_width + kAtlasPadding,
             (cell / kAtlasColumns) * cell_height + kAtlasPadding, &glyphs[i],
             1);
  }
  GdiFlush();

  // GDI draws grayscale coverage; every channel of the texture gets it.
  std::vector<uint32_t> pixels(static_cast<size_t>(width) * height);
  const uint32_t* source = static_cast<const uint32_t*>(bits);
  for (size_t i = 0; i < pixels.size(); ++i) {
    uint32_t coverage = std::max({(source[i] >> 16) & 0xFF,
                                  (source[i] >> 8) & 0xFF, source[i] & 0xFF});
    pixels[i] = coverage * 0x01010101u;
  }

  SelectObject(dc, old_bitmap);
  DeleteObject(bitmap);
  SelectObject(dc, old_font);
  DeleteObject(font);
  DeleteDC(dc);

  D3D11_TEXTURE2D_DESC desc = {};
  desc.Width = width;
  desc.Height = height;
  desc.MipLevels = 0;
  desc.ArraySize = 1;
  desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
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
  context->UpdateSubresource(texture.Get(), 0, nullptr, pixels.data(),
                             width * sizeof(uint32_t), 0);
  context->GenerateMips(view.Get());

  view_ = view;
  glyph_count_ = static_cast<int>(glyphs.size());
  cell_width_ = static_cast<float>(cell_width);
  cell_height_ = static_cast<float>(cell_height);
  ascent_ = static_cast<float>(metrics.tmAscent);
  texture_width_ = static_cast<float>(width);
  texture_height_ = static_cast<float>(height);
  Wh_Log(L"Glyph atlas: font \"%s\", %d of %d characters, %dx%d", face_name,
         glyph_count_, static_cast<int>(candidates.size()), width, height);
  return true;
}

GlyphInstance GlyphAtlas::MakeGlyph(int glyph, float x, float baseline,
                                    float font_px, const Color& color) const {
  const int cell = glyph + 1;
  const float scale = font_px / kAtlasEm;
  GlyphInstance instance;
  instance.x = x - kAtlasPadding * scale;
  instance.y = baseline - (kAtlasPadding + ascent_) * scale;
  instance.width = cell_width_ * scale;
  instance.height = cell_height_ * scale;
  instance.u = (cell % kAtlasColumns) * cell_width_ / texture_width_;
  instance.v = (cell / kAtlasColumns) * cell_height_ / texture_height_;
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
    double last_step_ms = 0.0;
  };

  bool CreateDevice();
  void BindSharedState();
  bool CreateTargets();
  bool ResetAnimation(double now_ms);
  double StepIntervalMs(const RainLayer& layer) const;
  float BlurAmount(double now_ms) const;
  float Random() { return distribution_(random_engine_); }
  int RandomGlyph();

  void StepLayer(RainLayer& layer);
  void BlinkOverlay();
  void Composite(double now_ms);
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
  return CreateDevice() &&
         atlas_.Build(device_.Get(), context_.Get(), settings_.font_name) &&
         CreateTargets() && ResetAnimation(now_ms);
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
  return CreateTargets() && ResetAnimation(now_ms);
}

bool RainRenderer::ApplySettings(const Settings& settings, double now_ms) {
  const bool font_changed = settings.font_name != settings_.font_name;
  const bool layout_changed = font_changed ||
                              settings.layered != settings_.layered ||
                              settings.font_scale != settings_.font_scale;
  settings_ = settings;
  if (font_changed &&
      !atlas_.Build(device_.Get(), context_.Get(), settings_.font_name)) {
    return false;
  }
  return !layout_changed || ResetAnimation(now_ms);
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
    const int columns = std::max(1, static_cast<int>(width_ / layer.font_px));
    layer.drops.assign(columns, 0);
    layer.delays.assign(columns, 0.0f);
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

int RainRenderer::RandomGlyph() {
  const int count = atlas_.glyph_count();
  return std::min(static_cast<int>(Random() * count), count - 1);
}

void RainRenderer::Resume(double now_ms) {
  for (RainLayer& layer : layers_) {
    layer.last_step_ms = now_ms - StepIntervalMs(layer);
  }
  last_blink_ms_ = now_ms - kOverlayBlinkMs;
  next_blur_toggle_ms_ = now_ms + kOverlayBlurToggleMs;
}

FrameResult RainRenderer::Advance(double now_ms) {
  bool dirty = false;
  for (RainLayer& layer : layers_) {
    const double interval = StepIntervalMs(layer);
    if (now_ms + kEarlyStepMs < layer.last_step_ms + interval) continue;
    StepLayer(layer);
    layer.last_step_ms += interval;
    if (now_ms - layer.last_step_ms > interval) layer.last_step_ms = now_ms;
    dirty = true;
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

  if (!dirty) return FrameResult::kIdle;
  Composite(now_ms);
  const HRESULT result = swap_chain_->Present(0, 0);
  if (result == DXGI_ERROR_DEVICE_REMOVED ||
      result == DXGI_ERROR_DEVICE_RESET) {
    Wh_Log(L"Direct3D device lost: 0x%08lX",
           static_cast<unsigned long>(result));
    return FrameResult::kDeviceLost;
  }
  return FrameResult::kPresented;
}

double RainRenderer::NextFrameMs(double now_ms) const {
  double next_ms = now_ms + kMaxIdleMs;
  for (const RainLayer& layer : layers_) {
    next_ms = std::min(next_ms, layer.last_step_ms + StepIntervalMs(layer));
  }
  if (settings_.layered) {
    next_ms = std::min(
        {next_ms, last_blink_ms_ + kOverlayBlinkMs, next_blur_toggle_ms_});
    if (now_ms - blur_toggle_ms_ < kBlurTransitionMs) {
      next_ms = std::min(next_ms, now_ms + kTransitionFrameMs);
    }
  }
  return next_ms;
}

// One frame of matrixRain() in the original.
void RainRenderer::StepLayer(RainLayer& layer) {
  const RainLayerConfig& config = layer.config;
  const float font_px = layer.font_px;

  glyphs_.clear();
  // Fades the previous frames, which leaves trails behind the drops.
  glyphs_.push_back(atlas_.MakeSolid(0.0f, 0.0f, width_, height_,
                                     {0.0f, 0.0f, 0.0f, kTrailFadeAlpha}));
  for (size_t i = 0; i < layer.drops.size(); ++i) {
    const float x = i * font_px;
    const float y = layer.drops[i] * font_px;
    // The original also draws below the canvas; those glyphs are skipped.
    if (y - 2.0f * font_px < height_) {
      glyphs_.push_back(
          atlas_.MakeGlyph(RandomGlyph(), x, y, font_px, settings_.color));
    }

    if (layer.delays[i] <= 0.0f) {
      ++layer.drops[i];
      layer.delays[i] = Random() * (config.delay_factor / config.speed_factor);
    } else {
      layer.delays[i] -= 1.0f;
    }

    if (y > height_ && Random() > 1.0f - kDropResetChance) {
      layer.drops[i] = 0;
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

  if (!settings_.layered) return;

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

bool IsOnBattery() {
  SYSTEM_POWER_STATUS status;
  return GetSystemPowerStatus(&status) && status.ACLineStatus == 0;
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
  Settings settings_;
  std::unique_ptr<RainRenderer> renderer_;
};

DesktopWallpaper::~DesktopWallpaper() {
  destroying_ = true;
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
  return true;
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
  if (size.cx == width_ && size.cy == height_ && dpi_scale == dpi_scale_) {
    return;
  }
  width_ = size.cx;
  height_ = size.cy;
  dpi_scale_ = dpi_scale;
  Wh_Log(L"Resizing to %dx%d, DPI scale %.2f", width_, height_, dpi_scale_);
  SetWindowPos(window_, nullptr, 0, 0, width_, height_,
               SWP_NOZORDER | SWP_NOACTIVATE);
  if (renderer_ &&
      !renderer_->Resize(width_, height_, dpi_scale_, NowMs())) {
    renderer_.reset();
  }
  if (!paused_) ScheduleFrame(0.0);
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
