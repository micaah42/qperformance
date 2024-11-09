#ifndef MONITOR_H
#define MONITOR_H

#include <QObject>
#include <QTimer>

class Monitor : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int interval READ interval WRITE setInterval NOTIFY intervalChanged FINAL)
    Q_PROPERTY(int subscribers READ subscribers NOTIFY subscribersChanged FINAL)
    Q_PROPERTY(bool active READ active NOTIFY activeChanged FINAL)

public:
    explicit Monitor(QObject *parent = nullptr);

    void setInterval(int newInterval);
    int interval() const;

    int subscribers() const;
    bool active() const;

public slots:
    void unsubscribe(QObject *subscriber = nullptr);
    void subscribe(QObject *subscriber = nullptr);
    virtual void onTimeout() = 0;

protected:
    void setSubscribers(int newSubscribers);
    void setActive(bool newactive);

signals:
    void subscribersChanged();
    void intervalChanged();
    void activeChanged();

private:
    QTimer _timer;
    int m_subscribers;
};

#endif // MONITOR_H
