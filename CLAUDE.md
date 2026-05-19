# CLAUDE.md

Guidance for Claude Code when working in this repository.

## Project

Custom firmware for the **M5Stack Cardputer** that turns the device into a
graphical LG TV remote. The product priority is a daily-driver remote feel, not
a debug console. See `README.md` for the full spec.

- **MCU**: ESP32-S3 (Cardputer is a StampS3 + keyboard + 240x135 TFT)
- **IR LED**: GPIO 44 (set in `include/config.h`)
- **Display**: 240x135, ST7789, rotated to landscape

## Stack

| Layer       | Choice                          | Why                              |
|-------------|---------------------------------|----------------------------------|
| Build       | PlatformIO Core                 | Reproducible, board=`m5stack-stamps3` |
| Framework   | Arduino-ESP32                   | M5Cardputer + IRremote ecosystem |
| Display     | M5GFX (LovyanGFX-based)         | Cardputer-native; M5Canvas for double buffering |
| IR transmit | `crankyoldgit/IRremoteESP8266`  | Mature NEC implementation        |
| Std         | C++17 (`gnu++17` in `platformio.ini`) | enums, `using`-aliases, nested namespaces |

Library versions are pinned in `platformio.ini`. Do not unpin without reason.

## Architecture

```
src/main.cpp
   └── app::App                         single global, owns everything
        ├── ir::IrTransmitter           wraps IRsend on kIrLedPin
        ├── ir::RemoteController        CommandId -> active profile -> tx
        ├── profiles::ProfileManager    holds N built-in Profile pointers
        ├── input::Keyboard             poll() -> Event (Nav/Char/Confirm/...)
        ├── ui::Toast                   transient feedback strip
        ├── M5Canvas canvas_            off-screen 240x135 framebuffer
        └── app::ScreenManager          slot-based screen switcher
             ├── MainRemoteScreen       PWR/INP/MUTE + Dpad/OK + VOL/CH + nav row
             ├── ExtraKeysScreen        digits, color keys, INFO/EXIT/MENU
             └── SettingsScreen         profile switch + last-tx readout
```

### Hard rules

1. **UI never knows IR hex codes.** Buttons reference `ir::CommandId` only.
2. **IR layer never knows about pixels.** `RemoteController::send(CommandId)`
   looks up the active `Profile` and dispatches via `IrTransmitter`.
3. **One place per concern.** Tunable constants in `include/config.h`. IR codes
   in `src/profiles/lg_*.cpp`. Colors in `src/ui/Theme.h`. Key bindings in
   `src/input/Keyboard.cpp`.
4. **No giant files.** Each screen is its own .cpp; each profile its own .cpp.
5. **No designated initializers for `Button`** - PlatformIO's GCC config rejects
   `.field = ...`. Use positional aggregate init or the `mk(...)` factory in
   `MainRemoteScreen.cpp`.

### Rendering model

Every screen renders into `App::canvas_` (an `M5Canvas` sprite). At end of each
loop tick `canvas_.pushSprite(&display, 0, 0)` does a single DMA blit -
**no flicker**, no partial frames.

`Screen::render()` clears the canvas with `fillScreen` only when
`needsFullRedraw_` is set. Otherwise it overdraws buttons in place. Set
`needsFullRedraw_ = true` (via `requestFullRedraw()`) on any change that
affects layout, header, footer, or background.

Toast paints over the footer strip; when it expires `Toast::update()` returns
true and the App forces a full redraw to clear stale toast pixels.

### Input model (post-refactor)

| Cardputer key        | Effect                                            |
|----------------------|---------------------------------------------------|
| `; , . /`            | Arrow events; on remote screens fire TV Up/Down/Left/Right (auto-repeat on hold). On Settings screen, move focus. |
| Enter                | Confirm event; on remote screens fires TV OK. On Settings, activates focused button. |
| `` ` ``              | Jump to Main Remote                               |
| Tab                  | Toggle Main Remote / Extra Keys                   |
| `p`                  | Power shortcut (works on any screen)              |
| `s`                  | Open Setup                                        |
| `h`                  | Toggle on-screen hotkey hint badges               |
| Any letter           | Tries to match a button's `hotkey` on this screen |
| `0-9` on Extra Keys  | Sends the digit                                   |
| `[ / ]` on Setup     | Switch IR profile                                 |

Hold-to-repeat is implemented in `input::Keyboard::processHold` for Enter and
each of the four arrows. Repeatability of the resulting command is gated by
`IrCommand::repeatable` in the profile table (Vol/Ch/Arrows = true, Power = false).

### Profile system

A `profiles::Profile` is a `const` table of `{CommandId, IrCommand}` rows
compiled into flash. Adding a new TV variant:

1. Create `src/profiles/lg_<name>.{h,cpp}` modeled on `lg_alt.{h,cpp}`.
2. Register it in `profiles::ProfileManager::begin()` (file
   `src/profiles/ProfileManager.cpp`).
3. Optionally bump `config::kDefaultProfileIndex`.

Unmapped commands draw a diagonal strike-through and report `UNMAPPED` in the
Settings "LAST SENT" block.

## Conventions

- Filenames: PascalCase for classes (`Button.cpp`), snake_case for profile
  tables (`lg_default.cpp`).
- Namespaces: `app`, `ui`, `ui::screens`, `input`, `ir`, `profiles`. Match the
  directory.
- No emojis in source.
- Comments: only for non-obvious WHY. Don't narrate what well-named code already says.
- Don't introduce abstractions beyond what the task requires.
- Logical command names go through `ir::commandIdName()` (single source of
  truth for human-readable labels).

## Common tasks (cheat sheet)

| Want to...                            | Edit                                            |
|---------------------------------------|-------------------------------------------------|
| Move/resize a button                  | `src/ui/screens/<Screen>.cpp::build()`          |
| Add a new button                      | Same + add `CommandId` if new                   |
| Change a color                        | `src/ui/Theme.h`                                |
| Change an LG IR code                  | `src/profiles/lg_default.cpp`                   |
| Add a TV-brand profile                | New `.cpp` in `src/profiles/` + register        |
| Add a logical command (e.g. Subtitle) | Enum in `CommandId.h` + name in `CommandId.cpp` + row in each profile |
| Change hold-repeat timings            | `include/config.h::kHoldRepeat*`                |
| Change toast duration                 | `include/config.h::kToastMs`                    |
| Add a hotkey binding                  | Set the button's `hotkey` char in screen build  |
| Change a global keybinding            | `src/input/Keyboard.cpp::poll()`                |

## Build / flash

`make` targets are defined in `Makefile`. Common ones:

| Target          | Effect                                          |
|-----------------|-------------------------------------------------|
| `make build`    | `pio run`                                       |
| `make flash`    | Build + upload                                  |
| `make monitor`  | Serial monitor                                  |
| `make clean`    | Wipe `.pio/build/`                              |
| `make size`     | Print firmware size summary                     |
| `make dev`      | Flash + monitor in one shot                     |

The first `make build` downloads the Xtensa toolchain + Arduino-ESP32 framework
(~2 GB cache in `~/.platformio`). Subsequent incremental builds are ~5-10 s.

## Don't

- Don't add features beyond the requested task.
- Don't refactor opportunistically inside a bug fix.
- Don't reintroduce forward-declarations of `M5GFX` - `M5GFX.h` declares it via
  `using`-alias, so a `class M5GFX;` decl shadows it and breaks compilation.
- Don't include `<LovyanGFX.h>` (not a real header in M5GFX). Use `<M5GFX.h>`.
- Don't track `.pio/` in git (see `.gitignore`).
- Don't pass `--no-verify` or skip CI hooks.

## Known unknowns

- LG color/media codes are LOW confidence in `lg_default.cpp` - verify on
  target hardware via the Settings "TEST POWER" screen and adjust.
- Some Cardputer keyboard revisions may report different chars for Tab / Esc;
  see `input::Keyboard` if Tab/Esc shortcuts misbehave.
- IR LED drive current depends on the Cardputer hardware rev; effective range
  is ~3-5 m with line of sight.
