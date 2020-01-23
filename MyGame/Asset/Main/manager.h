#ifndef __C_MANAGER__H__
#define __C_MANAGER__H__

#include <string>

#define DATA_FOLDER_NAME (std::string("Data"))
#define DATA_FOLDER_PATH (DATA_FOLDER_NAME + std::string("/"))

class CManager
{

public:
	static void Init();
	static void Uninit();
	static void Update();
	static void Draw();

	static const std::string dataFolderName;
	static const std::string dataFolderPath;
};

#endif //!__C_MANAGER__H__