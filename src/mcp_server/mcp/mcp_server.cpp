// Copyright (c) 2024 qtPilot Contributors
// SPDX-License-Identifier: MIT

#include "mcp/mcp_server.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

#include <QJsonArray>
#include <QJsonDocument>

#include "state/server_state.h"

namespace qtPilot::mcp {

namespace {
constexpr int kParseError = -32700;
constexpr int kMethodNotFound = -32601;
constexpr int kInvalidParams = -32602;
constexpr int kInternalError = -32603;
}  // namespace

McpServer::McpServer(state::ServerState& state) : state_(state) {}

int McpServer::exec() {
  while (true) {
    const QByteArray raw = readOneMessage();
    if (raw.isEmpty()) {
      return 0;
    }

    QJsonParseError parseError;
    const QJsonDocument doc = QJsonDocument::fromJson(raw, &parseError);
    if (parseError.error != QJsonParseError::NoError || !doc.isObject()) {
      writeOneMessage(makeError(QJsonValue(), kParseError, QStringLiteral("Invalid JSON")));
      continue;
    }

    const QJsonObject response = handleRequest(doc.object());
    if (!response.isEmpty()) {
      writeOneMessage(response);
    }
  }
}

QJsonObject McpServer::handleRequest(const QJsonObject& request) const {
  const QJsonValue id = request.value(QStringLiteral("id"));
  const QString method = request.value(QStringLiteral("method")).toString();

  if (method == QStringLiteral("initialize")) {
    return handleInitialize(request);
  }
  if (method == QStringLiteral("tools/list")) {
    return handleToolsList(request);
  }
  if (method == QStringLiteral("tools/call")) {
    return handleToolsCall(request);
  }
  if (method == QStringLiteral("resources/read")) {
    return handleResourcesRead(request);
  }

  return makeError(id, kMethodNotFound, QStringLiteral("Unknown method"));
}

QJsonObject McpServer::handleInitialize(const QJsonObject& request) const {
  const QJsonValue id = request.value(QStringLiteral("id"));
  const QJsonObject capabilities{{"tools", QJsonObject()}, {"resources", QJsonObject()}};
  const QJsonObject result{{"protocolVersion", QStringLiteral("2025-03-26")},
                           {"serverInfo", QJsonObject{{"name", QStringLiteral("qtpilot-server")}, {"version", QStringLiteral("0.1.0")}}},
                           {"capabilities", capabilities}};
  return makeResult(id, result);
}

QJsonObject McpServer::handleToolsList(const QJsonObject& request) const {
  const QJsonValue id = request.value(QStringLiteral("id"));
  return makeResult(id, state_.toolRegistry().listTools());
}

QJsonObject McpServer::handleToolsCall(const QJsonObject& request) const {
  const QJsonValue id = request.value(QStringLiteral("id"));
  const QJsonObject params = request.value(QStringLiteral("params")).toObject();
  const QString name = params.value(QStringLiteral("name")).toString();
  const QJsonObject arguments = params.value(QStringLiteral("arguments")).toObject();

  if (name.isEmpty()) {
    return makeError(id, kInvalidParams, QStringLiteral("Missing tool name"));
  }

  const QJsonObject result = state_.toolRegistry().callTool(name, arguments);
  return makeResult(id, result);
}

QJsonObject McpServer::handleResourcesRead(const QJsonObject& request) const {
  const QJsonValue id = request.value(QStringLiteral("id"));
  const QJsonObject params = request.value(QStringLiteral("params")).toObject();
  const QString uri = params.value(QStringLiteral("uri")).toString();

  if (uri != QStringLiteral("qtpilot://status")) {
    return makeError(id, kInvalidParams, QStringLiteral("Unknown resource URI"));
  }

  return makeResult(id, QJsonObject{{"contents", QJsonArray{QJsonObject{{"uri", uri}, {"mimeType", QStringLiteral("application/json")}, {"text", QString::fromUtf8(QJsonDocument(state_.status()).toJson(QJsonDocument::Compact))}}}}});
}

QByteArray McpServer::readOneMessage() {
  std::string line;
  int contentLength = -1;

  while (std::getline(std::cin, line)) {
    if (line.empty() || line == "\r") {
      break;
    }
    constexpr const char* kHeader = "Content-Length:";
    if (line.rfind(kHeader, 0) == 0) {
      const std::string value = line.substr(std::strlen(kHeader));
      contentLength = std::atoi(value.c_str());
    }
  }

  if (!std::cin.good() && std::cin.eof()) {
    return QByteArray();
  }

  if (contentLength <= 0) {
    return QByteArray();
  }

  QByteArray payload(contentLength, '\0');
  std::cin.read(payload.data(), contentLength);
  return payload;
}

void McpServer::writeOneMessage(const QJsonObject& response) {
  const QByteArray payload = QJsonDocument(response).toJson(QJsonDocument::Compact);
  std::cout << "Content-Length: " << payload.size() << "\r\n\r\n";
  std::cout.write(payload.constData(), payload.size());
  std::cout.flush();
}

QJsonObject McpServer::makeResult(const QJsonValue& id, const QJsonObject& result) {
  return QJsonObject{{"jsonrpc", QStringLiteral("2.0")}, {"id", id}, {"result", result}};
}

QJsonObject McpServer::makeError(const QJsonValue& id, int code, const QString& message) {
  return QJsonObject{{"jsonrpc", QStringLiteral("2.0")},
                     {"id", id},
                     {"error", QJsonObject{{"code", code}, {"message", message}}}};
}

}  // namespace qtPilot::mcp
