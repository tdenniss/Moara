#pragma once
#include "EPieceType.h"
#include "EBoardSize.h"
#include "EBoardType.h"
#include "ImagesCache.h"

#include <QWidget>

using Buttons = std::vector<class NodeButton*>;
using Position = std::pair<int, int>;
using Positions = std::vector<Position>;
using NodeConnections = std::pair<int, int>;
using Connections = std::vector<NodeConnections>;
using PiecesIndexes = std::vector<uint8_t>;
using NodesInfo = std::vector<std::pair<int, EPieceType>>;

class IUiBoard : public QWidget
{
public:
	static IUiBoard* Produce(EBoardType type, EBoardSize size, const NodesInfo& nodes);
	virtual void InitializeButtons(const NodesInfo& nodes) = 0;

	virtual Buttons GetAllButtons() const = 0;
	virtual NodeButton* GetButton(int index) const = 0;

	virtual void SwapPieces(uint8_t fromNodeIndex, uint8_t toNodeIndex, EPieceType fromNodeType) = 0;
	virtual void AddPiece(uint8_t nodeIndex, EPieceType nodeType) = 0;
	virtual void RemovePiece(uint8_t index) = 0;

	virtual void UpdateNodePositions() = 0;
	virtual void UpdateBoard(const NodesInfo& nodes) = 0;

	virtual int GetFirstSelectedNodeIndex() = 0;
	virtual void SetFirstSelectedNodeIndex(int index) = 0;

	virtual void SetHighlightedNodeIndex(uint8_t index) = 0;
	virtual void HighlightNodes(PiecesIndexes indexes) = 0;
	virtual void ResetNodesHighlight() = 0;
	virtual void ResetComputerNodesHighlight() = 0;

	virtual ImagesCache GetImagesCache() const = 0;
};

