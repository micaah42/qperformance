#include "performancechart.h"

#include <QLoggingCategory>
#include <QPainter>
#include <QPainterPath>
#include <QQuickItemGrabResult>

namespace {
Q_LOGGING_CATEGORY(self, "performancechart")
}

PerformanceChartBase::PerformanceChartBase()
    : _duration{5000}
    , _colors{Qt::white}
    , _thickness{2.}
{
    connect(this, &QQuickItem::heightChanged, this, &PerformanceChartBase::reset);
    connect(this, &QQuickItem::widthChanged, this, &PerformanceChartBase::reset);
};

LongTimeChart::LongTimeChart() {}

void LongTimeChart::paint(QPainter *painter)
{
    if (_prevValues.size() != _values.size()) {
        qCWarning(self) << "value size mismatch!";
        return;
    }

    painter->setRenderHint(QPainter::Antialiasing);

    double pxPerMs = this->width() / (double) this->duration();
    double pxPerPercent = this->height() / 100.;
    double changedPx = pxPerMs * (_t - _prevT);

    QImage newImage(width(), height(), QImage::Format_ARGB32_Premultiplied);
    newImage.fill(Qt::transparent);

    QPainter pp{&newImage};
    pp.setRenderHint(QPainter::Antialiasing);

    QRectF target{0, 0, width() - changedPx, height()};
    QRectF source{changedPx, 0, width() - changedPx, height()};
    pp.drawImage(target, _image, source);

    for (int i = 0; i < _values.size(); ++i) {
        QPointF from{this->width() - changedPx, this->height() - pxPerPercent * _prevValues[i]};
        QPointF to{this->width(), this->height() - pxPerPercent * _values[i]};
        QPen pen{this->color(i), this->thickness(), Qt::SolidLine, Qt::RoundCap};
        pp.setPen(pen);
        pp.drawLine(from, to);
    }

    painter->drawImage(_image.rect(), newImage, _image.rect());
    _image = newImage;
}

QColor PerformanceChartBase::color(int i)
{
    return _colors.empty() ? QColor{0, 0, 0} : _colors[i % _colors.size()];
}

void LongTimeChart::newValues(const QList<double> &values, qint64 t)
{
    _prevValues = _values;
    _values = values;

    _prevT = _t;
    _t = t;

    this->update();
}

void LongTimeChart::reset()
{
    _image = QImage{QSize(this->width(), this->height()), QImage::Format_ARGB32_Premultiplied};
    _image.fill(Qt::transparent);
}

double PerformanceChartBase::duration() const
{
    return _duration;
}

void PerformanceChartBase::setDuration(double newDuration)
{
    if (_duration == newDuration)
        return;

    _duration = newDuration;
    emit durationChanged();
}

const QList<QColor> &PerformanceChartBase::colors() const
{
    return _colors;
}

void PerformanceChartBase::setColors(const QList<QColor> &newColors)
{
    if (_colors == newColors)
        return;

    _colors = newColors;
    emit colorsChanged();
}

PerformanceChart::PerformanceChart() {}

void PerformanceChart::newValues(const QList<double> &values, qint64 t)
{
    if (!_values.empty() && _values.back().size() != values.size()) {
        qCWarning(self) << "value size mismatch! resetting...";
        this->reset();
        return;
    }

    _values.push_back(values);
    _ts.push_back(t);

    while (_ts.front() < t - this->duration()) {
        _values.pop_front();
        _ts.pop_front();
    }

    this->update();
}

void PerformanceChart::reset()
{
    _values.clear();
    _ts.clear();
    this->update();
}

void PerformanceChart::paint(QPainter *painter)
{
    if (_ts.empty())
        return;

    painter->setRenderHint(QPainter::Antialiasing);

    double pxPerMs = this->width() / (double) this->duration();
    double pxPerPercent = this->height() / 100.;

    qint64 now = _ts.back();
    int n = _values.front().size();

    for (int j = 0; j < n; j++) {
        QPen pen{this->color(j), this->thickness(), Qt::SolidLine, Qt::RoundCap};
        painter->setPen(pen);

        for (size_t i = 1; i < _ts.size(); i++) {
            QPointF from{this->width() - pxPerMs * (now - _ts[i]), this->height() - pxPerPercent * _values[i][j]};
            QPointF to{this->width() - pxPerMs * (now - _ts[i - 1]), this->height() - pxPerPercent * _values[i - 1][j]};
            painter->drawLine(from, to);
        }
    }
}

double PerformanceChartBase::thickness() const
{
    return _thickness;
}

void PerformanceChartBase::setThickness(double newThickness)
{
    if (qFuzzyCompare(_thickness, newThickness))
        return;

    _thickness = newThickness;
    emit thicknessChanged();
}
