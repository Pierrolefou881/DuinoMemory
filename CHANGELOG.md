# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added
- Added `CHANGELOG.md` to track project updates.

### Changed
- `S_ptr::count()` now returns `size_t` instead of `uint16_t` for better 
compatibility with larger projects.
- Updated README.md to document potential memory allocation failures due to heap 
fragmentation and Arduino platform limitations.

## [1.0.0] - 2026-02-05

### Added
- Initial release of **DuinoMemory** with `U_ptr` (unique pointer) and `S_ptr` (shared pointer) for memory management on Arduino.
