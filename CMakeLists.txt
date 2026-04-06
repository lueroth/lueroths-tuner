cmake_minimum_required(VERSION 3.14.0)

option(SMTG_ENABLE_VST3_PLUGIN_EXAMPLES "Enable VST 3 Plug-in Examples" OFF)
option(SMTG_ENABLE_VST3_HOSTING_EXAMPLES "Enable VST 3 Hosting Examples" OFF)

set(SMTG_ENABLE_VSTGUI_SUPPORT ON)

set(CMAKE_OSX_DEPLOYMENT_TARGET 10.13 CACHE STRING "")

set(vst3sdk_SOURCE_DIR "C:/CLion_SDKs/VST_SDK/vst3sdk")
if(NOT vst3sdk_SOURCE_DIR)
    message(FATAL_ERROR "Path to VST3 SDK is empty!")
endif()



project(genTest
    # This is your plug-in version number. Change it here only.
    # Version number symbols usable in C++ can be found in
    # source/version.h and ${PROJECT_BINARY_DIR}/projectversion.h.
    VERSION 0.0.0.1
    DESCRIPTION "genTest VST 3 Plug-in"
)

set(SMTG_VSTGUI_ROOT "${vst3sdk_SOURCE_DIR}")

add_subdirectory(${vst3sdk_SOURCE_DIR} ${PROJECT_BINARY_DIR}/vst3sdk)
smtg_enable_vst3_sdk()

smtg_add_vst3plugin(genTest
        source/version.h
        source/cids.h
        source/processor.h
        source/processor.cpp
        source/controller.h
        source/controller.cpp
        source/entry.cpp
        source/Pitch_Detection/YINPitchDetection.h
        source/Pitch_Detection/YINPitchDetection.cpp
        source/Pitch_Detection/FindNote_n_Offset.h
        source/Pitch_Detection/FindNote_n_Offset.cpp
)

smtg_target_add_plugin_resources(genTest
    RESOURCES
        resource/editor.uidesc
)

smtg_target_add_plugin_snapshots (genTest
    RESOURCES
        resource/3086F695728B5E0F9A4CAA04C3E10EF7_snapshot.png
        resource/3086F695728B5E0F9A4CAA04C3E10EF7_snapshot_2.0x.png
)

target_link_libraries(genTest
    PRIVATE
        vstgui_support
        sdk
)

smtg_target_configure_version_file(genTest)

if(SMTG_MAC)
    smtg_target_set_bundle(genTest
        BUNDLE_IDENTIFIER com.lueroth.genTest
        COMPANY_NAME "lueroth"
    )
    smtg_target_set_debug_executable(genTest
        "/Applications/VST3PluginTestHost.app"
        "--pluginfolder;$(BUILT_PRODUCTS_DIR)"
    )
elseif(SMTG_WIN)
    target_sources(genTest PRIVATE
        resource/win32resource.rc
    )
    if(MSVC)
        set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT genTest)

        smtg_target_set_debug_executable(genTest
            "$(ProgramW6432)/Steinberg/VST3PluginTestHost/VST3PluginTestHost.exe"
            "--pluginfolder \"$(OutDir)/\""
        )
else ()
        target_link_options(validator PRIVATE "-shared" "-Wl,--subsystem,windows")

        smtg_target_set_debug_executable(genTest
                "$(ProgramW6432)/Steinberg/VST3PluginTestHost/VST3PluginTestHost.exe"
                "--pluginfolder;${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
        )
    endif()
endif(SMTG_MAC)