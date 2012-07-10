#include "StdAfx.h"
#include "InfoNode.h"

CInfoNode::CInfoNode(int current_channel)
: m_current_channel(current_channel)
{
	m_next[0] = NULL;
	m_next[1] = NULL;
}

CInfoNode::~CInfoNode(void)
{
	if(m_next[m_current_channel])
	{
		m_next[m_current_channel]->Unlink(m_current_channel);
		m_next[m_current_channel] = NULL;
	}
}

void CInfoNode::AddTail(CInfoNode* node, int channel)
{
	if(channel<0) channel = m_current_channel;
	CInfoNode*	p = this;
	while(p->GetNext(channel)) p=p->GetNext(channel);
	p->m_next[channel] = node;
	node->InitNext(channel);
}

CInfoNode* CInfoNode::GetNext(int channel)
{
	if(channel<0) channel = m_current_channel;
	return m_next[channel];
}

void CInfoNode::InitNext(int channel)
{
	if(channel<0) channel = m_current_channel;
	m_next[channel] = NULL;
}

void CInfoNode::Unlink(int channel)
{
	if(channel<0) channel = m_current_channel;
	if(m_next[channel])
	{
		m_next[channel]->Unlink(channel);
		m_next[channel] = NULL;
	}
	if(channel==m_current_channel)
	{
		delete this;
	}
}
