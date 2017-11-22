#pragma once

// hyperion includes
#include <utils/Logger.h>
#include <utils/jsonschema/QJsonSchemaChecker.h>
#include <utils/Components.h>
#include <hyperion/Hyperion.h>

// qt includess
#include <QTimer>
#include <QJsonObject>
#include <QMutex>
#include <QString>

// plugin
#include <plugin/PluginDefinition.h>

// createEffect helper
struct find_schema: std::unary_function<EffectSchema, bool>
{
	QString pyFile;
	find_schema(QString pyFile):pyFile(pyFile) { }
	bool operator()(EffectSchema const& schema) const
	{
		return schema.pyFile == pyFile;
	}
};

// deleteEffect helper
struct find_effect: std::unary_function<EffectDefinition, bool>
{
	QString effectName;
	find_effect(QString effectName) :effectName(effectName) { }
	bool operator()(EffectDefinition const& effectDefinition) const
	{
		return effectDefinition.name == effectName;
	}
};

class ImageProcessor;

class JsonAPI : public QObject
{
	Q_OBJECT

public:
	///
	/// Constructor
	///
	/// @param peerAddress provide the Address of the peer
	/// @param log         The Logger class of the creator
	/// @param parent      Parent QObject
	/// @param noListener  if true, this instance won't listen for hyperion push events
	///
	JsonAPI(QString peerAddress, Logger* log, QObject* parent, bool noListener = false);

	///
	/// Handle an incoming JSON message
	///
	/// @param message the incoming message as string
	///
	void handleMessage(const QString & message);

	///
	/// send a forced serverinfo to a client
	///
	void forceServerInfo();

public slots:
	/// _timer_ledcolors requests ledcolor updates (if enabled)
	void streamLedcolorsUpdate();

	/// push images whenever hyperion emits (if enabled)
	void setImage(int priority, const Image<ColorRgb> & image, int duration_ms);

	/// process and push new log messages from logger (if enabled)
	void incommingLogMessage(Logger::T_LOG_MESSAGE);

private slots:
	/// process plugin actions from Plugins
	void doPluginAction(PluginAction action, QString id, bool success, PluginDefinition def);

signals:
	///
	/// Signal which is emitted when a sendSuccessReply() has been executed
	///
	void pushReq();
	///
	/// Signal emits with the reply message provided with handleMessage()
	///
	void callbackMessage(QJsonObject);

	///
	/// Signal emits whenever a jsonmessage should be forwarded
	///
	void forwardJsonMessage(QJsonObject);

	///
	/// Signal emits whenever a plugin action is ongoing
	///
	void pluginAction(PluginAction action, QString id, bool success = true, PluginDefinition def = PluginDefinition());

private:
    /// The peer address of the client
    QString _peerAddress;

	/// Log instance
	Logger* _log;

	/// Hyperion instance
	Hyperion* _hyperion;

	/// The processor for translating images to led-values
	ImageProcessor * _imageProcessor;

    /// holds the state before off state
    static std::map<hyperion::Components, bool> _componentsPrevState;

	/// returns if hyperion is on or off
	inline bool hyperionIsActive() { return JsonAPI::_componentsPrevState.empty(); };

	/// timer for ledcolors streaming
	QTimer _timer_ledcolors;

	// streaming buffers
	QJsonObject _streaming_leds_reply;
	QJsonObject _streaming_image_reply;
	QJsonObject _streaming_logging_reply;

	/// flag to determine state of log streaming
	bool _streaming_logging_activated;

	/// mutex to determine state of image streaming
	QMutex _image_stream_mutex;

	/// timeout for live video refresh
	volatile qint64 _image_stream_timeout;

	/// Plugins instance
	Plugins* _plugins;

	/// if true the management for plugins is enabled
	bool _managePlugins = false;

	///
	/// Handle an incoming JSON Color message
	///
	/// @param message the incoming message
	///
	void handleColorCommand(const QJsonObject & message, const QString &command, const int tan);

	///
	/// Handle an incoming JSON Image message
	///
	/// @param message the incoming message
	///
	void handleImageCommand(const QJsonObject & message, const QString &command, const int tan);

	///
	/// Handle an incoming JSON Effect message
	///
	/// @param message the incoming message
	///
	void handleEffectCommand(const QJsonObject & message, const QString &command, const int tan);

	///
	/// Handle an incoming JSON Effect message (Write JSON Effect)
	///
	/// @param message the incoming message
	///
	void handleCreateEffectCommand(const QJsonObject & message, const QString &command, const int tan);

	///
	/// Handle an incoming JSON Effect message (Delete JSON Effect)
	///
	/// @param message the incoming message
	///
	void handleDeleteEffectCommand(const QJsonObject & message, const QString &command, const int tan);

	///
	/// Handle an incoming JSON System info message
	///
	/// @param message the incoming message
	///
	void handleSysInfoCommand(const QJsonObject & message, const QString &command, const int tan);

	///
	/// Handle an incoming JSON Server info message
	///
	/// @param message the incoming message
	///
	void handleServerInfoCommand(const QJsonObject & message, const QString &command, const int tan);

	///
	/// Handle an incoming JSON Clear message
	///
	/// @param message the incoming message
	///
	void handleClearCommand(const QJsonObject & message, const QString &command, const int tan);

	///
	/// Handle an incoming JSON Adjustment message
	///
	/// @param message the incoming message
	///
	void handleAdjustmentCommand(const QJsonObject & message, const QString &command, const int tan);

	///
	/// Handle an incoming JSON SourceSelect message
	///
	/// @param message the incoming message
	///
	void handleSourceSelectCommand(const QJsonObject & message, const QString &command, const int tan);

	/// Handle an incoming JSON GetConfig message and check subcommand
	///
	/// @param message the incoming message
	///
	void handleConfigCommand(const QJsonObject & message, const QString &command, const int tan);

	/// Handle an incoming JSON GetConfig message from handleConfigCommand()
	///
	/// @param message the incoming message
	///
	void handleSchemaGetCommand(const QJsonObject & message, const QString &command, const int tan);

	/// Handle an incoming JSON SetConfig message from handleConfigCommand()
	///
	/// @param message the incoming message
	///
	void handleConfigSetCommand(const QJsonObject & message, const QString &command, const int tan);

	///
	/// Handle an incoming JSON Component State message
	///
	/// @param message the incoming message
	///
	void handleComponentStateCommand(const QJsonObject & message, const QString &command, const int tan);

	/// Handle an incoming JSON Led Colors message
	///
	/// @param message the incoming message
	///
	void handleLedColorsCommand(const QJsonObject & message, const QString &command, const int tan);

	/// Handle an incoming JSON Logging message
	///
	/// @param message the incoming message
	///
	void handleLoggingCommand(const QJsonObject & message, const QString &command, const int tan);

	/// Handle an incoming JSON Proccessing message
	///
	/// @param message the incoming message
	///
	void handleProcessingCommand(const QJsonObject & message, const QString &command, const int tan);

	/// Handle an incoming JSON VideoMode message
	///
	/// @param message the incoming message
	///
	void handleVideoModeCommand(const QJsonObject & message, const QString &command, const int tan);

	/// Handle an incoming JSON Management message
	///
	/// @param message the incoming message
	///
	void handleManagementCommand(const QJsonObject & message, const QString &command, const int tan);

	/// Handle an incoming JSON plugin message
	///
	/// @param message the incoming message
	///
	void handlePluginCommand(const QJsonObject & message, const QString &command, const int tan);

	///
	/// Handle an incoming JSON message of unknown type
	///
	void handleNotImplemented();

	///
	/// Send a standard reply indicating success
	///
	void sendSuccessReply(const QString &command="", const int tan=0);

	///
	/// Send a standard reply indicating success with data
	///
	void sendSuccessDataReply(const QJsonObject &data, const QString &command="", const int &tan=0);

	///
	/// Send an error message back to the client
	///
	/// @param error String describing the error
	///
	void sendErrorReply(const QString & error, const QString &command="", const int tan=0);
};