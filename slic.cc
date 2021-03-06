/*!
  * @mainpage Simulator for the Linear Collider (SLIC) Documentation
  *
  * @par
  * The SLIC simulator is a Geant4 package that uses LCDD as its geometry input.
  *
  * @par
  * To see the available command line options, execute the following from the shell:
  * @verbatim slic --help @endverbatim
  *
  * @par
  * To print the version of the application, execute the following:
  * @verbatim slic --version @endverbatim
  *
  * @par
  * The primary source of documentation for SLIC is its
  * <a href="https://confluence.slac.stanford.edu/display/ilc/SLIC">Confluence Wiki page</a>.
  *
  * @par
  * Any questions can be directed to <a href="mailto:jeremym@slac.stanford.edu">Jeremy McCormick</a>
  * or posted in the <a href="http://forum.linearcollider.org/index.php?t=threadt&frm_id=7&rid=0&S=012b8714798313e7abf7c4d092eb9d0a">Full Simulations Area</a>
  * of the <a href="http://forum.linearcollider.org/">LinearCollider.org Forum</a>.
  */
// SLIC
#include "SlicMain.hh"

using namespace slic;

/**
 * The main function for the application, which delegates to a class.
 */
int main(int argc, char** argv) {
    return (new SlicMain())->main(argc, argv);
}


