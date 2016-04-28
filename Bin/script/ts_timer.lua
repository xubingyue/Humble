
local strpubdir = string.format("%s%s%s", g_strScriptPath, "public", "/")
package.path = string.format("%s;%s?.lua", package.path, strpubdir)

require("macros")
local serialize = require("serialize")
local humble = require("humble")
local table = table
local string = string
local channam = channam

if not g_tChan then
    g_tChan = {}    
end
local tChan = g_tChan

function inittask()
    tChan.timer = humble.regrecvchan(channam.timer, "ts_timer", 0)
end

function runtask()
    local itick, icount
    if tChan.timer:canRecv() then
        itick, icount = table.unpack(serialize.unpack(tChan.timer:Recv()))
        print(string.format("timer task tick %d count %d", itick, icount))
    end
end

function destroytask()
    
end
