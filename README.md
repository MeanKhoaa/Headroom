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

## Testing in VS Code or GitHub Codespaces

This repo includes a dev container and VS Code tasks so a beginner can build without manually setting up the Pebble SDK.

### GitHub Codespaces

1. Open this repo on GitHub.
2. Choose **Code > Codespaces > Create codespace on current branch**.
3. Wait for the dev container setup to finish. It installs the Pebble CLI and latest SDK.
4. In VS Code, open **Terminal > Run Task... > Pebble: Build**.
5. The app package should be created at `build/Headroom.pbw`.

Codespaces can build the app, but it usually cannot show a watch emulator UI in your browser. For visual testing, download `build/Headroom.pbw` or run the emulator on your own computer.

### Local VS Code

1. Install Docker and VS Code.
2. Install the VS Code **Dev Containers** extension.
3. Open this folder in VS Code and choose **Reopen in Container**.
4. Run **Terminal > Run Task... > Pebble: Build**.
5. To try a Pebble Time 2 emulator locally, run **Terminal > Run Task... > Pebble: Install Emulator (Time 2)**.
6. To install on a real watch through your phone, enable Developer Connection in the Pebble app, then run **Pebble: Install Phone** and enter the phone IP address.

### Fixing `cannot open source file "pebble.h"`

If VS Code shows this squiggle before the dev container setup finishes, wait for setup to complete and run **Terminal > Run Task... > Pebble: Build** once. The terminal equivalent is:

```sh
PATH=$HOME/.local/bin:$PATH pebble build
```

The repo includes `.vscode/c_cpp_properties.json` so the C/C++ extension can find Pebble SDK headers under `$HOME/.local/share/pebble-sdk/SDKs/current/`.

If you are not using the dev container, install the Pebble CLI and SDK first, then reload VS Code so the include path exists.

If it is still red after a successful build:

1. Make sure you are inside the dev container or Codespace, not your local machine without the SDK.
2. Run **Ctrl/Cmd+Shift+P > C/C++: Select a Configuration... > Pebble Time 2 (emery)**.
3. Run **Ctrl/Cmd+Shift+P > C/C++: Reset IntelliSense Database**.
4. Run **Developer: Reload Window**.

The repo also includes `.vscode/settings.json` with the same Pebble SDK include paths so VS Code has both workspace defaults and the named C/C++ configuration.
