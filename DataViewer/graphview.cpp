#include <QModelIndex>
#include <cmath>
#include <QStylePainter>
#include <QStyleOptionFocusRect>

#include "graphview.h"

GraphView::GraphView(QWidget * parent):
    QWidget(parent), labelX("labelX"), labelY("labelY")
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setFocusPolicy(Qt::StrongFocus);

    rubberBandIsShown = false;

    zoomInButton = new QToolButton(this);
    zoomInButton->setIcon(QIcon(":/images/zoomin.png"));
    zoomInButton->adjustSize();
    connect(zoomInButton, SIGNAL(clicked()), this, SLOT(zoomIn()));

    zoomOutButton = new QToolButton(this);
    zoomOutButton->setIcon(QIcon(":/images/zoomout.png"));
    zoomOutButton->adjustSize();
    connect(zoomOutButton, SIGNAL(clicked()), this, SLOT(zoomOut()));

    setPlotSettings(PlotSettings());
}

void GraphView::setPlotSettings(const PlotSettings &settings)
{
    zoomStack.clear();
    zoomStack.append(settings);
    curZoom = 0;

    zoomInButton->setDisabled(true);
    zoomOutButton->setDisabled(true);

    refreshPixmap();
}

void GraphView::setModel(TableModel *model)
{
    this->model=model;

    connect(model,SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(updateChangedData(QModelIndex ,QModelIndex)));
    connect(model,SIGNAL(layoutChanged()), this, SLOT(updateAllData()));

    labelX=model->headerData(0,Qt::Horizontal,Qt::DisplayRole).toString();
    labelY=model->headerData(1,Qt::Horizontal,Qt::DisplayRole).toString();
}


void GraphView::updateChangedData(QModelIndex topLeft ,QModelIndex bottomRight)
{
    for(int j=topLeft.row();j<=bottomRight.row();j++)
    {
        if(topLeft.column()==0)
        {
            if(j<dataX.size())
                dataX[j]=model->getData(j,0).toDouble();
            else
                dataX.append(model->getData(j,0).toDouble());
        }

        if(bottomRight.column()>0)
        {
            if(j<dataY.size())
                dataY[j]=model->getData(j,1).toDouble();
            else
                dataY.append(model->getData(j,1).toDouble());
        }
    }
    upDatePlotSettings();
}

void GraphView::updateAllData()
{
    dataX.clear();
    dataY.clear();
    for(int j=0; j<model->rowCount();j++)
    {
       dataX.append(model->getData(j,0).toDouble());
       dataY.append(model->getData(j,1).toDouble());
    }
    upDatePlotSettings();
}

void GraphView::upDatePlotSettings()
{
    double minX,minY,maxX,maxY;
    if(dataX.size()>0)
    {
        minX=maxX=dataX.first();
        minY=maxY=dataX.first();
        for(int j=0; j<dataX.size();j++)
        {
            if(minX>dataX[j])
                minX=dataX[j];
            if(minY>dataY[j])
                minY=dataY[j];
            if(maxX<dataX[j])
                maxX=dataX[j];
            if(maxY<dataY[j])
                maxY=dataY[j];
        }
    }
    PlotSettings plotSettings(minX,minY,maxX,maxY);
    plotSettings.adjust();
    setPlotSettings(plotSettings);
}

void GraphView::zoomOut()
{
    if (curZoom > 0)
    {
        --curZoom;
        zoomOutButton->setEnabled(curZoom > 0);
        zoomInButton->setEnabled(true);
        zoomInButton->show();
        refreshPixmap();
    }
}

void GraphView::zoomIn()
{
    if (curZoom < zoomStack.count() - 1)
    {
        ++curZoom;
        zoomInButton->setEnabled(curZoom < zoomStack.count() - 1);
        zoomOutButton->setEnabled(true);
        zoomOutButton->show();
        refreshPixmap();
    }
}

void GraphView::clearCurve()
{
    dataX.clear();
    dataY.clear();
    refreshPixmap();
}

void GraphView::refreshPixmap()
{
    pixmap = QPixmap(size());
    QPainter painter(&pixmap);
    painter.initFrom(this);
    drawGrid(&painter);
    drawCurves(&painter);
    update();
}

void GraphView::drawGrid(QPainter *painter)
{
    QRect rect(Margin, Margin,
               width() - 2 * Margin, height() - 2 * Margin);

    if (!rect.isValid())
        return;

    PlotSettings settings = zoomStack[curZoom];
    QPen quiteDark = palette().dark().color().light();
    QPen light = palette().light().color();
    for (int i = 0; i <= settings.numXTicks; ++i)
    {
        int x = rect.left() + (i * (rect.width() - 1)
                               / settings.numXTicks);
        double label = settings.minX + (i * settings.spanX()
                                        / settings.numXTicks);
        painter->setPen(quiteDark);
        painter->drawLine(x, rect.top(), x, rect.bottom());
        painter->setPen(light);
        painter->drawLine(x, rect.bottom(), x, rect.bottom() + 5);
        painter->drawText(x - 50, rect.bottom() + 5, 100, 15,
                          Qt::AlignHCenter | Qt::AlignTop,
                          QString::number(label));
    }

    painter->drawText(rect.center().x(), rect.bottom() + 30,
                      labelX);

    for (int j = 0; j <= settings.numYTicks; ++j)
    {
        int y = rect.bottom() - (j * (rect.height() - 1)
                                 / settings.numYTicks);
        double label = settings.minY + (j * settings.spanY()
                                        / settings.numYTicks);
        painter->setPen(quiteDark);
        painter->drawLine(rect.left(), y, rect.right(), y);
        painter->setPen(light);
        painter->drawLine(rect.left() - 5, y, rect.left(), y);
        painter->drawText(rect.left() - Margin, y - 10, Margin - 5, 20,
                          Qt::AlignRight | Qt::AlignVCenter,
                          QString::number(label));
    }
    painter->drawRect(rect.adjusted(0, 0, -1, -1));

    // ugly draw label Y
    painter->translate(rect.left() - 30, rect.center().y());
    painter->rotate(270);
    painter->drawText(0,0,labelY);
    painter->resetTransform();
}


void GraphView::drawCurves(QPainter *painter)
{
    static const QColor colorForIds[6] = {
        Qt::red, Qt::green, Qt::blue, Qt::cyan, Qt::magenta, Qt::yellow
    };
    PlotSettings settings = zoomStack[curZoom];
    QRect rect(Margin, Margin,
               width() - 2 * Margin, height() - 2 * Margin);
    if (!rect.isValid())
        return;

    painter->setClipRect(rect.adjusted(+1, +1, -1, -1));

    QPolygonF polyline(dataX.size());

    for (int j = 0; j < dataX.size(); ++j)
    {
         double dx = dataX[j] - settings.minX;
         double dy = dataY[j] - settings.minY;
         double pointX = rect.left() + (dx * (rect.width() - 1)
                                      / settings.spanX());
         double pointY = rect.bottom() - (dy * (rect.height() - 1)
                                        / settings.spanY());
         polyline[j] = QPointF(pointX, pointY);
    }

    painter->setPen(Qt::yellow);
    painter->drawPolyline(polyline);
}

QSize GraphView::minimumSizeHint() const
{
    return QSize(6 * Margin, 4 * Margin);
}

QSize GraphView::sizeHint() const
{
    return QSize(12 * Margin, 8 * Margin);
}

void GraphView::paintEvent(QPaintEvent * /* event */)
{
    QStylePainter painter(this);
    painter.drawPixmap(0, 0, pixmap);

    if (rubberBandIsShown)
    {
        painter.setPen(palette().light().color());
        painter.drawRect(rubberBandRect.normalized().adjusted(0, 0, -1, -1));
    }

    if (hasFocus())
    {
        QStyleOptionFocusRect option;
        option.initFrom(this);
        option.backgroundColor = palette().dark().color();
        painter.drawPrimitive(QStyle::PE_FrameFocusRect, option);
    }
}

void GraphView::resizeEvent(QResizeEvent * /* event */)
{
    int x = width() - (zoomInButton->width()
                       + zoomOutButton->width() + 10);

    zoomInButton->move(x, 5);
    zoomOutButton->move(x + zoomInButton->width() + 5, 5);

    refreshPixmap();
}

void GraphView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        QRect rect(Margin, Margin,
                   width() - 2 * Margin, height() - 2 * Margin);

        if (rect.contains(event->pos()))
        {
            rubberBandIsShown = true;
            rubberBandRect.setTopLeft(event->pos());
            rubberBandRect.setBottomRight(event->pos());
            updateRubberBandRegion();
            setCursor(Qt::CrossCursor);
        }
    }
    else if (event->button() == Qt::RightButton)
    {
        zoomOut();
    }
}

void GraphView::mouseMoveEvent(QMouseEvent *event)
{
    if (rubberBandIsShown)
    {
        updateRubberBandRegion();
        rubberBandRect.setBottomRight(event->pos());
        updateRubberBandRegion();
    }
}

void GraphView::mouseReleaseEvent(QMouseEvent *event)
{
    if ((event->button() == Qt::LeftButton) && rubberBandIsShown)
    {
        rubberBandIsShown = false;
        updateRubberBandRegion();

        unsetCursor();

        QRect rect = rubberBandRect.normalized();

        if (rect.width() < 4 || rect.height() < 4)
            return;

        rect.translate(-Margin, -Margin);

        PlotSettings prevSettings = zoomStack[curZoom];
        PlotSettings settings;

        double dx = prevSettings.spanX() / (width() - 2 * Margin);
        double dy = prevSettings.spanY() / (height() - 2 * Margin);

        settings.minX = prevSettings.minX + dx * rect.left();
        settings.maxX = prevSettings.minX + dx * rect.right();
        settings.minY = prevSettings.maxY - dy * rect.bottom();
        settings.maxY = prevSettings.maxY - dy * rect.top();

        settings.adjust();

        zoomStack.resize(curZoom + 1);
        zoomStack.append(settings);
        zoomIn();
    }
}

void GraphView::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
        case Qt::Key_Plus:
            zoomIn();
            break;
        case Qt::Key_Minus:
            zoomOut();
            break;
        case Qt::Key_Left:
            zoomStack[curZoom].scroll(-1, 0);
            refreshPixmap();
            break;
        case Qt::Key_Right:
            zoomStack[curZoom].scroll(+1, 0);
            refreshPixmap();
            break;
        case Qt::Key_Down:
            zoomStack[curZoom].scroll(0, -1);
            refreshPixmap();
            break;
        case Qt::Key_Up:
            zoomStack[curZoom].scroll(0, +1);
            refreshPixmap();
            break;
        default:
            QWidget::keyPressEvent(event);
    }
}

void GraphView::wheelEvent(QWheelEvent *event)
{
    int numDegrees = event->delta() / 8;
    int numTicks = numDegrees / 15;

    if (event->orientation() == Qt::Horizontal)
    {
        zoomStack[curZoom].scroll(numTicks, 0);
    }
    else
    {
        zoomStack[curZoom].scroll(0, numTicks);
    }

    refreshPixmap();
}

void GraphView::updateRubberBandRegion()
{
    QRect rect = rubberBandRect.normalized();
    update(rect.left(), rect.top(), rect.width(), 1);
    update(rect.left(), rect.top(), 1, rect.height());
    update(rect.left(), rect.bottom(), rect.width(), 1);
    update(rect.right(), rect.top(), 1, rect.height());
}

PlotSettings::PlotSettings(double minX, double minY, double maxX, double maxY):
minX(minX), minY(minY), maxX(maxX), maxY(maxY), numXTicks(5), numYTicks(5)
{
}

void PlotSettings::scroll(int dx, int dy)
{
    double stepX = spanX() / numXTicks;
    minX += dx * stepX;
    maxX += dx * stepX;
    double stepY = spanY() / numYTicks;
    minY += dy * stepY;
    maxY += dy * stepY;
}

void PlotSettings::adjust()
{
    adjustAxis(minX, maxX, numXTicks);
    adjustAxis(minY, maxY, numYTicks);
}

// ajust Axis to provide a better view
void PlotSettings::adjustAxis(double &min, double &max,
                              int &numTicks)
{
    const int MinTicks = 4;
    double grossStep = (max - min) / MinTicks;
    double step = pow(10.0, floor(log10(grossStep)));
    if (5 * step < grossStep)
    {
        step *= 5;
    }
    else if (2 * step < grossStep)
    {
        step *= 2;
    }

    numTicks = int(ceil(max / step) - floor(min / step));
    if (numTicks < MinTicks)
        numTicks = MinTicks;

    min = floor(min / step) * step;
    max = ceil(max / step) * step;
}

