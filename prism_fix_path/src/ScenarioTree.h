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
 * ScenarioTree.h
 *
 *  Created on: Aug 4, 2014
 *      Author: gmoreno
 */

#ifndef SCENARIOTREE_H_
#define SCENARIOTREE_H_

#include <vector>
#include <string>
#include <iostream>

struct NodeData {
	std::vector<double> util;
	std::vector<int> next;
};

class ScenarioTree {

public:
	struct Edge;
	typedef std::vector<Edge> Edges;


	struct Node {
		Node(double value) : value(value), depth(0) {}
		Node(const Node& n) : value(n.value), edges(n.edges), data(n.data), depth(n.depth) {}
		double value;
		Edges edges;
		NodeData data;
		unsigned depth;
	};

	struct Edge {
		double probability;
		Node child;

		Edge(double probability, const Node& child)
			: probability(probability), child(child) {};
		Edge(const Edge& e) : probability(e.probability), child(e.child) {}
	};

	/**
	 * This struct is used to enumerate nodes (e.g., to map them to states)
	 */
    struct NodeStateIndex {
        NodeStateIndex(const Node* pNode, int stateIndex)
            : pNode(pNode), stateIndex(stateIndex) {};
        const Node* pNode;
        int stateIndex;
    };
protected:
	Node root;


public:
	ScenarioTree();
	Node& getRoot();
    const Node& getRoot() const;

	int getNumberOfNodes() const;

	/**
	 * The depth attribute of the nodes is not assigned until this is called
	 */
	void updateDepths();

	/**
	 * Creates a clone with a new root, making the (copy of) current root the only child of the new root
	 */
	ScenarioTree* cloneWithNewRoot();

	void generateDiagram(std::ostream& os) const;

	virtual ~ScenarioTree();

	friend std::ostream& operator<<(std::ostream& os, const ScenarioTree& tree);
};

#endif /* SCENARIOTREE_H_ */
