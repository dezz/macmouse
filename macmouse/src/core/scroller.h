
#ifndef __scroller_h
#define __scroller_h

/**
 * ���C���̂��
 */
class MacMouseScroller {
private:
	class impl;
	std::auto_ptr<impl> pimpl;

public:
	MacMouseScroller();
	~MacMouseScroller();
	void start();
	void end();
};

#endif // __scroller_h

