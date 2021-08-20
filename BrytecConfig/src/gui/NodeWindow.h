#pragma once

#include <vector>
#include <utility>
#include <memory>
#include "../data/Node.h"
#include "../data/Selectable.h"
#include <map>
#include <imnodes.h>

class NodeWindow
{
	enum class Mode {
		Build, Simulation
	};

	bool m_opened = true;
	std::weak_ptr<Selectable> m_lastSelected;
	Mode m_mode = Mode::Build;
	std::map<std::shared_ptr<Pin>, imnodes::EditorContext*> m_contexts;

public:
	NodeWindow();
	~NodeWindow();

	void drawWindow();
	void setOpenedState(bool state) { m_opened = state; }

private:
	void drawPopupMenu();
	void drawNode(std::shared_ptr<Node>& node);
	void addLinkData();
	void isLinkCreated();
	void isLinkDeleted();
	void isNodeDeleted();
	void saveNodePositions();
	void doValuePopup();
	imnodes::EditorContext* getContext(std::shared_ptr<Pin>& pin);

	//Node Types
	void drawOutput		(std::shared_ptr<Node>& node);
	void drawFinalValue	(std::shared_ptr<Node>& node);
	void drawRawInput	(std::shared_ptr<Node>& node);
	void drawPin		(std::shared_ptr<Node>& node);
	void drawAnd		(std::shared_ptr<Node>& node);
	void drawOr			(std::shared_ptr<Node>& node);
	void drawTwoStage	(std::shared_ptr<Node>& node);
	void drawCurve		(std::shared_ptr<Node>& node);
	void drawCompare	(std::shared_ptr<Node>& node);
	void drawOnOff		(std::shared_ptr<Node>& node);
	void drawInvert		(std::shared_ptr<Node>& node);
	void drawToggle		(std::shared_ptr<Node>& node);
	void drawDelay		(std::shared_ptr<Node>& node);
	void drawPushButton	(std::shared_ptr<Node>& node);
	void drawMapValue	(std::shared_ptr<Node>& node);
	void drawMath		(std::shared_ptr<Node>& node);
	void drawValue		(std::shared_ptr<Node>& node);

};

