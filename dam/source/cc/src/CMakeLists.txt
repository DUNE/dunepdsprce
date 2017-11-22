add_library(protodune-dam SHARED 
    Headers.cc
    DataFragmentUnpack.cc
    TpcFragmentUnpack.cc
    TpcStreamUnpack.cc
    DataFragment.cc
    Identifier.cc
    Originator.cc
    Data.cc
    TpcFragment.cc
    TpcStream.cc
    TpcRanges.cc
    TpcToc.cc
    TpcPacket.cc
    WibFrame.cc
    )

include_directories(../)

INSTALL(
    TARGETS protodune-dam
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION lib
    )

INSTALL(
    DIRECTORY ../dam
    DESTINATION include
    )
