--[[
    These are stubs for all StandardLibrary Functions (SL)
]]

---@class HudWindowRegistry
local HudWindowRegistry = {}

---Retrieves the handle of a HUD window by its name.
---@param name string The name of the HUD window.
---@return number|nil handle The handle of the window or nil if not found.
function HudWindowRegistry.gethandle(name) end


---Pushes the current handle of the HUD window onto the stack.
---@return number hanlde The current handle.
function getCurrentHandle() end

---Sets the name of the HUD window.
---@param handle number The handle of the HUD window.
---@param name string The new name for the HUD window.
function setHudWindowName(handle, name) end

---Sets the width of the HUD window.
---@param handle number The handle of the HUD window.
---@param width number The new width for the HUD window.
function setWidth(handle, width) end

---Sets the height of the HUD window.
---@param handle number The handle of the HUD window.
---@param height number The new height for the HUD window.
function setHeight(handle, height) end

---Gets the width of the HUD window.
---@param handle number The handle of the HUD window.
---@return number width The width of the HUD window.
function getWidth(handle) end

---Gets the height of the HUD window.
---@param handle number The handle of the HUD window.
---@return number height The height of the HUD window.
function getHeight(handle) end

---Gets the X position of the HUD window.
---@param handle number The handle of the HUD window.
---@return number x The X position of the HUD window.
function getX(handle) end

---Gets the Y position of the HUD window.
---@param handle number The handle of the HUD window.
---@return number y The Y position of the HUD window.
function getY(handle) end

---Sets the X position of the HUD window.
---@param handle number The handle of the HUD window.
---@param x number The new X position for the HUD window.
function setX(handle, x) end

---Sets the Y position of the HUD window.
---@param handle number The handle of the HUD window.
---@param y number The new Y position for the HUD window.
function setY(handle, y) end

---Checks if a certain key is pressed.
---@param keyCode number The code of the key to check.
---@return boolean Boolean value indicating if the key is pressed.
function isKeyPressed(keyCode) end

---Retrieves the window handle from a string.
---@param name string The name of the window to retrieve the handle for.
---@return number|nil handle The handle of the window or nil if not found.
function getWindowHandleFromString(name) end

---Gets a persistent boolean value.
---@param handle number The handle of the HUD window.
---@param name string The name of the boolean value.
---@return boolean|nil data The value of the persistent boolean or nil if not found.
function getPersistentBoolean(handle, name) end

---Sets a persistent boolean value.
---@param handle number The handle of the HUD window.
---@param name string The name of the boolean value.
---@param pushdata boolean The value to set.
function setPersistentBoolean(handle, name, pushdata) end

---Injects the HUD Window Standard Library into the Lua state.
---@param L userdata The current Lua state.
function InjectHudWinSL(L) end
