#pragma once

template <typename T>
class SceneGraphNode
{
public:
	std::vector<SceneGraphNode*> ChildNode;
	T Type;
};

template <typename T>
class SceneGraph
{
public:
	SceneGraph()
		: RootNode{ nullptr }
	{}

	SceneGraph(SceneGraphNode<T>* InRoot)
		: RootNode{ InRoot }
	{}

	void SetRoot(SceneGraphNode<T>* InRoot)
	{
		RootNode = InRoot;
	}

	void AddChild(SceneGraphNode<T>* Node)
	{
		SceneGraphNode<T>* NewNode = new SceneGraphNode<T>;
		Node->ChildNode.push_back(NewNode);
	}

	void Loop(SceneGraphNode<T>* StartNode, std::vector<SceneGraphNode<T>*> TempNodeVector)
	{
		for (auto NodeIt : StartNode->ChildNode)
		{
			TempNodeVector.push_back(NodeIt);
			Loop(NodeIt, TempNodeVector);
		}
	}

	void Traverse(SceneGraphNode<T>* StartNode, std::vector<SceneGraphNode<T>*>& NodeVectorTemp)
	{
		if (StartNode->ChildNode.size())
		{
			for (auto NodeIt : StartNode->ChildNode)
			{
				if (NodeIt)
				{
					NodeVectorTemp.push_back(NodeIt);
				}
				Traverse(NodeIt, NodeVectorTemp);
			}
		}
	}

	SceneGraphNode<T>* RootNode;
};