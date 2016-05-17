
#ifndef H_REG2LUA_H_
#define H_REG2LUA_H_

#include "LuaTask.h"

H_BNAMSP

void H_RegGlobal(struct lua_State *pLState);
void H_RegFuncs(struct lua_State *pLState);
void H_RegNetParser(struct lua_State *pLState);
void H_RegBinary(struct lua_State *pLState);
void H_RegNetWorker(struct lua_State *pLState);
void H_RegSender(struct lua_State *pLState);
void H_RegMail(struct lua_State *pLState);
void H_RegCharset(struct lua_State *pLState);
void H_RegClock(struct lua_State *pLState);
void H_RegIni(struct lua_State *pLState);
void H_RegSnowFlake(struct lua_State *pLState);
void H_RegTableFile(struct lua_State *pLState);
void H_RegUUID(struct lua_State *pLState);
void H_RegAES(struct lua_State *pLState);
void H_RegRSA(struct lua_State *pLState);
void H_RegSha1(struct lua_State *pLState);
void H_RegChan(struct lua_State *pLState);
void H_RegWorkerTask(struct lua_State *pLState);
void H_RegWorkerDisp(struct lua_State *pLState);

void H_RegAll(struct lua_State *pLState);

H_ENAMSP

#endif //H_REG2LUA_H_
