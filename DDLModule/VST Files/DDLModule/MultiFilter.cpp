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

	This software is based on the free sample code provided by Steinberg GmBH called ADELAY.

	See the example at: http://www.steinberg.net/en/company/developer.html
*/


#include <stdio.h>
#include <string.h>

#include "MultiFilter.h"
#include "ResonantLPF.h"

#include <math.h>

//-----------------------------------------------------------------------------
// Plug-In constructor
// 1) create programs
// 2) initialize Plug-in
CMultiFilter::CMultiFilter (audioMasterCallback audioMaster)
			: AudioEffectX (audioMaster, kNumPrograms, kNumParams)
{
	// create our inner plugin
	//
	// This also initializes all the controls; the plugin also
	// stores all its presets so no need to write those objects any more!
    m_pRAFXPlugIn = new CResonantLPF;
    m_pRAFXPlugIn->initialize();

	// if you are sr dependent
	m_fSampleRate = getSampleRate();

	// stereo IN
	setNumInputs(2);

	// stereo OUT
	setNumOutputs(2);

	// our GUID
	setUniqueID ('MFlt');

	// IF you have a custom GUI, un-comment this line otherwise will default to
	// standard I/F
	// cEffect.flags |= effFlagsHasEditor;

	// PAUSE function
	suspend ();		// flush buffer
}

//------------------------------------------------------------------------
// Plug-In destuctor
// destroy allocated stuff
CMultiFilter::~CMultiFilter ()
{
	if(m_pRAFXPlugIn)
		delete m_pRAFXPlugIn;
}

//------------------------------------------------------------------------
// startProcess()
// one time, post Constructor initialization if needed
VstInt32 CMultiFilter::startProcess()
{
	// one time, post Constructor initialization if needed
	return 1;
}

//------------------------------------------------------------------------
// stopProcess()
// one time, pre Destructor code if needed
VstInt32 CMultiFilter::stopProcess()
{
	// one time, pre Destructor code if needed
	return 1;
}

//------------------------------------------------------------------------
// setProgram()
// called by the client when the user selects a preset
void CMultiFilter::setProgram (VstInt32 program)
{
	// store (this is a base class variable
	curProgram = program;

	if(m_pRAFXPlugIn)
	{
		int nParams = m_pRAFXPlugIn->m_UIControlList.countLegalVSTIF();

		// iterate
		for(int i = 0; i < nParams; i++)
		{
			// they are in AU proper order in the ControlList - do NOT reference them with RackAFX ID values any more!
			CUICtrl* pUICtrl = m_pRAFXPlugIn->m_UIControlList.getAt(i);

			if(pUICtrl)
			{
				// call our own setParameter()
				setParameter(i, calcSliderVariable(pUICtrl->fUserDisplayDataLoLimit,
												   pUICtrl->fUserDisplayDataHiLimit,
												   pUICtrl->dPresetData[curProgram])); // control object stores its presets!
			}
		}

		// now do the Vector Joystick Programs
		float* pJSProg = m_pRAFXPlugIn->m_PresetJSPrograms[curProgram];
		for(int i=0; i<MAX_JS_PROGRAM_STEPS; i++)
		{
			m_pRAFXPlugIn->m_pVectorJSProgram[JS_PROG_INDEX(i,0)] = pJSProg[JS_PROG_INDEX(i,0)];
			m_pRAFXPlugIn->m_pVectorJSProgram[JS_PROG_INDEX(i,1)] = pJSProg[JS_PROG_INDEX(i,1)];
			m_pRAFXPlugIn->m_pVectorJSProgram[JS_PROG_INDEX(i,2)] = pJSProg[JS_PROG_INDEX(i,2)];
			m_pRAFXPlugIn->m_pVectorJSProgram[JS_PROG_INDEX(i,3)] = pJSProg[JS_PROG_INDEX(i,3)];
			m_pRAFXPlugIn->m_pVectorJSProgram[JS_PROG_INDEX(i,4)] = pJSProg[JS_PROG_INDEX(i,4)];
			m_pRAFXPlugIn->m_pVectorJSProgram[JS_PROG_INDEX(i,5)] = pJSProg[JS_PROG_INDEX(i,5)];
			m_pRAFXPlugIn->m_pVectorJSProgram[JS_PROG_INDEX(i,6)] = pJSProg[JS_PROG_INDEX(i,6)];
		}
	}

	// now the additional presets (advanced, only if you support it)
	float* pPresetData = m_pRAFXPlugIn->m_AddlPresetValues[curProgram];
	if(pPresetData)
	{
		int nCount = m_pRAFXPlugIn->getNumAddtlPresets();
		for(int i=0; i<nCount; i++)
		{
			m_pRAFXPlugIn->setAddtlPresetValue(i, pPresetData[i]);
		}
	}
}


//------------------------------------------------------------------------
// setProgramName()
// for clients that allow setting this (some don't and store in their own way)
void CMultiFilter::setProgramName (char *name)
{
	// RackAFX does not support saving user presets; client does that!
	// so no need to change a preset name here.
}

//------------------------------------------------------------------------
// getProgramName()
// called by client when filling the list of presets
void CMultiFilter::getProgramName (char *name)
{
	if(m_pRAFXPlugIn && kNumPrograms > 0)
		vst_strncpy(name, m_pRAFXPlugIn->m_PresetNames[curProgram], kVstMaxProgNameLen);
	else
		vst_strncpy(name, "Default", kVstMaxProgNameLen);

}

//------------------------------------------------------------------------
// getEffectName()
// You can change this if you don't want to use the RackAFX Name
// or want to have multiple plug-ins with different names
bool CMultiFilter::getEffectName (char* name)
{
	if(m_pRAFXPlugIn)
		vst_strncpy (name, m_pRAFXPlugIn->m_PlugInName, kVstMaxEffectNameLen);

	return true;
}

//------------------------------------------------------------------------
// getProductString()
// fill in if you have a separate product name (suite)
bool CMultiFilter::getProductString (char* text)
{
	vst_strncpy (text, "", kVstMaxProductStrLen);
	return true;
}

//------------------------------------------------------------------------
// getVendorString()
// your vendor string
bool CMultiFilter::getVendorString (char* text)
{
	vst_strncpy (text, "", kVstMaxVendorStrLen);
	return true;
}


//------------------------------------------------------------------------
// suspend()
// equivalent of pause
void CMultiFilter::suspend ()
{
	// prepare for play; flush buffers, etc... if needed after play event
	// generally not needed

}

//------------------------------------------------------------------------
// resume()
// equivalent of prepareForPlay()
// NOTE: Ableton only calls this once
//       and streams 0s throught the Plug-in when inactive!
void CMultiFilter::resume()
{
	// prepare for play; flush buffers, etc...
	m_fSampleRate = getSampleRate();

	// set sample rate
	// forward the prepareForPlay() call
	if(m_pRAFXPlugIn)
	{
		m_pRAFXPlugIn->m_nSampleRate = (int)m_fSampleRate;
		m_pRAFXPlugIn->prepareForPlay();
	}
}

//------------------------------------------------------------------------
// setParameter()
// equivalent to userInterfaceChange()
// value is a number between 0 -> 1.0
// usually you will have to cook it to make it meaningful, but here
// the volume control only goes from 1 to 1.0 so nothing to process
void CMultiFilter::setParameter (VstInt32 index, float value)
{
    if(m_pRAFXPlugIn)
    {
		// call the built-in CPlugIn method that converts
		// and sets the value and calls userInterfaceChange()
		m_pRAFXPlugIn->setParameter(index, value);
	}
}

//------------------------------------------------------------------------
// getParameter()
// returns a value between 0 -> 1.0 representing your variable
float CMultiFilter::getParameter (VstInt32 index)
{
    if(m_pRAFXPlugIn)
    {
		return m_pRAFXPlugIn->getParameter(index);
	}

	// should never happen
	return 0.0;
}

//------------------------------------------------------------------------
// getParameterName()
// for the client to display it
void CMultiFilter::getParameterName (VstInt32 index, char* text)
{
    if(m_pRAFXPlugIn)
    {
        // they are in VST proper order in the ControlList - do NOT reference them with RackAFX ID values any more!
        CUICtrl* pUICtrl = m_pRAFXPlugIn->m_UIControlList.getAt(index);

		// just copy the name
        if(pUICtrl)
			vst_strncpy (text, pUICtrl->cControlName, kVstMaxParamStrLen);
	}
}

//------------------------------------------------------------------------
// getParameterDisplay()
// for the client to display it
void CMultiFilter::getParameterDisplay (VstInt32 index, char* text)
{
	// helper functions from audioeffect.h
	/*
	dB2string (float value, char* text, VstInt32 maxLen);		///< Stuffs \e text with an amplitude on the [0.0, 1.0] scale converted to its value in decibels.
	Hz2string (float samples, char* text, VstInt32 maxLen);	///< Stuffs \e text with the frequency in Hertz that has a period of \e samples.
	ms2string (float samples, char* text, VstInt32 maxLen);	///< Stuffs \e text with the duration in milliseconds of \e samples frames.
	float2string (float value, char* text, VstInt32 maxLen);	///< Stuffs \e text with a string representation on the floating point \e value.
	int2string (VstInt32 value, char* text, VstInt32 maxLen);	///< Stuffs \e text with a string representation on the integer \e value.
	*/
	if(m_pRAFXPlugIn)
    {
        // they are in VST proper order in the ControlList - do NOT reference them with RackAFX ID values any more!
        CUICtrl* pUICtrl = m_pRAFXPlugIn->m_UIControlList.getAt(index);

        if(pUICtrl)
        {
            // these are the 4 RackAFX datatypes for controls
            // enum {intData, floatData, doubleData, UINTData, nonData};
            switch (pUICtrl->uUserDataType)
            {
                case intData:
					int2string(*pUICtrl->m_pUserCookedIntData, text, kVstMaxParamStrLen);
                    break;

				case floatData:
					float2string(*pUICtrl->m_pUserCookedFloatData, text, kVstMaxParamStrLen);
                    break;

				case doubleData:
					float2string((float)*pUICtrl->m_pUserCookedDoubleData, text, kVstMaxParamStrLen);

                    break;

				case UINTData:
				{
					char* pEnum;
					pEnum = getEnumString(pUICtrl->cEnumeratedList, (int)(*(pUICtrl->m_pUserCookedUINTData)));
					if(pEnum)
						vst_strncpy(text, pEnum, kVstMaxParamStrLen);

					break;
				}
                default:
                    break;
            }
		}
	}
}

//------------------------------------------------------------------------
// getEnumString()
// helper function to retun an enum sub-string for above function
char* CMultiFilter::getEnumString(char* string, int index)
{
	int nLen = strlen(string);
	char* copyString = new char[nLen+1];

	vst_strncpy (copyString, string, strlen(string));

	for(int i=0; i<index+1; i++)
	{
		char * comma = ",";

		int j = strcspn (copyString,comma);

		if(i==index)
		{
			char* pType = new char[j+1];
			strncpy (pType, copyString, j);
			pType[j] = '\0';
			delete [] copyString;

			// special support for 2-state switches
			// (new in RAFX 5.4.14)
			if(strcmp(pType, "SWITCH_OFF") == 0)
			{
				delete [] pType;
				return "OFF";
			}
			else if(strcmp(pType, "SWITCH_ON") == 0)
			{
				delete [] pType;
				return "ON";
			}

			return pType;
		}
		else // remove it
		{
			char* pch = strchr(copyString,',');

			int nLen = strlen(copyString);
			memcpy (copyString,copyString+j+1,nLen-j);
		}
	}

	delete [] copyString;

	return NULL;
}

//------------------------------------------------------------------------
// getParameterLabel()
// for the client to display yor units
void CMultiFilter::getParameterLabel (VstInt32 index, char* label)
{
    if(m_pRAFXPlugIn)
    {
        // they are in VST proper order in the ControlList - do NOT reference them with RackAFX ID values any more!
        CUICtrl* pUICtrl = m_pRAFXPlugIn->m_UIControlList.getAt(index);

		// just copy the name
        if(pUICtrl)
			vst_strncpy (label, pUICtrl->cControlUnits, kVstMaxParamStrLen);
	}
}

//---------------------------------------------------------------------------
// processReplacing()
// the core processing function
void CMultiFilter::processReplacing (float** inputs, float** outputs, VstInt32 inFramesToProcess)
{
	// NOTE: Assuming stereo in/out
	float* pInputL  = inputs[0];
	float* pOutputL = outputs[0];
	float* pInputR  = inputs[1];
	float* pOutputR = outputs[1];

	// if your RackAFX Plug-In can process VST/AU buffers, do it!
	if(m_pRAFXPlugIn->m_bWantVSTBuffers)
    {
        // NOTE: VST buffer processing only takes one channel value (assumes in and out are same numbers)
        //       So, check m_pVSTInputBuffers[1] == NULL and m_pVSTOutputBuffers[1] == NULL to figure out
        //       the channel counts.
        m_pRAFXPlugIn->processVSTAudioBuffer(inputs, outputs, 2, inFramesToProcess);
    }
    else // standard RAFX frames
    {
		// RackAFX Frames; max is 2-channel but you can
		// change this if you want to process more channels
		float input[2];
		float output[2];

		while (--inFramesToProcess >= 0)
		{
			// load RAFX frame buffers, increment pointers
			input[0] = *pInputL++;
			input[1] = *pInputR++;

			// forward the call to the inner object
			m_pRAFXPlugIn->processAudioFrame(&input[0], &output[0], 2, 2);

			// set outputs, increment pointers
			*pOutputL++ = output[0];
			*pOutputR++ = output[1];
		}
	}
}


//---------------------------------------------------------------------------
// dispatcher()
// If you make your own GUI, reply to the messages:
//
// effEditGetRect: return a pointer to an ERect struct that describes your GUI size
// effEditOpen: create/show your GUI
// effEditClose: close/hide your GUI
VstIntPtr CMultiFilter::dispatcher (VstInt32 opcode, VstInt32 index, VstIntPtr value, void* ptr, float opt)
{
	// success/failure code
	int result = 0;

	switch (opcode)
	{
		// called when Plug-In is opened
		case effOpen:
		{
			// prepareForPlay() stuff
			break;
		}

		// called when Plug-In is closed
		case effClose:
		{
			break;
		}

		// GUI Rect
		// Custom GUI - return a pointer to a rect here via the void* (ptr)
		case effEditGetRect:
		{
		//	if(m_pGUIRect)
		//	{
		//		*(ERect**)ptr = m_pGUIRect;
		//		result = 1;  //
		//	}
			break;
		}

		// Create your GUI
		// the void* is a Windows handle to a client window (HWND) for you to use
		// in the creation of your GUI
		//
		// remember to un-comment this line of code from the constructor too:
		// cEffect.flags |= effFlagsHasEditor;
		case effEditOpen:
		{
			//
			//	makeGUI((HWND)ptr);
				break;
		}

		// Destroy your Custom GUI
		case effEditClose:
		{
			//
			//	destoryGUI();
				break;
		}

		// remember this for all other messages (there are a TON of them)
		default:
		{
			 result = AudioEffectX::dispatcher(opcode, index, value, ptr, opt);
			 break;
		}
	}
	return result;
}
