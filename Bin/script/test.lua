local strpubdir = string.format("%s%s%s", g_strScriptPath, "public", "/")
package.path = string.format("%s;%s?.lua", package.path, strpubdir)

require("macros")
local humble = require("humble")
local utile = require("utile")
local table = table
local string = string

function initTask()

end

function runTask(pChan)
    local varRecv = pChan:Recv()
    local itick, icount = table.unpack(utile.unPack(varRecv))
    --print(string.format("timer task tick %d count %d", itick, icount))    
end

function destroyTask()
    
end