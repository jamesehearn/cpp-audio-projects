/*-----------------------------------------------------------------------------
© 2000 Will Pirkle All Rights Reserved
Portions © 1999, Steinberg Soft und Hardware GmbH, All Rights Reserved
				 Used under License Agreement
-------------------------------------------------------------------------------*/

#include "DDLModule.h"

extern "C"
{
	/* entry point DllMain
	BOOL APIENTRY DllMain(HANDLE hModule,
						  DWORD dwReason,
						  void* lpReserved)
	{
		if (dwReason == DLL_PROCESS_ATTACH)
		{
			g_hModule = (HMODULE)hModule ;
		}
		return TRUE ;
	}
	end entry point DllMain */

// VST Support
// VST Support - DO NOT EDIT THIS CODE
	// extern AudioEffect* createEffectInstance (audioMasterCallback audioMaster);

	#define VST_EXPORT

	// VST Export Function
	AEffect* VSTPluginMain (audioMasterCallback audioMaster)
	{
		// Get VST Version of the Host
		if (!audioMaster (0, audioMasterVersion, 0, 0, 0, 0))
			return 0;  // old version

		// Create the AudioEffect
		AudioEffect* effect = new CDDLModule(audioMaster);
		if (!effect)
			return 0;

		// Return the VST AEffect structur
		return effect->getAeffect ();
	}
// end VSTPlugInMain 0x7FFD
// END VST Support

}

#include <windows.h>
void* hInstance;
BOOL WINAPI DllMain (HINSTANCE hInst, DWORD dwReason, LPVOID lpvReserved)
{
	hInstance = hInst;
	return 1;
}

