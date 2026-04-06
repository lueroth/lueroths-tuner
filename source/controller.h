//------------------------------------------------------------------------
// Copyright(c) 2026 lueroth.
//------------------------------------------------------------------------

#pragma once

#include "public.sdk/source/vst/vsteditcontroller.h"
#include "vstgui/plugin-bindings/vst3editor.h"

namespace yin {

//------------------------------------------------------------------------
//  genTestController
//------------------------------------------------------------------------
	class genTestController : public Steinberg::Vst::EditControllerEx1, public VSTGUI::VST3EditorDelegate
	{
		public:
//------------------------------------------------------------------------
		genTestController () = default;
		~genTestController () SMTG_OVERRIDE = default;

    // Create function
		static Steinberg::FUnknown* createInstance (void* /*context*/) {

			return (Steinberg::Vst::IEditController*)new genTestController;
		}

	//--- from IPluginBase -----------------------------------------------
		Steinberg::tresult PLUGIN_API initialize (Steinberg::FUnknown* context) SMTG_OVERRIDE;
		Steinberg::tresult PLUGIN_API genTestController::notify(Steinberg::Vst::IMessage* message) SMTG_OVERRIDE;
		Steinberg::tresult PLUGIN_API terminate () SMTG_OVERRIDE;

	//--- from EditController --------------------------------------------
		Steinberg::tresult PLUGIN_API setComponentState (Steinberg::IBStream* state) SMTG_OVERRIDE;
		Steinberg::IPlugView* PLUGIN_API createView (Steinberg::FIDString name) SMTG_OVERRIDE;
		Steinberg::tresult PLUGIN_API setState (Steinberg::IBStream* state) SMTG_OVERRIDE;
		Steinberg::tresult PLUGIN_API getState (Steinberg::IBStream* state) SMTG_OVERRIDE;

		VSTGUI::CView* verifyView(VSTGUI::CView *view, const VSTGUI::UIAttributes &attributes, const VSTGUI::IUIDescription *description, VSTGUI::VST3Editor *editor) SMTG_OVERRIDE;
		void PLUGIN_API willClose(VSTGUI::VST3Editor *editor) SMTG_OVERRIDE;

	protected:
		VSTGUI::CTextLabel* noteLabel {nullptr};
		char note[4] = {0};

 	//---Interface---------
		DEFINE_INTERFACES
		// Here you can add more supported VST3 interfaces
		// DEF_INTERFACE (Vst::IXXX)
		END_DEFINE_INTERFACES(EditController)
		DELEGATE_REFCOUNT(EditController)

//------------------------------------------------------------------------
protected:
};

//------------------------------------------------------------------------
} // namespace yin
