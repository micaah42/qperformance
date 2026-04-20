#include "lazyredrawchart.h"

#include <QLoggingCategory>
#include <QPainter>

namespace {
Q_LOGGING_CATEGORY(self, "qperformance.lazychart")
}

LazyRedrawChart::LazyRedrawChart() {}

void LazyRedrawChart::paint(QPainter *painter)
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
        auto color = this->color(i);
        auto lineWidth = this->lineWidth(i);

        if (color.alpha() == 0 || lineWidth == 0)
            continue;

        QPointF from{this->width() - changedPx, this->height() - pxPerPercent * _prevValues[i]};
        QPointF to{this->width(), this->height() - pxPerPercent * _values[i]};
        QPen pen{color, lineWidth, Qt::SolidLine, Qt::RoundCap};
        pp.setPen(pen);
        pp.drawLine(from, to);
    }

    painter->drawImage(_image.rect(), newImage, _image.rect());
    _image = newImage;
}

void LazyRedrawChart::pushValues2(const QList<double> &values, qint64 t)
{
    _prevValues = _values;
    _values = values;

    _prevT = _t;
    _t = t;

    this->requestUpdate();
}

void LazyRedrawChart::reset()
{
    _image = QImage{QSize(this->width(), this->height()), QImage::Format_ARGB32_Premultiplied};
    _image.fill(Qt::transparent);
}
