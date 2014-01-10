/*	Copyright  2013 Tritone Systems, Inc. All Rights Reserved.
	
	Disclaimer: IMPORTANT:  This software is supplied to you by 
			Tritone Systems Inc. ("Tritone Systems") in consideration of your agreement to the
			following terms, and your use, installation, modification or
			redistribution of this Tritone Systems software constitutes acceptance of these
			terms.  If you do not agree with these terms, please do not use,
			install, modify or redistribute this Tritone Systems software.
			
			The Tritone Systems Software is provided by Tritone Systems on an "AS IS" basis.  
			Tritone Systems:
			MAKES NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION
			THE IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS
			FOR A PARTICULAR PURPOSE, REGARDING THE Tritone Systems SOFTWARE OR ITS USE AND
			OPERATION ALONE OR IN COMBINATION WITH YOUR PRODUCTS.
			
			IN NO EVENT SHALL Tritone Systems BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL
			OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
			SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
			INTERRUPTION) ARISING IN ANY WAY OUT OF THE USE, REPRODUCTION,
			MODIFICATION AND/OR DISTRIBUTION OF THE Tritone Systems SOFTWARE, HOWEVER CAUSED
			AND WHETHER UNDER THEORY OF CONTRACT, TORT (INCLUDING NEGLIGENCE),
			STRICT LIABILITY OR OTHERWISE, EVEN IF Tritone Systems HAS BEEN ADVISED OF THE
			POSSIBILITY OF SUCH DAMAGE.

	This software is based on the free sample code provided by Apple called FilterDemo. I modified 
	this demo software. Please see the original source at:
	
	https://developer.apple.com/library/mac/#samplecode/FilterDemo/Introduction/Intro.html#//apple_ref/doc/uid/DTS10003570

	for more information.
*/

#pragma once

#include "audioeffectx.h"
#include <string.h>
#define MAX_LEN 63

#include "plugin.h"

// -- These constants are written by RackAFX: DO NOT MODIFY 
//    if you indend on using RackAFX to update this project later!
//
// -- 0xA983 --
const int kNumPrograms = 3;
const int kNumParams = 6;
// -- 0xAF66 --


// ------------------------------------------------------------------ //
//
// The Derived Plug-In Class Definition
//
// ------------------------------------------------------------------ //
class CMultiFilter : public AudioEffectX
{
public:
	CMultiFilter(audioMasterCallback audioMaster);
	~CMultiFilter();

	// the set of virtual functions we need to override to make a meaningful Plug-In
	//
	//	
	// (optional) - these are called once after opening and just before closing
	virtual VstInt32 startProcess();
	virtual VstInt32 stopProcess();

	// effect, vendor, product name strings
	virtual bool getEffectName (char* name);
	virtual bool getVendorString (char* text);
	virtual bool getProductString (char* text);

	// processing (DSP stuff goes there); this is a pure virtual function, you MUST override it
	virtual void processReplacing (float** inputs, float** outputs, VstInt32 inFramesToProcess);

	// functions for dealing with factory presets
	virtual void setProgram(VstInt32 program);
	virtual void setProgramName(char *name);
	virtual void getProgramName(char *name);

	// functions for dealing with your control variable (parameters)
	// all values to and from the default VST Interface are floats 
	// with value range 0.0 -> 1.0
	// You NEED to supply the default I/F and fill these in even if your
	// plug-in has a GUI. Controls are zero-indexed
	virtual void setParameter (VstInt32 index, float value);
	virtual float getParameter (VstInt32 index);

	// functions to print text strings represeting your controls
	virtual void getParameterLabel (VstInt32 index, char* label);
	virtual void getParameterDisplay (VstInt32 index, char* text);
	virtual void getParameterName (VstInt32 index, char* text);

	// pause and resume functions
	virtual void suspend();
	virtual void resume();

	// message pump for the Plug-In; if you make your own GUI you need to reply to two
	// of the messages here
	virtual VstIntPtr dispatcher (VstInt32 opcode, VstInt32 index, VstIntPtr value, void* ptr, float opt);

private:
	// in case you need it
	float m_fSampleRate;

	CPlugIn* m_pRAFXPlugIn;
	char* getEnumString(char* string, int index);

};
// ------------------------------------------------------------------ //
