#pragma once

#include<memory>

class ImGUIContext
{
public:

	ImGUIContext() = default;
	virtual ~ImGUIContext(){};
	static std::unique_ptr<ImGUIContext> Create();

private:

};

