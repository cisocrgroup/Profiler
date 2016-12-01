#include "Profiler.h"

using namespace OCRCorrection;

////////////////////////////////////////////////////////////////////////////////
void
Profiler::createAdaptiveProfile(Document& sourceDoc)
{
	std::wofstream os("/tmp/adaptive-profiler.log");
	if (not os.good())
		throw std::system_error(errno, std::system_category(), "/tmp/adaptive-profiler.log");
	auto oldcout = std::wcout.rdbuf(os.rdbuf());
	auto oldcerr = std::wcerr.rdbuf(os.rdbuf());

	createNonAdaptiveProfile(sourceDoc);

	std::wcout.rdbuf(oldcout);
	std::wcerr.rdbuf(oldcerr);
}
