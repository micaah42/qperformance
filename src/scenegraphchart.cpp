// SPDX-FileCopyrightText: 2026 Michael Wipplinger (micaah42)
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "scenegraphchart.h"

#include <QLoggingCategory>
#include <QSGFlatColorMaterial>
#include <QSGGeometryNode>

namespace {
Q_LOGGING_CATEGORY(self, "qperformance.sgchart", QtWarningMsg)
}

SceneGraphChart::SceneGraphChart()
{
    setFlag(ItemHasContents);
    _elapsedTimer.start();
    _updateTimer.setTimerType(Qt::PreciseTimer);
    _updateTimer.setSingleShot(true);
    _updateTimer.setInterval(250);
    _updateTimer.callOnTimeout(this, [this]() {
        updateRange();
        update();
    });
}

SceneGraphChart::~SceneGraphChart()
{
    if (_monitor)
        _monitor->unsubscribe(this);
}

#define SETTER(Type, name, Name, extra) \
    void SceneGraphChart::set##Name(Type v) \
    { \
        if (_##name == v) \
            return; \
        _##name = v; \
        extra; \
        emit name##Changed(); \
    }

SETTER(double, duration, Duration, requestUpdate())
SETTER(const QList<QColor> &, colors, Colors, requestUpdate())
SETTER(const QList<double> &, lineWidths, LineWidths, requestUpdate())
SETTER(double, min, Min, requestUpdate())
SETTER(double, max, Max, requestUpdate())

void SceneGraphChart::setMaxUpdateInterval(int v)
{
    if (_updateTimer.interval() == v)
        return;
    _updateTimer.setInterval(v);
    emit maxUpdateIntervalChanged();
}

void SceneGraphChart::setMonitor(Monitor *m)
{
    if (_monitor == m)
        return;

    if (_monitor) {
        disconnect(_monitor, &Monitor::valuesAvailable, this, &SceneGraphChart::pushValues);
        _monitor->unsubscribe(this);
    }
    _monitor = m;
    emit monitorChanged();
    if (_monitor) {
        connect(_monitor, &Monitor::valuesAvailable, this, &SceneGraphChart::pushValues);
        _monitor->subscribe(this);
    }
}

void SceneGraphChart::requestUpdate()
{
    if (!_updateTimer.isActive())
        _updateTimer.start();
}

void SceneGraphChart::pushValues(const QList<double> &values)
{
    pushValues2(values, _elapsedTimer.elapsed());
}

void SceneGraphChart::pushValues2(const QList<double> &values, qint64 t)
{
    if (!_values.empty() && _values.back().size() != values.size()) {
        qCWarning(self) << "value size mismatch! resetting...";
        reset();
        return;
    }

    _values.push_back(values);
    _ts.push_back(t);
    while (_ts.front() < t - _duration) {
        _values.pop_front();
        _ts.pop_front();
    }
    requestUpdate();
}

void SceneGraphChart::reset()
{
    _values.clear();
    _ts.clear();
    requestUpdate();
}

void SceneGraphChart::updateRange()
{
    double lo = std::numeric_limits<double>::infinity();
    double hi = -lo;
    for (const auto &row : _values)
        for (const double v : row) {
            lo = std::min(lo, v);
            hi = std::max(hi, v);
        }

    if (lo != _minValue) {
        _minValue = lo;
        emit minValueChanged();
    }
    if (hi != _maxValue) {
        _maxValue = hi;
        emit maxValueChanged();
    }
}

QSGNode *SceneGraphChart::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *)
{
    auto *root = oldNode ? oldNode : new QSGNode;

    // Rebuild children; geometry changes completely whenever time advances.
    while (root->firstChild())
        delete root->firstChild();

    if (_ts.size() < 2 || _max == _min || width() <= 0 || height() <= 0)
        return root;

    const int n = _values.front().size();
    const size_t count = _ts.size();
    const double xf = width() / _duration;
    const double yf = height() / (_max - _min);
    const qint64 now = _ts.back();

    QList<QPointF> pts(count);
    for (int j = 0; j < n; j++) {
        const QColor c = _colors.empty() ? QColor(Qt::black) : _colors[j % _colors.size()];
        const double w = _lineWidths.empty() ? 1. : _lineWidths[j % _lineWidths.size()];
        if (c.alpha() == 0 || w <= 0)
            continue;

        for (size_t i = 0; i < count; i++)
            pts[i] = {width() - xf * (now - _ts[i]), height() - (_values[i][j] - _min) * yf};

        auto *geometry = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), int(count) * 2);
        geometry->setDrawingMode(QSGGeometry::DrawTriangleStrip);
        auto *vs = geometry->vertexDataAsPoint2D();
        for (size_t i = 0; i < count; i++) {
            const QPointF d = pts[std::min(i + 1, count - 1)] - pts[i ? i - 1 : 0];
            const double len = std::hypot(d.x(), d.y());
            const QPointF off = len > 0 ? QPointF(-d.y(), d.x()) * (w / 2 / len) : QPointF(0, w / 2);
            vs[2 * i].set(pts[i].x() + off.x(), pts[i].y() + off.y());
            vs[2 * i + 1].set(pts[i].x() - off.x(), pts[i].y() - off.y());
        }

        auto *material = new QSGFlatColorMaterial;
        material->setColor(c);

        auto *node = new QSGGeometryNode;
        node->setGeometry(geometry);
        node->setFlag(QSGNode::OwnsGeometry);
        node->setMaterial(material);
        node->setFlag(QSGNode::OwnsMaterial);
        root->appendChildNode(node);
    }
    return root;
}
