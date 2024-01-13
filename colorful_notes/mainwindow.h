#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QListWidgetItem>
#include <QColor>
#include <QMap>
#include <QFont>
#include <QFont>
#include <QLineEdit>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

void addNewNoteButtonClicked();
void connections();
void textColorButtonClicked();
void copyTextButtonClicked();
void cutTextButtonClicked();
void pasteTextButtonClicked();
void undoTextButtonClicked();
void redoTextButtonClicked();
void textFontButtonClicked();
void deleteNoteButtonClicked();

protected:

void closeEvent(QCloseEvent *event) override;

private:

void updateLists(const QString &text,const bool &whichFuncCalled);
void widgetItemSettings();
void createFilePaths();
void saveToTxt(const QString &configFilePath,const QString &noteFilePath,const int &i);
void getNotesStyle();
void createFolders();
void onListItemSelectionChanged();
void loadTextEditContent(const QString &content,const QList<QColor>&colorList,const QList<QFont>&fontList);
void onSelectedItemContentChanged();
bool saveNotes();
QList<QColor> getCurrentColor() const;
QList<QFont> getCurrentFont() const;
void setListWidget();
void noteTitleChanged();
void updateTitleList(const QString &text ,const bool &whichFuncCalled);
void getFilePathsForRead(const QString &notesFolderPath,const QString &configFolderPath);
void readFiles(const QString &notePath,const QString &configPath);
void loadNotesToListWidget(const QString &title,const QString &note,const QList<QColor> &colorFormat, const QList<QFont> &fontFormat,const QString &notePath,const QString &configPath);

private:

Ui::MainWindow *ui;

QMap<QListWidgetItem*, QString> itemMap;
QMap<QListWidgetItem*,QList<QColor>>itemColor;
QMap<QListWidgetItem*,QList<QFont>>itemFont;

QString configDirectory;
QString noteDirectory;

QList<QList<QColor>> colorList;
QList<QList<QFont>> fontList;
QList<QLineEdit*> titles;
QStringList notes;
QStringList noteTxtPath;
QStringList configTxtPath;

};
#endif // MAINWINDOW_H
