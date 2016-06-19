#include "odcrypt.h"
#include "ui_odcrypt.h"
#include <QInputDialog>
#include <QApplication>

ODCrypt::ODCrypt(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ODCrypt)
{
    ui->setupUi(this);
    initModelTableView();
    initActions();

    settingsDestPath = "";
    addFile = "";
    toRemoveOrigins = "false";
    loadSettings();
    currentFile = "";
}

ODCrypt::~ODCrypt()
{
    delete ui;
    delete model;
}

void ODCrypt::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);

    ui->tableView->setGeometry(5, 5, this->width() - 10, 
        this->height() - (15 + ui->encryptButton->height() + ui->menuBar->height()));

    ui->addButton->setGeometry(5, ui->tableView->y() + ui->tableView->height() + 5, 
        ui->addButton->width(), ui->addButton->height());

    ui->decryptButton->setGeometry(this->width() - (ui->decryptButton->width() + 5), 
        ui->addButton->y(), ui->decryptButton->width(), ui->decryptButton->height());

    ui->encryptButton->setGeometry(ui->decryptButton->x() - (ui->encryptButton->width() + 5), 
        ui->addButton->y(), ui->decryptButton->width(), ui->decryptButton->height());

    ui->removeButton->setGeometry(ui->addButton->x() + ui->addButton->width() + 5, 
        ui->addButton->y(), ui->removeButton->width(), ui->removeButton->height());

    ui->removeAllButton->setGeometry(ui->removeButton->x() + ui->removeButton->width() + 5, 
        ui->addButton->y(), ui->removeAllButton->width(), ui->removeAllButton->height());
    
    ui->tableView->setColumnWidth(0, ui->tableView->width());
}

void ODCrypt::initModelTableView()
{
    model = new QStandardItemModel(0, 1, this);
    model->setHorizontalHeaderItem(0, new QStandardItem(QString("File path")));
    ui->tableView->setModel(model);
}

void ODCrypt::initActions()
{
    ui->actionOpen->setShortcut(QKeySequence::New);
    ui->actionSave->setShortcut(QKeySequence::Save);
    ui->actionSaveAs->setShortcut(QKeySequence::SaveAs);
    ui->actionExit->setShortcut(QKeySequence::Quit);

    connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(menu_open()));
    connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(menu_save()));
    connect(ui->actionSaveAs, SIGNAL(triggered()), this, SLOT(menu_saveAs()));
    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(menu_exit()));
    connect(ui->actionSet_Destination, SIGNAL(triggered()), this, SLOT(menu_setDestination()));
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(menu_about()));
}

void ODCrypt::menu_open()
{
      QString filepath = QFileDialog::getOpenFileName(this, tr("List of Encrypted File(s)"), 
                                                        "", tr("*.lef (*.lef)"));

      QFile file(filepath);
      if(!file.open(QIODevice::ReadOnly))
          return;

      QTextStream in(&file);

      if (in.atEnd())
      {
          file.close();
          return;
      }
      currentFile = filepath;
      while(!in.atEnd())
      {
        QString line = in.readLine();
        QFile file1(line);
        if(!file1.open(QIODevice::ReadOnly))
        {
            QMessageBox::critical(0, "Error!", line + " not exist.");
            continue;
        }
        file1.close();
        addDataInTableView(line);
      }
      file.close();
}

void ODCrypt::menu_save()
{
    if (currentFile == "")
        saveDialog();
    else
    {
        QFile file(currentFile);
        if ( file.open(QIODevice::ReadWrite) )
        {
            QTextStream stream(&file);

            QModelIndex mi;
            QVariant v;
            for (int i = 0; i < model->rowCount(); i++)
            {
                mi = model->index(i, 0);
                v = mi.data();

                stream << v.toString() + "\n";
            }
        }
        file.close();
    }
}

void ODCrypt::menu_saveAs()
{
    saveDialog();
}

void ODCrypt::menu_exit()
{
    this->close();
}

void ODCrypt::menu_setDestination()
{ 
    QString path = QFileDialog::getExistingDirectory(this, tr("Choose Or Create Directory"),
                                                        settingsDestPath,
                                                        QFileDialog::DontResolveSymlinks | 
                                                        QFileDialog::ReadOnly);
    if (path != "")
    {
        settingsDestPath = path;
        saveSettings();
    }
}

void ODCrypt::menu_about()
{
    QMessageBox::about(NULL, "ODCrypter v0.0.1b", "Created by N. Tarasevich, BSUIR, 450503, 2016");
}

void ODCrypt::on_addButton_clicked()
{
    QStringList l_path = QFileDialog::getOpenFileNames(this, tr("Add file(s)"), addFile, tr("Files (*.*)"));
    if (l_path.size() == 0)
        return;

    QString temp = "";
    if (!getDirNameFromPath(l_path[0], temp))
        addFile = temp;

    for (int i = 0; i < l_path.size(); i++)
        addDataInTableView(l_path[i]);
}

void ODCrypt::on_removeButton_clicked()
{
    //get selected rows
    QModelIndexList indexes = ui->tableView->selectionModel()->selectedRows();

    qSort(indexes.begin(), indexes.end());

    //remove last index in list
    while (!indexes.isEmpty())
    {
        model->removeRows(indexes.last().row(), 1);
        indexes.removeLast();
    }
}


//encrypt button
void ODCrypt::on_encryptButton_clicked()
{
    bool ok=1;
    QInputDialog* inputDialog = new QInputDialog();
    inputDialog->setOptions(QInputDialog::NoButtons);

    QString text =  inputDialog->getText(NULL ,"Encrypt",
                                              "Password:", QLineEdit::Password,
                                              "", &ok);
    if (!ok)
        return;

    if (text.isEmpty())
    {
        QMessageBox::information(0, "", "The password field cannot be empty!");
        on_encryptButton_clicked();
        return;
    }

    if (model->rowCount() == 0)
    {
                QMessageBox::information(0, "", "Please add some file(s) to encrypt!");
                return;
    }

    QModelIndex mi;
    QVariant v;

    deleteEncryptionFileNameList();

    //check for exist path
    for (int i = 0; i < model->rowCount(); i++)
    {
        mi = model->index(i,0);
        v = mi.data();
        QString fileName;
        getFileNameFromPath(v.toString(), fileName);
        QString destPath = settingsDestPath + "/" + fileName;

        if (destPath == v.toString())
            destPath += "_en";

        sourceFilesList.push_back(new QString(v.toString()));
        destinationFilesList.push_back(new QString (destPath));
    }
    newThread(text, true);
}

void ODCrypt::on_decryptButton_clicked()
{
    bool ok;
    QInputDialog* inputDialog = new QInputDialog();
    inputDialog->setOptions(QInputDialog::NoButtons);

    QString text =  inputDialog->getText(NULL, "Decrypt",
                                              "Password:", QLineEdit::Password,
                                              "", &ok);
    if (!ok)
        return;
    if (text.isEmpty())
    {
        QMessageBox::information(0, "", "The password field cannot be empty!");
        on_decryptButton_clicked();
        return;
    }
    if (model->rowCount() == 0)
    {
        QMessageBox::information(0, "", "Please add some file(s) to dectypt!");
        return;
    }

    QModelIndex mi;
    QVariant v;

    deleteEncryptionFileNameList();

    //check for exist path
    for (int i = 0; i < model->rowCount(); i++)
    {
        mi = model->index(i, 0);
        v = mi.data();
        QString fileName;
        getFileNameFromPath(v.toString(), fileName);
        QString destPath = settingsDestPath + "/" + fileName;
        if (destPath == v.toString())
            destPath += "_de";

        sourceFilesList.push_back(new QString(v.toString()));
        destinationFilesList.push_back(new QString(destPath));
    }
    newThread(text, false);
}

//add data to tableView widget
void ODCrypt::addDataInTableView(QString fileName)
{
    //check for existing fileName in tableView
    QModelIndex mi;
    QVariant v;
    for (int i = 0; i < model->rowCount(); i++)
    {
        mi = model->index(i,0);
        v = mi.data();
        if (v.toString() == fileName)
            return;
    }

    //Add fileName to tableView
    model->setRowCount(model->rowCount() + 1);
    model->setData(model->index(model->rowCount() - 1, 0), fileName);

    //set data not editable
    model->item(model->rowCount() - 1, 0)->setEditable(false);

}

void ODCrypt::loadSettings()
{
    QFile file("settings");
    if(!file.open(QIODevice::ReadOnly))
        return;

    QTextStream in(&file);

    if (in.atEnd())
    {
        file.close();
        return;
    }
    QString line = in.readLine();
    line.contains("settingsDestPath:");
    QStringList sl = line.split(":");
    if (sl.size() < 2)
    {
        file.close();
        return;
    }
    if (sl.at(0) != "settingsDestPath")
    {
        file.close();
        return;
    }
    settingsDestPath = sl.at(1);
    settingsDestPath = settingsDestPath.replace("\n", "");

    if (in.atEnd())
    {
        file.close();
        return;
    }

    file.close();
}

void ODCrypt::saveSettings()
{
    QFile file("settings");
    if ( file.open(QIODevice::ReadWrite) )
    {
        QTextStream stream( &file );

        stream << "settingsDestPath:" + settingsDestPath + "\n";
    }
    file.close();
}

int ODCrypt::getDirNameFromPath(QString path, QString &dirName)
{
    QString separator;
    separator = "/";

    int index = -1;
    for (int i = path.length() - 1; i >= 0; i--)
    {

        if (path[i] == separator[0])
        {
            index = i;
            break;
        }
    }
    if (index == -1)
        return 1;

    for (int i = 0; i <= index; i++)
        dirName += path[i];

    return 0;
}

void ODCrypt::newThread(QString password, bool encryption)
{
    thread = new Thread();
    connect(thread, SIGNAL(processFinished(bool)),this, SLOT(on_processFinished(bool)));
    thread->setSourceDestionationFiles(sourceFilesList, destinationFilesList);
    thread->setPassword(password);
    thread->isEncrypt(encryption);
    thread->start();
}

void ODCrypt::on_processFinished(bool encrypt)
{
    thread->deleteLater();
    if (encrypt)
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, 
            "", "Encryption has been completed. Do you want to save a list of encrypted file(s) path(s)?",
            QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::Yes)
        {
            QString file_name = QFileDialog::getSaveFileName(this, 
                tr("Save list of file(s)"), "", tr("*.lef (*.lef)"));

            QFile file(file_name + ".lef");
            if ( file.open(QIODevice::ReadWrite) )
            {
                QTextStream stream(&file);

                for (int i = 0; i < destinationFilesList.size(); i++)
                    stream << *destinationFilesList[i] + "\n";
            }
            file.close();
        }
    }
    else
        QMessageBox::information(this, "", "Decryption has been completed successfully.");
}

//'remove all' button
void ODCrypt::on_removeAllButton_clicked()
{
    model->removeRows(0, model->rowCount());
}

void ODCrypt::saveDialog()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save list of file(s)"), "", tr("*.lef (*.lef)"));

    currentFile = fileName;
    QFile file(fileName + ".lef");
    if ( file.open(QIODevice::ReadWrite) )
    {
        QTextStream stream( &file );

        QModelIndex mi;
        QVariant v;
        for (int i = 0; i < model->rowCount(); i++)
        {
            mi = model->index(i,0);
            v = mi.data();

            stream << v.toString() + "\n";
        }
        deleteEncryptionFileNameList();
    }
    file.close();
}

void ODCrypt::deleteEncryptionFileNameList()
{
    if (sourceFilesList.size() > 0)
    {
        qDeleteAll(sourceFilesList);
        sourceFilesList.clear();
    }
    if (destinationFilesList.size() > 0)
    {
        qDeleteAll(destinationFilesList);
        destinationFilesList.clear();
    }
}

int ODCrypt::getFileNameFromPath(QString path, QString &fileName)
{
    QString separator, tempFileName = "";
    separator = "/";

    if (separator.length() < 1)
        return 1;

    for (int i = path.length() - 1; i >= 0; i--)
    {

        if (path[i] == separator[0])
            break;
        if (i == 0) //if not found separator, return 1, mean error
            return 1;
        tempFileName = path[i] + tempFileName;
    }
    fileName = tempFileName;
    return 0;
}
