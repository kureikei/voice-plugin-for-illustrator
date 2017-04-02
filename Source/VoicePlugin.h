#ifndef __VoicePlugin_h__
#define __VoicePlugin_h__

#include "Plugin.hpp"
#include "SDKErrors.h"
#include "VoiceID.h"

#include <map>
#include <cstdlib>
#include <string>
#include <stdexcept>
#pragma comment(lib,"winmm")


/**	Creates a new VoicePlugin.
	@param pluginRef IN unique reference to this plugin.
	@return pointer to new VoicePlugin.
*/
Plugin* AllocatePlugin(SPPluginRef pluginRef);

/**	Reloads the VoicePlugin class state when the plugin is 
	reloaded by the application.
	@param plugin IN pointer to plugin being reloaded.
*/
void FixupReload(Plugin* plugin);

/**	Hooks Voice up as an Illustrator plug-in.
	@ingroup Voice
*/
class VoicePlugin : public Plugin
{
private:
	AINotifierHandle	fShutdownApplicationNotifier;
	AINotifierHandle	fDocumentNewNotifier;

	std::map<AINotifierHandle, MCI_OPEN_PARMS> devices;

public:
	/** Constructor.
		@param pluginRef IN reference to this plugin.
	*/
	VoicePlugin(SPPluginRef pluginRef);

	/** Destructor.
	*/
	virtual ~VoicePlugin();
  
  /**	Restores state of VoicePlugin during reload.
	*/
	FIXUP_VTABLE_EX(VoicePlugin, Plugin);
  
  /**	Initializes the plugin.
		@param message IN message sent by the plugin manager.
		@return kNoErr on success, other ASErr otherwise.
	*/
	ASErr StartupPlugin( SPInterfaceMessage * message );

	/**	Removes the plugin.
		@param message IN message sent by the plugin manager.
		@return kNoErr on success, other ASErr otherwise.
	*/
	ASErr ShutdownPlugin( SPInterfaceMessage * message );

protected:
	/**	Handles notifiers this plugin is registered for when they
	are received.
	@param message IN message data.
	@return kNoErr on success, other ASErr otherwise.
	*/
	virtual ASErr Notify(AINotifierMessage* message);

	///**	Registers this plug-in to receive the selection changed
	//	notifier.
	//	@param message IN message data.
	//	@return kNoErr on success, other ASErr otherwise.
	//*/
	ASErr AddNotifier(SPInterfaceMessage* message);

};

#endif
