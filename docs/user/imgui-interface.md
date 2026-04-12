# Working with the ImGui Interface

RoboMotion Simulator uses [Dear ImGui](https://github.com/ocornut/imgui) for its UI. Understanding a few ImGui-specific behaviors makes the interface much easier to use.

## Window Management

Each panel (Render Settings, Input Settings, Robot Stats, Robot Designer) opens as a **separate OS window**. You can:

- **Reposition** any window by dragging its title bar.
- **Resize** any window by dragging its edges or corners.
- Window positions and sizes are **saved automatically** and restored on the next launch.

> **Note**: Windows fill their own OS frame entirely — there is no panel docking or tabbing.

## Opening the Menu

Click the **=** button in the top-right corner of the main window to open the menu. Click any menu item to open that panel. Press **ESC** or click outside the menu to dismiss it without selecting anything.

## Slider and Number Input Tricks

ImGui controls support several input methods beyond clicking and dragging:

| Input method | How to do it |
|---|---|
| Type an exact value | **Ctrl+click** on any slider or drag field to switch to text input mode |
| Fine-tune with keyboard | While text input is active, use **Up/Down arrow** keys to increment |
| Step buttons on InputFloat | Click **+** / **-** buttons on the sides; hold **Shift** for a larger step |
| Cancel text input | Press **Escape** while the field is focused |

## Render Settings Panel

- Changes apply **live** — the main view updates immediately as you drag sliders or toggle checkboxes.
- The **color picker** uses a hue wheel. The color type dropdown (Grid color / SubGrid color / Trail Color) selects which color you are editing.
- **Reset to Defaults** restores all render settings at once. Individual controls cannot be individually reset.
- **Reset Trail** clears the position history without changing any other setting.

## Robot Designer Panel

### Axle List

Each axle is shown as a **collapsible tree node** (e.g. "Axle 0"). Click the node label to expand or collapse it. This keeps the list compact when you have many axles.

- Changes to axle parameters are **not applied to the simulation automatically**. You must press **Apply to Simulation** when you are ready.
- Pressing **Apply to Simulation** validates the config first. Any validation errors appear in a modal popup — click **OK** to dismiss and fix the issue.

### Live Preview

Toggle **Show Preview** to see a top-down rendering of the robot shape based on the current designer state. The preview updates as you edit wheel positions. Turning it off speeds up the UI if you have many axles.

### File Operations

- **Save As...** and **Load from** open native OS file dialogs filtered to `.ini` files.
- Loading a file **does not immediately apply** it to the simulation — review the loaded config and click **Apply to Simulation**.

## Input Settings Panel

### Rebinding Keys (Keyboard mode)

1. Click the button next to the action you want to rebind. It changes to "Press a key...".
2. Press the desired key.
3. Press **Escape** to cancel without changing the binding.

### Rebinding Controller Inputs

Each action can be mapped to either an **Axis** or **Buttons** (two buttons for negative/positive direction):

- Switch between modes with the Buttons / Axis dropdown next to each action.
- **Axis mode**: click the axis button and move the desired stick or trigger past ~50% to capture it.
- **Button mode**: click Button 1 or Button 2 and press the desired controller button.
- Press **Escape** at any point to cancel the capture.
- The **Deadzone** slider sets the minimum axis movement (in percent) that registers as input.

### IPC and Serial modes

- **IPC** (ZMQ): set the address and ports, then start your external controller. The simulator reconnects automatically when the address changes.
- **Serial**: type the port name directly (e.g. `COM3` on Windows, `/dev/ttyUSB0` on Linux) and select the baud rate.

## Robot Stats Panel

The stats panel is **read-only** — it only displays data, nothing is editable here except logging controls.

- Progress bars for velocities and wheel speeds saturate at 3000 mm/s (full bar = 3 m/s).
- Wheel columns are arranged automatically: up to 3 columns for larger wheel counts.
- When **[REC] Recording...** is shown, data is being written to a CSV file in the selected log folder. Click **Stop Logging** to finalize the file.

## Overlays in the Main Window

| Shortcut | Toggle |
|---|---|
| **F7** | Timestamp overlay (top-left corner) |
| **F8** | FPS overlay (bottom-right corner) |


## Keyboard Navigation

ImGui keyboard navigation is enabled (`NavEnableKeyboard`). You can tab between focusable controls within a panel. The windowing switcher (Ctrl+Tab) is disabled to avoid conflicts with the simulation.
