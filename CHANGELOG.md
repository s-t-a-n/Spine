# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](http://keepachangelog.com/en/1.0.0/)
and this project adheres to [Semantic Versioning](http://semver.org/spec/v2.0.0.html).

## [Unreleased]

-

<!--

### Added

### Changed

### Fixed

### Removed

-->

## [0.0.1] - 2024-09-23

### Added

- **Core**: Provides (SI) units and datetime utilities, debugging tools, exception handling, and logging.
- **Event System**: Manages runtime through an event-driven architecture.
- **Filters**: Includes bandpass, EWMA, inverting, transposition, and passthrough filters bundled in a filter stack.
- **I/O**: Implements buffered streams and newline-terminated 'transactions.'
- **Platform Abstraction**: Layers for GPIO, vendor HAL, and stdio; supports Arduino and native x86.
- **Data Structures**: Array, ring buffer, vector, stack, pool, static string, and a result type adhering to the single
  allocation principle.
- **Controllers**: Includes a PID controller and SR-Latch.

