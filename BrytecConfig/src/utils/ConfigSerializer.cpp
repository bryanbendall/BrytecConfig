#include "ConfigSerializer.h"

ConfigSerializer::ConfigSerializer(std::shared_ptr<Config>& config)
	: m_config(config)
{
}

void ConfigSerializer::SerializeText(const std::filesystem::path& filepath)
{
}

bool ConfigSerializer::DeserializeText(const std::filesystem::path& filepath)
{
	return false;
}
