IF( NOT GDML_TAG )
    SET( GDML_TAG "master" )
ENDIF()

EXTERNALPROJECT_ADD(

    GDML

    DEPENDS XERCES Geant4 

    GIT_REPOSITORY "https://github.com/slaclab/gdml"
    GIT_TAG ${GDML_TAG}
    
    UPDATE_COMMAND ""
    PATCH_COMMAND ""

    SOURCE_DIR "${CMAKE_BINARY_DIR}/gdml"
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${DEPENDENCY_INSTALL_DIR}/gdml -DXERCES_DIR=${XERCES_DIR} -DGeant4_DIR=${Geant4_DIR}
    
    BUILD_COMMAND make -j4
)

SET( GDML_DIR ${DEPENDENCY_INSTALL_DIR}/gdml CACHE PATH "GDML install dir" FORCE )
