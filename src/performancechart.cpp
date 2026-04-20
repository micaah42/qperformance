#include "performancechart.h"

#include <QLoggingCategory>
#include <QPainter>
#include <QPainterPath>
#include <QQuickItemGrabResult>

namespace {
Q_LOGGING_CATEGORY(self, "qperformance.chart")
}

PerformanceChart::PerformanceChart() {}

void PerformanceChart::pushValues2(const QList<double> &values, qint64 t)
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

    this->requestUpdate();
}

void PerformanceChart::reset()
{
    _values.clear();
    _ts.clear();
    this->requestUpdate();
}

void PerformanceChart::paint(QPainter *painter)
{
    if (_ts.empty())
        return;

    painter->setRenderHint(QPainter::Antialiasing);

    if (this->max() == this->min())
        return;

    double ft = this->width() / (double) this->duration();
    double yf = 1. / (this->max() - this->min());

    qint64 now = _ts.back();
    int n = _values.front().size();

    for (int j = 0; j < n; j++) {
        auto color = this->color(j);
        auto lineWidth = this->lineWidth(j);

        if (color.alpha() == 0 || lineWidth == 0)
            continue;

        QPen pen{color, lineWidth, Qt::SolidLine, Qt::RoundCap};
        painter->setPen(pen);

        for (size_t i = 1; i < _ts.size(); i++) {
            double y1 = (_values[i][j] - this->min()) * yf;
            double y2 = (_values[i - 1][j] - this->min()) * yf;
            QPointF from{
                this->width() - ft * (now - _ts[i]),
                this->height() * (1 - y1),
            };
            QPointF to{
                this->width() - ft * (now - _ts[i - 1]),
                this->height() * (1 - y2),
            };
            painter->drawLine(from, to);
        }
    }
}
