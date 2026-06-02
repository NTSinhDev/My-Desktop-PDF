# Architecture — My Desktop PDF

## Layers

```
ui (Qt views) → modules (use cases) → core (domain + interfaces)
                                      ↓
                              infrastructure (adapters)
```

- **core** has no dependency on Qt or PDFium.
- **infrastructure** implements `IPdfEngine`, `IPageRenderer`, repositories, LRU cache.
- **ui** binds events from `EventBus` and user input to services.

## Key subsystems

### PDFium adapter

- `PdfiumLibrary` — lazy `FPDF_InitLibrary`
- `PdfiumEngine` — document map, per-document mutex
- `PdfiumRenderer` — async render via thread pool + LRU cache

### Rendering

- Virtualized `PdfScrollView` requests visible page range ±2
- Cache key: `(document, page, scale×1000, dpr×1000)`
- Max bitmap edge: 4096 px

### Search

- Background sequential page scan with cancel flag
- Highlights drawn as semi-transparent rects on tiles

### Persistence

- `%LOCALAPPDATA%/MyDesktopPDF/app.db`
- Tables: `recent_files`, `bookmarks`

## Threading

| Thread | Role |
|--------|------|
| Qt main | UI, SQLite (short transactions) |
| Render pool (3) | PDFium render + search |

Results posted to UI via `QMetaObject::invokeMethod` / `EventBus`.

## Patterns

Repository, Adapter, Factory (composition root in `ApplicationContext`), Observer (`EventBus`), MVVM-ready (MVP uses `MainWindow` orchestration).

## Future extension

- Multi-tab documents
- Thumbnail sidebar (reuse render pipeline at low scale)
- macOS/Linux path abstraction
