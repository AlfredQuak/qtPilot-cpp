// Copyright (c) 2024 qtPilot Contributors
// SPDX-License-Identifier: MIT

#include "logging/message_logger.h"

#include <QDateTime>
#include <QJsonArray>
#include <QJsonDocument>

namespace qtPilot::logging {

QJsonObject MessageLogger::start(const QString& path, int level, int bufferSize) {
  if (active_) {
    stop();
  }

  path_ = path.isEmpty() ? defaultPath() : path;
  level_ = level;
  bufferSize_ = bufferSize;
  buffer_.clear();
  entryCount_ = 0;

  file_.setFileName(path_);
  if (!file_.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
    return QJsonObject{{"logging", false}, {"error", QStringLiteral("Cannot open log file")}};
  }

  active_ = true;
  return QJsonObject{{"logging", true}, {"path", path_}, {"level", level_}, {"buffer_size", bufferSize_}};
}

QJsonObject MessageLogger::stop() {
  if (!active_) {
    return QJsonObject{{"logging", false}, {"entries", 0}};
  }

  file_.flush();
  file_.close();
  active_ = false;

  return QJsonObject{{"logging", false}, {"path", path_}, {"entries", entryCount_}};
}

QJsonObject MessageLogger::status() const {
  return QJsonObject{{"logging", active_}, {"path", path_}, {"level", level_}, {"entries", entryCount_}};
}

QJsonObject MessageLogger::tail(int count) const {
  QJsonArray entries;
  const int start = std::max(0, static_cast<int>(buffer_.size()) - count);
  for (int i = start; i < static_cast<int>(buffer_.size()); ++i) {
    entries.append(buffer_[i]);
  }
  return QJsonObject{{"entries", entries}, {"count", entries.size()}, {"total_logged", entryCount_}};
}

void MessageLogger::log(const QJsonObject& entry) {
  if (!active_) {
    return;
  }

  QJsonObject line = entry;
  line.insert("ts", QDateTime::currentDateTimeUtc().toString(Qt::ISODateWithMs));

  file_.write(QJsonDocument(line).toJson(QJsonDocument::Compact));
  file_.write("\n");
  file_.flush();

  buffer_.push_back(line);
  while (static_cast<int>(buffer_.size()) > bufferSize_) {
    buffer_.pop_front();
  }

  ++entryCount_;
}

bool MessageLogger::isActive() const { return active_; }

QString MessageLogger::defaultPath() const {
  return QStringLiteral("qtPilot-log-%1.jsonl")
      .arg(QDateTime::currentDateTimeUtc().toString(QStringLiteral("yyyyMMdd-hhmmss")));
}

}  // namespace qtPilot::logging
