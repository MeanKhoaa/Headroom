# Headroom Coach

A button-first Pebble smartwatch app for badminton tournament mental reset.

## Modes

- **Warm-up**: short physical and tactical readiness checklist.
- **Match Mode**: one-rally focus prompt, point outcome buttons, tilt spiral reset after three lost points in a row, and long-press emergency reset.
- **Between Games**: 1-5 headroom rating and one simple plan choice.
- **Match Review**: quick post-match reflection for result, headroom, and main issue.

## Controls

- **UP/DOWN**: move through modes and choices. In Match Mode, UP records a won point and DOWN records a lost point.
- **SELECT**: start a mode, confirm a choice, or trigger a Match Mode reset prompt.
- **Hold SELECT**: Emergency Reset with a stronger vibration pattern.

Version 1 intentionally avoids complex scorekeeping so the app stays fast, readable, and usable without touch input.

## Beginner testing and install

### 1. Install the Pebble tools

Follow the current Rebble/Core Devices SDK instructions for your computer, then run:

```sh
pebble sdk install latest
```

### 2. Build the app

From this project folder, run:

```sh
pebble build
```

The build creates `build/Headroom.pbw`, which is the file you install on a watch or emulator.

### 3. Test in an emulator

For a Pebble Time style screen, run:

```sh
pebble install --emulator basalt
```

For Pebble Time 2, use the latest SDK and run:

```sh
pebble install --emulator emery
```

### 4. Install on a real watch

Pair the watch with the Pebble mobile app, enable Developer Connection in the app, note the phone IP address shown there, then run:

```sh
pebble install --phone YOUR_PHONE_IP
```

You can also copy the built `.pbw` file to your phone and open it with the Pebble app. On Android, Rebble's Sideload Helper can help if the file does not open in the Pebble app directly.
