#include "thread.h"

Thread::Thread(QObject *parent) :
    QThread(parent)
{
}

void Thread::setSourceDestionationFiles(QList<QString *> source, QList<QString *> destination)
{
    sourceFilesList = source;
    destFilesList = destination;
}

void Thread::isEncrypt(bool encrypt)
{
    this->encrypt = encrypt;
}

void Thread::run()
{
    if (sourceFilesList.size() != destFilesList.size())
    {
        exec();
        emit processFinished(encrypt);
        return;
    }

    if (encrypt)
    {
        for (int i = 0; i < sourceFilesList.size(); i++)
        {
            emit setLabel("Encrypt: " + *sourceFilesList[i]);
            encryptFile(sourceFilesList[i]->toLatin1().data(), destFilesList[i]->toLatin1().data(), 
                password.toLatin1().data());
        }
    }
    else
    {
        for (int i = 0; i < sourceFilesList.size(); i++)
        {
            emit setLabel("Decrypt: " + *sourceFilesList[i]);
            decryptFile(sourceFilesList[i]->toLatin1().data(), destFilesList[i]->toLatin1().data(), 
                password.toLatin1().data());
        }
    }
    emit setLabel("");
    emit processFinished(encrypt);
    exec();
}

void Thread::setPassword(QString password)
{
    this->password = password;
}

void Thread::encryptFile(const char *in, const char *out, const char *passPhrase)
{
    CryptoPP::FileSource f(in, true, new CryptoPP::DefaultEncryptorWithMAC(passPhrase,
        new CryptoPP::FileSink(out)));
}


void Thread::decryptFile(const char *in, const char *out, const char *passPhrase)
{
    CryptoPP::FileSource f(in, true,
        new CryptoPP::DefaultDecryptorWithMAC(passPhrase, new CryptoPP::FileSink(out)));
}
