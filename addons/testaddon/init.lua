Handle = getCurrentHandle()

setHudWindowName(Handle, "Gamer Time")

setWidth(Handle, 200)
setHeight(Handle, 700)

local textIdent = "testText"
local buttonIdent = "button"

addTextWidget(Handle, textIdent, 0)
setTextWidgetContent(Handle, textIdent, "Hello, World!")

addButtonWidget(Handle, buttonIdent, 0)
setButtonOnClick(Handle, buttonIdent, "testCallback")
setButtonWidgetLabel(Handle, buttonIdent, "funnymonkey")

setWidgetWidth(Handle, buttonIdent, 100)
setWidgetHeight(Handle, buttonIdent, 20)

setWidgetX(Handle, buttonIdent, 45)
setWidgetY(Handle, buttonIdent, 105)

setWidgetX(Handle, textIdent, 45)
setWidgetY(Handle, textIdent, 105)

setPersistentFloat(Handle, "XDirection", 500)
setPersistentFloat(Handle, "YDirection", 500)
