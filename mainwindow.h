#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "IniFile.h"
#include <QMainWindow>
#include <QMessageBox>
#include <QFileDialog>
#include <QListWidget>
#include <QTreeWidgetItem>
#include <QInputDialog>
#include <QValidator>
#include <set>
#include <string>
#include <QString>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    IniFile *myIniFile;

    QValidator* intValid;
    QValidator* doubleValid;
    QValidator* boolValid;

private slots:
    void on_actionOpen_file_triggered();
    void on_actionOpen_directory_triggered();
    void on_listWidget_itemClicked(QListWidgetItem *item);
    void on_button_Plus_clicked();
    void on_button_Minus_clicked();
    void on_treeWidget_itemSelectionChanged();
    void on_comboBox_sections_currentTextChanged(const QString &arg1);
    void on_comboBox_type_currentTextChanged(const QString &arg1);

    void on_actionSave_triggered();

    void on_button_add_clicked();

    void on_button_delete_clicked();

private:
    void setInitialization();
    void setBlock();
    void setUnblock();
    void setClear();
    void setInfoOfFile();
    Ui::MainWindow *ui;
    IniFile *_IniFile;
};
#endif // MAINWINDOW_H
