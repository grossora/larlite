#ifndef SELECTIONTOOL_SPALGOSINGLEE_CXX
#define SELECTIONTOOL_SPALGOSINGLEE_CXX

#include "SPAlgoSingleE.h"

namespace sptool {

  size_t single_e_counter = 0;
  size_t total_e_showers = 0;
  size_t total_g_showers = 0;
  size_t nonzero_dedx_counter = 0;

  
  SPAlgoSingleE::SPAlgoSingleE() : SPAlgoBase()
  {

    _name     = "SPAlgoSingleE";
    e_ll_values = 0;
    dedx_values = 0;
  }

  void SPAlgoSingleE::Reset()
  {
    std::cout<<__FUNCTION__<<" found "<<single_e_counter<<" events with a single electron in it."<<std::endl;
    std::cout<<"and "<<nonzero_dedx_counter<<" nonzero dedx showers"<<std::endl;
    std::cout<<"Found "<<total_e_showers<<" total e showers"<<std::endl;
    std::cout<<"Found "<<total_g_showers<<" total g showers"<<std::endl;
  }
  
  void SPAlgoSingleE::ProcessBegin()
  {

    _alg_emp.LoadParams();
    _alg_emp.ProcessBegin();

    //kaleko
    _alg_emp.SetMode(true);

    if(!e_ll_values)
      e_ll_values = new TH1F("e_ll_values","e_ll_values",1000,-1,0);

    if(!dedx_values)
      dedx_values = new TH1F("dedx_values","dedx_values",1000,0,8);

    return;
  }

  void SPAlgoSingleE::LoadParams(std::string fname, size_t version){
    
    // Load EMPart params
    _alg_emp.LoadParams(fname,version);
    
    return;
  }

  SPArticleSet SPAlgoSingleE::Reconstruct(const SPAData &data){
    //kaleko
    _alg_emp.Reconstruct(data);


    SPArticleSet res;
    
    /// Get a list of the event showers that are electron like
    auto e_showers = ElectronLikeShowers( data._showers );

    /// Get a list of the electron showers that are start-point-isolated
    auto isolated_e_showers = IsolatedStartPtShowers( e_showers );

    if(isolated_e_showers.size() == 1) single_e_counter++;

    /// Make an electron SPArticle for each independent shower and add it to the set
    for(auto const& isol_shower : isolated_e_showers){
      SPArticle p_e;
      p_e.pdg_code(11);
      p_e.pos(isol_shower.Start());
      p_e.mom(isol_shower.Dir()); // for now fill with direction - unit vector
      p_e.energy(isol_shower._energy);
      	
      res.push_back(p_e);

    }
     
    return res;
  }
  

  bool SPAlgoSingleE::IsShowerElectron(const sptool::SPAShower shower){

    //Make sure the shower is likely an electron
    //by using LL function from an SPAlgoEMPart instance
    //(LL uses only dEdX data if "dist" is plugged in negative)
    //If likelihood its gamma is more than electron, skip
    
    //if dedx value == 0, we don't know whether this is electron,
    //so let's assume it isn't.
    if(shower._dedx < 0.02 || shower._dedx > 10.) return false;
    dedx_values->Fill(shower._dedx);
    

    
    nonzero_dedx_counter++;
    
    e_ll_values->Fill(
      _alg_emp.LL(true,shower._dedx,-1.));

    
    bool e_like = (
      _alg_emp.LL(true, shower._dedx, -1.) >
      _alg_emp.LL(false,shower._dedx, -1.)) ?
      true : false;
    
    if(e_like) total_e_showers++;
    else total_g_showers++;

    return e_like;
    
  }

  bool SPAlgoSingleE::AreShowersStartPtCorrelated(const sptool::SPAShower s1, const sptool::SPAShower s2){

    //Is the start point of s1 close to start point of s2?
    double dist = s1.Start().Dist(s2.Start());
    //Hard cut on 1cm for now. This value chosen after viewing some std::couts.
    //Later: use a PDF to determine the likelihood "dist" is small enough
    return  (dist < 1.) ? true : false;

  }

  const std::vector<sptool::SPAShower> SPAlgoSingleE::IsolatedStartPtShowers(const std::vector<sptool::SPAShower> showers){

    //Function purpose: loop over electron showers, make sure they aren't
    //correlated (start points are close) w/ other electron showers
    std::vector<sptool::SPAShower> isolated_showers;
    isolated_showers.clear();

    //Save some time if the list of showers is length 0
    //(return empty vector)
    if(showers.size() == 0) return isolated_showers;

    //Save some time if the list of showers is length 1
    //(return a vector with just the index "0" in it)
    if(showers.size() == 1){
      isolated_showers.push_back(showers.at(0));
      return isolated_showers;
    }

    //For more than 1 electron showers, need to do some combinatorics
    //Strategy: Make a list of the shower indices that are bad (E.G. are close to other showers)
    //Then use that list to return only the good showers.
    std::vector<size_t> bad_shower_indices;

    for(size_t i = 0; i < showers.size()-1; ++i){
      for(size_t j = i+1; j < showers.size(); ++j){
	
	if(AreShowersStartPtCorrelated(showers.at(i),showers.at(j))){
	  //if list of showers that aren't independent doesn't already contain "i"
	  //add "i" to the list of showers
	  if( !(std::find(bad_shower_indices.begin(),bad_shower_indices.end(),i) != bad_shower_indices.end()) )
	    bad_shower_indices.push_back(i);

	  //if list of showers that aren't independent doesn't already contain "j"
	  //add "j" to the list of showers
	  if( !(std::find(bad_shower_indices.begin(),bad_shower_indices.end(),j) != bad_shower_indices.end()) )
	    bad_shower_indices.push_back(j);
	}
      }
    }


    //Now that we have a list of *bad* shower indices
    //Return a vector of *good* showers
    for(size_t idx = 0; idx < showers.size(); idx++)
      //if shower index is not in "bad showers", it is a good shower
      if( !(std::find(bad_shower_indices.begin(),bad_shower_indices.end(),idx) != bad_shower_indices.end()) )
	isolated_showers.push_back( showers.at(idx) );
    
    return isolated_showers;

  }


  const std::vector<sptool::SPAShower> SPAlgoSingleE::ElectronLikeShowers(const std::vector<sptool::SPAShower> showers){


    std::vector<sptool::SPAShower> e_showers;
    e_showers.clear();

    //Loop over all showers in the event, store ones that are electron-like
    for(auto const& shower : showers) 
      if(IsShowerElectron(shower)) e_showers.push_back(shower);
    

    return e_showers;

  }

  void SPAlgoSingleE::ProcessEnd(TFile* fout){
    
    _alg_emp.ProcessEnd(fout);
    
    if(fout){
      fout->cd();
      
      if(e_ll_values)
	e_ll_values->Write();

      if(dedx_values)
	dedx_values->Write();
    }

  }
}

#endif
