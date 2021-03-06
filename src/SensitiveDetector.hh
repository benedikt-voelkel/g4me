/// @author: Roberto Preghenella
/// @email: preghenella@bo.infn.it

#ifndef _SensitiveDetector_h_
#define _SensitiveDetector_h_

#include "G4VSensitiveDetector.hh"

namespace G4me {

class SensitiveDetector : public G4VSensitiveDetector {
  
public:
  
  SensitiveDetector(const G4String &name) : G4VSensitiveDetector(name) {};
  ~SensitiveDetector() = default;

protected:

  G4bool ProcessHits(G4Step *aStep, G4TouchableHistory *ROhist) override;
  
};

} /** namespace G4me **/
  
#endif /** _SensitiveDetector_h_ **/
