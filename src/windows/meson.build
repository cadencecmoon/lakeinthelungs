if sys == 'windows' 
    engine_sources += files(
        'process.c',
        'threads.c',
        'time.c',
        'win32_display.c',
    )
    defs += [
        '-DPLATFORM_WINDOWS=1',
        '-DUNICODE',
        '-D_WIN32_WINNT=0x0A00',
        '-DVK_USE_PLATFORM_WIN32_KHR' ]
    deps += dep_threads

    dep_d3d12 = cc.find_library('d3d12', required : get_option('d3d12'))
    dep_dxcore = cc.find_library('dxcore', required : cc.get_id() == 'msvc')
    if dep_d3d12.found()
        #engine_sources += files('')
        defs += '-DREZNOR_D3D12=1'
        #deps += [ dep_d3d12, dep_dxcore ]
    endif

    found_wasapi = cc.check_header('audioclient.h', required : get_option('wasapi'))
    if found_wasapi
        #engine_sources += files('')
        defs += '-DSOMA_WASAPI=1'
        #deps += cc.find_library('mfplat', required : true)
        #deps += cc.find_library('mfuuid', required : true)
        #deps += cc.find_library('ksuser', required : true)
        #deps += cc.find_library('wmcodecdspuuid', required : true)
    endif

    dep_xaudio = cc.find_library('xaudio2', required : get_option('xaudio2'))
    if dep_xaudio.found()
        #engine_sources += files('')
        defs += '-DSOMA_XAUDIO2=1'
        #deps += dep_xaudio
    endif
endif
