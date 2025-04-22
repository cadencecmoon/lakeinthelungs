if sys == 'windows'
    sources += files(
        'win32_mmap.c',
        'win32_module.c',
        'win32_process.c',
        'win32_threads.c',
        'win32_time.c',
    )
    defs += [
        '-DLAKE_PLATFORM_WINDOWS=1',
        '-DUNICODE',
        '-D_WIN32_WINNT=0x0A00',
        '-DVK_USE_PLATFORM_WIN32_KHR' ]

    dep_d3d12 = cc.find_library('d3d12', required : get_option('d3d12'))
    dep_dxcore = cc.find_library('dxcore', required : cc.get_id() == 'msvc')
    if dep_d3d12.found()
        #sources += files('')
        defs += '-DXAKU_D3D12=1'
    endif
    
    # TODO asio sdk
    asio_found = false
    if asio_found
        sources += files(
            'iasiothiscallresolver.cpp',
        )
        defs += '-DSOMA_ASIO=1'
    endif

    wasapi_found = cc.check_header('audioclient.h', required : get_option('wasapi'))
    if wasapi_found
        #sources += files('')
        defs += '-DSOMA_WASAPI=1'
    endif

    dep_xaudio = cc.find_library('xaudio2', required : get_option('xaudio2'))
    if dep_xaudio.found()
        #sources += files('')
        defs += '-DSOMA_XAUDIO2=1'
    endif
endif

