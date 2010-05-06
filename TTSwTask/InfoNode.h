#pragma once

class CInfoNode
{
public:
	CInfoNode(int current_channel);
	virtual ~CInfoNode(void);
protected:
	int			m_current_channel;
	CInfoNode*	m_next[2];
public:
	void		AddTail(CInfoNode* node, int channel=-1);
	CInfoNode*	GetNext(int channel=-1);
	void		InitNext(int channel=-1);
	void		Unlink(int channel=-1);
};
