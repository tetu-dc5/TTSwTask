#pragma once
#include "InfoNode.h"

class CInfoList
{
public:
	CInfoList(int current_channel);
	virtual ~CInfoList(void);
private:
	int				m_current_channel;
	CInfoNode*		m_node[2];
protected:
	CInfoNode*		GetAt(int idx, int channel=-1);
public:
	int				GetCount(int channel=-1);
	CInfoNode*		GetTop(int channel=-1);
	void			AddNode(CInfoNode* node, int channel=-1);
	void			RemoveAll(int channel=-1);
};
