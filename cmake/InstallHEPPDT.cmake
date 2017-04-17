EXTERNALPROJECT_ADD(
    HEPPDT

    URL "http://lcgapp.cern.ch/project/simu/HepPDT/download/HepPDT-3.04.01.tar.gz"

    SOURCE_DIR "${CMAKE_BINARY_DIR}/extdep/heppdt"
    
    UPDATE_COMMAND ""
    PATCH_COMMAND ""
    
    CONFIGURE_COMMAND "${CMAKE_BINARY_DIR}/extdep/heppdt/configure" --prefix=${GLOBAL_INSTALL_DIR}/heppdt
)

SET( HEPPDT_DIR ${GLOBAL_INSTALL_DIR}/heppdt CACHE PATH "HEPPDT install dir" FORCE )