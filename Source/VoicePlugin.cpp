#include "IllustratorSDK.h"
#include "VoicePlugin.h"
#include "VoiceSuites.h"

static MCI_PLAY_PARMS play;

Plugin* AllocatePlugin(SPPluginRef pluginRef)
{
	return new VoicePlugin(pluginRef);
}

void FixupReload(Plugin* plugin)
{
	VoicePlugin::FixupVTable((VoicePlugin*)plugin);
}

VoicePlugin::VoicePlugin(SPPluginRef pluginRef) :
	Plugin(pluginRef),
	fShutdownApplicationNotifier(NULL),
	fDocumentNewNotifier(NULL)
{
	strncpy(fPluginName, kVoicePluginName, kMaxStringLength);
}

VoicePlugin::~VoicePlugin()
{
}

ASErr VoicePlugin::StartupPlugin(SPInterfaceMessage *message)
{
	ASErr result = kNoErr;
	try
	{
		result = Plugin::StartupPlugin(message);
		aisdk::check_ai_error(result);

		// Add notifier.
		result = this->AddNotifier(message);
		aisdk::check_ai_error(result);

	}
	catch (ai::Error& ex) {
		result = ex;
	}
	catch (...)
	{
		result = kCantHappenErr;
	}
	return result;
}

ASErr VoicePlugin::ShutdownPlugin(SPInterfaceMessage *message)
{
	ASErr result = kNoErr;
	try {
		result = Plugin::ShutdownPlugin(message);
		aisdk::check_ai_error(result);
	}
	catch (ai::Error& ex) {
		result = ex;
	}
	catch (...) {
		result = kCantHappenErr;
	}
	return result;
}


/*
*/
ASErr VoicePlugin::Notify(AINotifierMessage* message)
{
	ASErr error = kNoErr;

	try
	{
		if (message->notifier == fShutdownApplicationNotifier)
		{
			// close
			for (auto itr = devices.begin(); itr != devices.end(); ++itr) {
				mciSendCommand(itr->second.wDeviceID, MCI_CLOSE, 0, 0);
			}
		}
		if (message->notifier == fDocumentNewNotifier)
		{
			auto itr = devices.find(fDocumentNewNotifier);
			bool failed = false;

			MCI_OPEN_PARMS open;
			if (itr == devices.end()) {
				// create new

				open.lpstrDeviceType = "MPEGVideo";
				open.lpstrElementName = getenv("AI_VOICE_PLUGIN_START");

				int result = mciSendCommand(0, MCI_OPEN, MCI_OPEN_TYPE | MCI_OPEN_ELEMENT, (DWORD_PTR)&open);
				if (result) {
					// failed
					failed = true;
					char buf[1000];
					mciGetErrorString(result, buf, sizeof(buf));
					OutputDebugString(buf);
				}
				else {
					// add to map
					devices[fDocumentNewNotifier] = open;
				}
			}
			else {
				open = itr->second;
			}

			if (!failed) {
				MCI_STATUS_PARMS status;
				status.dwItem = MCI_STATUS_MODE;

				int result = mciSendCommand(open.wDeviceID, MCI_STATUS, MCI_STATUS_ITEM, (DWORD_PTR)&status);
				if (result) {
					// failed
					char buf[1000];
					mciGetErrorString(result, buf, sizeof(buf));
					OutputDebugString(buf);
				}
				if (status.dwReturn == MCI_MODE_PLAY) {
					// stutasu = plyaing
					// do nothing
				}
				else {
					// play
					mciSendCommand(open.wDeviceID, MCI_STOP, 0, 0);
					mciSendCommand(open.wDeviceID, MCI_SEEK, MCI_SEEK_TO_START, 0);
					mciSendCommand(open.wDeviceID, MCI_PLAY, MCI_NOTIFY, (DWORD_PTR)&play);
				}
			}
		}
	}
	catch (ai::Error& ex) {
		error = ex;
	}
	catch (...)
	{
		error = kCantHappenErr;
	}

	return error;
}

/*
*/
ASErr VoicePlugin::AddNotifier(SPInterfaceMessage *message)
{
	ASErr result = kNoErr;
	try {
		result = sAINotifier->AddNotifier(fPluginRef, "VoicePlugin", kAIApplicationShutdownNotifier, &fShutdownApplicationNotifier);
		aisdk::check_ai_error(result);
		result = sAINotifier->AddNotifier(fPluginRef, "VoicePlugin", kAIDocumentNewNotifier, &fDocumentNewNotifier);
		aisdk::check_ai_error(result);

	}
	catch (ai::Error& ex) {
		result = ex;
	}
	catch (...)
	{
		result = kCantHappenErr;
	}
	return result;
}
