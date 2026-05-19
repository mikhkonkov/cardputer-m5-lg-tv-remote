# Cardputer LG Remote

Custom firmware for the M5Stack **Cardputer** that turns it into a real-feeling
LG TV remote control. Designed as a daily-driver remote, not a debug console.

---

## SECTION 1 - Solution overview

- **Stack**: PlatformIO + Arduino framework + `M5Cardputer` + `IRremoteESP8266`.
  Mature, well-supported on ESP32-S3, fast iteration, easy local builds.
- **Approach**: graphical remote UI rendered on the 240x135 display. Cardputer
  keys are only navigation; the user "presses" virtual remote buttons.
- **IR**: profile-driven. Logical command IDs (`Power`, `VolUp`, ...) are mapped
  to physical NEC codes in a single replaceable table per profile.
- **Architecture**: hard split between UI, input, IR transport, and profiles so
  that swapping IR codes never requires UI changes (and vice versa).

## SECTION 2 - Architecture

```
              +---------------------+
              |   src/app/App       |   wiring + main loop
              +----+----+-----+-----+
                   |    |     |
        +----------+    |     +-----------------+
        |               |                       |
+-------v------+ +------v---------+ +-----------v-----------+
| src/input/   | | src/ui/Screen  | | src/ir/RemoteController|
|  Keyboard    | |  + screens/*   | |   uses ProfileManager  |
+--------------+ +-------+--------+ +-----------+------------+
                         |                      |
                         |                +-----v------+
                         |                | IrTransmitter
                         |                | (IRsend ESP)|
                         |                +-----+------+
                         |                      |
                         |                +-----v------+
                         |                | profiles/  |
                         |                |  lg_default|
                         |                |  lg_alt    |
                         |                +------------+
                         |
                         v
                  ui/Theme, Button, Toast
```

**State model.** A single `ScreenManager` holds the current screen pointer.
Three screens (`MainRemote`, `ExtraKeys`, `Settings`) are instantiated once and
swapped by id. No screen stack; the user always knows where they are.

**Profile system.** Each `Profile` is a `const` table of `{CommandId, IrCommand}`
rows. Profiles are compiled in (so the MVP needs no filesystem) but the table
is the only place IR values live. Adding a new TV variant = drop a new
`.cpp` table and register it in `ProfileManager::begin()`.

## SECTION 3 - UI specification

### Screens

1. **Main Remote** - power/input/mute top row, center D-pad cluster with OK,
   right-side volume + channel column, bottom Back/Menu/Home/Exit row.
2. **Extra Keys** - 3x3+1 digit grid, INFO/EXIT/MENU column, color row.
3. **Settings** - profile selector, last-TX readout, mapping count. Visually
   distinct from the main UX so it never feels mixed with normal use.

### Header

A 14 px strip across the top of every screen shows:

- Screen title (left).
- Active profile brand (right side).
- Live battery percent (far right). Goes red below 20 %.

The header is redrawn every frame so the battery indicator stays current.

### Input model

Each screen renders a fixed-size button array. The focused button has a
two-pixel white border for visibility.

- On **Main Remote** and **Extra Keys**, arrow keys (`; , . /`) and Enter map
  *directly* to the TV's D-pad and OK. The focus highlight follows the matching
  button so the user still sees what was sent, but the user does not need to
  steer focus to operate the TV. Volume / channel / digits are reached either
  by their own hotkey or by holding Enter on the focused button.
- On **Settings**, arrows move focus geometrically (2D nearest-neighbor search)
  and Enter activates the focused control. This is intentional: Settings is a
  configuration screen, not a remote.

Press `h` on any screen to toggle small per-button hint badges that show the
physical Cardputer key bound to that button. Each remote button can carry a
single-letter `hotkey`; pressing it sends that button's command immediately
regardless of focus.

### Interaction states

- **Normal**: theme fill for the button group.
- **Focused**: white double border.
- **Pressed**: inverted fill for ~110 ms after a send.
- **Unmapped**: diagonal strike-through in dim gray; sending it logs UNMAPPED
  in Setup.
- **Toast**: 12 px strip at the bottom showing the label of the sent command,
  green for OK / red for FAIL / shown for ~380 ms.

### Key bindings (physical Cardputer keyboard)

| Key        | Action                                                           |
|------------|------------------------------------------------------------------|
| `; , . /`  | Send TV Up/Down/Left/Right on remote screens (hold = repeat). Move focus on Settings. |
| Enter      | Send TV OK on remote screens. Activate focused button on Settings. |
| `` ` ``    | Jump to Main Remote                                              |
| Tab        | Toggle Main Remote / Extra Keys                                  |
| `p`        | Send Power immediately (works anywhere)                          |
| `s`        | Open Settings                                                    |
| `h`        | Toggle per-button hotkey hint badges                             |
| Esc        | Return to Main Remote                                            |
| Any letter | If a button on this screen has that letter as `hotkey`, send it  |
| `0..9`     | (on Extra Keys) send that digit directly                         |
| `[` / `]`  | (on Settings) switch IR profile                                  |

Hold-repeat is gated per command via `IrCommand::repeatable` - arrows and
volume/channel repeat on hold, Power and OK do not.

These are convenience shortcuts. The on-screen remote is the primary UX -
the user does not need to memorize keys to operate it.

## SECTION 4 - Project tree

```
cardputer-lg-remote/
  platformio.ini
  Makefile                   // pio wrapper - build / flash / monitor / clean
  CLAUDE.md                  // architecture notes for future AI sessions
  README.md
  include/
    config.h                 // tunable constants
  src/
    main.cpp                 // Arduino entry
    app/
      App.h / App.cpp        // wiring + main loop
      ScreenManager.h / .cpp // three-slot screen switcher
    ui/
      Theme.h                // colors / sizes
      Button.h / Button.cpp  // virtual button rendering
      Screen.h / Screen.cpp  // base screen + focus search
      Toast.h / Toast.cpp    // transient feedback strip
      screens/
        MainRemoteScreen.h / .cpp
        ExtraKeysScreen.h  / .cpp
        SettingsScreen.h   / .cpp
    input/
      Keyboard.h / Keyboard.cpp   // raw keys -> Events (with hold-repeat)
    ir/
      CommandId.h / CommandId.cpp // logical command enum
      IrCommand.h                 // physical command record
      IrTransmitter.h / .cpp      // IRremoteESP8266 wrapper
      RemoteController.h / .cpp   // CommandId -> profile -> transmitter
    profiles/
      Profile.h                   // table struct
      ProfileManager.h / .cpp     // registry + active selection
      lg_default.h / .cpp         // LG NEC32 standard codes
      lg_alt.h / .cpp             // alternative LG variant
```

## SECTION 5 - Code

See the corresponding files. Highlights:

- `src/ir/IrCommand.h` - data class for one physical command (protocol + bits +
  repeatable flag + defined flag).
- `src/profiles/lg_default.cpp` - the LG codes, in one place, easy to edit.
- `src/ui/Screen.cpp::pickNeighbor` - 2D focus search (Settings screen only).
- `src/ui/Screen.cpp::sendCommand` - shared helper used by the remote screens
  to dispatch a TV command and optionally flash the matching button.
- `src/input/Keyboard.cpp` - hold-to-repeat detection on Enter and each of the
  four arrow keys (per-direction `HoldState`).
- `src/app/App.cpp` - event dispatch and the main render loop. Renders into an
  off-screen `M5Canvas` and blits once per frame via `pushSprite` for
  flicker-free updates.

## SECTION 6 - LG profile mapping

The default profile (`profiles/lg_default.cpp`) uses LG's published NEC32
codes, address prefix `0x20DF`, carrier 38 kHz. Confidence per group:

| Group                           | Confidence | Notes                                  |
|---------------------------------|------------|----------------------------------------|
| Power, Mute, Input              | HIGH       | Same across nearly all LG remotes      |
| Volume +/-, Channel +/-         | HIGH       | Repeatable; works on all LG TVs        |
| D-pad + OK                      | HIGH       |                                        |
| Digits 0-9                      | HIGH       |                                        |
| Back                            | MEDIUM     | Some older sets used different mapping |
| Home (WebOS), Menu, Exit        | MEDIUM     | WebOS vs. older LG diverges            |
| Settings, Info                  | MEDIUM     |                                        |
| Color (R/G/Y/B)                 | LOW        | Verify per model                       |
| Media (Play/Pause/...)          | LOW        | Left undefined; uncomment to enable    |

Anything uncertain is **isolated to the profile table only** - replace a
hex value and rebuild; no other code changes.

The alternate profile `lg_alt.cpp` is provided as a starting point for
older / regional LG sets; it deliberately omits WebOS keys so the
"unmapped" strike-through is visible on Settings.

## SECTION 7 - Build and flash instructions

### Prerequisites

- [PlatformIO Core](https://platformio.org/install/cli). On macOS with Homebrew
  Python, `pip` is blocked by PEP 668; use `pipx install platformio` instead.
- USB cable. Cardputer enumerates as a native USB-CDC device on macOS / Linux /
  Windows once you press the reset button while holding `G0` (bottom-left key).

### Build / flash / monitor

A `Makefile` wraps the common `pio` commands. From the project root:

| Target          | Effect                                          |
|-----------------|-------------------------------------------------|
| `make build`    | Compile firmware                                |
| `make flash`    | Compile + upload to the connected Cardputer     |
| `make monitor`  | Serial monitor at 115200 baud                   |
| `make dev`      | Flash then immediately open the monitor         |
| `make size`     | Print RAM / flash usage                         |
| `make clean`    | Wipe `.pio/build/`                              |
| `make fullclean`| Wipe `.pio/` entirely (rebuilds libdeps too)    |

Override the upload port if multiple devices are connected:

```sh
UPLOAD_PORT=/dev/cu.usbmodem101 make flash
```

The first `make build` downloads the Xtensa toolchain + Arduino-ESP32 framework
(~2 GB cache in `~/.platformio`). Subsequent incremental builds are ~5-10 s.

Put the device into download mode if `make flash` fails: hold `G0`
(bottom-left key), tap reset, release `G0`, then re-run `make flash`. Once
flashed, press reset once to start the firmware.

## SECTION 8 - Customization guide

| Want to change...        | Edit                                                |
|--------------------------|-----------------------------------------------------|
| Button positions / size  | `src/ui/screens/*Screen.cpp` (per-screen `build()`) |
| Button labels            | Same files - second arg of `Button{...}`            |
| Colors                   | `src/ui/Theme.h`                                    |
| IR codes                 | `src/profiles/lg_default.cpp` (or add new profile)  |
| Add a new profile        | Copy `lg_alt.cpp`, register in `ProfileManager::begin()` |
| Default profile on boot  | `include/config.h::kDefaultProfileIndex`            |
| Hold-repeat timings      | `include/config.h::kHoldRepeatDelayMs`, `kHoldRepeatPeriodMs` |
| Toast duration           | `include/config.h::kToastMs`                        |
| IR LED pin (rare)        | `include/config.h::kIrLedPin`                       |
| Brightness               | `include/config.h::kDefaultBrightness`              |
| Key bindings             | `src/input/Keyboard.cpp`                            |

To add a new logical command (e.g. `Subtitle`):

1. Add `Subtitle` to the `CommandId` enum in `src/ir/CommandId.h`.
2. Add a label case in `commandIdName()` in `src/ir/CommandId.cpp`.
3. Add a row in any profile's `kMap[]` array.
4. Place a button referencing `CommandId::Subtitle` on whichever screen.

## SECTION 9 - Acceptance checklist

Boot:
- [ ] Display shows "REMOTE" header with brand "LG" and live battery percent
      on the right.
- [ ] All Main Remote buttons render; focus on OK; bottom row reads
      BCK / MENU / HOME / EXIT.

Main Remote screen:
- [ ] Arrow keys (`; , . /`) send TV D-pad commands (the focus highlight moves
      to the matching arrow button and flashes).
- [ ] Holding an arrow key auto-repeats the direction.
- [ ] Enter sends TV OK (single press only; OK does not auto-repeat).
- [ ] `p` sends Power (and the TV visibly toggles).
- [ ] Pressing `h` toggles the on-screen hotkey hint badges.
- [ ] Letter hotkeys fire the labeled command (e.g. `m` for Mute, `b` for
      Back, `o` for Menu, `n` for Home, `e` for Exit).
- [ ] Tab switches to the Extra Keys screen and back.
- [ ] Pressing `` ` `` from any screen returns to Main Remote.

Extra Keys screen:
- [ ] Digit grid is visible.
- [ ] Typing a digit on the physical keyboard fires that digit immediately.
- [ ] Color buttons render in their colors. `r g y u` hotkeys send R/G/Y/B.
- [ ] Arrows still send TV D-pad and Enter still sends OK.

Settings:
- [ ] `s` opens it; shows current profile, model hint, and mapping count.
- [ ] Arrows on Settings move focus (do **not** send TV commands).
- [ ] `[` / `]` switch profiles; the header brand and mapping count update.
- [ ] "TEST POWER" sends Power and updates the "LAST SENT" block.
- [ ] Last-sent block shows command name, hex code, OK/FAIL, "Ns ago".
- [ ] Pressing `` ` `` or Esc leaves Settings to Main Remote.

Robustness:
- [ ] Header battery percent updates without forcing a full redraw.
- [ ] Unmapped commands are drawn with a diagonal strike and report UNMAPPED.
- [ ] Toast appears over the footer strip and clears cleanly when it expires.
- [ ] No flicker during repeated VOL+ holds (double-buffered via `M5Canvas`).
- [ ] D-pad cluster does not overlap the bottom button row.
