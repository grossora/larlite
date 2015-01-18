/**
 * \file AlgoEMPart.h
 *
 * \ingroup ERTool
 * 
 * \brief Class def header for a class AlgoEMPart
 *
 * @author jhewes15
 */

/** \addtogroup ERTool

    @{*/

#ifndef ERTOOL_ALGOGAMMASELECTION_H
#define ERTOOL_ALGOGAMMASELECTION_H

#include "Base/AlgoBase.h"
#include "Base/PdfFactory.h"
#include "Base/RangeVar.h"
#include <RooPlot.h>
#include <RooProdPdf.h>
#include <TCanvas.h>
#include <TH1D.h>
#include <TH2D.h>
#include <cstdlib>
#include <ctime>

namespace ertool {

  /**
     \class AlgoEMPart
     User custom SPAFilter class made by jhewes15
   */
  class AlgoEMPart : public AlgoBase {
  
  public:

    /// Default constructor
    AlgoEMPart();

    /// Default destructor
    virtual ~AlgoEMPart(){}

    /// Called only once by the constructor in its lifespan
    void Init();

    /// Override the sptool::SPTBase::LoadParams function
    virtual void LoadParams(std::string fname="",size_t version=kINVALID_SIZE);
    
    /// Switch e- (false) / gamma (true) mode
    void SetMode(const bool gamma) { _mode = gamma; }

    /// What to do before event-loop begins
    virtual void ProcessBegin();

    /// Function to evaluate input showers and determine a score
    virtual ParticleSet Reconstruct(const EventData &data);

    /// What to do once event-loop is over
    virtual void ProcessEnd(TFile* fout);

    /// Likelihood for a particle being gamma/electron
    double LL(bool is_electron, double dedx, double rad_length = -1.);

    /// Set bounds for dEdx PDF fitting - Electron
    void set_e_dEdxFitRange(double min, double max) { _e_dedx_fitMin = min; _e_dedx_fitMax = max; }
    /// Set bounds for dEdx PDF fitting - Gamma
    void set_g_dEdxFitRange(double min, double max) { _g_dedx_fitMin = min; _g_dedx_fitMax = max; }

  protected:

    bool _mode; ///< e-/gamma mode (true: gamma, false: e-)
    
    // Variables
    RangeVar _dedx;     ///< dE/dx [MeV/cm] range
    RangeVar _radl;     ///< radiation length [cm] range
    RangeVar _dedx_fit; ///< dE/dx [MeV/cm] range for fit
    RangeVar _radl_fit; ///< radiation length [cm] range for fit

    double _g_lval;    ///< gamma rad length param mean [cm]
    double _g_lmin;    ///< gamma rad length param range min [cm]
    double _g_lmax;    ///< gamma rad length param range max [cm]
    double _g_dedxfrac; ///< gamma frac that has e dEdx
    double _g_dedxmu1; ///< gamma dEdx param mean [MeV/cm]
    double _g_dedxsigma1; ///< gamma dEdx param sigma [MeV/cm]
    double _g_dedxmu2; ///< gamma dEdx param mean [MeV/cm]
    double _g_dedxsigma2; ///< gamma dEdx param sigma [MeV/cm]
    double _g_dedxmin; ///< gamma dEdx param range min [MeV/cm]
    double _g_dedxmax; ///< gamma dEdx param range max [MeV/cm]
    double _g_dedx_fitMin; ///< where to start fitting [MeV/cm]
    double _g_dedx_fitMax; ///< where to end fitting [MeV/cm]

    // Variables
    RooRealVar* _dEdxVar;
    RooRealVar* _radLenVar;

    // e- RadLen PDF 
    RooAbsPdf*  _e_radLenPdf;  ///< e- RadLen PDF
    RooDataSet* _e_radLenData; ///< e- pdf data set
    RooRealVar* _e_radLenVal;

    // e- dEdx PDF
    RooAbsPdf*  _e_dEdxPdf;   ///< e- dEdx PDF
    RooDataSet* _e_dEdxData;
    RooRealVar* _e_dEdxMu;
    RooRealVar* _e_dEdxSigma;

    // e- dEdx Conv Pdf
    RooRealVar* _e_landauMu;
    RooRealVar* _e_landauSigma;
    RooAbsPdf*  _e_dEdxConvPdf;
    // variables
    double _e_landauMu_val;
    double _e_landauMu_min;
    double _e_landauMu_max;
    double _e_landauSigma_val;
    double _e_landauSigma_min;
    double _e_landauSigma_max;

    // gamma RadLen PDF
    RooAbsPdf*  _g_radLenPdf;     ///< gamma RadLen Pdf
    RooDataSet* _g_radLenData;    ///< gamma pdf data set
    RooRealVar* _g_radLenVal;

    // gamma dEdx PDF
    RooAbsPdf*  _g_dEdxPdf; ///< gamma dEdx Pdf
    RooDataSet* _g_dEdxData;
    RooRealVar* _g_dEdxFrac;
    RooRealVar* _g_dEdxMu1;
    RooRealVar* _g_dEdxSigma1;
    RooRealVar* _g_dEdxMu2;
    RooRealVar* _g_dEdxSigma2;

    // gamma dEdx Conv Pdf
    RooRealVar* _g_landauMu;
    RooRealVar* _g_landauSigma;
    RooAbsPdf*  _g_dEdxConvPdf;
    // variables
    double _g_landauMu_val;
    double _g_landauMu_min;
    double _g_landauMu_max;
    double _g_landauSigma_val;
    double _g_landauSigma_min;
    double _g_landauSigma_max;
    double _g_gaussMu_val;
    double _g_gaussMu_min;
    double _g_gaussMu_max;
    double _g_gaussSigma_val;
    double _g_gaussSigma_min;
    double _g_gaussSigma_max;

  };
}
#endif

/** @} */ // end of doxygen group 
