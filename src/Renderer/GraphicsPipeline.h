#pragma once


class GraphicsPipeline
{
public:
	
	GraphicsPipeline() = default;
	virtual ~GraphicsPipeline(){};

	virtual void* getPipeline() = 0;


};

