#ifndef POINTRANSAC_H
#define POINTRANSAC_H

#include <QObject>
#include <AbstractModel.hpp>
#include <QPointF>

using namespace GRANSAC;

class pointRANSAC : public AbstractParameter
{
public:
    QVector<pointRANSAC>* m_Point2D;
    pointRANSAC();
    pointRANSAC(QPointF point)
    {

    //    m_Point2D[0] = point.x();
    //	m_Point2D[1] = point.y();
    }
};

#endif // POINTRANSAC_H
