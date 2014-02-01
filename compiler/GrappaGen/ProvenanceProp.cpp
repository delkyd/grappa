#include "Passes.h"
#include <llvm/Analysis/CFGPrinter.h>
#include <llvm/Support/GraphWriter.h>

using namespace Grappa;

const Value* ProvenanceProp::UNKNOWN       = reinterpret_cast<Value*>(0);
const Value* ProvenanceProp::INDETERMINATE = reinterpret_cast<Value*>(1);

char ProvenanceProp::ID = 0;

bool ProvenanceProp::runOnFunction(Function& F) {
  return false;
}

//////////////////////////////
// Register optional pass
static RegisterPass<ProvenanceProp> X( "provenance-prop", "Provenance Prop", false, false );


struct ProvenancePropPrinter {
  Function *fn;
  std::map<Value*,Value*>& provenance;
};

template <>
struct GraphTraits<ProvenancePropPrinter> : public GraphTraits<const BasicBlock*> {
  static NodeType *getEntryNode(ProvenancePropPrinter p) { return &p.fn->getEntryBlock(); }
  
  // nodes_iterator/begin/end - Allow iteration over all nodes in the graph
  typedef Function::iterator nodes_iterator;
  static nodes_iterator nodes_begin(ProvenancePropPrinter p) { return p.fn->begin(); }
  static nodes_iterator nodes_end  (ProvenancePropPrinter p) { return p.fn->end(); }
  static size_t         size       (ProvenancePropPrinter p) { return p.fn->size(); }
};

template<>
struct DOTGraphTraits<ProvenancePropPrinter> : public DefaultDOTGraphTraits {
  DOTGraphTraits(bool simple=false): DefaultDOTGraphTraits(simple) {}
  
  using FTraits = DOTGraphTraits<const Function*>;
  
  static std::string getGraphName(ProvenancePropPrinter f) { return "ProvenanceProp"; }
  
  std::string getNodeLabel(const BasicBlock *Node, ProvenancePropPrinter p) {
    // return FTraits::getCompleteNodeLabel(Node, p.fn);
    std::string str;
    raw_string_ostream o(str);
    
    o << "<<table border='0' cellborder='0'>";
    
    for (auto& inst : *const_cast<BasicBlock*>(Node)) {
      const char * color = "black";
      if (p.provenance[&inst] == ProvenanceProp::UNKNOWN)
        color = "black";
      else if (p.provenance[&inst] == ProvenanceProp::INDETERMINATE)
        color = "red";
      else if (dyn_cast_addr<GLOBAL_SPACE>(inst.getType()))
        color = "blue";
      else if (dyn_cast_addr<SYMMETRIC_SPACE>(inst.getType()))
        color = "cyan";
      
      o << "<tr><td align='left'><font color='" << color << "'>" << inst << "</font></td></tr>";
    }
    
    o << "</table>>";
    
    return o.str();
  }
  
  template< typename T >
  static std::string getEdgeSourceLabel(const BasicBlock *Node, T I) {
    return FTraits::getEdgeSourceLabel(Node, I);
  }
  
  static std::string getNodeAttributes(const BasicBlock* Node, ProvenancePropPrinter p) {
    return "";
  }
};

void ProvenanceProp::viewGraph(Function *fn) {
  ProvenancePropPrinter ppp = {fn,provenance};
  ViewGraph(ppp, "provenance." + Twine(fn->getValueID()));
}
