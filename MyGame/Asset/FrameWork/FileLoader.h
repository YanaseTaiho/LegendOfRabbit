#ifndef _FILE_LOADER_H_
#define _FILE_LOADER_H_

#define NOMINMAX

#include <Windows.h>
#include <vector>
#include <string>
#include <stdexcept>

namespace FileLoader
{
	// フォルダ内のファイル名一覧を取得する関数（ extension : 拡張子 ）
	static std::vector<std::string> GetFileListInFolder(const std::string& folderPath, const std::string& extension) noexcept(false)
	{
		HANDLE hFind;
		WIN32_FIND_DATA win32fd;//defined at Windwos.h
		std::vector<std::string> file_names;

		//拡張子の設定
		std::string search_name = folderPath + "\\*." + extension;

		hFind = FindFirstFile(search_name.c_str(), &win32fd);

		if (hFind == INVALID_HANDLE_VALUE) {
			return file_names;
		}

		do {
			if (win32fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			}
			else {
				file_names.push_back(win32fd.cFileName);
				printf("%s\n", file_names.back().c_str());

			}
		} while (FindNextFile(hFind, &win32fd));

		FindClose(hFind);

		return file_names;
	}
}

#endif // !_FILE_LOADER_H_

