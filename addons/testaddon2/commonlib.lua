---@diagnostic disable: missing-return, lowercase-global
--[[
    These are stubs for all StandardLibrary Functions (SL)
]]

---Retrieves the handle of a HUD window by its name.
---@param name string The name of the HUD window.
---@return number|nil handle The handle of the window or nil if not found.
function gethandle(name) end

-- clamps v between i and a
---@param v number value
---@param i number minimum
---@param a number maximum
---@return number clamped clamped value between i and a
function clamp(v, i, a) end

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

---Gets a persistent floating point value.
---@param handle number The handle of the HUD window.
---@param name string The name of the boolean value.
---@return number|nil data The value of the persistent float or nil if not found.
function getPersistentFloat(handle, name) end

---Sets a persistent floating point value.
---@param handle number The handle of the HUD window.
---@param name string The name of the float value.
---@param pushdata number The value to set.
function setPersistentFloat(handle, name, pushdata) end

-- Adds a text widget to a specific HUD window
---@param handle number The handle of the HUD window.
---@param identifier string The desired identifier, must be unique
---@param renderPriority number Lower number renders first, higher number renders last
function addTextWidget(handle, identifier, renderPriority) end

-- Modifies the text of a text widget
---@param handle number The handle of the HUD window.
---@param identifier string The desired identifier
---@param content string The text
---@return boolean success returns whether or not the action was successful
function setTextWidgetContent(handle, identifier, content) end

-- Sets the X position of a widget
---@param handle number The handle of the HUD window.
---@param identifier string The desired identifier
---@param x number The text
---@return boolean success returns whether or not the action was successful
function setWidgetX(handle, identifier, x) end

-- Sets the Y position of a widget
---@param handle number The handle of the HUD window.
---@param identifier string The desired identifier
---@param y number The text
---@return boolean success returns whether or not the action was successful
function setWidgetY(handle, identifier, y) end

-- Gets the X position of a widget
---@param handle number The handle of the HUD window.
---@param identifier string The desired identifier
---@return number x retrieves X position
function getWidgetX(handle, identifier) end

-- Gets the Y position of a widget
---@param handle number The handle of the HUD window.
---@param identifier string The desired identifier
---@return number y retrieves Y position
function getWidgetY(handle, identifier) end

-- Gets the time between the last frame
---@return number dt time in seconds between the last frame
function getDeltaTime() end


-- Gets the time between the last frame
---@param url string The base URL of the resource
---@param path string The path the resource is located at
---@param callback string a callback function
function getResourceAt(url, path, callback) end