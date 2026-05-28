// Copyright (c) 2024 qtPilot Contributors
// SPDX-License-Identifier: MIT

#pragma once

#include <QJsonObject>
#include <QString>

#include "mcp/tool_registry.h"

namespace qtPilot::state {
class ServerState;
}

namespace qtPilot::mcp {

class McpServer {
 public:
  explicit McpServer(state::ServerState& state);

  int exec();

 private:
  QJsonObject handleRequest(const QJsonObject& request) const;
  QJsonObject handleInitialize(const QJsonObject& request) const;
  QJsonObject handleToolsList(const QJsonObject& request) const;
  QJsonObject handleToolsCall(const QJsonObject& request) const;
  QJsonObject handleResourcesRead(const QJsonObject& request) const;

  static QByteArray readOneMessage();
  static void writeOneMessage(const QJsonObject& response);
  static QJsonObject makeResult(const QJsonValue& id, const QJsonObject& result);
  static QJsonObject makeError(const QJsonValue& id, int code, const QString& message);

  state::ServerState& state_;
};

}  // namespace qtPilot::mcp
