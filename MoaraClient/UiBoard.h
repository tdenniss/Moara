#pragma once

#include "NodeButton.h"

#include <QWidget>

#include <vector>

class INode;

class UiBoard : public IUiBoard
{
	Q_OBJECT
public:
	UiBoard(const std::string& typePath, const std::string& sizePath, const NodesInfo& nodes, float nodesDistanceFactor = 1.0f);
	void InitializeButtons(const NodesInfo& nodes) override;

	//IUiBoard
	Buttons GetAllButtons() const override;
	NodeButton* GetButton(int index) const override;

	static std::vector<std::pair<int, int>> LoadValues(const std::string& filePath);

	void UpdateNodePositions() override;
	void UpdateBoard(const NodesInfo& nodes) override;

	void SwapPieces(uint8_t fromNodeIndex, uint8_t toNodeIndex, EPieceType fromNodeType) override;
	void AddPiece(uint8_t nodeIndex, EPieceType nodeType) override;
	void RemovePiece(uint8_t index) override;

	void SetHighlightedNodeIndex(uint8_t index) override;
	void HighlightNodes(PiecesIndexes indexes) override;
	void ResetComputerNodesHighlight() override;
	void ResetNodesHighlight() override;

	int GetFirstSelectedNodeIndex() override;
	void SetFirstSelectedNodeIndex(int index) override;

	ImagesCache GetImagesCache() const override;
public slots:
	void OnNodeClicked(uint8_t index);
	void OnRemoveNodeClicked(uint8_t index);
	void OnMove(uint8_t fromIndex, uint8_t toIndex);
	void OnHighlightMove(uint8_t index);
signals:
	void nodeClicked(uint8_t index);
	void removeNodeClicked(uint8_t index);
	void move(uint8_t fromIndex, uint8_t toIndex);
	void highlightMove(uint8_t index);
protected:
	void paintEvent(QPaintEvent* event) override;
	void resizeEvent(QResizeEvent* event) override;
private:
	void ConnectButtons();

private:
	Buttons m_buttons;

	Positions m_nodePositions;
	Connections m_nodeConnections;

	PiecesIndexes m_highlightedNodes;
	PiecesIndexes m_highlightedComputerNodes;

	ImagesCache m_imagesCache;

	int m_firstSelectedNodeIndex;

	float  m_nodesDistanceFactor;
};

