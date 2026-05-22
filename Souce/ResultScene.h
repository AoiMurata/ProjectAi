// =============================================================================
// ResultScene.h
// Result screen scene (clear or game over)
// =============================================================================
#pragma once

#include "SceneBase.h"

class ResultScene : public SceneBase
{
public:
	void OnEnter() override;
	SceneType Update() override;
	void Draw() override;

private:
	const char* m_resultText = "Result";
	const char* m_guideText  = "Press SPACE for Title";
	int m_resultColor = 0;
};
