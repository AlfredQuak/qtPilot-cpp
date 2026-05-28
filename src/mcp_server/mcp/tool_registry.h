// Copyright (c) 2024 qtPilot Contributors
// SPDX-License-Identifier: MIT

#pragma once

#include <functional>
#include <QJsonObject>
#include <QHash>
#include <QString>

namespace qtPilot::mcp {

using ToolHandler = std::function<QJsonObject(const QJsonObject& arguments)>;

struct ToolDefinition {
  QString name;
  QString description;
  QJsonObject inputSchema;
  ToolHandler handler;
};

class ToolRegistry {
 public:
  void registerTool(const ToolDefinition& tool);
  void unregisterTool(const QString& name);
  bool hasTool(const QString& name) const;

  QJsonObject callTool(const QString& name, const QJsonObject& arguments) const;
  QJsonObject listTools() const;

 private:
  QHash<QString, ToolDefinition> tools_;
};

}  // namespace qtPilot::mcp
