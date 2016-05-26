
require("macros")

local msgtype = {
    "tick",
    "onesec",
    "link",
    "close",
    "send",
    "read",
}
msgtype = table.enum(msgtype, -1)

return msgtype
