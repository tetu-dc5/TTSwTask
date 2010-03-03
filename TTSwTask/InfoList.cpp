#include "StdAfx.h"
#include "InfoList.h"

CInfoList::CInfoList(int channel)
: m_current_channel(channel)
{
	m_node[0] = NULL;
	m_node[1] = NULL;
}

CInfoList::~CInfoList(void)
{
	RemoveAll(m_current_channel);
}

int CInfoList::GetCount(int channel)
{
	if(channel<0) channel=m_current_channel;
	if(!m_node[channel]) return 0;
	CInfoNode*	node = GetTop(channel);
	int			cnt  = 0;
	while(node)
	{
		cnt++;
		node = node->GetNext(channel);
	}
	return cnt;
}

CInfoNode* CInfoList::GetAt(int idx, int channel)
{
	if(channel<0) channel=m_current_channel;
	if(idx<0) return NULL;
	CInfoNode*	node = GetTop(channel);
	for(int i=0;i<idx;i++)
	{
		node = node->GetNext(channel);
		if(!node) return NULL;
	}
	return node;
}

CInfoNode* CInfoList::GetTop(int channel)
{
	if(channel<0) channel=m_current_channel;
	return m_node[channel];
}

void CInfoList::AddNode(CInfoNode* node, int channel)
{
	if(channel<0) channel=m_current_channel;
	if(!GetTop(channel))
	{
		m_node[channel] = node;
		m_node[channel]->InitNext(channel);
	}
	else
	{
		m_node[channel]->AddTail(node, channel);
	}
}

void CInfoList::RemoveAll(int channel)
{
	if(channel<0) channel=m_current_channel;
	if(m_node[channel])
	{
		m_node[channel]->Unlink(channel);
		m_node[channel] = NULL;
	}
}
