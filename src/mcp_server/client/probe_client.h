// Copyright (c) 2024 qtPilot Contributors
// SPDX-License-Identifier: MIT

#pragma once

#include <functional>

#include <QEventLoop>
#include <QHash>
#include <QJsonObject>
#include <QWebSocket>

namespace qtPilot::client {

class ProbeClient : public QObject {
  Q_OBJECT

 public:
  explicit ProbeClient(QObject* parent = nullptr);

  bool connectToUrl(const QString& wsUrl, int timeoutMs = 3000);
  void disconnectFromProbe();
  bool isConnected() const;
  QString wsUrl() const;

  QJsonObject call(const QString& method, const QJsonObject& params = QJsonObject());

  void addNotificationHandler(const std::function<void(const QString&, const QJsonObject&)>& handler);

 private slots:
  void onTextMessageReceived(const QString& message);

 private:
  struct PendingCall {
    bool completed = false;
    QJsonObject result;
    QJsonObject error;
  };

  qint64 nextId_ = 1;
  QString wsUrl_;
  QWebSocket socket_;
  QHash<qint64, PendingCall> pending_;
  QList<std::function<void(const QString&, const QJsonObject&)>> notificationHandlers_;
};

}  // namespace qtPilot::client
