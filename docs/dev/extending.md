# Extending the Simulator

## Adding a New Drive Type

1. **Add the enum value** in `src/core/include/RobotConfig.hpp`:
   ```cpp
   enum class RobotDriveType {
       DIFFERENTIAL = 0,
       OMNI_WHEEL = 1,
       MECANUM = 2,
       CUSTOM = 3,
       MY_DRIVE = 4  // new
   };
   ```

2. **Add validation rules** in `RobotConfig::validateConfig()` — define minimum wheel count and geometric constraints.

3. **Add movement constraints** in `PhysicsEngine::limitMovement()` — specify which degrees of freedom are available.

4. **Update the config parser** in `ImportHelper::createFromConfigDriveType()` to recognize the new drive type string.

5. **Update the exporter** in `ExportHelper::exportToConfig(RobotDriveType)` to serialize the new type.

6. **Update the UI** in `RobotDesignerWindow::renderContent()` to include the new option in the drive type dropdown and set the default roller angle in `buildRobotConfig()`.

## Adding a New Input Source

1. Create a new class in `src/inputManager/include/` (e.g., `MyInput.hpp`).

2. Implement the update interface:
   ```cpp
   class MyInput {
   public:
       MyInput(ConfigSection<MyMapping>& mapping);
       // Returns error string on failure, nullopt on success
       std::optional<std::string> update(RobotState& state);
   private:
       const MyMapping& _mapping;
       ScopedSubscription<MyMapping> _subscription;
   };
   ```

3. Subscribe to config changes in the constructor for reactive settings:
   ```cpp
   _subscription = mapping.scopedSubscribe([this](const MyMapping&) {
       updateAfterSettingsChange();
   });
   ```

4. Register it in `InputManager` — add the new `InputType` enum value and dispatch in `update()` / `checkForInputCompletion()`.

5. Add a configuration struct in `AppConfig.hpp`, wrap it in `ConfigSection<T>`, and add JSON serialization in `ConfigJsonSerialization.hpp`.

6. Add a UI panel in `InputSettingsWindow` for configuring the new input source.

## Adding a New Visualization Element

1. Create a shape or renderer class in `src/visualization/include/rendering/` or `shapes/`.

2. Implement SFML drawing:
   ```cpp
   class MyElement : public sf::Drawable {
       void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
   };
   ```

3. Instantiate it in `RenderEngine` and call its draw method in `RenderEngine::draw()`.

4. Add any toggle or configuration options to `RenderSettings` in `AppConfig.hpp` and expose them in `RenderSettingsWindow`.

## Adding a New UI Window

1. Create a window class in `src/visualization/include/windows/` inheriting from `SettingsWindowBase`:

   ```cpp
   class MyWindow : public SettingsWindowBase {
   public:
       MyWindow(const sf::Image& icon);
       void open();
       void close(bool closeFromRoot = false);
       void update(sf::Time dt);
   private:
       void renderContent();
   };
   ```

2. `SettingsWindowBase` handles window creation, ImGui initialization, event processing, and the open/close lifecycle. Override `update()` to call `processEvents()`, `ImGui::SFML::Update()`, and your `renderContent()`.

3. Add a menu item in `MainWindow::renderImGuiMenu()` to toggle the window.

4. Register it in `MainWindow` — add to `initializeOtherWindows()`, `closeOtherWindows()`, `updateAllOtherWindows()`, and `drawAllOtherWindows()`.

5. Add a `WindowConfig` field to `AppConfig` and save/restore it in `MainWindow::saveConfig()`.

## Code Conventions

- Headers use `.hpp`, implementations use `.cpp`.
- Public headers go in `include/`, implementations in `src/`.
- Use C++23 features where appropriate.
- All angles are stored in **radians** internally; convert to degrees only for display and config files.
- Positions and distances are in **millimeters**.
- Prefer value types and const references; avoid raw pointers where possible.
- Use `ConfigSection<T>` with `ScopedSubscription` for reactive settings rather than manual polling.
- Member variables use `_camelCase` prefix convention.
