#pragma once

#include <memory>
#include "../data/Config.h"
#include <filesystem>

class ConfigSerializer
{

public:
	ConfigSerializer(std::shared_ptr<Config>& config);

	void SerializeText(const std::filesystem::path& filepath);
	bool DeserializeText(const std::filesystem::path& filepath);

private:
	std::shared_ptr<Config> m_config;

};