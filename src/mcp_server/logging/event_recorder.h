// Copyright (c) 2024 qtPilot Contributors
// SPDX-License-Identifier: MIT

#pragma once

#include <QJsonArray>
#include <QJsonObject>

namespace qtPilot::logging {

class EventRecorder {
 public:
  QJsonObject start();
  QJsonObject stop();
  QJsonObject status() const;

  void recordNotification(const QString& method, const QJsonObject& params);

 private:
  bool recording_ = false;
  qint64 startedAtMs_ = 0;
  QJsonArray events_;
};

}  // namespace qtPilot::logging
