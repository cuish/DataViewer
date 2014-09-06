#ifndef TABLEMODEL_H
#define TABLEMODEL_H

#include <QAbstractTableModel>
#include <QTextStream>
#include <QStringList>

// class to represent one Row Data
class RowData{
public:
    RowData(double column1, unsigned int column2){
        this->column1=column1; this->column2=column2;
    };

    bool operator <(const RowData other) const{
        return column1<other.column1;
    };

    double column1;
    unsigned int column2;
    // so that default table delegate will check for valid inputs
};

class TableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit TableModel(QObject *parent = 0);

    bool loadFile(QTextStream &in);
    void saveFile(QTextStream &out);

    int rowCount(const QModelIndex &parent=QModelIndex()) const;
    int columnCount(const QModelIndex &parent=QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role= Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);

    // getData without delegate
    QVariant getData(const int row, const int column) const
    {
        if(column==0)
            return mData.at(row).column1;
        else if(column==1)
            return mData.at(row).column2;
        return QVariant::Invalid;
    };

    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role);

    Qt::ItemFlags flags(const QModelIndex &index) const;

    bool insertRows(int row, int count, const QModelIndex &parent);
    bool removeRows(int row, int count, const QModelIndex &parent);

    bool insert(const QModelIndex &parent){
        return insertRows( parent.row(), 1, parent);
    };
    bool remove(const QModelIndex &parent){
        return removeRows( parent.row(), 1, parent);
    };

    bool isFileDataChanged() const{return fileDataChanged;};

signals:

public slots:

private:
    void sortByColumn1();

    QStringList mHeader;
    QList<RowData> mData;

    bool fileDataChanged;
};

#endif // TABLEMODEL_H
