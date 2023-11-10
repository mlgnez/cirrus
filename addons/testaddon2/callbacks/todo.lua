Handle = getCurrentHandle()

local textIdent = "testText"

local json = jsonParse(response)













setTextWidgetContent(Handle, textIdent, "title: " .. json.title)
