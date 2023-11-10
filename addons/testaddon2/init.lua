Handle = getCurrentHandle()

setHudWindowName(Handle, "Https Test!")

setWidth(Handle, 500)
setHeight(Handle, 500)

setX(Handle, 912)

local textIdent = "testText"

addTextWidget(Handle, textIdent, 0)
setTextWidgetContent(Handle, textIdent, "Loading...")



setWidgetX(Handle, textIdent, 45)
setWidgetY(Handle, textIdent, 105)

getResourceAt("https://jsonplaceholder.typicode.com", "/todos/1", "todo")