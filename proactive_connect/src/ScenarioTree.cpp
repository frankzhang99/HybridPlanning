//
 // Copyright (c) 2015 Carnegie Mellon University. All Rights Reserved.

 // Redistribution and use in source and binary forms, with or without
 // modification, are permitted provided that the following conditions
 // are met:

 // 1. Redistributions of source code must retain the above copyright
 // notice, this list of conditions and the following acknowledgments
 // and disclaimers.

 // 2. Redistributions in binary form must reproduce the above
 // copyright notice, this list of conditions and the following
 // disclaimer in the documentation and/or other materials provided
 // with the distribution.

 // 3. The names "Carnegie Mellon University," "SEI" and/or "Software
 // Engineering Institute" shall not be used to endorse or promote
 // products derived from this software without prior written
 // permission. For written permission, please contact
 // permission@sei.cmu.edu.

 // 4. Products derived from this software may not be called "SEI" nor
 // may "SEI" appear in their names without prior written permission of
 // permission@sei.cmu.edu.

 // 5. Redistributions of any form whatsoever must retain the following
 // acknowledgment:

 // This material is based upon work funded and supported by the
 // Department of Defense under Contract No. FA8721-05-C-0003 with
 // Carnegie Mellon University for the operation of the Software
 // Engineering Institute, a federally funded research and development
 // center.

 // Any opinions, findings and conclusions or recommendations expressed
 // in this material are those of the author(s) and do not necessarily
 // reflect the views of the United States Department of Defense.

 // NO WARRANTY. THIS CARNEGIE MELLON UNIVERSITY AND SOFTWARE
 // ENGINEERING INSTITUTE MATERIAL IS FURNISHED ON AN "AS-IS"
 // BASIS. CARNEGIE MELLON UNIVERSITY MAKES NO WARRANTIES OF ANY KIND,
 // EITHER EXPRESSED OR IMPLIED, AS TO ANY MATTER INCLUDING, BUT NOT
 // LIMITED TO, WARRANTY OF FITNESS FOR PURPOSE OR MERCHANTABILITY,
 // EXCLUSIVITY, OR RESULTS OBTAINED FROM USE OF THE MATERIAL. CARNEGIE
 // MELLON UNIVERSITY DOES NOT MAKE ANY WARRANTY OF ANY KIND WITH
 // RESPECT TO FREEDOM FROM PATENT, TRADEMARK, OR COPYRIGHT
 // INFRINGEMENT.

 // This material has been approved for public release and unlimited
 // distribution.

 // DM-0002494
 //
/*
 * ScenarioTree.cpp
 *
 *  Created on: Aug 4, 2014
 *      Author: gmoreno
 */

#include "ScenarioTree.h"
#include <sstream>
#include <stack>
#include <iomanip>

using namespace std;

ScenarioTree::ScenarioTree() : root(0) {
}

ScenarioTree::Node& ScenarioTree::getRoot() {
	return root;
}

const ScenarioTree::Node& ScenarioTree::getRoot() const {
    return root;
}



ScenarioTree::~ScenarioTree() {
}

ScenarioTree* ScenarioTree::cloneWithNewRoot() {
    ScenarioTree* pTree = new ScenarioTree;
    pTree->getRoot().edges.push_back(Edge(1, root));

    updateDepths();
    return pTree;
}

int ScenarioTree::getNumberOfNodes() const {
    // TODO this traversal is repeated in several places...should refactor it as an iterator
    stack<ScenarioTree::NodeStateIndex> traversalStack;
    int stateIndex = 0;
    traversalStack.push(ScenarioTree::NodeStateIndex(&root, stateIndex++));
    while (!traversalStack.empty()) {
        const ScenarioTree::Node* pNode = traversalStack.top().pNode;
        traversalStack.pop();

        for (ScenarioTree::Edges::const_iterator it = pNode->edges.begin();
                it != pNode->edges.end(); it++) {
            traversalStack.push(ScenarioTree::NodeStateIndex(&(it->child), stateIndex++));
        }
    }
    return stateIndex;
}

void ScenarioTree::updateDepths() {
    stack<ScenarioTree::Node*> traversalStack;
    root.depth = 0;
    traversalStack.push(&root);
    while (!traversalStack.empty()) {
        ScenarioTree::Node* pNode = traversalStack.top();
        traversalStack.pop();

        for (ScenarioTree::Edges::iterator it = pNode->edges.begin();
                it != pNode->edges.end(); it++) {
            it->child.depth = pNode->depth + 1;
            traversalStack.push(&(it->child));
        }
    }
}

std::ostream& operator<<(std::ostream& os, const ScenarioTree& tree) {
	stack<ScenarioTree::NodeStateIndex> traversalStack;
	int stateIndex = 0;
	traversalStack.push(ScenarioTree::NodeStateIndex(&tree.root, stateIndex++));
	while (!traversalStack.empty()) {
		const ScenarioTree::Node* pNode = traversalStack.top().pNode;
		int nodeStateIndex = traversalStack.top().stateIndex;
		traversalStack.pop();

		//string padding(pNode->depth + 1, ' '); // TODO eventually replace this, depth was only used here
		string padding(" ");
		os << padding << '[' << nodeStateIndex << "] v=" << pNode->value << endl;
		for (ScenarioTree::Edges::const_iterator it = pNode->edges.begin();
				it != pNode->edges.end(); it++) {
			os << padding << "   " << "p=" << it->probability << "-> ["
					<< stateIndex << ']' << endl;
			traversalStack.push(ScenarioTree::NodeStateIndex(&(it->child), stateIndex++));
		}
	}

	return os;
}

void ScenarioTree::generateDiagram(std::ostream& os) const {
    const string STATE_VALUE_FORMULA = "formula stateValue = ";

    string result;
    stringstream edgesOutput;

    stack<ScenarioTree::NodeStateIndex> traversalStack;
    stringstream nodesOutput;

    int stateIndex = 0;
    string padding(STATE_VALUE_FORMULA.length(), ' ');

    traversalStack.push(ScenarioTree::NodeStateIndex(&getRoot(), stateIndex++));
    while (!traversalStack.empty()) {
        const ScenarioTree::Node* pNode = traversalStack.top().pNode;
        int nodeStateIndex = traversalStack.top().stateIndex;
        traversalStack.pop();

        // define node
        stringstream labelSS;
        labelSS << 's' << nodeStateIndex;
        string label = labelSS.str();

        nodesOutput << label
                << " [label=\"" << label
                << '=' << setprecision(3) << pNode->value
                << "\"]" << endl;

        for (ScenarioTree::Edges::const_iterator it = pNode->edges.begin();
                it != pNode->edges.end(); it++) {
            edgesOutput << label << " -> s" << stateIndex
                    << " [label=\"" << it->probability << "\"];" << endl;
            traversalStack.push(ScenarioTree::NodeStateIndex(&(it->child), stateIndex++));
        }
    }
    os << "digraph probability_tree {" << endl;
    os << nodesOutput.str() << edgesOutput.str();
    os << "}" << endl;
}
