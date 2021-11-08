#pragma once

#include <QObject>
#include <QList>
#include <QSharedPointer>

enum class MeasureStatus
{
    Error,
    Ok
};

class MeasureElement
{
    Q_GADGET
    Q_PROPERTY(double x READ x WRITE setX)
    Q_PROPERTY(double y READ y WRITE setY)
public:
    MeasureElement() = default;
    MeasureElement(double yValue, double xValue);

    double x() const { return m_x; }
    double y() const { return m_y; }
    void setX(const double &xValue) { m_x = xValue; }
    void setY(const double &yValue) { m_y = yValue; }

private:
    double m_y;
    double m_x;
};

using MeasureList = QList<MeasureElement>;
using MeasureListPtr = QSharedPointer<MeasureList>;

Q_DECLARE_METATYPE(MeasureElement)
Q_DECLARE_METATYPE(MeasureList)
