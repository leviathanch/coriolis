// This file is part of the Coriolis Software.
// Copyright (c) UPMC 2008-2014, All Rights Reserved
//
// +-----------------------------------------------------------------+ 
// |                   C O R I O L I S                               |
// |          Alliance / Hurricane  Interface                        |
// |      Yacc Grammar for Alliance Structural VHDL                  |
// |                                                                 |
// |  Author      :                    Jean-Paul CHAPUT              |
// |  E-mail      :       Jean-Paul.Chaput@asim.lip6.fr              |
// |                                                                 |
// +-----------------------------------------------------------------+


#include <stdio.h>
#include <string.h>
#include <string>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
using namespace std;

#include "hurricane/Warning.h"
#include "hurricane/Net.h"
#include "hurricane/Cell.h"
#include "hurricane/Plug.h"
#include "hurricane/Instance.h"
#include "hurricane/UpdateSession.h"
using namespace Hurricane;

#include "crlcore/Utilities.h"
#include "crlcore/Catalog.h"
#include "crlcore/AllianceFramework.h"
#include "crlcore/NetExtension.h"
#include "crlcore/ToolBox.h"
#include "crlcore/Blif.h"
using namespace CRL;


namespace {
  using namespace std;

  void  addSupplyNets ( Cell* cell )
  {
    Net* vss = Net::create ( cell, "vss" );
    vss->setExternal ( true );
    vss->setGlobal   ( true );
    vss->setType     ( Net::Type::GROUND );

    Net* vdd = Net::create ( cell, "vdd" );
    vdd->setExternal ( true );
    vdd->setGlobal   ( true );
    vdd->setType     ( Net::Type::POWER );
  }

  void  connectSupplyNets ( Instance * instance, Cell* design )
  {
    Cell* instcell = instance->getCell();
    if(instcell == NULL){
        throw Error("Instance's cell is null\n");
    }
    Net* vssint = instcell->getNet( "vss" );
    Net* vddint = instcell->getNet( "vdd" );
    Net* vssext = design->getNet( "vss" );
    Net* vddext = design->getNet( "vdd" );
    auto vssplug = instance->getPlug( vssint );
    auto vddplug = instance->getPlug( vddint );
    vssplug->setNet( vssext );
    vddplug->setNet( vddext );
  }

  void  SetNetType ( Net* net, AllianceFramework * framework )
  {
    if ( framework->isPOWER(net->getName()) ) {
      net->setType   ( Net::Type::POWER  );
      net->setGlobal ( true );
    } else if ( framework->isGROUND(net->getName()) ) {
      net->setType   ( Net::Type::GROUND );
      net->setGlobal ( true );
    } else if ( framework->isCLOCK(net->getName()) ) {
      net->setType   ( Net::Type::CLOCK );
    } else
      net->setType ( Net::Type::LOGICAL );
  }

  enum ParserState{
    EXT     = 0x00,
    MODEL   = 0x01,
    SUBCKT  = 0x02,
    NAMES   = 0x04,
    INPUTS  = 0x08,
    OUTPUTS = 0x16
  };

  struct subckt{
    string cell;
    vector<pair<string, string> > pins;
  };

  struct model{
    string name;
    unordered_map<string, Net::Direction> pins;
    vector<subckt> subcircuits;
    vector<pair<string, string> > aliases;
    bool operator<(model const & o) const{ return name < o.name; }
  };

  std::vector<std::string> tokenize(std::string const & s){
    std::vector<std::string> ret;
    auto b_it = s.begin();
    while(b_it != s.end()){
      // Remove whitespace
      while(b_it != s.end() && (*b_it == ' ' or *b_it == '\t'))
        ++b_it;
      // Find the end of the token
      auto e_it = b_it;
      while(e_it != s.end() && *e_it != ' ' && *e_it != '\t' and *e_it != '#')
        ++e_it;
      // Create the token
      if(e_it != b_it)
        ret.push_back(std::string(b_it, e_it));
      // Handle comments
      if(e_it != s.end() && *e_it == '#')
        e_it = s.end();
      b_it = e_it;
    }
    std::reverse(ret.begin(), ret.end());
    return ret;
  }

}  // End of anonymous namespace.


namespace CRL {

// 
// Can only parse simple, netlist BLIF files generated by Yosys
// Ignores all ".names" and uses only the .subckt, .model, .input and .output 
//

  Cell* Blif::load ( string cellPath )
  {
    using namespace std;

    Cell*  mainModel = NULL;
    string mainName;
    string blifFile  = cellPath;
    size_t dot       = cellPath.find_first_of('.');
    if (dot != string::npos) {
      blifFile = cellPath.substr( 0, dot );
      mainName = cellPath.substr( dot+1 );
    }
  
    auto framework = AllianceFramework::get ();
  
    std::ifstream ccell ( blifFile+".blif" );
    if(ccell.fail()){
      throw Error( "Unable to open BLIF file %s.blif\n", blifFile.c_str() );
    }
    cmess2 << "     " << tab++ << "+ " << blifFile << " [blif]" << endl;
  
    std::vector<model> models;
    ParserState state = EXT;
    bool hasName = true;
  
    string line;
    vector<string> current_names;
    while(getline(ccell, line)){
      istringstream linestream(line);
      vector<string> tokens = tokenize(line);
      if(not tokens.empty()){
        string const token = tokens.back();
        tokens.pop_back();
        assert(not token.empty());
        if(token[0] == '.'){
          // Process finished .names statement
          if(not current_names.empty()){
            if( current_names.size() != 4
              or current_names[2] != "1" or current_names[3] != "1"
              ){
              ostringstream mess;
              mess << "\'.names\' statement is not an alias and will be ignored.\n"
                   << "          Map to standard cells for functions and tie cells for constants.\n"
                   << "            ";
              for ( size_t iname=0 ; iname<current_names.size() ; ++iname ) {
                if (iname) mess << ", ";
                mess << "\'" << current_names[iname] << "\'";
              }
              cerr << Warning(mess.str()) << endl;
            }
            else{
              // Name statement is an alias: the second signal will map to the first
              models.back().aliases.push_back(pair<string, string>(current_names[0], current_names[1]));
            }
            current_names.clear();
          }
          if(token == ".model"){
            if(state != EXT)
              throw Error("Nested model are not supported\n");
            state = MODEL;
            hasName = false;
            models.push_back(model());
          }
          else if(token == ".subckt"){
            if(state == EXT)
              throw Error("Subcircuit without an enclosing model are not supported\n");
            if(state == MODEL and not hasName)
              throw Error("Model has no name\n");
            state = SUBCKT;
            hasName = false;
            models.back().subcircuits.push_back(subckt());
          }
          else if(token == ".names"){
            if(state == EXT)
              throw Error("Names without an enclosing model are not supported\n");
            if(state == MODEL and not hasName)
              throw Error("Model has no name\n");
            state = NAMES;
          }
          else if(token == ".latch"){
            throw Error("Latch constructs are not understood by the parser\n");
          }
          else if(token == ".inputs"){
            if(state == EXT)
              throw Error("Inputs have been found without an enclosing model\n");
            state = INPUTS;
          }
          else if(token == ".outputs"){
            if(state == EXT)
              throw Error("Outputs have been found without an enclosing model\n");
            state = OUTPUTS;
          }
          else if(token == ".end"){
            if(state == EXT)
              throw Error("A .end has been found out of a model\n");
            state = EXT;
          }
          else{
            ostringstream mess;
            mess << "Unknown control token <" << token << ">\n";
            throw Error(mess.str());
          }
  
        }
        else if(state == NAMES){
          // Part of a cover for a logic function
          current_names.push_back(token);
        }
        else{
          ostringstream mess;
          mess << "Encountered a non-control token at the beginning of a line: <" << token << ">\n";
          throw Error(mess.str());
        }
      }
      // Process all tokens after the control
      while(not tokens.empty()){
        string const token = tokens.back();
        tokens.pop_back();
        assert(not token.empty());
        // Either a pin or an input/output definition
        if(state == INPUTS or state == OUTPUTS){
          auto it = models.back().pins.find(token);
          Net::Direction D = (state == INPUTS)? Net::Direction::DirIn : Net::Direction::DirOut;
          if(it != models.back().pins.end()){
            it->second = static_cast<Net::Direction::Code>(D | it->second);
          }
          else{
            models.back().pins.insert(pair<string, Net::Direction>(token, D));
          }
        }
        else if(state == SUBCKT){
          if(hasName){
            // Encountered a pin: need to be processed
            auto equal_pos = token.find_first_of('=');
            if(equal_pos+1 < token.size()){
              string before_space = token.substr(0, equal_pos);
              string after_space  = token.substr(equal_pos+1, string::npos);
              models.back().subcircuits.back().pins.push_back(pair<string, string>(before_space, after_space));
            }
            else{
              ostringstream mess;
              mess << "Unable to parse the subckt pin specification <"
                   << token << ">\n";
              Error(mess.str());
            }
          }
          else{
            models.back().subcircuits.back().cell = token;
            hasName = true;
          }
        }
        else if(state == NAMES){
            current_names.push_back(token);
        }
        else if(state == MODEL){
          if(hasName)
            throw Error("Unexpected token after model name\n");
          else{
            models.back().name = token;
            cmess2 << "     " << tab << "+ " << token << " [interface+signals]" << endl;
            hasName = true;
          }
        }
        else{
          throw Error("Unexpected token\n");
        }
      }
      line.clear();
    }
    if(state != EXT){
      cerr << Warning("End of model has not been found\n");
    }
  
    /*
    for(auto & M : models){
      cout << "Model: " << M.name << endl;
      for(auto & S : M.subcircuits){
        cout << "\tInstance of " << S.cell;
        for(auto & P : S.pins){
            cout << " " << P.first << ":" << P.second;
        }
        cout << endl;
      }
    }
    */
  
    // Two passes: first create the cells and their nets, then create the internals
    std::vector<Cell*> model_cells;
    for(auto M : models){
      Cell * design = framework->createCell(M.name);
      if(design == NULL){
        throw Error("Model " + M.name + " is NULL\n");
      }
      if (M.name == mainName) mainModel = design;
  
      model_cells.push_back(design);
      addSupplyNets(design);
  
      unordered_set<string> net_names;
      for(auto const & S : M.subcircuits){
        for(auto const & P : S.pins){
          net_names.insert(P.second);
        }
        for(auto const & A : M.aliases){
          net_names.insert(A.first);
          net_names.insert(A.second);
        }
      }
  
      for(auto const & P : M.pins){
        net_names.insert(P.first);
      }
  
      for(string const & N : net_names){
        Net* new_net = Net::create( design, N );
        auto it = M.pins.find(N);
        if(it != M.pins.end()){
          new_net->setExternal( true );
          new_net->setDirection( it->second );
        }
      }
    }
  
    // Second pass: every cell and its nets have already been created
    for(size_t i=0; i<models.size(); ++i){
      cmess2 << "     " << tab++ << "+ " << models[i].name << " [instances]" << endl;
      auto const & M = models[i];
      Cell * design = model_cells[i];
      for(size_t j=0; j<M.subcircuits.size(); ++j){
        auto & S = M.subcircuits[j];
        ostringstream subckt_name;
        subckt_name << "subckt_" << j;
        Cell * cell = framework->getCell(S.cell, Catalog::State::Views, 0);
        if(cell == NULL){
          throw Error("Cell <" + S.cell + "> to instanciate hasn't been found\n");
        }
        //cmess2 << "Creating instance <" << subckt_name.str() << "> of <" << S.cell << "> in <" << models[i].name << ">" << endl;
        Instance* instance = Instance::create( design, subckt_name.str(), cell);
  
        for(auto & P : S.pins){
          Net* internalNet = cell->getNet( P.first );
          Net* externalNet = design->getNet( P.second );
          assert(internalNet != NULL and externalNet != NULL and instance != NULL);
          instance->getPlug( internalNet )->setNet( externalNet );
        }
        //connectSupplyNets(instance, design);
      }
      // Merge the aliased nets
      for(auto alias : M.aliases){
        //cmess2 << "Merging nets <" << alias.first << "> and <" << alias.second << ">" << endl;
        Net * first_net  = design->getNet( alias.first );
        Net * second_net = design->getNet( alias.second );
        if(first_net == NULL or second_net == NULL){
          ostringstream mess;
          mess << "Trying to create an alias for non-instantiated nets:";
          if(first_net == NULL)
            mess << " <" << alias.first << ">";
          if(second_net == NULL)
            mess << " <" << alias.second << ">";
          mess << ", will be ignored\n";
          cerr << Warning(mess.str());
          continue;
        }
        if(!first_net->isExternal())
            swap(first_net, second_net); // If only one net is external, it needs to be the first
        first_net->merge(second_net);
      }
      --tab;
    }
    --tab;

    for ( auto model : model_cells )
      CRL::NamingScheme::toVhdl( model, CRL::NamingScheme::Recursive|CRL::NamingScheme::FromVerilog );

    if(model_cells.empty()){
      throw Error("No model found in the file\n");
    }
    else if(mainModel) {
      return mainModel;
    }
    else if (model_cells.size() > 1){
      cerr << Warning( "Blif::load(): Several models found, returned the first one %s.\n"
                     , getString(model_cells[0]->getName()).c_str()
                     );
    }

    return model_cells[0];
  }


}  // CRL namespace.

