#include "gyroscope.h"
#include <QDebug>

Gyroscope::Gyroscope(QObject *parent)
    : QObject(parent), alpha(0.0), alphaKalman(0.1, 1, 0.1, 0.0)
{
    sensor = new QGyroscope(this);
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Gyroscope::onSensorReadingChanged);
}

Gyroscope::~Gyroscope()
{
    stop();
    delete sensor;
}

void Gyroscope::start()
{
    if (!sensor->isActive())
    {
        sensor->start();
        timer->start(5);
        emit activeChanged();
        qDebug() << "Gyroscope started.";
    }
}

void Gyroscope::stop()
{
    if (sensor->isActive())
    {
        sensor->stop();
        timer->stop();
        emit activeChanged();
        qDebug() << "Gyroscope stopped.";
    }
}

void Gyroscope::reset()
{
    alpha = 0.0;
    emit gyroscopeReset();
    qDebug() << "Gyroscope reset.";
}

void Gyroscope::onSensorReadingChanged()
{
    QGyroscopeReading *reading = sensor->reading();
    if (reading)
    {
        double z = reading->z(); // Rotation around the Z axis

        // Apply Kalman filter
        z = alphaKalman.update(z);

        alpha += z * 0.005; // 0.005 is the sample interval in seconds

        QString output = QStringLiteral("Rotation (alpha): %1").arg(QString::number(alpha, 'f', 2));
        emit readingUpdated(output);
        emit newRotation(alpha);
        qDebug() << output;
    }
    else
    {
        qDebug() << "No reading available.";
    }
}
