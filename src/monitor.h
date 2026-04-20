#ifndef MONITOR_H
#define MONITOR_H

#include <QObject>

class Monitor : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool active READ active NOTIFY activeChanged FINAL)

public:
    explicit Monitor(QObject *parent = nullptr);

    bool active() const;

public slots:
    void unsubscribe(QObject *subscriber = nullptr);
    void subscribe(QObject *subscriber = nullptr);

signals:
    void valuesAvailable(const QList<double> values);
    void intervalChanged();
    void activeChanged();

protected:
    virtual void deactivate() = 0;
    virtual void activate() = 0;
    void updateActive();

private:
    bool _active;
    QList<QObject *> _subscribers;
};

#endif // MONITOR_H
