/******** Wrapper for QCustomPlot in a Model/View design pattern****/

#ifndef GRAPHVIEW_H
#define GRAPHVIEW_H

#include <QtGui>
#include <QWidget>
#include <QToolButton>
#include <QAbstractItemModel>
#include <QModelIndexList>
#include "tablemodel.h"

class PlotSettings
{
public:
    PlotSettings(double minX=0, double minY=0,double maxX=10,double maxY=10);
    void scroll(int dx, int dy);
    void adjust();
    double spanX() const { return maxX - minX; }
    double spanY() const { return maxY - minY; }
    double minX, minY, maxX, maxY;
    int numXTicks, numYTicks;
private:
    static void adjustAxis(double &min, double &max, int &numTicks);
};

class GraphView: public QWidget
{
    Q_OBJECT

public:
    GraphView(QWidget * parent = 0);

    void setModel(TableModel *model);

    void setPlotSettings(const PlotSettings &settings);

    void setCurve(const QVector<QPointF> &data);
    void clearCurve();

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

public slots:
    void updateChangedData(QModelIndex topLeft ,QModelIndex bottomRight);
    void updateAllData();

    void zoomIn();
    void zoomOut();

protected:
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void wheelEvent(QWheelEvent *event);

private:
    void updateRubberBandRegion();
    void refreshPixmap();
    void drawGrid(QPainter *painter);
    void drawCurves(QPainter *painter);
    void upDatePlotSettings();

    enum { Margin = 50 };

    QVector<double> dataX,dataY;
    QString labelX,labelY;
    TableModel *model;

    QToolButton *zoomInButton;
    QToolButton *zoomOutButton;

    QVector<PlotSettings> zoomStack;
    int curZoom;
    bool rubberBandIsShown;
    QRect rubberBandRect;
    QPixmap pixmap;
};

#endif // GRAPHVIEW_H
