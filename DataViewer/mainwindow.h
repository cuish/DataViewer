#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAction>
#include <QMenu>
#include <QString>
#include <QModelIndex>
#include <QTableView>
#include <QtWidgets>

#include "tablemodel.h"
#include "graphview.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void newFile();
    void open();
    bool save();
    bool saveAs();

private slots:
     void onCustomContextMenu(const QPoint &);
     void insert();
     void remove();

private:
    void createActions();
    void createMenus();
    void readSettings();
    void writeSettings();
    bool maybeSave();
    void loadFile(const QString &fileName);
    bool saveFile(const QString &fileName);
    void setCurrentFile(const QString &fileName);

    QString curFile;

    QMenu *fileMenu;            // File operation menu (new/open/save/save as/exit)
    QMenu *contextMenu;         // Right click menu for table actions (insert Column/remove Column)

    // File operation actions
    QAction *newAct;
    QAction *openAct;
    QAction *saveAct;
    QAction *saveAsAct;
    QAction *exitAct;

    // Table operation actions
    QAction *insertAct;
    QAction *removeAct;

    TableModel *model;
    QTableView *tableView;
    GraphView *graphView;
    QModelIndex index;
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
