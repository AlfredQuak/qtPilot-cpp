// Copyright (c) 2024 qtPilot Contributors
// SPDX-License-Identifier: MIT

#include "client/discovery_listener.h"

#include <QJsonDocument>
#include <QNetworkDatagram>

namespace qtPilot::client {

DiscoveryListener::DiscoveryListener(QObject* parent) : QObject(parent) {
  connect(&socket_, &QUdpSocket::readyRead, this, &DiscoveryListener::onReadyRead);
}

bool DiscoveryListener::start(quint16 port) {
  running_ = socket_.bind(QHostAddress::AnyIPv4, port,
                          QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
  return running_;
}

void DiscoveryListener::stop() {
  socket_.close();
  running_ = false;
}

bool DiscoveryListener::isRunning() const { return running_; }

QJsonObject DiscoveryListener::probes() const { return probes_; }

void DiscoveryListener::onReadyRead() {
  while (socket_.hasPendingDatagrams()) {
    QNetworkDatagram datagram = socket_.receiveDatagram();
    const QJsonDocument doc = QJsonDocument::fromJson(datagram.data());
    if (!doc.isObject()) {
      continue;
    }
    const QJsonObject msg = doc.object();
    if (msg.value("protocol").toString() != QStringLiteral("qtPilot-discovery")) {
      continue;
    }

    const QString key = QStringLiteral("%1:%2:%3")
                            .arg(datagram.senderAddress().toString())
                            .arg(msg.value("pid").toInt())
                            .arg(msg.value("wsPort").toInt(9222));

    if (msg.value("type").toString() == QStringLiteral("goodbye")) {
      probes_.remove(key);
    } else {
      QJsonObject entry = msg;
      entry.insert("address", datagram.senderAddress().toString());
      probes_.insert(key, entry);
    }
  }
}

}  // namespace qtPilot::client
