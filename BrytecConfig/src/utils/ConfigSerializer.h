#pragma once

#include <memory>
#include "data/Config.h"
#include <filesystem>

class ConfigSerializer
{

public:
	ConfigSerializer(std::shared_ptr<Config>& config);

	void serializeText(const std::filesystem::path& filepath);
	bool deserializeText(const std::filesystem::path& filepath);

private:
	std::shared_ptr<Config> m_config;

};