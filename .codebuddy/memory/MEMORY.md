# Project Memory

## Technical Decisions
- **Project**: Tuxiaolai_Toolbox (兔小赖工具箱), Qt 6.10.2 + C++17, CMake build system
- **DPI handling**: Use `qputenv("QT_ENABLE_HIGHDPI_SCALING", "0")` before QApplication construction to disable DPI scaling in Qt6. `Qt::AA_DisableHighDpiScaling` was removed in Qt6 and has no effect.
- **Layout**: Sidebar (50px fixed) | Activity Bar ↔ Content Area (QSplitter)
