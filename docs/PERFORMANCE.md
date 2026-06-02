# Performance Notes

## Targets (MVP)

| Metric | Target | Implementation |
|--------|--------|----------------|
| Cold start | < 1 s perceived | Show UI first; `FPDF_InitLibrary` after `show()` |
| Large PDF | 500–2000 pages | Virtualized scroll; no full rasterization |
| Memory | Bounded | LRU 48 entries, ~256 MB byte cap |
| High zoom | No OOM | Max edge 4096 px |

## Manual checks

1. **Scroll test** — Open a large PDF, scroll top-to-bottom; Task Manager working set should plateau.
2. **Zoom test** — Ctrl+wheel to 400%; verify page still renders without crash.
3. **Search test** — Search common term; UI remains responsive; status shows progress.
4. **Startup** — Launch without file argument; window appears before heavy PDFium work.

## Tuning constants

- `LruPageCache`: 48 entries, 256 MB — `application_context.cpp`
- `kMaxEdgePixels`: 4096 — `pdfium_renderer.cpp`
- Render threads: 3 — `render_thread_pool`
