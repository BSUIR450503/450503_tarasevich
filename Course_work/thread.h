#ifndef THREAD_H
#define THREAD_H

#include <QThread>
#include <QMessageBox>
#include <QList>

#include <iostream>
#include <cryptopp/files.h>
#define CRYPTOPP_DEFAULT_NO_DLL
#include <cryptopp/dll.h>
#include <cryptopp/default.h>
#ifdef CRYPTOPP_WIN32_AVAILABLE
#include <windows.h>
#endif

class Thread : public QThread
{
    Q_OBJECT
public:
    explicit Thread(QObject *parent = 0);
    void setSourceDestionationFiles(QList<QString*>, QList<QString*>);
    void isEncrypt(bool);
    void run();
    void setPassword(QString);
private:
    QList<QString*> sourceFilesList;
    QList<QString*> destFilesList;
    bool encrypt;
    QString password;

    void encryptFile(const char*, const char*, const char*);
    void decryptFile(const char*, const char*, const char*);
signals:
    void processFinished(bool);
    void setLabel(QString);
};

#endif // THREAD_H
