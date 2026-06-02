# Build Guide — My Desktop PDF

## 1. Install toolchain

1. Visual Studio 2022 — **Desktop development with C++**
2. CMake ≥ 3.24
3. Ninja (`choco install ninja` or via VS)
4. Clone vcpkg and set `VCPKG_ROOT`

```powershell
git clone https://github.com/microsoft/vcpkg.git C:\dev\vcpkg
C:\dev\vcpkg\bootstrap-vcpkg.bat
[Environment]::SetEnvironmentVariable("VCPKG_ROOT", "C:\dev\vcpkg", "User")
```

## 2. Configure

From repository root:

```powershell
cd "D:\my tools\my-desktop-pdf"

cmake --preset windows-x64-release `
  -DCMAKE_TOOLCHAIN_FILE="$env:VCPKG_ROOT\scripts\buildsystems\vcpkg.cmake"
```

First configure downloads and builds Qt6, PDFium, SQLite, spdlog, and GTest via manifest (`vcpkg.json`). Expect a long first run.

## 3. Build

```powershell
cmake --build --preset windows-x64-release
```

Output executable:

`build\windows-x64-release\app\MyDesktopPDF.exe`

## 4. Debug preset

```powershell
cmake --preset windows-x64-debug -DCMAKE_TOOLCHAIN_FILE="$env:VCPKG_ROOT\scripts\buildsystems\vcpkg.cmake"
cmake --build --preset windows-x64-debug
```

## 5. Tests

```powershell
ctest --test-dir build\windows-x64-release
```

## 6. Troubleshooting

| Issue | Action |
|-------|--------|
| PDFium not found | Ensure vcpkg manifest mode: `-DCMAKE_TOOLCHAIN_FILE=...` |
| Qt platform plugin missing | Run from build dir or deploy Qt DLLs (`windeployqt`) |
| Path with spaces | Always quote `"D:\my tools\my-desktop-pdf"` |

## 7. Performance validation (Phase 5)

- Open a 500+ page PDF and scroll — memory should stay bounded (LRU cache).
- Zoom to 400% on a single page — bitmap capped at 4096 px edge.
- Cold start: PDFium initializes after main window is shown.

See [PERFORMANCE.md](PERFORMANCE.md).
