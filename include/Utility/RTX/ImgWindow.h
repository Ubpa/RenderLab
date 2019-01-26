#ifndef _IMG_WINDOW_H_
#define _IMG_WINDOW_H_

#include <Utility/RTX/TexWindow.h>
#include <Utility/Basic/Image.h>

namespace RayTracing {
	class ImgWindow : public TexWindow{
	public:
		ImgWindow(const std::string & title = "Window");

		CppUtility::Other::Image & GetImg() { return img; };

		bool Run(CppUtility::Other::Ptr<CppUtility::Other::Operation> imgUpdateOp);
	private:
		CppUtility::Other::Image img;
	};
}

#endif // !_IMG_WINDOW_H_
