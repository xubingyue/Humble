local strpubdir = string.format("%s%s%s", g_strScriptPath, "public", "/")
package.path = string.format("%s;%s?.lua", package.path, strpubdir)

require("macros")
local serialize = require("serialize")
local humble = require("humble")
local table = table
local string = string
local ChanNam = ChanNam

if not g_tChan then
    g_tChan = {}    
end
local tChan = g_tChan

function initTask()
    tChan.echo = humble.regRecvChan("echochan", "echo")
end

function runTask()
    local varRecv = tChan.echo:Recv()
    if varRecv then
        local sock, uiSession, strMsg = table.unpack(serialize.unpack(varRecv))
        humble.Send(sock, uiSession, strMsg)
    end
end

function destroyTask()
    
end
