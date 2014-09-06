#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    model = new TableModel;
    ui->tableView->setModel(model);
    ui->graphView->setModel(model);

    createActions();
    createMenus();
    readSettings();
    setCurrentFile("");

    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableView, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(onCustomContextMenu(const QPoint &)));
}

MainWindow::~MainWindow()
{
    if(ui!=NULL)
        delete ui;
    if(contextMenu!=NULL)
        delete contextMenu;
    if(model!=NULL)
        delete model;
}

void MainWindow::insert()
{
    model->insert(index);
}

void MainWindow::remove()
{
    if (index.isValid())
    {
        model->remove(index);
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (maybeSave())
    {
        writeSettings();
        event->accept();
    }
    else
    {
        event->ignore();
    }
}

void MainWindow::onCustomContextMenu(const QPoint &point)
{
    index = ui->tableView->indexAt(point);
    if (index.isValid())
    {
        contextMenu->exec(ui->tableView->mapToGlobal(point));
    }
}

void MainWindow::newFile()
{
    if (maybeSave())
    {
        setCurrentFile("");
        if (model!=NULL)
        {
            delete model;
            model= new TableModel;
            ui->tableView->setModel(model);
        }
    }
}

void MainWindow::open()
{
    if (maybeSave())
    {
        QString fileName = QFileDialog::getOpenFileName(this,
                                                        tr("Open File"),
                                                        "",
                                                        tr("Tables (*.csv)"));
        if (!fileName.isEmpty())
            loadFile(fileName);
    }
}

bool MainWindow::save()
{
    if (curFile.isEmpty())
    {
        return saveAs();
    }
    else {
        return saveFile(curFile);
    }
}

bool MainWindow::saveAs()
{
    QString file = QFileDialog::getSaveFileName(this,
                                               tr("Save as"),
                                               tr(""),
                                               tr("Tables (*.csv)"));
    if (file.isEmpty())
        return false;

    return saveFile(file);
}

void MainWindow::createActions()
{
    newAct = new QAction(tr("&New"), this);
    connect(newAct, SIGNAL(triggered()), this, SLOT(newFile()));

    openAct = new QAction(tr("&Open..."), this);
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    saveAct = new QAction(tr("&Save"), this);
    connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

    saveAsAct = new QAction(tr("Save &As..."), this);
    connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

    exitAct = new QAction(tr("&Exit"), this);
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    insertAct= new QAction(tr("&Insert"), this);
    connect(insertAct, SIGNAL(triggered()), this, SLOT(insert()));

    removeAct= new QAction(tr("&Remove"), this);
    connect(removeAct, SIGNAL(triggered()), this, SLOT(remove()));
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newAct);

    fileMenu->addAction(openAct);

    fileMenu->addAction(saveAct);

    fileMenu->addAction(saveAsAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    contextMenu=new QMenu();
    contextMenu->addAction(insertAct);
    contextMenu->addAction(removeAct);
}

void MainWindow::readSettings()
{
    QSettings settings("QtProject", "csv");
    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("size", QSize(400, 400)).toSize();
    resize(size);
    move(pos);
}

void MainWindow::writeSettings()
{
    QSettings settings("QtProject", "csv");
    settings.setValue("pos", pos());
    settings.setValue("size", size());
}


bool MainWindow::maybeSave()
{
    if (model->isFileDataChanged())
    {
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(this, tr("Application"),
                     tr("The document has been modified.\n"
                        "Do you want to save your changes?"),
                     QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        if (ret == QMessageBox::Save)
            return save();
        else if (ret == QMessageBox::Cancel)
            return false;
    }
    return true;
}

void MainWindow::loadFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return;
    }

    QTextStream in(&file);
    model->loadFile(in);
    ui->tableView->setModel(model);
    setCurrentFile(fileName);
}

bool MainWindow::saveFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text))
    {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }

    QTextStream out(&file);
    model->saveFile(out);

    setCurrentFile(fileName);
    return true;
}

void MainWindow::setCurrentFile(const QString &fileName)
{
    curFile = fileName;
    setWindowModified(false);

    QString shownName = curFile;
    if (curFile.isEmpty())
        shownName = "untitled.csv";
    setWindowFilePath(shownName);
}

