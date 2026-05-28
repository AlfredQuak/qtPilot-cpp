// Copyright (c) 2024 qtPilot Contributors
// SPDX-License-Identifier: MIT

#include "logging/event_recorder.h"

#include <QDateTime>

namespace qtPilot::logging {

QJsonObject EventRecorder::start() {
  recording_ = true;
  startedAtMs_ = QDateTime::currentMSecsSinceEpoch();
  events_ = QJsonArray();
  return QJsonObject{{"recording", true}};
}

QJsonObject EventRecorder::stop() {
  const qint64 now = QDateTime::currentMSecsSinceEpoch();
  const double duration = recording_ ? static_cast<double>(now - startedAtMs_) / 1000.0 : 0.0;

  QJsonObject result{{"recording", false}, {"duration", duration}, {"event_count", events_.size()}, {"events", events_}};
  recording_ = false;
  events_ = QJsonArray();
  return result;
}

QJsonObject EventRecorder::status() const {
  return QJsonObject{{"recording", recording_}, {"event_count", events_.size()}};
}

void EventRecorder::recordNotification(const QString& method, const QJsonObject& params) {
  if (!recording_) {
    return;
  }
  events_.append(QJsonObject{{"t", static_cast<double>(QDateTime::currentMSecsSinceEpoch() - startedAtMs_) / 1000.0},
                             {"method", method},
                             {"params", params}});
}

}  // namespace qtPilot::logging
