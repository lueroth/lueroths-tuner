//------------------------------------------------------------------------
// Copyright(c) 2026 lueroth.
//------------------------------------------------------------------------

#include "processor.h"

#include <thread>

#include "cids.h"
#include "Pitch_Detection/FindNote_n_Offset.h"

#include "base/source/fstreamer.h"
#include "pluginterfaces/vst/ivstparameterchanges.h"
#include "vstgui4/vstgui/lib/platform/win32/winstring.h"
//#include "pluginterfaces/vst/ivstparameterchanges.h"

using namespace Steinberg;

namespace yin {
//------------------------------------------------------------------------
// genTestProcessor
//------------------------------------------------------------------------

	genTestProcessor::genTestProcessor() {
	//--- set the wanted controller for our processor
		setControllerClass (kgenTestControllerUID);
	}

//------------------------------------------------------------------------
	genTestProcessor::~genTestProcessor() = default;

//------------------------------------------------------------------------
	tresult PLUGIN_API genTestProcessor::initialize(FUnknown* context) {
	// Here the Plug-in will be instantiated
	//---always initialize the parent-------
		tresult result = AudioEffect::initialize(context);

	// if everything Ok, continue
		if (result != kResultOk) {
			return result;
		}

	//--- create Audio IO ------
		addAudioInput (STR16 ("Stereo In"), Vst::SpeakerArr::kStereo);
		addAudioOutput (STR16 ("Stereo Out"), Vst::SpeakerArr::kStereo);

	/* If you don't need an event bus, you can remove the next line */
		addEventInput (STR16 ("Event In"), 1);

		accumulator.reserve(reqSize);

		OutputDebugStringA("Tuner by lueroth has successfully been started. \n");
		return kResultOk;
	}

//------------------------------------------------------------------------
	tresult PLUGIN_API genTestProcessor::terminate () {
	// Here the Plug-in will be de-instantiated, last possibility to remove some memory!

	//---do not forget to call parent ------
		return AudioEffect::terminate ();
	}

//------------------------------------------------------------------------
	tresult PLUGIN_API genTestProcessor::setActive (TBool state) {
	//--- called when the Plug-in is enabled/disabled (On/Off) -----
		return AudioEffect::setActive (state);
	}

//------------------------------------------------------------------------
	tresult PLUGIN_API genTestProcessor::process (Vst::ProcessData& data) {
		//--- First : Read inputs parameter changes-----------
/*
		if (data.inputParameterChanges)
		{
			int32 numParamsChanged = data.inputParameterChanges->getParameterCount ();
			for (int32 index = 0; index < numParamsChanged; index++)
			{
				if (auto* paramQueue = data.inputParameterChanges->getParameterData (index))
				{
					Vst::ParamValue value;
					int32 sampleOffset;
					int32 numPoints = paramQueue->getPointCount ();
					switch (paramQueue->getParameterId ())
					{
					}
				}
			}
		}*/
		//--- Here you have to implement your processing

		if (data.numSamples <= 0) return kResultOk;

		Vst::AudioBusBuffers& inputBus {data.inputs[0]};

		if (!(data.inputs[0].silenceFlags & 1)) {
			if (data.numInputs > 0 && data.inputs[0].numChannels > 0) {
				float* input {inputBus.channelBuffers32[0]};

				// if there is an input present
				if (input) {
					accumulator.insert(accumulator.end(), input, input + data.numSamples);

					if (accumulator.size() >= reqSize) {

						auto vector = accumulator;
						std::thread([this, vector]() mutable {
							fno::note_n_offset result = find_note_n_offset_.findNote(vector.data(), vector.size());

							offset.store(result.offset);
							strncpy(note, result.note.c_str(), result.note.length());
							update.store(true);
						}).detach();

						accumulator.clear();
					}
				}
			}
		}

		if (update.load()) {
			if (data.outputParameterChanges) {
				int32 index {0};
				Vst::IParamValueQueue* queue = data.outputParameterChanges->addParameterData(kCentsOffset, index);
				if (queue) {
					int32 refIndex {0};
					queue->addPoint(0, offset.load(), refIndex);
				}
			}

			if (auto message = allocateMessage()) {
				message->setMessageID("NoteUpdate");

				Vst::String128 realNote;

				str8ToStr16(realNote, note, 128);
				message->getAttributes()->setString("Note", realNote);
				sendMessage(message);
			}
			update.store(false);
		}

		return kResultOk;
	}

//------------------------------------------------------------------------
	tresult PLUGIN_API genTestProcessor::setupProcessing (Vst::ProcessSetup& newSetup) {
	//--- called before any processing ----
		find_note_n_offset_.setSampleRate(processSetup.sampleRate);
		return AudioEffect::setupProcessing (newSetup);
	}

//------------------------------------------------------------------------
	tresult PLUGIN_API genTestProcessor::canProcessSampleSize (int32 symbolicSampleSize) {
	// by default kSample32 is supported
		if (symbolicSampleSize == Vst::kSample32) return kResultTrue;

	// disable the following comment if your processing support kSample64
	/* if (symbolicSampleSize == Vst::kSample64)
		return kResultTrue; */

		return kResultFalse;
	}

//------------------------------------------------------------------------
	tresult PLUGIN_API genTestProcessor::setState (IBStream* state)
	{
	// called when we load a preset, the model has to be reloaded
		IBStreamer streamer (state, kLittleEndian);
		streamer.writeFloat(offset.load());
	
		return kResultOk;
	}

//------------------------------------------------------------------------
	tresult PLUGIN_API genTestProcessor::getState (IBStream* state)
	{
	// here we need to save the model
		IBStreamer streamer (state, kLittleEndian);
		float savedOffset = 0.0f;
		if (streamer.readFloat(savedOffset) == false) return kResultFalse;
		offset.store(savedOffset);

		return kResultOk;
	}
//------------------------------------------------------------------------
} // namespace yin
