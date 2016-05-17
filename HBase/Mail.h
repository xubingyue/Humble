
#ifndef H_MAIL_H_
#define H_MAIL_H_

#include "RecvTask.h"
#include "Singleton.h"
#include "jwsmtp/mailer.h"

H_BNAMSP

//邮件发送
class CMail : public CRecvTask<char>, public CSingleton<CMail>
{
public:
    CMail(void);
    ~CMail(void);

    void runTask(char *pMsg);

    //发送mail
    void sendMail(const char *pszMail);

    //设置
    void setServer(const std::string strServer);
    void setAuthType(const int uiType);
    void setSender(const std::string strSender);
    void setUserName(const std::string strUserName);
    void setPassWord(const std::string strPassWord);
    bool getSetted(void);

private:
    H_DISALLOWCOPY(CMail);
    int parseMail(const char *pMail); 

private:
    bool m_bSet;
    jwsmtp::mailer m_objMailer;
};

H_ENAMSP

#endif//H_MAIL_H_
