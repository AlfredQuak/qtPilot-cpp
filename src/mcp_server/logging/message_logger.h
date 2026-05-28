// Copyright (c) 2024 qtPilot Contributors
// SPDX-License-Identifier: MIT

#pragma once

#include <deque>

#include <QFile>
#include <QJsonObject>

namespace qtPilot::logging {

class MessageLogger {
 public:
  QJsonObject start(const QString& path = QString(), int level = 2, int bufferSize = 200);
  QJsonObject stop();
  QJsonObject status() const;
  QJsonObject tail(int count = 50) const;

  void log(const QJsonObject& entry);
  bool isActive() const;

 private:
  QString defaultPath() const;

  bool active_ = false;
  QFile file_;
  QString path_;
  int level_ = 2;
  int entryCount_ = 0;
  std::deque<QJsonObject> buffer_;
  int bufferSize_ = 200;
};

}  // namespace qtPilot::logging
