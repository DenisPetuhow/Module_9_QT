#include "signals.h"

Signals::Signals(QObject *parent)
    : QObject(parent)
{}

void Signals::SendSignal()
{
    emit sig_SendSimleSignal();
}

Signals::~Signals()
{
    qDebug() << "END";
}
