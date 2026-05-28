// Copyright (c) 2024 qtPilot Contributors
// SPDX-License-Identifier: MIT

#include <QCommandLineParser>
#include <QCoreApplication>

#include "download/downloader.h"
#include "mcp/mcp_server.h"
#include "state/server_state.h"
#include "tools/chrome_tools.h"
#include "tools/cu_tools.h"
#include "tools/discovery_tools.h"
#include "tools/logging_tools.h"
#include "tools/native_tools.h"
#include "tools/recording_tools.h"

int main(int argc, char* argv[]) {
  QCoreApplication app(argc, argv);
  app.setApplicationName(QStringLiteral("qtpilot-server"));
  app.setApplicationVersion(QStringLiteral("0.1.0"));

  QCommandLineParser parser;
  parser.addHelpOption();
  parser.addVersionOption();

  parser.addPositionalArgument(QStringLiteral("command"),
                               QStringLiteral("Command: serve, demo, download-tools"));

  QCommandLineOption modeOption(QStringLiteral("mode"),
                                QStringLiteral("Mode: native|cu|chrome|all"),
                                QStringLiteral("mode"), QStringLiteral("native"));
  QCommandLineOption wsUrlOption(QStringLiteral("ws-url"),
                                 QStringLiteral("Probe websocket URL"),
                                 QStringLiteral("url"));
  QCommandLineOption qtVersionOption(QStringLiteral("qt-version"),
                                     QStringLiteral("Qt version"),
                                     QStringLiteral("version"));
  QCommandLineOption outputOption(QStringList() << QStringLiteral("o") << QStringLiteral("output"),
                                  QStringLiteral("Output directory"),
                                  QStringLiteral("dir"));

  parser.addOption(modeOption);
  parser.addOption(wsUrlOption);
  parser.addOption(qtVersionOption);
  parser.addOption(outputOption);
  parser.process(app);

  const QStringList positional = parser.positionalArguments();
  const QString command = positional.isEmpty() ? QStringLiteral("serve") : positional.first();

  if (command == QStringLiteral("download-tools")) {
    qtPilot::download::Downloader downloader;
    return downloader.downloadTools(parser.value(qtVersionOption), parser.value(outputOption)) ? 0 : 1;
  }

  qtPilot::state::ServerState state;
  state.setMode(parser.value(modeOption));

  if (!parser.value(wsUrlOption).isEmpty()) {
    state.connectProbe(parser.value(wsUrlOption));
  }

  qtPilot::tools::registerDiscoveryTools(state);
  qtPilot::tools::registerLoggingTools(state);
  qtPilot::tools::registerRecordingTools(state);
  qtPilot::tools::registerNativeTools(state);
  qtPilot::tools::registerCuTools(state);
  qtPilot::tools::registerChromeTools(state);

  qtPilot::mcp::McpServer server(state);
  return server.exec();
}
