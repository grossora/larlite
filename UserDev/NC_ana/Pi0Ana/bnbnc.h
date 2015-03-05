/**
 * \file bnbnc.h
 *
 * \ingroup Pi0Ana
 * 
 * \brief Class def header for a class bnbnc
 *
 * @author ryan
 */

/** \addtogroup Pi0Ana

    @{*/

#ifndef LARLITE_BNBNC_H
#define LARLITE_BNBNC_H

#include "Analysis/ana_base.h"

namespace larlite {
  /**
     \class bnbnc
     User custom analysis class made by SHELL_USER_NAME
   */
  class bnbnc : public ana_base{
  
  public:

    /// Default constructor
    bnbnc(){ _name="bnbnc"; _fout=0;}

    /// Default destructor
    virtual ~bnbnc(){}

    /** IMPLEMENT in bnbnc.cc!
        Initialization method to be called before the analysis event loop.
    */ 
    virtual bool initialize();

    /** IMPLEMENT in bnbnc.cc! 
        Analyze a data event-by-event  
    */
    virtual bool analyze(storage_manager* storage);

    /** IMPLEMENT in bnbnc.cc! 
        Finalize method to be called after all events processed.
    */
    virtual bool finalize();

  protected:
    
  };
}
#endif

//**************************************************************************
// 
// For Analysis framework documentation, read Manual.pdf here:
//
// http://microboone-docdb.fnal.gov:8080/cgi-bin/ShowDocument?docid=3183
//
//**************************************************************************

/** @} */ // end of doxygen group 
