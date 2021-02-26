/// @author: Roberto Preghenella
/// @email: preghenella@bo.infn.it

#ifndef _DetectorConstruction_h_
#define _DetectorConstruction_h_

#include "G4VUserDetectorConstruction.hh"
#include "G4UImessenger.hh"
#include <map>


class G4String;
class G4Material;
class G4OpticalSurface;

class G4UIcommand;
class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWithADoubleAndUnit;

namespace G4me {

namespace TOFRICH {
  class DetectorConstruction;
}

namespace FCT {
  class DetectorConstruction;
}

namespace ABSO {
  class DetectorConstruction;
}

class DetectorConstruction : public G4VUserDetectorConstruction,
			     public G4UImessenger
{

public:

  DetectorConstruction();
  ~DetectorConstruction() override;

  G4VPhysicalVolume *Construct() override;
  void ConstructSDandField() override;

protected:

  void SetNewValue(G4UIcommand *command, G4String value);

  G4UIdirectory *mDetectorDirectory;
  G4UIcmdWithAString *mDetectorEnableCmd;

  G4UIdirectory *mGeometryIODirectory;
  G4UIcmdWithAString *mPVIDMapFileCmd;
  G4String mPVIDMapFile;

  TOFRICH::DetectorConstruction *mTOFRICH;
  FCT::DetectorConstruction *mFCT;
  ABSO::DetectorConstruction *mABSO;

  G4UIdirectory *mPipeDirectory;
  G4UIcmdWithADoubleAndUnit *mPipeRadiusCmd;
  G4UIcmdWithADoubleAndUnit *mPipeThicknessCmd;
  G4UIcmdWithADoubleAndUnit *mPipeLengthCmd;
  double mPipeRadius;
  double mPipeLength;
  double mPipeThickness;

  G4UIdirectory *mTrackerDirectory;
  G4UIcommand *mTrackerAddLayerCmd;
  std::vector<std::map<std::string, double>> mTrackerLayer;

};

} /** namespace G4me **/

#endif /** _DetectorConstruction_h_ **/
