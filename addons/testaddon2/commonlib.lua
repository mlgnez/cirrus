---@diagnostic disable: missing-return, lowercase-global
--[[
    These are stubs for all StandardLibrary Functions (SL)
]]

-- clamps v between i and a
---@param v number value
---@param i number minimum
---@param a number maximum
---@return number clamped clamped value between i and a
function clamp(v, i, a) end

--- Gets the name of the currently focused window as a string
---@return string name Gets the name of the focused window.
function getfocusedWindowName() end

--- Gets the file path of the currently focused window as a string
---@return string name Gets the file path of the focused window.
function getfocusedWindowFileName() end

--- Get the main screen width
---@return number dimension Screen Width
function getScreenWidth() end

--- Get the main screen height
---@return number dimension Screen Height
function getScreenHeight() end

--[[
    These are stubs for all StandardRenderLibrary Functions (SRL)
]]
---Retrieves the handle of a HUD window by its name.
---@param name string The name of the HUD window.
---@return number|nil handle The handle of the window or nil if not found.
function gethandle(name) end

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

---Checks for the existance of a persistent boolean
---@param handle number The handle of the HUD window.
---@param name string The name of the boolean value.
---@return boolean existance Whether or not the persistent entry exists
function persistentBooleanExists(handle, name) end

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

---Checks for the existance of a persistent float
---@param handle number The handle of the HUD window.
---@param name string The name of the float value.
---@return boolean existance Whether or not the persistent entry exists
function persistentFloatExists(handle, name) end

---Sets a persistent floating point value.
---@param handle number The handle of the HUD window.
---@param name string The name of the float value.
---@param pushdata number The value to set.
function setPersistentFloat(handle, name, pushdata) end

---Gets a persistent String value.
---@param handle number The handle of the HUD window.
---@param name string The name of the String value.
---@return string|nil data The value of the persistent String or nil if not found.
function getPersistentString(handle, name) end

---Checks for the existance of a persistent string
---@param handle number The handle of the HUD window.
---@param name string The name of the string value.
---@return boolean existance Whether or not the persistent entry exists
function persistentStringExists(handle, name) end

---Sets a persistent String value.
---@param handle number The handle of the HUD window.
---@param name string The name of the String value.
---@param pushdata string The value to set.
function setPersistentString(handle, name, pushdata) end

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

-- Adds a buttton widget to a specific HUD window
---@param handle number The handle of the HUD window.
---@param identifier string The desired identifier, must be unique
---@param renderPriority number Lower number renders first, higher number renders last
function addButtonWidget(handle, identifier, renderPriority) end

-- Adds an onclick callback to a specific Button <br/><strong>(WARNING: Only 1 callback per button allowed)</strong>
---@param handle number The handle of the HUD window.
---@param identifier string The desired identifier, must be unique
---@param callback string a callback function
function setButtonOnClick(handle, identifier, callback) end

-- Adds a label to a specific Button
---@param handle number The handle of the HUD window.
---@param identifier string The desired identifier, must be unique
---@param label string the label
function setButtonWidgetLabel(handle, identifier, label) end

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

-- Sets the width of a widget
---@param handle number The handle of the HUD window.
---@param identifier string The desired identifier
---@param width number The width of the widget
---@return boolean success returns whether or not the action was successful
function setWidgetWidth(handle, identifier, width) end

-- Sets the height of a widget
---@param handle number The handle of the HUD window.
---@param identifier string The desired identifier
---@param height number The height of the widget
---@return boolean success returns whether or not the action was successful
function setWidgetHeight(handle, identifier, height) end

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

-- Gets the visibility of a widget
---@param handle number The handle of the HUD window.
---@param identifier string The desired identifier
---@return boolean visibility retrieves visibility
function getWidgetHidden(handle, identifier) end

-- Sets the visibility of a widget
---@param handle number The handle of the HUD window.
---@param identifier string The desired identifier
---@param visibility boolean The desired visibility
function setWidgetHidden(handle, identifier, visibility) end

-- Gets the Width of a string of text once rendered
---@param text string Text to calculate width of
---@return number width retrieves Width
function calculateTextWidth(text) end

-- Gets the Height of a string of text once rendered
---@param text string Text to calculate Height of
---@return number width retrieves Height
function calculateTextHeight(text) end

-- Gets the time between the last frame
---@return number dt time in seconds between the last frame
function getDeltaTime() end

--[[
    These are stubs for all HTTPLibrary Functions (Httplib)
]]
-- Gets the time between the last frame
---@param url string The base URL of the resource
---@param path string The path the resource is located at
---@param callback string a callback function
function getResourceAt(url, path, callback) end

-- Parses from a JSON string into a lua table
---@param json string JSON String
---@return table string A table that represents the JSON string
function jsonParse(json) end