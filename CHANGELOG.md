# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](http://keepachangelog.com/en/1.0.0/)
and this project adheres to [Semantic Versioning](http://semver.org/spec/v2.0.0.html).

## [Unreleased]

<!--

### Added

### Changed

### Fixed

### Removed

-->

## [0.1.0] - 2024-10-12

### Added

- debugging: Add `spn_expect`-macro that allows for logging an error without throwing an exception
- platform: Add logic to set UART RX/TX buffer size
- structure/Result: Expanded functionality with more Rust sauce
- platform: Added Zephyr as platform. To accomplish this, a beautiful melange of preprocessor macros was added to the
  platform code.
- controller/pid: Added setter functions to properly choose between ON_ERROR and ON_MEASUREMENT modality of proportional
  response.

### Changed

- core: Moved traits/enum stuff to core/meta
- debugging: changed `assert` to `spn_assert`
- logging: changed `LOG` to `SPN_LOG` and respectively for `WARN`, `ERR` and `DBG` whilst defining the original macros
  if not already defined.
- logging: changed default logbuffer size `SPINE_DEBUG_BUFFER_SIZE` from 512 to 256 as 512 seemed a bit overkill
- platformio: cleaned up buildflags. Added stricter formatting errors. Added -Os to deployment builds

### Fixed

- eventsystem: added missing forward of EventData argument for `trigger()` function

### Removed

- Removed all leftovers from ArduinoFakeIT as all mocking is now handled by platform/mock

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

