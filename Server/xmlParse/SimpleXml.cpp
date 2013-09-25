//#include "stdafx.h"
//#include <Windows.h>
#include "FuncTools.h"
#include "SimpleXml.h"

CSimpleXml::CSimpleXml()
{
    m_pDoc = NULL;
    m_pRoot = NULL;
    m_pCurrNode = NULL;
}

bool CSimpleXml::OpenXml(const string &strXml, int nXmlType)
{
	if(strXml.empty())
	{
		return false;
	}
//	m_pDoc = new TiXmlDocument(strXml.c_str());
	if(XMLFILE == nXmlType)//�����ļ�
	{
            m_pDoc = new TiXmlDocument(strXml.c_str());
		if(!m_pDoc->LoadFile())  
		{
			return false;
		}
	}
	else if(XMLSTR == nXmlType)//�����ַ���
	{
            m_pDoc = new TiXmlDocument;
		 m_pDoc->Parse(strXml.c_str());
	}
	m_pRoot = m_pDoc->RootElement(); 
	if(NULL == m_pDoc || NULL == m_pRoot)
	{
		return false;
	}
	TiXmlPrinter printer;
	printer.SetStreamPrinting();
	if(m_pDoc->Accept(&printer))
	{
		m_strXml = printer.CStr();
		return true;
	}
	return false;
}

CSimpleXml::~CSimpleXml(void)
{
    if(m_pDoc)
    {
        delete(m_pDoc);
    }
}

bool CSimpleXml::ModifyXml(list<XMLNODEINFO> &lstXmlNodes)
{
    if(m_pDoc == NULL || m_pRoot == NULL)
    {
        return false;
    }
    list<XMLNODEINFO>::iterator itlist = lstXmlNodes.begin();
    int nSize = lstXmlNodes.size();
    for(int i = 0; i < nSize; ++i)
    {
        if(!AccessXmlNode((*itlist), MODIFY))
        {
            return false;
        }
        ++itlist;
    }
    TiXmlPrinter printer;
    printer.SetStreamPrinting();
    if(m_pDoc->Accept( &printer ))
    {
        m_strXml = printer.CStr();
    }
    else
    {
        return false;
    }
    return true;
}

//�޸Ľ��struNote���򷵻ؽ��struNode��ֵ
bool CSimpleXml::AccessXmlNode( XMLNODEINFO &struNote, int nType)
{
    int nEnd = 0;
    if(m_pDoc == NULL || m_pRoot == NULL)
    {
        return false;
    }
    TiXmlText* pText = NULL; // һ��ָ��Text��ָ��
    pText = new TiXmlText(struNote.strData.c_str()) ;
    string strNodeName = struNote.strNodeName;
    nEnd = strNodeName.find("/");
    string strNode = strNodeName.substr(0, nEnd);
    strNodeName = strNodeName.substr(nEnd + 1, strNodeName.length() - nEnd);

    TiXmlNode *node = m_pRoot;
    TiXmlNode *destnode = NULL;
    while(node)
    {
        string strchildnode = node->Value();
        if(strNode != strchildnode)
        {
            node = node->NextSibling();
            continue;//���ӽ�������Ҫ���ӽ�㣬������һ���ӽ��
        }
        if((strNode == strNodeName) && (strNode == node->Value()))//Node�������Ƿ��ʵĽ�㡣
        {
            destnode = node->FirstChild();
            if(destnode && destnode->Type() == TiXmlNode::TEXT)//��Ҷ�ӽ�㣬�޸�Ϊ���ǵ�ֵ
            {
                if(nType == QUERY)
                {
                    struNote.strData = destnode->Value();//��ѯ����ֵ
                }
                else
                {
                    destnode->SetValue(struNote.strData.c_str());//�޸�Ϊ���ǵ�ֵ
                }
            }
            else if(destnode && destnode->Type() == TiXmlNode::ELEMENT)//����Ҷ�ӽ�㣬�������ǵ�ֵ
            {
                node->InsertBeforeChild(destnode, *pText);
            }
            else if(!destnode)//Ҫд�Ľ��ԭֵΪ�գ��������ǵ�ֵ
            {
                node->InsertEndChild(*pText);
            }
            return true;
        }
        node = node->FirstChild();//Node�������Ƿ��ʵĽ�㣬��ʼ����Node���ӽ��
        nEnd = strNodeName.find("/");
        strNode = strNodeName.substr(0, nEnd);
        strNodeName = strNodeName.substr(nEnd + 1, strNodeName.length() - nEnd);
        if(node && (strNode == strNodeName) && (strNode == node->Value()))//Node�������Ƿ��ʵĽ��
        {
            destnode = node->FirstChild();
            if(destnode && destnode->Type() == TiXmlNode::TEXT)//��Ҷ�ӽ�㣬�������ǵ�ֵ
            {
                if(nType == QUERY)
                {
                    struNote.strData = destnode->Value();//��ѯ����ֵ
                }
                else
                {
                    destnode->SetValue(struNote.strData.c_str());//�޸�Ϊ���ǵ�ֵ
                }
            }
            else if(destnode && destnode->Type() == TiXmlNode::ELEMENT)//����Ҷ�ӽ�㣬�������ǵ�ֵ
            {
                node->InsertBeforeChild(destnode, *pText);
            }
            else if(!destnode)//Ҫд�Ľ��ԭֵΪ�գ��������ǵ�ֵ
            {
                node->InsertEndChild(*pText);
            }
            return true;
        }
    }
    return false;
}

bool CSimpleXml::GetXml(string &strXml)
{
    strXml = m_strXml;
    if(m_strXml.empty())
    {
        return false;
    }
    return true;
}

// ��xmlת����list<map<string, string> >
void CSimpleXml::Xml2Listmap(list<map<string, string> > &lstmapData)
{
    TiXmlNode *node = m_pRoot;
    TiXmlNode *destnode = NULL;
    while(NULL != node)
    {
        if (strcmp("MAP", node->Value()) == 0)
        {
            map<string, string> mapData;
            destnode = node->FirstChild();
            while (NULL != destnode)
            {
                if(destnode->Type() == TiXmlNode::ELEMENT)
                {
                    string strKey = destnode->Value();
                    if (NULL != destnode->FirstChild())
                    {
                        mapData.insert(make_pair(strKey, destnode->FirstChild()->Value()));
                    }
                }
                destnode = destnode->NextSibling();
            }
            lstmapData.push_back(mapData);
        }
        node = node->NextSibling();
    }
}

// ��list<map<string, string> >ת����xml
void CSimpleXml::Listmap2Xml(list<map<string, string> > &lstmapData, string &strXml)
{
    list<map<string, string> >::iterator itlstmap = lstmapData.begin();
    for (; itlstmap != lstmapData.end(); ++itlstmap)
    {
        strXml += "<MAP>";

        char szBuf[4096];
        memset(szBuf, 0, 4096);
        map<string, string>::iterator itmapData = itlstmap->begin();
        for (; itmapData != itlstmap->end(); ++itmapData)
        {
            char szData[256];
            sprintf(szData, "<%s>%s</%s>", itmapData->first.c_str(), itmapData->second.c_str(), itmapData->first.c_str());
            strcat(szBuf, szData);
        }
        strXml += szBuf;
        strXml += "</MAP>";
    }
}

// ��ָ���ڵ��µĽڵ�װ��map��strNodeֻ֧�ֵ�һ��
void CSimpleXml::GetSpecifyNode(const string &strNode, map<string, string> &mapNode, TiXmlNode *begin/* = NULL*/)
{
    TiXmlNode *node = NULL;
    if (NULL == begin)
    {
        node = m_pRoot;
    }
    else
    {
        node = begin;
    }

    while(NULL != node)
    {
        if (strNode == node->Value())
        {
            TiXmlNode *destnode = node->FirstChild();
            while (NULL != destnode)
            {
                if(destnode->Type() == TiXmlNode::ELEMENT)
                {
                    string strKey = destnode->Value();
                    TiXmlNode *nodechild = destnode->FirstChild();
                    if (NULL != nodechild)
                    {
                        if (NULL != nodechild->FirstChild())
                        {
                            while (NULL != nodechild)
                            {
                                TiXmlNode *grandson = nodechild->FirstChild();
                                string strKey = nodechild->Value();
                                if (NULL != grandson)
                                {
                                    mapNode.insert(make_pair(strKey, grandson->Value()));
                                }

                                nodechild = nodechild->NextSibling();
                            }

                        }
                        else
                        {
                            mapNode.insert(make_pair(strKey, nodechild->Value()));
                        }
                    }
                }
                destnode = destnode->NextSibling();
            }
        }
        node = node->NextSibling();
    }
}

// �ҵ�ָ��·���ĵ�һ����������map
bool CSimpleXml::FirstSpecifyNode(const string &strNode, map<string, string> &mapNode)
{
    if(m_pDoc == NULL || m_pRoot == NULL)
    {
        return false;
    }
    m_pCurrNode = m_pRoot;

    list<string> lstNode;
    SeperateString(lstNode, strNode, "/");

    string strEnd = lstNode.back();
    lstNode.pop_back();

    list<string>::iterator itlstNode = lstNode.begin();

    while (NULL != m_pCurrNode)
    {
        if (itlstNode != lstNode.end() && *itlstNode == m_pCurrNode->Value())
        {
            if (++itlstNode == lstNode.end())
            {
                m_pCurrNode = m_pCurrNode->FirstChild();
                if (NULL != m_pCurrNode)
                {
                    GetSpecifyNode(strEnd, mapNode, m_pCurrNode);
                    return true;
                }
                return false;
            }
            m_pCurrNode = m_pCurrNode->FirstChild();
        }
        else
        {
            m_pCurrNode = m_pCurrNode->NextSibling();
        }
    }
    return false;
}

// �ҵ���һ��
bool CSimpleXml::NextSpecifyNode(const string &strNode, map<string, string> &mapNode)
{
    list<string> lstNode;
    SeperateString(lstNode, strNode, "/");

    string strEnd = lstNode.back();
    lstNode.pop_back();

    if (NULL != m_pCurrNode)
    {
        m_pCurrNode = m_pCurrNode->NextSibling();
        if (NULL != m_pCurrNode)
        {
            GetSpecifyNode(strEnd, mapNode, m_pCurrNode);
            if (mapNode.empty())
            {
                return false;
            }
            return true;
        }
    }
    return false;
}

bool CSimpleXml::ModifyXmlAttri(list<XMLNODEINFO> &lstNode)
{
    if(m_pDoc == NULL || m_pRoot == NULL)
    {
        return false;
    }

    list<XMLNODEINFO>::iterator itlst = lstNode.begin();
    for(; itlst != lstNode.end(); ++itlst)
    {
        if(!ModifyOneAttri(*itlst))
        {
            return false;
        }
    }

    TiXmlPrinter printer;

    char szBuf[32];
    memset(szBuf, 0, 32);
    szBuf[0] = 0x9;
    printer.SetIndent(szBuf);
    szBuf[0] = 0x0a;
    printer.SetLineBreak(szBuf);
    if(m_pDoc->Accept(&printer))
    {
        m_strXml = printer.CStr();
    }
    else
    {
        return false;
    }
    return true;
}

bool CSimpleXml::ModifyOneAttri(XMLNODEINFO &xmlnode)
{
    if(m_pDoc == NULL || m_pRoot == NULL)
    {
        return false;
    }
    TiXmlElement *pCurrNode = m_pRoot;

    list<string> lstNode;
    SeperateString(lstNode, xmlnode.strNodeName, "/");

    list<string>::iterator itlstNode = lstNode.begin();

    while (NULL != pCurrNode)
    {
        if (itlstNode != lstNode.end() && *itlstNode == pCurrNode->Value())
        {
            string strShow = pCurrNode->Value();
            if (++itlstNode == lstNode.end())
            {
                pCurrNode->SetAttribute(xmlnode.strAttri.c_str(), xmlnode.strData.c_str());
                return true;
            }
            pCurrNode = pCurrNode->FirstChildElement();
        }
        else
        {
            pCurrNode = pCurrNode->NextSiblingElement();
        }
    }
    return false;
}

bool CSimpleXml::AccessXmlAttri(XMLNODEINFO &xmlnode)
{
    if(m_pDoc == NULL || m_pRoot == NULL)
    {
        return false;
    }
    TiXmlElement *pCurrNode = m_pRoot;

    list<string> lstNode;
    SeperateString(lstNode, xmlnode.strNodeName, "/");

    list<string>::iterator itlstNode = lstNode.begin();

    while (NULL != pCurrNode)
    {
        if (itlstNode != lstNode.end() && *itlstNode == pCurrNode->Value())
        {
            string strShow = pCurrNode->Value();
            if (++itlstNode == lstNode.end())
            {
                xmlnode.strData = pCurrNode->Attribute(xmlnode.strAttri.c_str());
                return true;
            }
            pCurrNode = pCurrNode->FirstChildElement();
        }
        else
        {
            pCurrNode = pCurrNode->NextSiblingElement();
        }
    }
    return false;
}

string CSimpleXml::GetAttri(const string &strPath, const string strAttri/* = "value"*/)
{
    if(m_pDoc == NULL || m_pRoot == NULL)
    {
        return false;
    }
    TiXmlElement *pCurrNode = m_pRoot;

    list<string> lstNode;
    SeperateString(lstNode, strPath, "/");

    list<string>::iterator itlstNode = lstNode.begin();

    while (NULL != pCurrNode)
    {
        if (itlstNode != lstNode.end() && *itlstNode == pCurrNode->Value())
        {
            string strShow = pCurrNode->Value();
            if (++itlstNode == lstNode.end())
            {
                return pCurrNode->Attribute(strAttri.c_str());
            }
            pCurrNode = pCurrNode->FirstChildElement();
        }
        else
        {
            pCurrNode = pCurrNode->NextSiblingElement();
        }
    }
    return 0;
}

bool CSimpleXml::AddNodes(const string &strPath, list<XMLNODEINFO> &lstaddnode)
{
    if(m_pDoc == NULL || m_pRoot == NULL)
    {
        return false;
    }
    TiXmlElement *pCurrNode = m_pRoot;

    list<string> lstNode;
    SeperateString(lstNode, strPath, "/");

    list<string>::iterator itlstNode = lstNode.begin();

    while (NULL != pCurrNode)
    {
        if (itlstNode != lstNode.end() && *itlstNode == pCurrNode->Value())
        {
            string strShow = pCurrNode->Value();
            if (++itlstNode == lstNode.end())
            {
                AddOneNode(&pCurrNode, lstaddnode);
                goto SIMXML;
            }
            pCurrNode = pCurrNode->FirstChildElement();
        }
        else
        {
            pCurrNode = pCurrNode->NextSiblingElement();
        }
    }
    return false;

    SIMXML:

    TiXmlPrinter printer;

    char szBuf[32];
    memset(szBuf, 0, 32);
    szBuf[0] = 0x9;
    printer.SetIndent(szBuf);
    szBuf[0] = 0x0a;
    printer.SetLineBreak(szBuf);
    if(m_pDoc->Accept(&printer))
    {
        m_strXml = printer.CStr();
    }
    else
    {
        return false;
    }
    return true;
}

bool CSimpleXml::AddOneNode(TiXmlElement **ppNode, list<XMLNODEINFO> &lstnode)
{
    TiXmlElement *pNode = *ppNode;

    list<XMLNODEINFO>::iterator itlstnode = lstnode.begin();
    for (; itlstnode != lstnode.end(); ++itlstnode)
    {
        TiXmlElement *pCliNode = new TiXmlElement(itlstnode->strNodeName.c_str());
        pNode->LinkEndChild(pCliNode);

        pCliNode->SetAttribute(itlstnode->strAttri.c_str(), itlstnode->strData.c_str());
    }
    return true;
}

//add by yinxiao 
bool CSimpleXml::ModifyXmlAttrContinue(list<XMLNODEINFO> &lstNode)
{
    if(m_pDoc == NULL || m_pRoot == NULL)
    {
        return false;
    }

    list<XMLNODEINFO>::iterator itlst = lstNode.begin();
    for(; itlst != lstNode.end(); ++itlst)
    {
        if(!ModifyOneAttri(*itlst))
        {
            continue;
        }
        else
        {
            cout<<endl<<"------>"<<(*itlst).strNodeName<<endl;
        }
    }

    TiXmlPrinter printer;

    char szBuf[32];
    memset(szBuf, 0, 32);
    szBuf[0] = 0x9;
    printer.SetIndent(szBuf);
    szBuf[0] = 0x0a;
    printer.SetLineBreak(szBuf);
    if(m_pDoc->Accept(&printer))
    {
        m_strXml = printer.CStr();
    }
    else
    {
        return false;
    }
    return true;
}
