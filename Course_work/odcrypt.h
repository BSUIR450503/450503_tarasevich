#ifndef ODCRYPT_H
#define ODCRYPT_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QFileDialog>
#include <QFile>
#include <QDebug>
#include <QMessageBox>
#include <QStringList>
#include <QDialog>
#include <math.h>
#include "thread.h"

namespace Ui {
class ODCrypt;
}

class ODCrypt : public QMainWindow
{
    Q_OBJECT

public:
    explicit ODCrypt(QWidget *parent = 0);
    ~ODCrypt();
    void resizeEvent(QResizeEvent*);

private:
    Ui::ODCrypt *ui;
    QStandardItemModel *model;
    QString settingsDestPath;
    QString toRemoveOrigins;
    QString addFile;
    QString currentFile;
    Thread *thread;
    QList<QString*> sourceFilesList;
    QList<QString*> destinationFilesList;

    void initModelTableView();
    void initActions();
    void addDataInTableView(QString);
    void loadSettings();
    void saveSettings();
    void saveDialog();
    void deleteEncryptionFileNameList();

    int getFileNameFromPath(QString, QString&);
    int getDirNameFromPath(QString, QString&);

    void newThread(QString, bool);

public slots:
    void on_processFinished(bool);

private slots:
    void menu_open();
    void menu_save();
    void menu_saveAs();
    void menu_exit();
    void menu_setDestination();
    void menu_about();
    void on_addButton_clicked();
    void on_removeButton_clicked();
    void on_encryptButton_clicked();
    void on_decryptButton_clicked();
    void on_removeAllButton_clicked();
};

#endif // ODCRYPT_H
