// Copyright 2010, Pierre-Antoine Delsart, Kurtis Geerlings, Joey Huston,
//                 Brian Martin, and Christopher Vermilion
//
//----------------------------------------------------------------------
// This file is part of SpartyJet.
//
//  SpartyJet is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 3 of the License, or
//  (at your option) any later version.
//
//  SpartyJet is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with SpartyJet; if not, write to the Free Software
//  Foundation, Inc.:
//      59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//----------------------------------------------------------------------

#ifndef _JETSELECTORTOOL_HH_
#define _JETSELECTORTOOL_HH_

#include "JetCore/JetTool.hh"
#include "JetCore/JetMomentMap.hh"
#include <limits>

namespace SpartyJet { 

class JetSelectorTool : public JetTool {
public :
  JetSelectorTool(std::string name) : JetTool(name), m_debug(false) {}
  JetSelectorTool(double cut, std::string name="selector") : JetTool(name), m_cut1(cut), m_debug(false) {}
  JetSelectorTool(double cut1, double cut2, std::string name="selector") : JetTool(name) ,m_cut1(cut1),m_cut2(cut2), m_debug(false) {}
  JetSelectorTool(std::vector<int> cuts, std::string name="selector") : JetTool(name) ,m_cutsI(cuts), m_debug(false) {}

  virtual void execute(JetCollection &inputJets);
  virtual bool pass_cut(Jet* jet) {return true;}

  virtual void set_cut(double c) {m_cut1 = c;}
  virtual void set_cut2(double c) {m_cut2 = c;}
  
  void setDebug() {m_debug = true;} // print jet info for any jet passing

protected:
  double m_cut1,m_cut2;
  std::vector<int> m_cutsI;
  JetMomentMap* m_map;
  bool m_debug; // if true, print jet info whenever a jet passes
};


class JetPtSelectorTool : public JetSelectorTool  {
public :
  JetPtSelectorTool(std::string name="JetPtSelector"): JetSelectorTool(name), m_N(-1) {}
  JetPtSelectorTool(double pt, int N=-1): JetSelectorTool(pt,"JetPtSelector"), m_N(N) {}
  virtual bool pass_cut(Jet* jet) {return (jet->pt()> m_cut1);}
  virtual void execute(JetCollection &inputJets);

protected:
  int m_N;
};


class JetPtORESelectorTool : public JetSelectorTool  {
public :
  JetPtORESelectorTool(std::string name="JetPtORESelector"): JetSelectorTool(name){}
  JetPtORESelectorTool(double pt, double e): JetSelectorTool(pt,e,"JetPtORESelector"){}
  virtual bool pass_cut(Jet* jet) {return ( (jet->pt()> m_cut1)|| (jet->e()>m_cut2));}

};


class JetEtaCentralSelectorTool : public JetSelectorTool {
public :
  JetEtaCentralSelectorTool(std::string name="JetEtaCentralSelector"):JetSelectorTool(name){}
  JetEtaCentralSelectorTool(double abs_eta) : JetSelectorTool(abs_eta,-abs_eta,"JetEtaCentralSelectorTool"){}
  JetEtaCentralSelectorTool(double etamin, double etamax): JetSelectorTool(etamax, etamin,"JetEtaCentralSelectorTool"){}
  virtual bool pass_cut(Jet* jet){return (jet->eta() > m_cut2) && (jet->eta()<m_cut1);}
};

class JetEtaForwardSelectorTool : public JetSelectorTool {
public :
  JetEtaForwardSelectorTool(std::string name="JetEtaForwardSelector"):JetSelectorTool(name){}
  JetEtaForwardSelectorTool(double abs_eta) : JetSelectorTool(abs_eta,-abs_eta,"JetEtaForwardSelectorTool"){}
  JetEtaForwardSelectorTool(double etamin, double etamax) : JetSelectorTool(etamax, etamin,"JetEtaForwardSelectorTool"){}
  virtual bool pass_cut(Jet* jet){return (jet->eta() < m_cut2) || (jet->eta()>m_cut1);}
};

// ugly ROOT hack because rootcint can't seem to deal with the standard library
#ifdef __CINT__
namespace std {
  template<class T> class numeric_limits {
  public:
    static double max() {return 1e10;}
  };
}
#endif

class JetMassSelectorTool : public JetSelectorTool  {
public :
  JetMassSelectorTool(std::string name="JetMassSelector"): JetSelectorTool(name) {}
  JetMassSelectorTool(double mass_min, double mass_max=std::numeric_limits<double>::max())
    : JetSelectorTool(mass_min, mass_max, "JetMassSelector") {}
  virtual bool pass_cut(Jet* jet) {return (jet->m() > m_cut1 && jet->m() < m_cut2);}
};


class JetInputPdgIdSelectorTool : public JetSelectorTool {
public :
  JetInputPdgIdSelectorTool(std::string name="JetInputPdgIdSelector"):JetSelectorTool(name){}
  JetInputPdgIdSelectorTool(std::vector<int> pdgIds) : JetSelectorTool(pdgIds,"JetInputPdgIdSelectorTool"){}
  virtual bool pass_cut(Jet* jet);
};


// Simple selector that tests if a given moment (pass the name) falls within
//  the specified range.  Can be any type of moment that supports the less-than
//  operator.  ROOT dictionaries for float and int are generated by default; to
//  use anything else in a script, edit JetToolsLinkDef.hpp
template <class T=float>
class JetMomentSelectorTool : public JetSelectorTool {
public:
  JetMomentSelectorTool(std::string momentName, T min, T max)
    : JetSelectorTool("JetMomentSelector"), m_name(momentName), m_min(min), m_max(max) {}
  virtual bool pass_cut(Jet *j) {
    T moment_val = m_map->get_jet_moment<T>(m_name, j);
    return (m_min <= moment_val) && (moment_val <= m_max);
  }
protected:
  std::string m_name;
  T m_min, m_max;
};

}  // namespace SpartyJet
#endif