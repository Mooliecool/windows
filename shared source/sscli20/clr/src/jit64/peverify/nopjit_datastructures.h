// ==++==
// 
//   
//    Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//   
//    The use and distribution terms for this software are contained in the file
//    named license.txt, which can be found in the root of this distribution.
//    By using this software in any fashion, you are agreeing to be bound by the
//    terms of this license.
//   
//    You must not remove this notice, or any other, from this software.
//   
// 
// ==--==

struct EHRegion {
    ReaderBaseNS::RegionKind    kind;     
    ReaderBaseNS::TryKind       tryType;
    FlowGraphNode*   startBlk;        // node in region with smallest start offset
    FlowGraphNode*   endBlk;          // node in region with greatest end offset

    IRNode*     headNode;
    IRNode*     lastNode;

    EHRegion*       parent;
    EHRegionList*   childList;

    unsigned int extentStart;         // min offset of this region and all children
    unsigned int extentEnd;           // max offset of this region and all children
    unsigned int canonicalExitOffset;

    EHRegion*   tryRegion;
    EHRegion*   filterRegion;
    EHRegion*   handlerRegion;

    //struct
    //{
        unsigned isVisited:1;
        unsigned isLive:1;
        unsigned hasNonLocalFlow:1;
        unsigned usesExCode:1;
        unsigned endFinallyIsReachable:1;
    //};

    mdToken catchClassToken;

    EHRegion();
    EHRegion(ReaderBaseNS::RegionKind kind, FlowGraphNode *startB, FlowGraphNode *endB);

    void print(void);
    void printTree(void);
    void addToChildList(EHRegion* child);
};

struct EHRegionList {
    EHRegionList*   next;
    EHRegion*       region;
};

class FlowGraphNode {
public:
    unsigned  startOffset,endOffset;          // msil start and end offset
    FlowGraphEdgeList *predList, *succList;   // ptr to predecessor and successor lists
    FlowGraphNode *next;                      // ptr to next fg node
    EHRegion *rgn;
    IRNode *startNode;                        // IRNode pointers for codegen
    ReaderStack* stack;
    int nBlockNum;
    GLOBALVERIFYDATA* globVerData; 
    //struct{
        unsigned isVisited:1;
        unsigned isSizeZero:1;
    //} flags;

    FlowGraphNode();

    bool isExitBlock(void);

    void addSuccessor(FlowGraphNode* newSucc, bool isNominal, bool isEndFinally);
    void addPredecessor(FlowGraphNode* newPred, bool isNominal, bool isEndFinally);

    // Remove edge from source and destination nodes
    bool removePredecessorArc(FlowGraphNode* deadPred, bool* wasEndFinally);
    bool removeSuccessorArc(FlowGraphNode* deadSucc, bool* wasEndFinally);

    // Util routine - remove edge only from source fg node
    bool removePredecessor(FlowGraphNode* deadPred, bool* wasEndFinally);
    bool removeSuccessor(FlowGraphNode* deadSucc, bool* wasEndFinally);

    void print(void);
    void printAll(void);
};

// Fg List node, used for successor and predecessor lists on FlowGraphNode
class FlowGraphEdgeList {
public:
    FlowGraphNode      *block;
    FlowGraphEdgeList  *next;

    // nominal edges are not actual program flow, they exist for reachability
    //struct {
        unsigned isNominal:1;
        unsigned isEndFinally:1;
        unsigned isDeletedEdge:1;
        // this is different than a nominal edge which is used for EH.
        // fakeForUnreachable means that the edge does not correspond to 
        // ANY flow in the program.  It is a fake edge in the flowgraph for
        // the purposes of keeping _truly_ unreachable MSIL wired into the FG
        // so PEVerify can analyze it.
        unsigned isFakeForUnreachable:1;
    //} flags;

    FlowGraphEdgeList(FlowGraphNode* b, FlowGraphEdgeList* n, bool nom, bool endFinally){
        block = b;
        next = n;
        isNominal = nom;
        isEndFinally = endFinally;
        isDeletedEdge = 0;
    }
};


// The nopjit instantiates very few actual tuples.  The ones that it does instantiate
// can be of one of these kinds.
enum IRNodeKind
{
    IR_label,               // The target of a branch, switch, etc.  (has a branchList associated with it pointing to the branches that may target it)
    IR_branch,              // Conditional/Unconditional (
    IR_ehFlow, 
    IR_ehStart, 
    IR_ehEnd, 
    IR_throw_rethrow, 
    IR_switch, 
    IR_jmp, 
    IR_endfinally, 
    IR_block
};

class IRNode {
public:
    IRNode         *next;
    IRNode         *prev;
    FlowGraphNode  *fgNode;
    EHRegion       *region;
    IRNodeKind      kind;
    BranchList     *branchList;
    unsigned        msilOffset;
    ReaderBaseNS::RegionKind regionKind;



    //struct {
        unsigned isFallThrough:1;
    //} flags;
};


// This data structure is overloaded.  We have a BranchList on all labels (pointing to the branches that target the label).
// We also have a BranchList on all branching instructions themselves (pointing to the lables that they may target).
class BranchList {
public:
    BranchList     *next;
    IRNode         *branch;
};

#ifdef DEBUG
void PrintAllIR(FlowGraphNode *headBlock);
void PrintIRNode(IRNode *node, int indent=4);
void PrintFgNode(FlowGraphNode *node, bool printIr=false);
#endif

















