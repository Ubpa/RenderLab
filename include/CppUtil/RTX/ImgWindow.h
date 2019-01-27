#ifndef _IMG_WINDOW_H_
#define _IMG_WINDOW_H_

#include <CppUtil/RTX/TexWindow.h>
#include <CppUtil/Basic/Image.h>

namespace RTX {
	class ImgWindow : public TexWindow{
	public:
		ImgWindow(const std::string & title = "Window");

		CppUtil::Basic::Image & GetImg() { return img; };

		bool Run(CppUtil::Basic::Ptr<CppUtil::Basic::Operation> imgUpdateOp);
	private:
		CppUtil::Basic::Image img;
	};
}

#endif // !_IMG_WINDOW_H_
