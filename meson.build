project(
    'sdbusplus-examples',
    'cpp',
    version: '1.0',
    license: 'GPLv2',
    default_options: [
        'cpp_std=c++17',
        'werror=true',
    ],
)

executable(
    'sdbusplus-server-example-asio',
    [
        'sdbusplus-server-example-asio.cpp',
    ],
    cpp_args: [
        '-Wno-psabi',
        '-DBOOST_SYSTEM_NO_DEPRECATED',
        '-DBOOST_ASIO_DISABLE_THREADS',
        '-DBOOST_ERROR_CODE_HEADER_ONLY',
        '-DBOOST_NO_RTTI',
        '-DBOOST_NO_TYPEID',
        '-DBOOST_ALL_NO_LIB',
    ],
    dependencies: [
        dependency('sdbusplus'),
    ],
    install: true
)

executable(
    'sdbusplus-server-example-sdevent',
    [
        'sdbusplus-server-example-sdevent.cpp',
    ],
    cpp_args: [
        '-Wno-psabi',
    ],
    dependencies: [
        dependency('sdbusplus'),
        dependency('sdeventplus'),
    ],
    install: true
)
