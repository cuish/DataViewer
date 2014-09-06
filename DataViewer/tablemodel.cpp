#include "tablemodel.h"

TableModel::TableModel(QObject *parent) :
    QAbstractTableModel(parent), fileDataChanged(false)
{
    mHeader.append("Energy (keV)");
    mHeader.append("Counts");
    mData.append(RowData(0,0));
}

// read data from filestream
bool TableModel::loadFile(QTextStream &in)
{
    mHeader.clear();
    mData.clear();

    QString line;
    QStringList lineSplit;

    line=in.readLine();
    lineSplit= line.split(",",QString::SkipEmptyParts);
    if(lineSplit.size()>2)
    {
        return false;   // for now we only handle two-columned data
    }
    mHeader.append(lineSplit.at(0));
    mHeader.append(lineSplit.at(1));

    RowData rowData(0,0);
    double column1, column2;
    while((line=in.readLine())!=NULL)
    {
        lineSplit= line.split(",",QString::SkipEmptyParts);
        if(lineSplit.size()>2)
        {
            return false; // for now we only handle two-columned data
        }
        column1=lineSplit.at(0).toDouble();
        column2=lineSplit.at(1).toDouble();
        rowData.column1=column1;
        rowData.column2=column2;
        mData.append(rowData);
    }

    sortByColumn1(); // sort source data
    return true;
}

// write data to filestream
void TableModel::saveFile(QTextStream &out)
{
    out<<mHeader.at(0)<<","<<mHeader.at(1)<<endl;
    for(int i=0; i<mData.size();i++)
    {
        out<<mData.at(i).column1<<","<<mData.at(i).column2<<endl;
    }
    fileDataChanged = false;
}

int TableModel::columnCount(const QModelIndex &parent) const
{
    return mHeader.size();
}

int TableModel::rowCount(const QModelIndex &parent) const
{
    return mData.size();
}

QVariant TableModel::data(const QModelIndex &index, int role ) const
{
    if(index.isValid() && (role == Qt::EditRole|| role == Qt::DisplayRole))
    {
         if(index.column()==0)
            return mData.at(index.row()).column1;
         else if(index.column()==1)
            return mData.at(index.row()).column2;
    }
    return QVariant();
}

bool TableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(index.isValid() && role==Qt::EditRole)
    {
         if(index.column()==0)
         {
            for(int i=0; i<mData.size(); i++)
            {
                // we should not have the same energy value twice
                if(value.toDouble()==mData.at(i).column1 && (i!=index.column()))
                    return false;
            }
            mData[index.row()].column1=value.toDouble();
            sortByColumn1(); //will emit layoutChanged();
         }
         else if(index.column()==1)
         {
            mData[index.row()].column2=value.toDouble();
            emit dataChanged(index, index);
         }
         fileDataChanged = true;
         return true;
    }
    return false;
}


QVariant TableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation==Qt::Horizontal  && (role == Qt::EditRole|| role == Qt::DisplayRole))
        if(section<mHeader.size())
            return mHeader.at(section);
    return QVariant();
}

bool TableModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
    if (orientation==Qt::Horizontal && role == Qt::EditRole)
        if(section<mHeader.size())
        {
            mHeader[section]=value.toString();
            emit headerDataChanged(orientation, section, section);
            return true;
        }
    return false;
}

Qt::ItemFlags TableModel::flags(const QModelIndex &index) const
{
    if(index.isValid() && index.column()<2 && index.row()<mData.size())
    {
        return Qt::ItemIsEditable|Qt::ItemIsEnabled;
    }
    return Qt::NoItemFlags;
}

bool TableModel::  insertRows(int row, int count, const QModelIndex &parent)
{
    if (row<mData.size() && count>0)
    {
        QAbstractItemModel::beginInsertRows(parent,row,row+count-1);
        while(count>0)
        {
            count--;
            mData.insert(row,RowData(0,0));
        }
        emit layoutChanged();
        QAbstractItemModel::endInsertRows();
        return true;
    }
    return false;
}


bool TableModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if(row<mData.size() && count>0)
    {
        QAbstractItemModel::beginRemoveRows(parent,row,row+count-1);
        while(count>0)
        {
            count--;
            if(mData.size()>1)
                mData.removeAt(row);
            else
                mData[0]=RowData(0,0);
        }
        emit layoutChanged();
        QAbstractItemModel::endRemoveRows();
        return true;
    }
    return false;
}

// sort by column1 data
void TableModel::sortByColumn1()
{
    std::sort(mData.begin(),mData.end());
    emit layoutChanged();;
}
