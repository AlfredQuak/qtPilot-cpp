// Copyright (c) 2024 qtPilot Contributors
// SPDX-License-Identifier: MIT

#include "client/probe_client.h"

#include <stdexcept>

#include <QJsonDocument>
#include <QTimer>

namespace qtPilot::client {

ProbeClient::ProbeClient(QObject* parent) : QObject(parent) {
  connect(&socket_, &QWebSocket::textMessageReceived, this, &ProbeClient::onTextMessageReceived);
}

bool ProbeClient::connectToUrl(const QString& wsUrl, int timeoutMs) {
  wsUrl_ = wsUrl;
  QEventLoop loop;
  QTimer timer;
  bool connected = false;

  timer.setSingleShot(true);
  connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
  connect(&socket_, &QWebSocket::connected, &loop, [&]() {
    connected = true;
    loop.quit();
  });

  socket_.open(QUrl(wsUrl));
  timer.start(timeoutMs);
  loop.exec();

  return connected;
}

void ProbeClient::disconnectFromProbe() { socket_.close(); }

bool ProbeClient::isConnected() const { return socket_.state() == QAbstractSocket::ConnectedState; }

QString ProbeClient::wsUrl() const { return wsUrl_; }

QJsonObject ProbeClient::call(const QString& method, const QJsonObject& params) {
  if (!isConnected()) {
    throw std::runtime_error("Not connected to probe");
  }

  const qint64 id = nextId_++;
  pending_[id] = PendingCall{};

  const QJsonObject request{{"jsonrpc", "2.0"}, {"method", method}, {"params", params}, {"id", id}};
  socket_.sendTextMessage(QString::fromUtf8(QJsonDocument(request).toJson(QJsonDocument::Compact)));

  QEventLoop loop;
  QTimer timer;
  timer.setSingleShot(true);
  connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
  timer.start(5000);

  while (!pending_[id].completed && timer.isActive()) {
    QTimer::singleShot(10, &loop, &QEventLoop::quit);
    loop.exec();
  }

  const PendingCall call = pending_[id];
  pending_.erase(id);

  if (!call.error.isEmpty()) {
    throw std::runtime_error(call.error.value("message").toString().toStdString());
  }
  if (!call.completed) {
    throw std::runtime_error("Probe request timeout");
  }

  return call.result;
}

void ProbeClient::addNotificationHandler(
    const std::function<void(const QString&, const QJsonObject&)>& handler) {
  notificationHandlers_.append(handler);
}

void ProbeClient::onTextMessageReceived(const QString& message) {
  const QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8());
  if (!doc.isObject()) {
    return;
  }

  const QJsonObject msg = doc.object();
  if (msg.contains("id")) {
    const qint64 id = static_cast<qint64>(msg.value("id").toDouble());
    auto it = pending_.find(id);
    if (it == pending_.end()) {
      return;
    }
    it->second.completed = true;
    if (msg.contains("error")) {
      it->second.error = msg.value("error").toObject();
    } else {
      it->second.result = msg.value("result").toObject();
    }
    return;
  }

  const QString method = msg.value("method").toString();
  const QJsonObject params = msg.value("params").toObject();
  for (const auto& handler : notificationHandlers_) {
    handler(method, params);
  }
}

}  // namespace qtPilot::client
