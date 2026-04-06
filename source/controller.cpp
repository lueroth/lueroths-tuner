//------------------------------------------------------------------------
// Copyright(c) 2026 lueroth.
//------------------------------------------------------------------------

#include "controller.h"
#include "cids.h"
#include "base/source/fstreamer.h"
#include "vstgui4/vstgui/plugin-bindings/vst3editor.h"
#include "pluginterfaces/gui/iplugview.h"
#include "vstgui/lib/platform/win32/winstring.h"
#include "vstgui/uidescription/uiattributes.h"

using namespace Steinberg;

namespace yin {

//------------------------------------------------------------------------
// genTestController Implementation
//------------------------------------------------------------------------

	tresult PLUGIN_API genTestController::initialize (FUnknown* context)
	{
	// Here the Plug-in will be instantiated

	//---do not forget to call parent ------
		tresult result = EditControllerEx1::initialize (context);
		if (result != kResultOk) {
			return result;
		}

		auto* centParam = new Vst::RangeParameter(STR16("Cents offset"), kCentsOffset, STR16("units"), -50.0, 50.0, 0.0, 100, Vst::ParameterInfo::kIsReadOnly);
		parameters.addParameter(centParam);

	// Here you could register some parameters
		return result;
	}

	tresult PLUGIN_API genTestController::notify(Vst::IMessage* message) {
		if (!message) return kInvalidArgument;

		if (FIDStringsEqual (message->getMessageID(), "NoteUpdate")) {
			Vst::TChar realNote[128];
			if (message->getAttributes()->getString("Note", realNote, 128) == kResultOk) {
				if (noteLabel) {
					char note8[8] = {0};
					wcstombs(note8, reinterpret_cast<const wchar_t*>(realNote), 8);
					noteLabel->setText(note8);
					noteLabel->invalid();
				}
			}
			return kResultOk;
		}
		return EditController::notify(message);
	}

//------------------------------------------------------------------------
	tresult PLUGIN_API genTestController::terminate ()
	{
	// Here the Plug-in will be de-instantiated, last possibility to remove some memory!

	//---do not forget to call parent ------
		return EditControllerEx1::terminate ();
	}

//------------------------------------------------------------------------
	tresult PLUGIN_API genTestController::setComponentState (IBStream* state)
	{
	// Here you get the state of the component (Processor part)
		if (!state) return kResultFalse;

		IBStreamer streamer(state, kLittleEndian);
		float savedOffset = 0.0f;
		if (streamer.readFloat(savedOffset) == false) return kResultFalse;

		setParamNormalized(kCentsOffset, savedOffset);

		return kResultOk;
	}

//------------------------------------------------------------------------
	tresult PLUGIN_API genTestController::setState (IBStream* state)
	{
	// Here you get the state of the controller
		return kResultTrue;
	}

//------------------------------------------------------------------------
	tresult PLUGIN_API genTestController::getState (IBStream* state)
	{
	// Here you are asked to deliver the state of the controller (if needed)
	// Note: the real state of your plug-in is saved in the processor
		return kResultTrue;
	}



//------------------------------------------------------------------------
	IPlugView* PLUGIN_API genTestController::createView (FIDString name)
	{
		// Here the Host wants to open your editor (if you have one)
		if (FIDStringsEqual (name, Vst::ViewType::kEditor))
		{
			// create your editor here and return a IPlugView ptr of it
			return new VSTGUI::VST3Editor (this, "view", "editor.uidesc");
		}
		return nullptr;
	}

	VSTGUI::CView *genTestController::verifyView(VSTGUI::CView *view, const VSTGUI::UIAttributes &attributes, const VSTGUI::IUIDescription *description, VSTGUI::VST3Editor *editor) {
		if (auto* label = dynamic_cast<VSTGUI::CTextLabel*>(view)) {
			const std::string* name = attributes.getAttributeValue("custom-view-name");
			if (name && *name == "noteLabel") noteLabel = label;
		}
		return view;
	}

	void genTestController::willClose(VSTGUI::VST3Editor *editor) {
		noteLabel = nullptr;
	}

	//------------------------------------------------------------------------

} // namespace yin
