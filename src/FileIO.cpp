#include <fstream>
#include <filesystem>
#include <vector>

std::vector<char> read_file_binary(const std::filesystem::path& path) {
	std::ifstream file(path, std::ios::binary | std::ios::ate);
	if (!file) {
		throw std::ios_base::failure("Failed to open file: " + path.string());
	}

	std::streamsize size = file.tellg();
	file.seekg(0, std::ios::beg);

	std::vector<char> buffer(static_cast<std::size_t>(size));
	if (!file.read(buffer.data(), size)) {
		throw std::ios_base::failure("Failed to read file: " + path.string());
	}

	return buffer;
}
