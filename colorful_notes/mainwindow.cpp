#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QScrollBar>
#include <QVBoxLayout>
#include <QColorDialog>
#include <QFontDialog>
#include <QDir>
#include <QMessageBox>
#include <QList>
#include <QCloseEvent>
#include <QLineEdit>
#include <qstandardpaths.h>

//notes and config files saves -> %appdata% -> colorful_notes folder -> notes folder (notes files) config folder(config files)

bool deactiveSignalCurrentItemChanged = false;
bool deactiveSignalTextChanged = false;


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)


{
    ui->setupUi(this);
    connections();
    widgetItemSettings();
    createFolders();
    getFilePathsForRead(noteDirectory,configDirectory);

}

MainWindow::~MainWindow()
{
    delete ui;   
}

void MainWindow::connections(){

connect(ui->addNewNoteButton,&QPushButton::clicked,this,&MainWindow::addNewNoteButtonClicked);
connect(ui->textColorButton,&QPushButton::clicked,this,&MainWindow::textColorButtonClicked);
connect(ui->copyTextButton,&QPushButton::clicked,this,&MainWindow::copyTextButtonClicked);
connect(ui->cutTextButton,&QPushButton::clicked,this,&MainWindow::cutTextButtonClicked);
connect(ui->pasteTextButton,&QPushButton::clicked,this,&MainWindow::pasteTextButtonClicked);
connect(ui->undoTextButton,&QPushButton::clicked,this,&MainWindow::undoTextButtonClicked);
connect(ui->redoTextButton,&QPushButton::clicked,this,&MainWindow::redoTextButtonClicked);
connect(ui->textFontButton,&QPushButton::clicked,this,&MainWindow::textFontButtonClicked);
connect(ui->deleteNoteButton,&QPushButton::clicked,this,&MainWindow::deleteNoteButtonClicked);
connect(ui->listWidget, &QListWidget::currentItemChanged, this, &MainWindow::onListItemSelectionChanged);
connect(ui->textEdit, &QTextEdit::textChanged, this, &MainWindow::onSelectedItemContentChanged);
}

void MainWindow::widgetItemSettings(){

ui->deleteNoteButton->setToolTip("Delete Checked Note");
ui->addNewNoteButton->setToolTip("Add New Note");
ui->copyTextButton->setToolTip("Copy Selected Text");
ui->cutTextButton->setToolTip("Cut Selected Text");
ui->pasteTextButton->setToolTip("Paste Selected Text");
ui->redoTextButton->setToolTip("Redo");
ui->textColorButton->setToolTip("Change Color of Selected Text");
ui->textFontButton->setToolTip("Change Font of Selected Text");
ui->undoTextButton->setToolTip("Undo");
setListWidget();

}

void MainWindow::createFolders(){

QString folderPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
QString noteFolderPath = QDir(folderPath).filePath("notes");
QString configFolderPath = QDir(folderPath).filePath("config");

if (!QDir(noteFolderPath).exists()) {

    QDir().mkpath(noteFolderPath);
}

if (!QDir(configFolderPath).exists()) {

    QDir().mkpath(configFolderPath);
}

noteDirectory = noteFolderPath;
configDirectory = configFolderPath;

}

void MainWindow::getFilePathsForRead(const QString &notesFolderPath,const QString &configFolderPath){

QDir notes_directory(notesFolderPath);
QDir config_directory(configFolderPath);
QStringList nameFilters;
nameFilters << "*.txt";
QStringList noteFiles = notes_directory.entryList(nameFilters, QDir::Files);
QStringList configFiles = config_directory.entryList(nameFilters,QDir::Files);

   for (int i = 0; i < noteFiles.size(); ++i) {

      QString notePath = notes_directory.filePath(noteFiles[i]);
      QString configPath = config_directory.filePath(configFiles[i]);
      readFiles(notePath,configPath);
      //noteTxtPath.append(notePath);
      //configTxtPath.append(configPath);
    }
}

void MainWindow::readFiles(const QString &notePath,const QString &configPath){

    QFile noteFile(notePath);
    QString title;
    QString note;
    QList<QColor>colorFormat;
    QList<QFont> fontFormat;

    if (noteFile.open(QIODevice::ReadOnly | QIODevice::Text)){

      QTextStream in(&noteFile);
      title = in.readLine();    
      note = in.readAll();    
      noteFile.close();
    }

    else{

      qDebug() << "File cannot opened: " << notePath;
    }


    QFile configFile(configPath);

    if (configFile.open(QIODevice::ReadOnly | QIODevice::Text)){

      QTextStream configIn(&configFile);

      QString line = configIn.readLine();
      QString nextLine = configIn.readLine();    
      QStringList colorCodes = line.mid(6).split(',');
      QStringList fontCodes = nextLine.mid(5).split(':');

      for (int i = 0; i < colorCodes.size(); ++i) {

          QColor color;
          color.setRgb(QRgb(colorCodes[i].mid(1).toUInt(nullptr, 16)));
          QFont font;
          font.fromString(fontCodes[i]);
          colorFormat.append(color);
          fontFormat.append(font);
       }
      configFile.close();

    }

    else{

      qDebug() << "File cannot opened: " << configPath;

    }

    loadNotesToListWidget(title,note,colorFormat,fontFormat,notePath,configPath);
}

void MainWindow::loadNotesToListWidget(const QString &title,const QString &note,const QList<QColor> &colorFormat, const QList<QFont> &fontFormat,const QString &notePath,const QString &configPath){

    QLineEdit * noteTitle = new QLineEdit(this);
    QLabel *lastModified = new QLabel(this);
    QString lastM = QDateTime::currentDateTime().toString("yyyy.MM.dd-hh:mm:ss");

    QString noteTitleStyle = "QLineEdit {"
    "    border: 0px;"
    "    background: transparent;"
    "    border-bottom: 1px solid black;"
    "}";

    noteTitle->setStyleSheet(noteTitleStyle);
    noteTitle->setText(title);
    //noteTitle->setSelection(0,noteTitle->text().size());
    lastModified->setText(lastM);

    QWidget* widget = new QWidget(this);

    QVBoxLayout* layout = new QVBoxLayout(widget);

    layout->addWidget(noteTitle,0,Qt::AlignLeft);
    layout->addWidget(lastModified,0,Qt::AlignLeft);

    QListWidgetItem * item = new QListWidgetItem;

    ui->listWidget->addItem(item);
    item->setSizeHint(widget->sizeHint());
    ui->listWidget->setItemWidget(item, widget);

    itemMap.insert(item, note);
    notes.append(note);
    colorList.append(colorFormat);
    fontList.append(fontFormat);
    itemColor.insert(item,colorFormat);
    itemFont.insert(item,fontFormat);
    titles.append(noteTitle);
    noteTxtPath.append(notePath);
    configTxtPath.append(configPath);
}

void MainWindow::setListWidget(){

    QLineEdit * noteTitle = new QLineEdit(this);
    QLabel *lastModified = new QLabel(this);
    QString lastM = QDateTime::currentDateTime().toString("yyyy.MM.dd-hh:mm:ss");

    QString noteTitleStyle = "QLineEdit {"
                             "    border: 0px;"
                             "    background: transparent;"
                             "    border-bottom: 1px solid black;"
                             "}";

    noteTitle->setStyleSheet(noteTitleStyle);
    noteTitle->setText("New Note");
    //noteTitle->setSelection(0,noteTitle->text().size());
    lastModified->setText(lastM);

    QWidget* widget = new QWidget(this);

    QVBoxLayout* layout = new QVBoxLayout(widget);

    layout->addWidget(noteTitle,0,Qt::AlignLeft);
    layout->addWidget(lastModified,0,Qt::AlignLeft);

    QListWidgetItem * item = new QListWidgetItem;
    deactiveSignalCurrentItemChanged = true;
    ui->listWidget->insertItem(0, item);
    ui->listWidget->setCurrentItem(item);
    item->setSizeHint(widget->sizeHint());
    ui->listWidget->setItemWidget(item, widget);

}

void MainWindow::getNotesStyle(){

    QTextCursor cursor =ui->textEdit->textCursor();
    QList<QColor>colorFormat;
    QList<QFont>fontFormat;

    for (int i = 0; i <ui->textEdit->toPlainText().size(); ++i) {

      cursor.setPosition(i);
      cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
      QTextCharFormat charFormat = cursor.charFormat();
      QColor color = charFormat.foreground().color();
      QFont font =charFormat.font();
      colorFormat.append(color);
      fontFormat.append(font);
    }
    colorList.append(colorFormat);
    fontList.append(fontFormat);
    itemColor.insert(ui->listWidget->currentItem(),colorFormat);
    itemFont.insert(ui->listWidget->currentItem(),fontFormat);

}

void MainWindow::addNewNoteButtonClicked(){

    if(!itemMap.contains(ui->listWidget->currentItem())){

      itemMap.insert(ui->listWidget->currentItem(), ui->textEdit->toPlainText());
      getNotesStyle();
      notes.append(itemMap.value(ui->listWidget->currentItem()));
      QWidget* widget = ui->listWidget->itemWidget(ui->listWidget->currentItem());
      QLineEdit* currentLineEdit = widget->findChild<QLineEdit*>();
      titles.append(currentLineEdit);
      createFilePaths();
    }

    ui->textEdit->clear();
    setListWidget();
}

void MainWindow::onListItemSelectionChanged(){


if(deactiveSignalCurrentItemChanged == false){

    QListWidgetItem *currentItem = ui->listWidget->currentItem();

    if (itemMap.contains(currentItem) && itemColor.contains(currentItem) && itemFont.contains(currentItem)){

        loadTextEditContent(itemMap.value(currentItem),itemColor.value(currentItem),itemFont.value(currentItem));

    }

    else{

        ui->textEdit->clear();
    }
}
deactiveSignalCurrentItemChanged = false;
}

void MainWindow::loadTextEditContent(const QString &content,const QList<QColor>&colorList,const QList<QFont>&fontList) {

deactiveSignalTextChanged = false;

if(deactiveSignalTextChanged == false){

    ui->textEdit->setText(content);
    QTextCursor cursor =ui->textEdit->textCursor();

    for (int i = 0; i <ui->textEdit->toPlainText().size(); ++i) {

        cursor.setPosition(i);
        cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
        QTextCharFormat charFormat = cursor.charFormat();
        QColor color = colorList[i];
        QFont font = fontList[i];
        charFormat.setForeground(color);
        charFormat.setFont(font);
        cursor.setCharFormat(charFormat);
    }
}
deactiveSignalTextChanged = true;
}

void MainWindow::onSelectedItemContentChanged(){

    if(ui->textEdit->toPlainText().size() != 0 && deactiveSignalTextChanged == true){

        QListWidgetItem *currentItem = ui->listWidget->currentItem();

       if (itemMap.contains(currentItem) && itemColor.contains(currentItem) && itemFont.contains(currentItem)){

            QString currentValue = itemMap.value(currentItem);
            QList<QColor> currentColor = itemColor.value(currentItem);
            QList<QFont> currentFont = itemFont.value(currentItem);                       

         if(currentValue != ui->textEdit->toPlainText() || currentColor != getCurrentColor() || currentFont != getCurrentFont()){

               updateLists(currentValue,0);
               currentValue = ui->textEdit->toPlainText();
               itemMap.insert(currentItem, currentValue);
               itemColor.insert(currentItem,getCurrentColor());
               itemFont.insert(currentItem,getCurrentFont());
            }
        }
    }
}

void MainWindow::updateLists(const QString &text,const bool &whichFuncCalled){

    for(int i = 0; i < notes.size(); i++){

        if(text == notes[i]){

            if(whichFuncCalled == 0){

               notes[i] = ui->textEdit->toPlainText();
               colorList[i] = getCurrentColor();
               fontList[i] = getCurrentFont();


               break;
            }

            else{

               notes.removeAt(i);
               colorList.removeAt(i);
               fontList.removeAt(i);
               titles.removeAt(i);

               QFile deleteNoteTxt(noteTxtPath[i]);
               QFile deleteConfigTxt(configTxtPath[i]);

               if(deleteNoteTxt.exists() && deleteConfigTxt.exists()){
                   deleteNoteTxt.remove();
                   deleteConfigTxt.remove();
               }
               noteTxtPath.removeAt(i);
               configTxtPath.removeAt(i);
               break;
            }
        }
    }
}

QList<QColor> MainWindow::getCurrentColor() const {

    QList<QColor> currentColorList;
    QTextCursor cursor = ui->textEdit->textCursor();

    for (int i = 0; i < ui->textEdit->toPlainText().size(); ++i) {

        cursor.setPosition(i);
        cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
        QTextCharFormat charFormat = cursor.charFormat();
        QColor color = charFormat.foreground().color();
        currentColorList.append(color);
    }
    return currentColorList;
}

QList<QFont> MainWindow::getCurrentFont() const {

    QList<QFont> currentFontList;
    QTextCursor cursor = ui->textEdit->textCursor();

    for (int i = 0; i < ui->textEdit->toPlainText().size(); ++i) {

        cursor.setPosition(i);
        cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
        QTextCharFormat charFormat = cursor.charFormat();
        QFont font = charFormat.font();
        currentFontList.append(font);
    }
    return currentFontList;
}

void MainWindow::textColorButtonClicked(){

 QColor color = QColorDialog::getColor(ui->textEdit->textColor(), this, "Select Color", QColorDialog::DontUseNativeDialog);

 if(color.isValid()){

     ui->textEdit->setTextColor(color);
    }
}

void MainWindow::copyTextButtonClicked(){

    ui->textEdit->copy();
}

void MainWindow::cutTextButtonClicked(){

    ui->textEdit->cut();
}

void MainWindow::pasteTextButtonClicked(){

    ui->textEdit->paste();
}

void MainWindow::undoTextButtonClicked(){

    ui->textEdit->undo();
}

void MainWindow::redoTextButtonClicked(){

    ui->textEdit->redo();
}

void MainWindow::textFontButtonClicked(){

    bool ok;
    QFont font = QFontDialog::getFont(&ok,ui->textEdit->font(),this);

    if(ok){

     QTextCursor cursor = ui->textEdit->textCursor();
     QTextCharFormat format;
     format.setFont(font);
     cursor.mergeCharFormat(format);
    }
}

void MainWindow::deleteNoteButtonClicked(){

   auto  * currentItem =ui->listWidget->currentItem();

    if(currentItem != nullptr){

     int ret = QMessageBox::question(this,"Delete","Delete the note ?",QMessageBox::Ok|QMessageBox::Cancel);

     if(ret == QMessageBox::Ok){


            if (itemMap.contains(currentItem) && itemColor.contains(currentItem) && itemFont.contains(currentItem)){

               updateLists(itemMap.value(currentItem),1);

               itemMap.remove(currentItem);
               itemFont.remove(currentItem);
               itemColor.remove(currentItem);
            }
            delete currentItem;
        }
    }
}


bool MainWindow::saveNotes() {

    for(int i = 0; i < noteTxtPath.size(); i++){

        saveToTxt(configTxtPath[i],noteTxtPath[i],i);

    }
    return true;
}

void MainWindow::closeEvent(QCloseEvent *event) {

    addNewNoteButtonClicked();

    if (saveNotes()) {

      event->accept();

    }

    else {

      event->ignore();
    }
}



void MainWindow::createFilePaths(){

      QString uniqueFileName =QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss_zzz") + "_note.txt";
      QString noteFilePath = QDir(noteDirectory).filePath(uniqueFileName);
      QString configFilePath = QDir(configDirectory).filePath(uniqueFileName);
      noteTxtPath.append(noteFilePath);
      configTxtPath.append(configFilePath);

}

void MainWindow::saveToTxt(const QString &configFilePath,const QString &noteFilePath,const int &index){



    QString note = notes[index];
    QString title = titles[index]->text();

    if(notes[index].size()!=0){

      QFile notefile(noteFilePath);
      if (notefile.open(QIODevice::WriteOnly | QIODevice::Text)){

            QTextStream outNote(&notefile);
            outNote <<title<<"\n"<<note;


            notefile.close();
      }

      else{
            qDebug() << "File cannot opened:";
      }

      QFile configfile(configFilePath);

      if (configfile.open(QIODevice::WriteOnly | QIODevice::Text)){

            QTextStream outConfig(&configfile);

            outConfig<<"Color:";

            for (int i = 0; i < colorList[index].size(); ++i) {

               outConfig << colorList[index][i].name();

               if(i< colorList[index].size()-1){

                   outConfig<<",";

               }
            }

            outConfig<<"\n"<<"Font:";


            for (int i = 0; i < fontList[index].size(); ++i) {

               outConfig << fontList[index][i].toString();

               if(i<fontList[index].size()-1){

                   outConfig<<":";
               }
            }
            configfile.close();
        }

      else{
            qDebug() << "File cannot opened:";
        }
    }
}





