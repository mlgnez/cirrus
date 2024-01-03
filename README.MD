This was made for a school project, do not expect a compiling project.
Thanks to github.com/Rubyboat1207 for doing all of the Lua interpreting stuff and helping with C++

# Lua Functions Documentation

## HudWindowRegistry

### `gethandle`
- **Description**: Retrieves the handle of a HUD window by its name.
- **Parameters**: `name` (string) - The name of the HUD window.
- **Returns**: `std::optional<int>` - The handle of the window or an empty optional if not found.

### `getStringFromLuaState`
- **Description**: Gets a string from the Lua state at the specified stack index.
- **Parameters**: 
  - `L` (lua_State*) - The current Lua state.
  - `stackIdx` (int) - The index on the stack where the string is located.
- **Returns**: `std::string` - The string from the Lua state.

## HudWindow Functions

### `getCurrentHandle`
- **Description**: Pushes the current handle of the HUD window onto the stack.
- **Lua Return**: The current handle.

### `setHudWindowName`
- **Description**: Sets the name of the HUD window.
- **Parameters**: 
  - `handle` (int) - The handle of the HUD window.
  - `name` (string) - The new name for the HUD window.

### `setWidth` / `setHeight`
- **Description**: Sets the width or height of the HUD window.
- **Parameters**: 
  - `handle` (int) - The handle of the HUD window.
  - `size` (float) - The new width or height for the HUD window.

### `getWidth` / `getHeight`
- **Description**: Gets the width or height of the HUD window.
- **Parameters**: `handle` (int) - The handle of the HUD window.
- **Lua Return**: The width or height of the HUD window.

### `getX` / `getY`
- **Description**: Gets the X or Y position of the HUD window.
- **Parameters**: `handle` (int) - The handle of the HUD window.
- **Lua Return**: The X or Y position of the HUD window.

### `setX` / `setY`
- **Description**: Sets the X or Y position of the HUD window.
- **Parameters**: 
  - `handle` (int) - The handle of the HUD window.
  - `position` (float) - The new X or Y position for the HUD window.

### `isKeyPressed`
- **Description**: Checks if a certain key is pressed.
- **Parameters**: `keyCode` (int) - The code of the key to check.
- **Lua Return**: Boolean value indicating if the key is pressed.

### `getWindowHandleFromString`
- **Description**: Retrieves the window handle from a string.
- **Parameters**: `name` (string) - The name of the window to retrieve the handle for.
- **Lua Return**: The handle of the window or nil if not found.

### `getPersistentBoolean` / `setPersistentBoolean`
- **Description**: Gets or sets a persistent boolean value.
- **Parameters**: 
  - `handle` (int) - The handle of the HUD window.
  - `name` (string) - The name of the boolean value.
  - `pushdata` (bool) - The value to set (only for `setPersistentBoolean`).
- **Lua Return**: The value of the persistent boolean (only for `getPersistentBoolean`).

## Lua State Injection

### `InjectHudWinSL`
- **Description**: Injects the HUD Window Standard Library into the Lua state.
- **Parameters**: `L` (lua_State*) - The current Lua state.
- **Effect**: Registers functions into the Lua state.
