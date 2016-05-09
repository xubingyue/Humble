--[[
utile
--]]

require("macros")
local serialize = require("serialize")
local type = type
local table = table
local string = string
local xpcall = xpcall
local assert = assert
local Sleep = Sleep
local H_LOG = H_LOG
local CRC16 = CRC16
local CRC32 = H_CRC32
local urlEncode = urlEncode
local urlDecode = urlDecode
local zlibEncode = zlibEncode
local zlibDecode = zlibDecode
local b64Encode = b64Encode
local b64Decode = b64Decode
local md5Str = md5Str
local md5File = md5File
local LogLV = LogLV
local bDebug = true

local utile = {}

function utile.Sleep(uims)
    Sleep(uims)
end

function utile.Debug(fmt, ...)
    if bDebug then
        local strMsg = string.format(fmt, table.unpack({...}))
        print(string.format("[%s][LDebug] %s", os.date(), strMsg))
    end
end

function utile.Log(loglv, fmt, ...)
    local strmsg = string.format(fmt, table.unpack({...}))
    H_LOG(loglv, strmsg)
end

function utile.CRC16(strval)
    return CRC16(strval, string.len(strval))
end

function utile.CRC32(strval)
    return CRC32(strval, string.len(strval))
end

function utile.urlEncode(strval)
    return urlEncode(strval, string.len(strval))
end

function utile.urlDecode(strval)
    return urlDecode(strval, string.len(strval))
end

function utile.zlibEncode(strval)
    return zlibEncode(strval, string.len(strval))
end

function utile.zlibDecode(strval)
    return zlibDecode(strval, string.len(strval))
end

function utile.b64Encode(strval)
    return b64Encode(strval, string.len(strval))
end

function utile.b64Decode(strval)
    return b64Decode(strval, string.len(strval))
end

function utile.md5Str(strval)
    return md5Str(strval, string.len(strval))
end

function utile.md5File(strFile)
    return md5File(strFile)
end

function utile.Pack(tVal)
    return serialize.pack(tVal)
end

function utile.unPack(pVal)
    return serialize.unpack(pVal)
end

function utile.md5File(strFile)
    return md5File(strFile)
end

function utile.callFunc(Func, ...)
    assert("function" == type(Func))

    local function onExcept(strMsg)
        local strStack = debug.traceback()
        utile.Log(LogLV.Err, "%s", strMsg)
        utile.Log(LogLV.Err, "%s", strStack)
    end
    
    return xpcall(Func, onExcept, table.unpack({...}))
end

function string.split(str, delimiter)
    assert("string" == type(str))
    assert("string" == type(delimiter))
    
    if ('' == delimiter) then 
        return {str} 
    end
    
    local pos,arr = 0, {}
    for st,sp in function() return string.find(str, delimiter, pos, true) end do
        table.insert(arr, string.sub(str, pos, st - 1))
        pos = sp + 1
    end
    
    table.insert(arr, string.sub(str, pos))
    
    return arr
end

function string.trim(str) 
    return (string.gsub(str, "^%s*(.-)%s*$", "%1"))
end 

function table.print(lua_table, indent)    
    assert("table" == type(lua_table))    
    indent = indent or 0
    for k, v in pairs(lua_table) do
        if type(k) == "string" then
            k = string.format("%q", k)
        end
        
        local szSuffix = ""
        if type(v) == "table" then
            szSuffix = "{"
        end
        
        local szPrefix = string.rep("    ", indent)
        formatting = szPrefix.."["..k.."]".." = "..szSuffix
        
        if type(v) == "table" then
            print(formatting)
            table.print(v, indent + 1)
            print(szPrefix.."},")
        else
            local szValue = ""
            if type(v) == "string" then
                szValue = string.format("%q", v)
            else
                szValue = tostring(v)
            end
            
            print(formatting..szValue..",")
        end
    end
end

function table.empty(lua_table)
    assert("table" == type(lua_table))    
    for _, _ in pairs(lua_table) do
        return false
    end
    
    return true
end

function table.len(lua_table)
    assert("table" == type(lua_table))    
    local iCount = 0
    for _, _ in pairs(lua_table) do
        iCount = iCount + 1
    end
    
    return iCount
end

function table.copy(tTable)
    assert("table" == type(lua_table))    
    local tNewTab = {}  
    for i, v in pairs(tTable) do  
        local vtyp = type(v)
        
        if ("table" == vtyp) then  
            tNewTab[i] = table.copy(v)  
        elseif ("thread" == vtyp) then  
            tNewTab[i] = v  
        elseif ("userdata" == vtyp) then  
            tNewTab[i] = v  
        else  
            tNewTab[i] = v  
        end
    end  
    
    return tNewTab
end

return utile
