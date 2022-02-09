#pragma once

#include <string>

namespace Veist
{

	struct NametagComponent
	{
		NametagComponent() = default;
		NametagComponent(std::string nametag) : m_nametag(nametag) {};
		NametagComponent(const NametagComponent&) = default;
		

		std::string* nametag() { return &m_nametag; };
		const std::string* nametag() const { return &m_nametag; };

	private:

		std::string m_nametag;

	};

	
struct TestComponent
{

	TestComponent() = default;
	TestComponent(const TestComponent&) = default;
	TestComponent(uint32_t val1, uint32_t val2) : m_value1(val1), m_value2(val2) {};

	uint32_t	m_value1;
	uint32_t	m_value2;


};

}
