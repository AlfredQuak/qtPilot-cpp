// Copyright (c) 2024 qtPilot Contributors
// SPDX-License-Identifier: MIT

#pragma once

#include <QJsonObject>
#include <QUdpSocket>

namespace qtPilot::client {

class DiscoveryListener : public QObject {
  Q_OBJECT

 public:
  explicit DiscoveryListener(QObject* parent = nullptr);

  bool start(quint16 port = 9221);
  void stop();
  bool isRunning() const;

  QJsonObject probes() const;

 private slots:
  void onReadyRead();

 private:
  QUdpSocket socket_;
  QJsonObject probes_;
  bool running_ = false;
};

}  // namespace qtPilot::client
