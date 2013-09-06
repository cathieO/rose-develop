#ifndef CFANALYZER_H
#define CFANALYZER_H

/*************************************************************
 * Copyright: (C) 2012 Markus Schordan                       *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
 *************************************************************/

#include "SgNodeHelper.h"
#include "Labeler.h"
#include "CommandLineOptions.h"

namespace CodeThorn {

  enum EdgeType { EDGE_UNKNOWN=0, EDGE_FORWARD, EDGE_BACKWARD, EDGE_TRUE, EDGE_FALSE, EDGE_LOCAL, EDGE_CALL, EDGE_CALLRETURN, EDGE_EXTERNAL, EDGE_PATH };

  class Edge;
  typedef set<Edge> EdgeSet;

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
class Edge {
 public:
  Edge();
  Edge(Label source0,Label target0);
  Edge(Label source0,EdgeType type0,Label target0);
  Edge(Label source0,set<EdgeType> type0,Label target0);
  string toString() const;
  string toStringNoType() const;
  string toDotColored() const;
  string toDotFixedColor(string) const;
  //string typeToString() const;
  string typesToString() const;
  static string typeToString(EdgeType et);
  Label source;
  //EdgeType type;
  Label target;
  bool isType(EdgeType et) const;
  void addType(EdgeType et);
  void addTypes(set<EdgeType> ets);
  void removeType(EdgeType et);
  set<EdgeType> types() const;
  long typesCode() const;
  string color() const;
  string dotEdgeStyle() const;
  long hash() const;
 private:
  set<EdgeType> _types;
};

 bool operator==(const Edge& e1, const Edge& e2);
 bool operator!=(const Edge& e1, const Edge& e2);
 bool operator<(const Edge& e1, const Edge& e2);

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
 class Flow : public set<Edge> {
 public:  
  Flow();
  Flow operator+(Flow& s2);
  Flow& operator+=(Flow& s2);
  LabelSet nodeLabels();
  LabelSet sourceLabels();
  LabelSet targetLabels();
  LabelSet pred(Label label);
  LabelSet succ(Label label);
  LabelSet reachableNodes(Label start);

  //LabelSet reachableNodesOnPath(Label start, Label target);
  /* computes all nodes which are reachable in the graph from the start node. A path is terminated by either the target node 
	or by a node without successors. The start node is not included in the result set, except
     it is reachable on some path starting from the start node.
  */
  LabelSet reachableNodesButNotBeyondTargetNode(Label start, Label target);

  Flow inEdges(Label label);
  Flow outEdges(Label label);
  Flow edgesOfType(EdgeType edgeType);
  Flow outEdgesOfType(Label label, EdgeType edgeType);
  void setDotOptionDisplayLabel(bool opt);
  void setDotOptionDisplayStmt(bool opt);
  void setDotOptionFixedColor(bool opt);
  void setDotFixedColor(string color);
  void setDotOptionHeaderFooter(bool opt);
  string toDot(Labeler *labeler);
  void setTextOptionPrintType(bool opt);
  void resetDotOptions();
  string toString();
  // deletes all Edges of type edgeType. The return value is the number of deleted edges.
  size_t deleteEdges(EdgeType edgeType);
  size_t deleteEdges(Flow& flow);
 private:
  bool _dotOptionDisplayLabel;
  bool _dotOptionDisplayStmt;
  bool _stringNoType;
  bool _dotOptionFixedColor;
  string _fixedColor;
  bool _dotOptionHeaderFooter;
};

class InterEdge {
 public:
  InterEdge(Label call, Label entry, Label exit, Label callReturn);
  string toString() const;
  Label call;
  Label entry;
  Label exit;
  Label callReturn;
};

bool operator<(const InterEdge& e1, const InterEdge& e2);
bool operator==(const InterEdge& e1, const InterEdge& e2);
bool operator!=(const InterEdge& e1, const InterEdge& e2);

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
class InterFlow : public set<InterEdge> {
 public:
  string toString() const;
};

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
class CFAnalyzer {
 public:
  CFAnalyzer(Labeler* l);
  Label getLabel(SgNode* node);
  SgNode* getNode(Label label);
  Label initialLabel(SgNode* node);
  LabelSet finalLabels(SgNode* node);
  LabelSet functionCallLabels(Flow& flow);
  LabelSet functionEntryLabels(Flow& flow);
  LabelSet conditionLabels(Flow& flow);
  Label correspondingFunctionExitLabel(Label entryLabel);
  Label correspondingFunctionCallReturnLabel(Label callLabel);
  int numberOfFunctionParameters(Label entryNode);
  bool isVoidFunction(Label entryNode);
  LabelSetSet functionLabelSetSets(Flow& flow);
  LabelSet functionLabelSet(Label entryLabel, Flow& flow);
  Flow flow(SgNode* node);
  Flow flow(SgNode* s1, SgNode* s2);
  Labeler* getLabeler();
  // computes from existing intra-procedural flow graph(s) the inter-procedural call information
  InterFlow interFlow(Flow& flow); 
  void intraInterFlow(Flow&, InterFlow&);
  int reduceNode(Flow& flow, Label lab);
  int reduceBlockBeginNodes(Flow& flow);
  int reduceEmptyConditionNodes(Flow& flow);

  /*! 
   * This function performs inlining on the ICFG by reducing
   * call/entry/exit/callreturn-nodes, if the function being called is
   * a "trivial" function. A "trivial" function has no formal parameters and
   * is called exactly once in a program.
   */
  int inlineTrivialFunctions(Flow& flow);
  size_t deleteFunctioncCallLocalEdges(Flow& flow);
 private:
  Flow WhileAndDoWhileLoopFlow(SgNode* node, Flow edgeSet, EdgeType param1, EdgeType param2);
  Labeler* labeler;
};    

} // end of namespace CodeThorn

#endif
