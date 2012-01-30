
#ifndef __tasktray_h
#define __tasktray_h

class TaskTrayIconRegistry {
private:
	class impl;
	std::auto_ptr<impl> pimpl;
public:
	TaskTrayIconRegistry(HICON icon, LPCTSTR text, HWND owner, UINT message);
	~TaskTrayIconRegistry();
};

#endif // __tasktray_h
