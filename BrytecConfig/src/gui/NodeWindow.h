#pragma once

#include <vector>
#include <utility>
#include <memory>
#include "../data/NodeGroup.h"
#include "../data/Selectable.h"
#include <map>
#include <imnodes.h>

class NodeWindow
{
	enum class Mode {
		Build, Simulation
	};

	bool m_opened = true;
	bool m_isFocused = false;
	std::weak_ptr<Selectable> m_lastSelected;
	Mode m_mode = Mode::Build;
	// TODO remove node groups when deleted
	std::weak_ptr<NodeGroup> m_nodeGroup;
	std::map<std::shared_ptr<NodeGroup>, imnodes::EditorContext*> m_contexts;

public:
	NodeWindow();
	~NodeWindow();

	void drawWindow();
	void setOpenedState(bool state) { m_opened = state; }
	bool getOpenedState() { return m_opened; }
	bool getIsWindowFocused() { return m_isFocused; }

private:
	void drawMenubar();
	void drawPopupMenu(std::shared_ptr<NodeGroup>& nodeGroup);
	void drawNode(std::shared_ptr<Node>& node);
	void addLinkData(std::shared_ptr<NodeGroup>& nodeGroup);
	void isLinkCreated(std::shared_ptr<NodeGroup>& nodeGroup);
	void isLinkDeleted(std::shared_ptr<NodeGroup>& nodeGroup);
	void isNodeDeleted(std::shared_ptr<NodeGroup>& nodeGroup);
	void saveNodePositions(std::shared_ptr<NodeGroup>& nodeGroup);
	void doValuePopup();
	imnodes::EditorContext* getContext(std::shared_ptr<NodeGroup>& nodeGroup);

	//Node Types
	void drawInitialValue	(std::shared_ptr<Node>& node);
	void drawFinalValue		(std::shared_ptr<Node>& node);
	void drawPin			(std::shared_ptr<Node>& node);
	void drawAnd			(std::shared_ptr<Node>& node);
	void drawOr				(std::shared_ptr<Node>& node);
	void drawTwoStage		(std::shared_ptr<Node>& node);
	void drawCurve			(std::shared_ptr<Node>& node);
	void drawCompare		(std::shared_ptr<Node>& node);
	void drawOnOff			(std::shared_ptr<Node>& node);
	void drawInvert			(std::shared_ptr<Node>& node);
	void drawToggle			(std::shared_ptr<Node>& node);
	void drawDelay			(std::shared_ptr<Node>& node);
	void drawPushButton		(std::shared_ptr<Node>& node);
	void drawMapValue		(std::shared_ptr<Node>& node);
	void drawMath			(std::shared_ptr<Node>& node);
	void drawValue			(std::shared_ptr<Node>& node);

};

