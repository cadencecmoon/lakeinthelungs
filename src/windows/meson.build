if sys == 'windows' 
    #engine_sources += files('')
    defs += [
        '-DPLATFORM_WINDOWS=1',
        '-DVK_USE_PLATFORM_WIN32_KHR' ]
    deps += dep_threads

    found_wasapi = cc.check_header('audioclient.h', required : get_option('wasapi'))
    if found_wasapi
        #engine_sources += files('')
        defs += '-DOCTAVIA_WASAPI=1'
        deps += cc.find_library('mfplat', required : true)
        deps += cc.find_library('mfuuid', required : true)
        deps += cc.find_library('ksuser', required : true)
        deps += cc.find_library('wmcodecdspuuid', required : true)
    endif

    dep_xaudio = cc.find_library('xaudio2', required : get_option('xaudio2'))
    if dep_xaudio.found()
        #engine_sources += files('')
        defs += '-DOCTAVIA_XAUDIO2=1'
    endif
endif
