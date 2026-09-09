# Module Integration Guide

This project can be configured via CMake or XMake. Alternatively, you may directly copy any subdirectory under the `src` directory into your own project.

## Static Library Linkage List

Some modules require additional static libraries to be linked during compilation. See the table below for details:

| Module (Corresponding Directory) | Windows                                | Unix (Linux/MacOS)                                         |
|----------------------------------|----------------------------------------|------------------------------------------------------------|
| `DateTime`                       | None                                   | None                                                       |
| `Event`                          | None                                   | `pthread` (Manual linkage required on BSD systems such as FreeBSD) |
| `Net`                            | `ws2_32`                               | None                                                       |
| `OS`                             | `advapi32`, `pdh`, `user32`, `shell32` | `pthread` (Manual linkage required on BSD systems such as FreeBSD) |
| `Parser`                         | None                                   | None                                                       |
| `TUI`                            | None                                   | None                                                       |

