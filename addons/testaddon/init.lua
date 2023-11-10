Handle = getCurrentHandle()

setHudWindowName(Handle, "Gamer Time")

setWidth(Handle, 200)
setHeight(Handle, 700)

local textIdent = "testText"

addTextWidget(Handle, textIdent, 0)
setTextWidgetContent(Handle, textIdent, "Hello, World!")



setWidgetX(Handle, textIdent, 45)
setWidgetY(Handle, textIdent, 105)

setPersistentFloat(Handle, "XDirection", 500)
setPersistentFloat(Handle, "YDirection", 500)
