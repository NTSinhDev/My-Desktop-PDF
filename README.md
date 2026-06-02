# My Desktop PDF

Offline desktop PDF reader for Windows x64, built with **C++20**, **Qt 6**, and **PDFium**.

## Features (MVP)

- Open PDF (file dialog, drag-and-drop, command-line path)
- Multi-page continuous scroll
- Zoom in/out (Ctrl+wheel, Ctrl+/-, Ctrl+0)
- Text search with highlight (Ctrl+F, F3 / Shift+F3)
- Text selection and copy (Ctrl+C)
- Recent files and bookmarks (SQLite)
- Keyboard shortcuts

## Requirements

- Windows 10/11 x64
- Visual Studio 2022 with C++ desktop workload
- CMake 3.24+
- [vcpkg](https://github.com/microsoft/vcpkg)
- Ninja (recommended)

## Quick build

```powershell
cd "D:\my tools\my-desktop-pdf"
$env:VCPKG_ROOT = "C:\path\to\vcpkg"

cmake --preset windows-x64-release `
  -DCMAKE_TOOLCHAIN_FILE="$env:VCPKG_ROOT\scripts\buildsystems\vcpkg.cmake"

cmake --build --preset windows-x64-release
```

Run:

```powershell
.\build\windows-x64-release\app\MyDesktopPDF.exe
```

See [docs/BUILD.md](docs/BUILD.md) for details.

## Project layout

- `app/` — entry point and composition root
- `core/` — domain models and interfaces (no Qt/PDFium)
- `modules/` — use-case orchestration
- `infrastructure/` — PDFium, SQLite, cache, threading
- `ui/` — Qt views (MVVM-ready)
- `tests/` — GoogleTest

## License

MIT — see [LICENSE](LICENSE). PDFium is BSD-licensed; see [docs/LICENSE-NOTICE.md](docs/LICENSE-NOTICE.md).
"# My-Desktop-PDF" 
