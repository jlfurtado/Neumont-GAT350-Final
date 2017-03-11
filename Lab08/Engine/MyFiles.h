#ifndef MYFILES_H
#define MYFILES_H

// Justin Furtado
// 6/26/2016
// MyFiles.h
// A utility class for file input

namespace Engine
{
	class MyFiles
	{
	public:
		static char *ReadFileIntoString(const char *const fileName);

	};
}

#endif // ifndef MYFILES_H