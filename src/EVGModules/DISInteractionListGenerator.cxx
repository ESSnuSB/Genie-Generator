//____________________________________________________________________________
/*!

\class   genie::DISInteractionListGenerator

\brief   Concrete implementations of the InteractionListGeneratorI interface.
         Generate a list of all the Interaction (= event summary) objects that
         can be generated by the DIS EventGenerator.

\author  Costas Andreopoulos <C.V.Andreopoulos@rl.ac.uk>
         CCLRC, Rutherford Appleton Laboratory

\created May 13, 2005

*/
//____________________________________________________________________________

#include "EVGModules/DISInteractionListGenerator.h"
#include "EVGCore/InteractionList.h"
#include "Interaction/Interaction.h"
#include "Messenger/Messenger.h"
#include "PDG/PDGCodes.h"
#include "PDG/PDGUtils.h"

using namespace genie;

//___________________________________________________________________________
DISInteractionListGenerator::DISInteractionListGenerator() :
InteractionListGeneratorI()
{
  fName = "genie::DISInteractionListGenerator";
}
//___________________________________________________________________________
DISInteractionListGenerator::DISInteractionListGenerator(
                                                    const char * param_set) :
InteractionListGeneratorI(param_set)
{
  fName = "genie::DISInteractionListGenerator";

  this->FindConfig();
}
//___________________________________________________________________________
DISInteractionListGenerator::~DISInteractionListGenerator()
{

}
//___________________________________________________________________________
InteractionList * DISInteractionListGenerator::CreateInteractionList(
                                       const InitialState & init_state) const
{
  LOG("InteractionList", pINFO)
                     << "\nGenerating Interaction List for \n" << init_state;

  bool isCC    = fConfig->Exists("is-CC") ?
                                    fConfig->GetBool("is-CC")    : false;
  bool isNC    = fConfig->Exists("is-NC") ?
                                    fConfig->GetBool("is-NC")    : false;
  bool isCharm = fConfig->Exists("is-Charm") ?
                                    fConfig->GetBool("is-Charm") : false;

  InteractionType_t inttype;
  if      (isCC) inttype = kIntWeakCC;
  else if (isNC) inttype = kIntWeakNC;
  else {
     LOG("InteractionList", pWARN)
                            << "\n**** Could not generate Interaction List";
     return 0;
  }

  int      nupdg  = init_state.GetProbePDGCode();
  Target * target = init_state.GetTargetPtr();

  if( !pdg::IsNeutrino(nupdg) && !pdg::IsAntiNeutrino(nupdg) ) {
     LOG("InteractionList", pWARN)
                            << "\n**** Could not generate Interaction List";
     return 0;
  }

  bool hasP = (target->Z() > 0);
  bool hasN = (target->N() > 0);

  InteractionList * intlist = new InteractionList;

  int nuclpdg[2] = { kPdgProton, kPdgNeutron };

  for(int inucl=0; inucl<2; inucl++) {

    int struck_nucleon = nuclpdg[inucl];

    if( (struck_nucleon == kPdgProton  && hasP) ||
        (struck_nucleon == kPdgNeutron && hasN) ) {

      ProcessInfo   proc_info(kScDeepInelastic, inttype);

      Interaction * interaction = new Interaction(init_state, proc_info);
      Target * target = interaction->GetInitialStatePtr()->GetTargetPtr();

      target->SetStruckNucleonPDGCode(struck_nucleon);

      if(isCharm) {
         XclsTag exclusive_tag;
         exclusive_tag.SetCharm();
         interaction->SetExclusiveTag(exclusive_tag);
      }
      intlist->push_back(interaction);
    }
  }

  if(intlist->size() == 0) {
     LOG("InteractionList", pWARN)
                             << "\n**** Returning empty Interaction List";
     delete intlist;
     return 0;
  }
  return intlist;
}
//___________________________________________________________________________

