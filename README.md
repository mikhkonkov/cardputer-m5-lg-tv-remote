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

1. **Main Remote** - power/input/mute top row, large center D-pad with OK,
   right-side volume + channel column, bottom Back/Home/Exit row.
2. **Extra Keys** - 3x3+1 digit grid, INFO/EXIT/SET column, color row.
3. **Setup** - profile selector, last-TX readout, mapping count. Visually
   distinct from the main UX so it never feels mixed with normal use.

### Focus model

Every screen renders a fixed-size button array. The focused button has a
two-pixel white border and slightly inset second border for visibility. Focus
moves by *geometric* nearest-neighbor search - press right and the focus moves
to the button whose center is closest in the requested direction, weighted by
perpendicular distance. This works regardless of grid alignment.

### Interaction states

- **Normal**: theme fill for the button group.
- **Focused**: white double border.
- **Pressed**: inverted fill for ~110 ms after a send.
- **Unmapped**: diagonal strike-through in dim gray; sending it logs UNMAPPED
  in Setup.
- **Toast**: 12 px strip at the bottom showing the label of the sent command,
  green for OK / red for FAIL / shown for ~380 ms.

### Key bindings (physical Cardputer keyboard)

| Key       | Action                                  |
|-----------|-----------------------------------------|
| `; , . /` | Move focus up/left/down/right           |
| Enter     | Send focused command (hold = repeat)    |
| `` ` ``   | Jump to Main Remote                     |
| Tab       | Toggle Main Remote / Extra Keys         |
| `p`       | Send Power immediately (works anywhere) |
| `s`       | Open Setup                              |
| `0..9`    | (on Extra Keys) send that digit directly|
| `[` / `]` | (on Setup) switch IR profile            |

These are convenience shortcuts. The on-screen remote is the primary UX -
the user does not need to memorize keys to operate it.

## SECTION 4 - Project tree

```
cardputer-lg-remote/
  platformio.ini
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
- `src/ui/Screen.cpp::pickNeighbor` - 2D focus search.
- `src/input/Keyboard.cpp` - hold-to-repeat detection on Enter.
- `src/app/App.cpp` - event dispatch; the only place screens are switched.

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

- [PlatformIO Core](https://platformio.org/install/cli) (`pip install -U platformio`).
- USB cable. Cardputer enumerates as a native USB-CDC device on macOS / Linux /
  Windows once you press the reset button while holding `G0` (bottom-left key).

### Build

```sh
cd cardputer-lg-remote
pio run
```

The first build downloads `M5Cardputer` and `IRremoteESP8266`; subsequent
builds are incremental.

### Flash

Put the device into download mode (hold `G0`, tap reset, release `G0`) and run:

```sh
pio run --target upload
```

The `upload_speed` is set high (921600); drop to 460800 if your USB cable is
flaky. Once flashed, press reset once to start the firmware.

### Monitor

```sh
pio device monitor
```

Useful only during development - the production UI does not print to serial.

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
- [ ] Display shows "REMOTE" header with brand "LG" on the right.
- [ ] All Main Remote buttons render; focus on OK.

Main Remote screen:
- [ ] Arrow keys (`; , . /`) move focus, including diagonally placed buttons.
- [ ] Enter on Power triggers a Power toast (and pressing Power on the TV
      visibly toggles it).
- [ ] Holding Enter on `VOL+` keeps volume rising; releasing stops.
- [ ] Holding Enter on `Power` does **not** repeat.
- [ ] Tab switches to the Extra Keys screen and back.
- [ ] Pressing `` ` `` from any screen returns to Main Remote.
- [ ] Pressing `p` sends Power regardless of focus.

Extra Keys screen:
- [ ] Digit grid is visible; OK on any digit changes the channel.
- [ ] Typing a digit on the physical keyboard fires that digit immediately.
- [ ] Color buttons render in their colors and are focusable.

Settings:
- [ ] `s` opens it; shows current profile, model hint, and mapping count.
- [ ] `[` / `]` switch profiles; the header brand and mapping count update.
- [ ] "TEST POWER" sends Power and updates the "LAST SENT" block.
- [ ] Last-sent block shows command name, hex code, OK/FAIL, "Ns ago".
- [ ] Pressing `` ` `` leaves Settings to Main Remote.

Robustness:
- [ ] Unmapped commands are drawn with a diagonal strike and report UNMAPPED.
- [ ] No serial errors on boot (`pio device monitor`).
- [ ] No flicker during repeated VOL+ holds.
