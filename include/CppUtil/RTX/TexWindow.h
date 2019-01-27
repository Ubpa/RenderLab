#ifndef _TEX_WINDOW_H_
#define _TEX_WINDOW_H_

#include <CppUtil/Basic/Config.h>
#include <CppUtil/OpenGL/Texture.h>

namespace CppUtil {
	namespace Basic {
		class Operation;
	}
}

namespace RTX {
	class TexWindow {
	public:
		enum ENUM_OPTION {
			ENUM_OPTION_EMPTY = 0,
			ENUM_OPTION_POST_PROCESS_FLIP = 1 << 0,
			ENUM_OPTION_POST_PROCESS_BLUR = 1 << 1,
			ENUM_OPTION_POST_PROCESS_INTERPOLATION = 1 << 2,
			ENUM_OPTION_POST_PROCESS_GAMMA = 1 << 3,
			ENUM_OPTION_SAVE_SRC_IMG = 1 << 4,
			ENUM_OPTION_SAVE_POST_PROCESS_IMG = 1 << 5,
			ENUM_OPTION_POST_PROCESS_ALL = ENUM_OPTION_POST_PROCESS_FLIP | ENUM_OPTION_POST_PROCESS_BLUR | ENUM_OPTION_POST_PROCESS_INTERPOLATION | ENUM_OPTION_POST_PROCESS_GAMMA,
			ENUM_OPTION_BASIC = ENUM_OPTION_POST_PROCESS_FLIP | ENUM_OPTION_POST_PROCESS_INTERPOLATION | ENUM_OPTION_SAVE_SRC_IMG,
			ENUM_OPTION_SAVE_ALL_IMG = ENUM_OPTION_SAVE_SRC_IMG | ENUM_OPTION_SAVE_POST_PROCESS_IMG,
			ENUM_OPTION_ALL = ENUM_OPTION_POST_PROCESS_ALL | ENUM_OPTION_SAVE_ALL_IMG,
		};

		TexWindow(const std::string & title = "Window");

		double GetScale() const { return scale; };
		void SetTex(const CppUtil::OpenGL::Texture & tex) { this->tex = tex; };
		bool IsValid() const { return isValid; };

		bool Run(CppUtil::Basic::Ptr<CppUtil::Basic::Operation> texUpdateOp);

		void Enable(ENUM_OPTION option) { this->option = static_cast<ENUM_OPTION>(this->option | option); }
		void Disable(ENUM_OPTION option) { this->option = static_cast<ENUM_OPTION>((this->option | option) - option); }

		int GetWidth() const { return width; }
		int GetHeight() const { return height; }
		const std::string GetRootPath() const { return rootPath; }
	protected:
		bool isValid;
	private:
		CppUtil::Basic::Ptr<CppUtil::Basic::Config> DoConfig();

		size_t fps;
		const std::string title;
		double scale;
		ENUM_OPTION option;
		std::string rootPath;
		CppUtil::OpenGL::Texture tex;
		int width, height;
		size_t blurNum;
	};
}

#endif // !_TEX_WINDOW_H_
