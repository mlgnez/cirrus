Handle = getCurrentHandle()
local i = "testText"

local width = getWidth(Handle) - 75
local height = getHeight(Handle) - 28

local ydirection = getPersistentFloat(Handle, "YDirection")
local xdirection = getPersistentFloat(Handle, "XDirection")

if xdirection ~= nil then
    local x = getWidgetX(Handle, i)
    if x >= width or x <= 0 then
        xdirection = -xdirection
        setWidgetX(Handle, i, width)
        print("bounce!")
    end
    
    local clampedX = clamp(x + xdirection * getDeltaTime(), 0, width)

    setWidgetX(Handle, i, clampedX)
    setPersistentFloat(Handle, "XDirection", xdirection)

end

if ydirection ~= nil then
    local y = getWidgetY(Handle, i)
    if y >= height or y <= 0 then
        ydirection = -ydirection
        setWidgetY(Handle, i, width)
        print("bounce!")
    end
    
    local clampedY = clamp(y + ydirection * getDeltaTime(), 0, height)

    setWidgetY(Handle, i, clampedY)
    setPersistentFloat(Handle, "YDirection", ydirection)
end




