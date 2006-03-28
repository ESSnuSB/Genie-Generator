//____________________________________________________________________________
/*!

\class   genie::GHepRecordHistory

\brief   Holds the history of the GHEP event record within a single event
         generation sequence as a function of its processing step.
         The event record history is used to step back in the generation
         sequence if a processing step is to be re-run (the GENIE event
         generation framework equivalent of 'Undo')

\author  Costas Andreopoulos <C.V.Andreopoulos@rl.ac.uk>
         CCLRC, Rutherford Appleton Laboratory

\created September 23, 2005

*/
//____________________________________________________________________________

#include <TSystem.h>

#include "GHEP/GHepRecordHistory.h"
#include "GHEP/GHepRecord.h"
#include "Messenger/Messenger.h"

using std::endl;

using namespace genie;

//____________________________________________________________________________
namespace genie {
 ostream & operator << (ostream & stream, const GHepRecordHistory & history)
 {
   history.Print(stream);
   return stream;
 }
}
//___________________________________________________________________________
GHepRecordHistory::GHepRecordHistory() :
map<int, GHepRecord*>()
{
  fIsEnabled = this->Enabled();
}
//___________________________________________________________________________
GHepRecordHistory::GHepRecordHistory(const GHepRecordHistory & history) :
map<int, GHepRecord*>()
{
  this->Copy(history);

  fIsEnabled = this->Enabled();
}
//___________________________________________________________________________
GHepRecordHistory::~GHepRecordHistory()
{
  this->PurgeHistory();
}
//___________________________________________________________________________
void GHepRecordHistory::AddSnapshot(int step, GHepRecord * record)
{
// Adds a GHepRecord 'snapshot' at the history buffer

  if(!fIsEnabled) return;

  if(!record) {
   LOG("GHEP", pWARN)
    << "Input GHEP record snapshot is null. Is not added at history record";
    return;
  }

  if( this->count(step) == 0 ) {

     LOG("GHEP", pNOTICE)
                     << "Adding GHEP snapshot for processing step: " << step;

     GHepRecord * snapshot = new GHepRecord(*record);
     this->insert( map<int, GHepRecord*>::value_type(step,snapshot));

  } else {
     LOG("GHEP", pWARN)
      << "GHEP snapshot for processing step: " << step << " already exists!";
  }
}
//___________________________________________________________________________
void GHepRecordHistory::PurgeHistory(void)
{
  LOG("GHEP", pNOTICE) << "Purging GHEP history buffer";

  GHepRecordHistory::iterator history_iter;
  for(history_iter = this->begin();
                              history_iter != this->end(); ++history_iter) {

    int step = history_iter->first;
    LOG("GHEP", pINFO) 
                  << "Deleting GHEP snapshot for processing step: " << step;

    GHepRecord * record = history_iter->second;
    if(record) {
      delete record;
      record = 0;
    }
  }
  this->clear();
}
//___________________________________________________________________________
void GHepRecordHistory::PurgeRecentHistory(int start_step)
{
// Snapshots are added to the history record *after* each processing step
// (marked 0,1,2,...). A special snapshot corresponding to the event record
// before any processing step is added with key = -1.
// Therefore GHepRecordHistory keys should be: -1,0,1,2,3,...

  LOG("GHEP", pNOTICE) 
       << "Purging recent GHEP history buffer (processing step >= " 
                                                      << start_step << ")";

  if(start_step < -1) {
    LOG("GHEP", pWARN) 
               << "Invalid starting step: " << start_step << " - Ignoring";
    return;
  }

  if(start_step == -1) {
    // delete everything
    this->PurgeHistory();
    return;
  }

  GHepRecordHistory::iterator history_iter;
  for(history_iter = this->begin();
                              history_iter != this->end(); ++history_iter) {

    if(history_iter->first >= start_step) { 
       int step = history_iter->first;
       LOG("GHEP", pINFO) 
                  << "Deleting GHEP snapshot for processing step: " << step;
       this->erase(history_iter); 
    }
  }
}
//___________________________________________________________________________
void GHepRecordHistory::Copy(const GHepRecordHistory & history)
{
  this->PurgeHistory();

  GHepRecordHistory::const_iterator history_iter;
  for(history_iter = this->begin();
                              history_iter != this->end(); ++history_iter) {

    unsigned int step   = history_iter->first;
    GHepRecord * record = history_iter->second;

    this->AddSnapshot(step, record);
  }
}
//___________________________________________________________________________
void GHepRecordHistory::Print(ostream & stream) const
{
  stream << "\n ****** Printing GHEP record history"
                              << " [depth: " << this->size() << "]" << endl;

  GHepRecordHistory::const_iterator history_iter;
  for(history_iter = this->begin();
                              history_iter != this->end(); ++history_iter) {

    unsigned int step   = history_iter->first;
    GHepRecord * record = history_iter->second;

    stream << "\n[After processing step = " << step << "] :";

    if(!record) {
      stream
         << "** ERR: No history record available for this processing step!";
    } else {
      stream << *record;
    }
  }
}
//___________________________________________________________________________
bool GHepRecordHistory::Enabled(void) const
{
  return (gSystem->Getenv("GHEPHISTENABLE") ? true : false);
}
//___________________________________________________________________________

